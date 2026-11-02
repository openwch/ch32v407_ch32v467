/********************************** (C) COPYRIGHT *****************************
 * File Name          : CH390_eth_driver.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2026/09/17
 * Description        : Main program body.
 ******************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 ******************************************************************************/


#include "debug.h"
#include "CH390.h"
#include "CH390_Interface.h"
#include "net_config.h"
#include "wchnet.h"

#define PHY_LINK_SUCCESS                (1 << 2)          //PHY connection success
uint8_t link_is_valid = 0;
uint8_t link_process_step = 0;
uint8_t phy_link_up = 0;
uint8_t accelerate_link_step = 0;
extern uint32_t volatile LocalTime;
volatile uint32_t TaskExecutionTime = 0;

/**
 * @name ch390_int_handler
 * @brief Handle CH390 interrupt events, include packet receive
 */
void ch390_int_handler()
{
    uint8_t int_status = ch390_get_int_status();
    // Link status changed
    if (int_status & ISR_LNKCHG)
    {
        uint16_t phy_stat = 0;
        Delay_Ms(65);
        if (ch390_get_link_status())
        {
            phy_stat |= PHY_LINK_SUCCESS;
            phy_link_up = 1; 
        }
        else
        {
            phy_stat &= ~PHY_LINK_SUCCESS;
            phy_link_up = 0;
            link_is_valid = 0;
            accelerate_link_step = 0;
        }
        WCHNET_PhyStatus( phy_stat, NETIF1_INDEX);
        ch390_write_reg(CH390_ISR, ISR_LNKCHG);
    }

    if (int_status & ISR_ROS)
    {
        /* Receive overflow */
    }

    if (int_status & ISR_ROO)
    {
        /* Overflow counter overflow */
    }

    if (int_status & ISR_PR)
    {
        /* Packet received */
    }
}

/**
 * @name phy_check_link_vaild
 * @brief check whether Link is valid
 */
void phy_check_link_vaild(void)
{
    static uint32_t check_link_valid_time = 0;
    static uint32_t check_time = 0;
    uint16_t phy_reg, phy_bcr;

    if(link_is_valid == 0)
    {
        // 20ms check interval
    	if(LocalTime - check_link_valid_time < 20) return;

        phy_bcr = ch390_read_phy(CH390_PHY_BMCR);
        if((phy_bcr & (1<<13)) == 0)   //Do nothing if Link mode is 10M.
        {
        	link_is_valid = 1;
        	check_time = 0;
            return;
        }
        ch390_write_phy(CH390_PHY_PAGE_SEL, 0);
        phy_reg = ch390_read_phy(0x10);
        if((phy_reg & (1<<9)) == 0)
        {
        	check_time++;
            if(check_time >= 5)
            {
            	check_time = 0;
                phy_link_up = 0;
                ch390_write_phy(CH390_PHY_BMCR, 0x8000); // reset phy
            }
        }
        else {
            check_time = 0;
        }
		check_link_valid_time = LocalTime;
    }
}

/**
 * @name phy_check_pn
 * @brief check PHY PN polarity
 */
void phy_check_pn(uint16_t time)
{
    static uint32_t check_pn_time = 0;
    uint16_t phy_stat;

    if(LocalTime - check_pn_time < time) return;

    ch390_write_phy(CH390_PHY_PAGE_SEL, 0);
    phy_stat = ch390_read_phy(0x10);
    if(phy_stat & (1<<12))
    {
        if(link_process_step == 0)
        {
            link_process_step = 1;
        } else{
            link_process_step = 0;
            phy_stat = ch390_read_phy(CH390_PHY_ANER);
            if((time == 200) || ((phy_stat & 1) == 0))
            {
                phy_stat = ch390_read_phy(0x1B);
                phy_stat |= 1<<15;
                ch390_write_phy(0x1B, phy_stat);
            }
        }
    } else {
        link_process_step = 0;
    }
    check_pn_time = LocalTime;
}

void phy_accelerate_link(void)
{
	static uint32_t accelerate_link_time = 0;
    uint16_t phy_reg;

    switch(accelerate_link_step)
    {
        case 0:
        	accelerate_link_step = 1;
        	accelerate_link_time = LocalTime;
            //power down
            phy_reg = ch390_read_phy(CH390_PHY_BMCR);
            phy_reg |= (1<<11);
            ch390_write_phy(CH390_PHY_BMCR, phy_reg);
            //decrease Link Time
            ch390_write_phy(CH390_PHY_PAGE_SEL, 0);
            ch390_write_phy(0x13, 0x4);
            break;

        case 1:
            if(LocalTime - accelerate_link_time > 1200)
            {
            	accelerate_link_step = 2;
                // power up
                phy_reg = ch390_read_phy(CH390_PHY_BMCR);
                phy_reg &= ~(1<<11);
                ch390_write_phy(CH390_PHY_BMCR, phy_reg);
            }
            break;

        case 2:
            ch390_write_phy(CH390_PHY_PAGE_SEL, 99);
            phy_reg = ch390_read_phy(0x19);
            if((phy_reg & 0xf) == 2)
            {
            	accelerate_link_step = 3;
                ch390_write_phy(CH390_PHY_PAGE_SEL, 0);
                ch390_write_phy(0x13, 0x0);
            }
            break;

        default:
            break;
    }
}

void phy_link_process(void)
{
    uint16_t phy_reg;

    if(LocalTime - TaskExecutionTime > 20)
    {
        ch390_write_phy(CH390_PHY_PAGE_SEL, 0);
        phy_reg = ch390_read_phy(16);
        if(phy_reg == 0) return;

        phy_link_up = ch390_get_link_status();
        if(phy_link_up)
        {
            phy_check_link_vaild();
        }
        else
        {
            phy_reg = ch390_read_phy(CH390_PHY_BMCR);
            if(phy_reg & (1<<12)) //auto-negotiation is enabled
            {
                phy_check_pn(300);
                phy_accelerate_link();
            }
            else if((phy_reg & (1<<13)) == 0) // Force 10M
            {
                phy_check_pn(200);
            }
        }
        TaskExecutionTime = LocalTime;
    }

}


/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *
 * @return  none
 */
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr)
{
	DMA_InitTypeDef DMA_InitStructure={0};

	RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );

	DMA_DeInit(DMA_CHx);

	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
	DMA_InitStructure.DMA_MemoryBaseAddr = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA_CHx, &DMA_InitStructure );
}

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the SPI1 DMA Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address..
 *
 * @return  none
 */
void DMA_Rx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr)
{
	DMA_InitTypeDef DMA_InitStructure={0};

	RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );

	DMA_DeInit(DMA_CHx);

	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
	DMA_InitStructure.DMA_MemoryBaseAddr = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA_CHx, &DMA_InitStructure );
}

/**
 * @brief  CH390TaskInit 
 * @param  mac_addr
 * @retval None
 */
void CH390TaskInit(uint8_t *mac_addr)
{
    ch390_gpio_init();
    ch390_spi_init();

    DMA_Tx_Init( DMA1_Channel3, (u32)&SPI1->DATAR);
	DMA_Rx_Init( DMA1_Channel2, (u32)&SPI1->DATAR);
	// DMA_Cmd( DMA1_Channel3, ENABLE );
	// DMA_Cmd( DMA1_Channel2, ENABLE );
	// SPI_I2S_DMACmd( SPI1, SPI_I2S_DMAReq_Tx, ENABLE );
	// SPI_I2S_DMACmd( SPI1, SPI_I2S_DMAReq_Rx, ENABLE );

    ch390_hardware_reset();
    // Don't read/write CH390 registers within 10ms after power on
    Delay_Ms(10);
    ch390_get_mac(mac_addr);
    ch390_default_config();
}

/**
 * @brief  CH390Task 
 * @param  None
 * @retval None
 */
void CH390Task(void)
{
    if (ch390_get_int_pin())
    {
        ch390_int_handler();
    }
    phy_link_process();

    if(ch390_read_reg(CH390_NSR) & NSR_RXRDY)
    {
        WCHNET_NetInput( NETIF1_INDEX );                     /* Ethernet data input */
    }
}

/**
 * @brief  CH390RxDataDeal 
 * @param  len
 *         pBuff
 * @retval None
 */
uint32_t CH390TxData(uint16_t len, uint32_t *pBuff )
{
    struct list *q;

    // Wait until last transmit complete
    for (q = (struct list *)pBuff; q != NULL; q = q->next)
    {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        ch390_write_mem(q->payload, q->len);
    }

    // // Wait until last transmit complete
    while(ch390_read_reg(CH390_TCR) & TCR_TXREQ);
    // Set current packet length
    ch390_write_reg(CH390_TXPLL, ((struct list *)pBuff)->tot_len & 0xff);
    ch390_write_reg(CH390_TXPLH, (((struct list *)pBuff)->tot_len >> 8) & 0xff);
    // Issue transmit request
    ch390_send_request();
    return ETH_SUCCESS; 
}