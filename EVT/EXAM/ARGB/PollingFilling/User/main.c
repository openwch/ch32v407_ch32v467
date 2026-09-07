/********************************** (C) COPYRIGHT  *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/08/27
* Description        : Main program body
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note WS2812B LED output routine:
*/
#include "debug.h"

/* Global define */
#define ARGB_MS2HBTICK(ms) ((uint32_t)((float)SystemCoreClock / 1000.0f * (float)ms))
#define ARGB_US2HBTICK(us) ((uint32_t)((float)SystemCoreClock / 1000000.0f * (float)us))
#define ARGB_NS2HBTICK(ns) ((uint32_t)((float)SystemCoreClock / 1000000000.0f * (float)ns))

#define ARGB_WS2812_NUM 1

#define ARGB_USE_IT 0

//                          G     R     B
uint8_t R_ARGB_Buffer[3] = {0x00, 0x22, 0x00};
uint8_t G_ARGB_Buffer[3] = {0x11, 0x00, 0x00};
uint8_t B_ARGB_Buffer[3] = {0x00, 0x00, 0x33};

uint8_t (*ptr)[3] = &R_ARGB_Buffer;

void ARGB_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      ARGB_WS2812B_Init
 *
 * @brief   The function ARGB_WS2812B_Init initializes the ARGB peripheral for WS2812B LED control.
 *
 * @param   ARGB_DMA_Buffer - Pointer to the ARGB data buffer
 *          buffer_size - Size of the ARGB data buffer
 *
 * @return  none
 */
void ARGB_WS2812B_Init(size_t buffer_size)
{
    // Enable the clock for ARGB peripheral
    RCC_HBPeriphClockCmd(RCC_HBPeriph_ARGB, ENABLE);

    // Initialize ARGB configuration structure
    ARGB_InitTypeDef ARGB_InitStruct = {0};

    // Configure ARGB parameters
    ARGB_InitStruct.ARGB_Length     = buffer_size - 1;         // Set buffer length (minus 1 for zero-based indexing)
    ARGB_InitStruct.ARGB_T1H        = ARGB_US2HBTICK(0.9f);    // Set high time for '1' bit (0.9��s)
    ARGB_InitStruct.ARGB_T0H        = ARGB_US2HBTICK(0.3f);    // Set high time for '0' bit (0.3��s)
    ARGB_InitStruct.ARGB_DataPeriod = ARGB_US2HBTICK(1.45f);   // Set data bit period (1.45��s)
    ARGB_InitStruct.ARGB_RSTPeriod  = ARGB_US2HBTICK(80.0f);   // Set reset period (80��s)
    ARGB_InitStruct.ARGB_Mode       = ARGB_Mode_SendRSTFirst;  // Set mode to send reset first
    ARGB_InitStruct.ARGB_Endian     = ARGB_Endian_MSB;         // Set most significant bit first
    ARGB_Init(&ARGB_InitStruct);                               // Apply ARGB configuration

#if ARGB_USE_IT
    NVIC_EnableIRQ(ARGB_IRQn);
#endif
}

uint32_t temp[3] = {0};

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    printf("WS2812 Test\r\n");
    ARGB_GPIO_Init();

    // Initialize WS2812B LED strip with data buffer size
    ARGB_WS2812B_Init(ARGB_WS2812_NUM * 3);

    // Enable ARGB TX mode, entering idle state
    ARGB_Cmd(ENABLE);

    while (1)
    {
        if (ARGB_GetCurrState() == ARGB_Idle)
        {
#if !ARGB_USE_IT
            for (int i = 0; i < ARGB_WS2812_NUM; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    while(!ARGB_GetFlagStatus(ARGB_IT_TXE));
                    ARGB_SendData((*ptr)[j]);
                }
            }
            ptr = (ptr == &R_ARGB_Buffer ? &G_ARGB_Buffer : ptr == &G_ARGB_Buffer ? &B_ARGB_Buffer : &R_ARGB_Buffer);
            Delay_Ms(1000);
#else
            Delay_Ms(1000);
            // Trigger the ARGB transmission
            ARGB_SendData((*ptr)[0]);
            ARGB_ITConfig(ARGB_IT_TXE, ENABLE);
#endif
        }
    }
}

#if ARGB_USE_IT
void __attribute__((interrupt("WCH-Interrupt-fast"))) ARGB_IRQHandler(void)
{
    if (ARGB_GetITStatus(ARGB_IT_TXE))
    {
        uint16_t CuCNT = ARGB_GetByteCNT();
        if (CuCNT + 1  < sizeof(*ptr))
        {
            ARGB_SendData((*ptr)[(CuCNT + 1) % sizeof(*ptr)]);
        }
        else
        {
            ARGB_ITConfig(ARGB_IT_TXE, DISABLE);
            ptr = (ptr == &R_ARGB_Buffer ? &G_ARGB_Buffer : ptr == &G_ARGB_Buffer ? &B_ARGB_Buffer : &R_ARGB_Buffer);
        }
    }
}
#endif