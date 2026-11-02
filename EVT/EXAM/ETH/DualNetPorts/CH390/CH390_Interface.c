/********************************** (C) COPYRIGHT *****************************
 * File Name          : CH390_Interface.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2026/09/24
 * Description        : CH390 interface
 ******************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 ******************************************************************************/
#include "debug.h"
#include "CH390.h"
#include "CH390_Interface.h"

/*
 * This file defines CH390 operation interface (Parallel
 * or SPI). User should implement one of them.
 *
 * Interface can be selected by macros in CH390.h:
 * CH390_INTERFACE_SPI
 * CH390_INTERFACE_8_BIT
 * CH390_INTERFACE_16_BIT
 */

#ifdef CH390_INTERFACE_SPI
#define CH390_INT_PIN GPIO_Pin_0  // PA0
#define CH390_RST_PIN GPIO_Pin_1  // PA1
#define CH390_CS_PIN GPIO_Pin_4   // PA4
#define CH390_SCK_PIN GPIO_Pin_5  // PA5
#define CH390_MISO_PIN GPIO_Pin_6 // PA6
#define CH390_MOSI_PIN GPIO_Pin_7 // PA7

void ch390_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);

    // Output pins
    GPIO_SetBits(GPIOA, CH390_CS_PIN | CH390_RST_PIN);
    GPIO_InitStructure.GPIO_Pin = CH390_CS_PIN | CH390_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Input pins
    GPIO_InitStructure.GPIO_Pin = CH390_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void ch390_spi_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = CH390_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CH390_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CH390_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

#define ch390_cs(value) GPIO_WriteBit(GPIOA, CH390_CS_PIN, (BitAction)value)
#define ch390_rst(value) GPIO_WriteBit(GPIOA, CH390_RST_PIN, (BitAction)value)

/**
 * @name ch390_get_int_pin
 * @brief Get ch390 interrupt signal
 */
uint16_t ch390_get_int_pin(void)
{
    return GPIOA->INDR & CH390_INT_PIN;
}

/**
 * @name ch390_delay_us
 * @brief Delay a few microseconds
 */
void ch390_delay_us(uint32_t time)
{
    Delay_Us(time);
}

/**
 * @name ch390_hardware_reset
 * @brief Hardware reset CH390 by pull down RSTB pin
 */
void ch390_hardware_reset()
{
    ch390_rst(0);
    ch390_delay_us(10);
    ch390_rst(1);
    ch390_delay_us(10000);
}

static uint8_t ch390_spi_exchange_byte(uint8_t byte)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0)
        ;
    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, byte);
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SPI_I2S_FLAG_RXNE)
        ;
    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

#define ch390_spi_dummy_read() ch390_spi_exchange_byte(0)

#else /* CH390_INTERFACE_SPI */
/* CH390_INTERFACE_8_BIT or CH390_INTERFACE_16_BIT*/

#define USE_FSMC 1

#if USE_FSMC

#define CH390_INT_PIN GPIO_Pin_0  // PA0
#define CH390_RST_PIN GPIO_Pin_1  // PA1
#define CH390_CS_PIN GPIO_Pin_4   // PA4
#define CH390_CMD_PIN GPIO_Pin_11 // PD11
#define CH390_RDB_PIN GPIO_Pin_4  // PD4
#define CH390_WRB_PIN GPIO_Pin_5  // PD5
#define CH390_WOL_PIN GPIO_Pin_8  // PA8

#ifdef CH390_INTERFACE_8_BIT
#define CH390_INDEX *(__IO uint8_t *)(0x60000000)
#define CH390_DATA  *(__IO uint8_t *)(0x60010000)
#else
#define CH390_INDEX *(__IO uint16_t *)(0x60000000)
#define CH390_DATA  *(__IO uint16_t *)(0x60020000)
#endif

/*
 * FSMC_D0 - PD14
 * FSMC_D1 - PD15
 * FSMC_D2 - PD0
 * FSMC_D3 - PD1
 * FSMC_D4 - PE7
 * FSMC_D5 - PE8
 * FSMC_D6 - PE9
 * FSMC_D7 - PE10
 * 
 * FSMC_D8 - PE11
 * FSMC_D9 - PE12
 * FSMC_D10 - PE13
 * FSMC_D11 - PE14
 * FSMC_D12 - PE15
 * FSMC_D13 - PD8
 * FSMC_D14 - PD9
 * FSMC_D15 - PD10
 */

void ch390_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

#ifdef CH390_INTERFACE_8_BIT
    // FSMC D0-D3: PD14 PD15 PD0 PD1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // FSMC D4-D7: PE7-PE10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

#else /* CH390_INTERFACE_8_BIT */

    // FSMC D0-D3: PD14 PD15 PD0 PD1 | FSMC D13-D15: PD8-PD10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1 |
                                  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // FSMC D4-D7: PE7-PE10 | FSMC D8-D12: PE11-PE15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                    GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
#endif

    // RD WR: PD4 PD5
    GPIO_InitStructure.GPIO_Pin = CH390_RDB_PIN | CH390_WRB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* CMD: FSMC_A16 - PD11 */
    GPIO_InitStructure.GPIO_Pin = CH390_CMD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // Alternate function output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure); // Initialize

    // CS: PA4  RST: PA1
    GPIO_SetBits(GPIOA, CH390_CS_PIN | CH390_RST_PIN);
    GPIO_InitStructure.GPIO_Pin = CH390_CS_PIN | CH390_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Input pins
    GPIO_InitStructure.GPIO_Pin = CH390_INT_PIN | CH390_WOL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // FSMC
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef readWriteTiming;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    readWriteTiming.FSMC_AddressSetupTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 10;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 5;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
#ifdef CH390_INTERFACE_8_BIT
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
#else
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
#endif
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

#define ch390_cs(value) GPIO_WriteBit(GPIOA, CH390_CS_PIN, (BitAction)value)
#define ch390_rst(value) GPIO_WriteBit(GPIOA, CH390_RST_PIN, (BitAction)value)

#else /* USE_FSMC */

#define CH390_INT_PIN GPIO_Pin_0  // PA0
#define CH390_RST_PIN GPIO_Pin_1  // PA1
#define CH390_CS_PIN GPIO_Pin_4   // PA4
#define CH390_CMD_PIN GPIO_Pin_5  // PA5
#define CH390_RDB_PIN GPIO_Pin_11 // PA11
#define CH390_WRB_PIN GPIO_Pin_12 // PA12
#define CH390_WOL_PIN GPIO_Pin_8  // PA8

void ch390_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Output pins
    GPIO_SetBits(GPIOA, CH390_RDB_PIN | CH390_WRB_PIN | CH390_CMD_PIN |
                            CH390_CS_PIN | CH390_RST_PIN);
    GPIO_InitStructure.GPIO_Pin = CH390_RDB_PIN | CH390_WRB_PIN | CH390_CMD_PIN |
                                  CH390_CS_PIN | CH390_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Input pins
    GPIO_InitStructure.GPIO_Pin = CH390_INT_PIN | CH390_WOL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // PB3 PB4 remap
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

#ifdef CH390_INTERFACE_16_BIT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
    // 0 - 7 Output
    GPIO_InitStructure.GPIO_Pin = 0x00FF;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 8 - 15 Input
    GPIO_InitStructure.GPIO_Pin = 0xFF00;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
}

#define ch390_cs(value) GPIO_WriteBit(GPIOA, CH390_CS_PIN, (BitAction)(value))
#define ch390_rst(value) GPIO_WriteBit(GPIOA, CH390_RST_PIN, (BitAction)(value))
#define ch390_cmd(value) GPIO_WriteBit(GPIOA, CH390_CMD_PIN, (BitAction)(value))
#define ch390_rdb(value) GPIO_WriteBit(GPIOA, CH390_RDB_PIN, (BitAction)(value))
#define ch390_wrb(value) GPIO_WriteBit(GPIOA, CH390_WRB_PIN, (BitAction)(value))

#ifdef CH390_INTERFACE_16_BIT
#define gpiob_input()            \
    {                            \
        GPIOB->CRL = 0x44444444; \
        GPIOB->CRH = 0x44444444; \
    }
#define gpiob_output()           \
    {                            \
        GPIOB->CRL = 0x33333333; \
        GPIOB->CRH = 0x33333333; \
    }
#else
#define gpiob_input()            \
    {                            \
        GPIOB->CRL = 0x44444444; \
    }
#define gpiob_output()           \
    {                            \
        GPIOB->CRL = 0x33333333; \
    }
#endif

#define ch390_ll_read() GPIO_ReadInputData(GPIOB)
#define ch390_ll_ioinput() gpiob_input()
#define ch390_ll_write(data)     \
    {                            \
        gpiob_output();          \
        GPIO_Write(GPIOB, data); \
    }

#endif /* USE_FSMC */

void ch390_interrupt_init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    // WOL interrupt
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @name ch390_get_int_pin
 * @brief Get ch390 interrupt signal
 */
uint16_t ch390_get_int_pin(void)
{
    return GPIOA->IDR & CH390_INT_PIN;
}

/**
 * @name ch390_delay_us
 * @brief Delay a few microseconds
 */
void ch390_delay_us(uint32_t time)
{
    Delay_Us(time);
}

/**
 * @name ch390_hardware_reset
 * @brief Hardware reset CH390 by pull down RSTB pin
 */
void ch390_hardware_reset()
{
    ch390_rst(0);
    ch390_delay_us(10);
    ch390_rst(1);
    ch390_delay_us(10000);
}

#endif /* CH390_INTERFACE_SPI */

/***************************************************************************
 * The following are the basic CH390 read/write operation functions.
 */

#ifdef CH390_INTERFACE_SPI
/**
 * @name ch390_read_reg
 * @brief Read register
 * @param reg - Target register address
 * @return Register value
 */
uint8_t ch390_read_reg(uint8_t reg)
{
    uint8_t spi_data;
    ch390_cs(0); // CS = 0
    ch390_spi_exchange_byte(reg | OPC_REG_R);
    spi_data = ch390_spi_dummy_read();
    ch390_cs(1); // CS = 1
    return spi_data;
}

/**
 * @name ch390_write_reg
 * @brief Write register
 * @param reg - Target register address
 * @param value - Value to be written
 */
void ch390_write_reg(uint8_t reg, uint8_t value)
{
    ch390_cs(0); // CS = 0
    ch390_spi_exchange_byte(reg | OPC_REG_W);
    ch390_spi_exchange_byte(value);
    ch390_cs(1); // CS = 1
    return;
}
/**
 * @name ch390_read_mem
 * @brief Read data from RX SRAM
 * @param data - Data buffer
 * @param length - Length to read
 */
void ch390_read_mem(uint8_t *data, int length)
{
    ch390_cs(0);
    while((SPI1->STATR & SPI_I2S_FLAG_TXE) == 0);
    SPI1->DATAR = OPC_MEM_READ;
    while((SPI1->STATR & SPI_I2S_FLAG_RXNE) == 0);
    (void) (SPI1->DATAR);

    DMA1_Channel3->CNTR = length;
    DMA1_Channel3->MADDR = (uint32_t)data;
    DMA1_Channel2->CNTR = length;
    DMA1_Channel2->MADDR = (uint32_t)data;
    SPI1->CTLR2 |= SPI_I2S_DMAReq_Tx;
    SPI1->CTLR2 |= SPI_I2S_DMAReq_Rx;
    DMA1_Channel3->CFGR |= DMA_CFGR1_EN;
    DMA1_Channel2->CFGR |= DMA_CFGR1_EN;
    while ((!DMA_GetFlagStatus(DMA1_FLAG_TC2)) || (!DMA_GetFlagStatus(DMA1_FLAG_TC3)));
    while((SPI1->STATR) & SPI_I2S_FLAG_BSY);
    ch390_cs(1);
    DMA_ClearFlag(DMA1_FLAG_TC3);
    DMA_ClearFlag(DMA1_FLAG_TC2);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_Cmd(DMA1_Channel2, DISABLE);
}
/**
 * @name ch390_write_mem
 * @brief Write data to TX SRAM
 * @param data - Data buffer
 * @param length - Length to write
 */
void ch390_write_mem(uint8_t *data, int length)
{
    ch390_cs(0);
    while((SPI1->STATR & SPI_I2S_FLAG_TXE) == 0);
    SPI1->DATAR = OPC_MEM_WRITE;
    while((SPI1->STATR & SPI_I2S_FLAG_RXNE) == 0);
    (void) (SPI1->DATAR);
    SPI1->CTLR2 |= SPI_I2S_DMAReq_Tx;
    DMA1_Channel3->CNTR = length;
    DMA1_Channel3->MADDR = (uint32_t)data;
    DMA1_Channel3->CFGR |= DMA_CFGR1_EN;
    while ((!DMA_GetFlagStatus(DMA1_FLAG_TC3)));
    while((SPI1->STATR) & SPI_I2S_FLAG_BSY);
    ch390_cs(1);

    DMA_ClearFlag(DMA1_FLAG_TC3);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    (void)SPI1->DATAR;
}

#else /* CH390_INTERFACE_SPI */

#if USE_FSMC

/**
 * @name ch390_read_reg
 * @brief Read register
 * @param reg - Target register address
 * @return Register value
 */
uint8_t ch390_read_reg(uint8_t reg)
{
    uint8_t data;

    ch390_cs(0);
    CH390_INDEX = reg;
    data = CH390_DATA;
    ch390_cs(1);
    return data;
}

/**
 * @name ch390_write_reg
 * @brief Write register
 * @param reg - Target register address
 * @param value - Value to be written
 */
void ch390_write_reg(uint8_t reg, uint8_t value)
{
    ch390_cs(0);
    CH390_INDEX = reg;
    CH390_DATA = value;
    ch390_cs(1);
}

#ifdef CH390_INTERFACE_8_BIT
/**
 * @name ch390_read_mem
 * @brief Read data from RX SRAM
 * @param data - Data buffer
 * @param length - Length to read
 */
void ch390_read_mem(uint8_t *data, int length)
{
    int i;
    ch390_cs(0);
    CH390_INDEX = CH390_MRCMD;
    ch390_cs(1);

    ch390_cs(0);
    for (i = 0; i < length; i++)
    {
        data[i] = CH390_DATA;
    }
    ch390_cs(1);
}
/**
 * @name ch390_write_mem
 * @brief Write data to TX SRAM
 * @param data - Data buffer
 * @param length - Length to write
 */
void ch390_write_mem(uint8_t *data, int length)
{
    int i;
    ch390_cs(0);
    CH390_INDEX = CH390_MWCMD;
    ch390_cs(1);

    ch390_cs(0);
    for (i = 0; i < length; i++)
    {
        CH390_DATA = data[i];
    }
    ch390_cs(1);
}

#endif // CH390_INTERFACE_8_BIT
#ifdef CH390_INTERFACE_16_BIT
void ch390_read_mem(uint8_t *data, int length)
{
    int i;
    uint16_t *data_w = (uint16_t *)data;
    length = (length + 1) / 2;

    ch390_cs(0);
    CH390_INDEX = CH390_MRCMD;
    ch390_cs(1);

    ch390_cs(0);
    for (i = 0; i < length; i++)
    {
        data_w[i] = CH390_DATA;
    }
    ch390_cs(1);
}
void ch390_write_mem(uint8_t *data, int length)
{
    int i;
    uint16_t *data_w = (uint16_t *)data;
    length = (length + 1) / 2;

    ch390_cs(0);
    CH390_INDEX = CH390_MWCMD;
    ch390_cs(1);

    ch390_cs(0);
    for (i = 0; i < length; i++)
    {
        CH390_DATA = data_w[i];
    }
    ch390_cs(1);
}
#endif // CH390_INTERFACE_16_BIT

#else /* USE_FSMC */

/**
 * @name ch390_read_reg
 * @brief Read register
 * @param reg - Target register address
 * @return Register value
 */
uint8_t ch390_read_reg(uint8_t reg)
{
    uint8_t data;
    // Set index
    ch390_cs(0);
    ch390_cmd(0);
    ch390_wrb(0);
    ch390_ll_write(reg);
    ch390_wrb(1);
    ch390_cmd(1);

    // Read data
    ch390_ll_ioinput();
    ch390_rdb(0);
    data = (uint8_t)(ch390_ll_read());
    ch390_rdb(1);
    ch390_cs(1);
    return data;
}

/**
 * @name ch390_write_reg
 * @brief Write register
 * @param reg - Target register address
 * @param value - Value to be written
 */
void ch390_write_reg(uint8_t reg, uint8_t value)
{
    // Set index
    ch390_cs(0);
    ch390_cmd(0);
    ch390_wrb(0);
    ch390_ll_write(reg);
    ch390_wrb(1);
    ch390_cmd(1);

    // Write data
    ch390_wrb(0);
    ch390_ll_write(value);
    ch390_wrb(1);
    ch390_cs(1);
}

void set_index(uint16_t reg)
{
    ch390_cs(0);
    ch390_cmd(0);
    ch390_wrb(0);
    ch390_ll_write(reg);
    ch390_wrb(1);
    ch390_cmd(1);
    ch390_cs(1);
}

#ifdef CH390_INTERFACE_8_BIT
/**
 * @name ch390_read_mem
 * @brief Read data from RX SRAM
 * @param data - Data buffer
 * @param length - Length to read
 */
void ch390_read_mem(uint8_t *data, int length)
{
    int i;
    set_index(CH390_MRCMD);
    ch390_ll_ioinput();
    ch390_cs(0);
    for (i = 0; i < length; i++)
    {
        ch390_rdb(0);
        data[i] = ch390_ll_read();
        ch390_rdb(1);
    }
    ch390_cs(1);
}
/**
 * @name ch390_write_mem
 * @brief Write data to TX SRAM
 * @param data - Data buffer
 * @param length - Length to write
 */
void ch390_write_mem(uint8_t *data, int length)
{
    int i;
    set_index(CH390_MWCMD);
    ch390_cs(0);
    for (i = 0; i < length; i++)
    {
        ch390_wrb(0);
        ch390_ll_write(data[i]);
        ch390_wrb(1);
    }
    ch390_cs(1);
}

#endif // CH390_INTERFACE_8_BIT
#ifdef CH390_INTERFACE_16_BIT
void ch390_read_mem(uint8_t *data, int length)
{
    int i;
    uint16_t *data_w = (uint16_t *)data;
    length = (length + 1) / 2;

    set_index(CH390_MRCMD);
    ch390_ll_ioinput();
    ch390_cs(0);
    for (i = 0; i < length; i++)
    {
        ch390_rdb(0);
        data_w[i] = ch390_ll_read();
        ch390_rdb(1);
    }
    ch390_cs(1);
}
void ch390_write_mem(uint8_t *data, int length)
{
    int i;
    uint16_t *data_w = (uint16_t *)data;
    length = (length + 1) / 2;

    set_index(CH390_MWCMD);
    ch390_cs(0);
    for (i = 0; i < length; i++)
    {
        ch390_wrb(0);
        ch390_ll_write(data_w[i]);
        ch390_wrb(1);
    }
    ch390_cs(1);
}

#endif // CH390_INTERFACE_16_BIT

#endif /* USE_FSMC */

#endif /* CH390_INTERFACE_SPI */
