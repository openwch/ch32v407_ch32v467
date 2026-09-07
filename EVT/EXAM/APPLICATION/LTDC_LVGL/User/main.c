/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/08/03
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
  LTDC + LVGL routine
*/

#include "string.h"
#include "PSRAM.h"
#include "debug.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "demos/lv_demos.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      GPIO_Config
 * 
 * @brief   Configure the GPIO pins
 * 
 * @param   none
 */

void GPIO_Config2(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOD, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOE, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

    // LTDC_CLK PE1
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // LTDC_HSYNC PB4
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_VSYNC PB5
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_DE PE0
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // LTDC_R0 PC0
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_R1 PC1
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_R2 PC2
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_R3 PC3
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_R4 PA1
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_R5 PA2
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_R6 PA3
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_R7 PA7
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_G0 PC4
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_G1 PC5
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_G2 PB0
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_G3 PB1
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_G4 PB2
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_G5 PD8
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // LTDC_G6 PB12
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_G7 PB13
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B0 PB14
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B1 PB15
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B2 PB11
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B3 PB10
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B4 PA8
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_B5 PA13
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_B6 PA14
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_B7 PA15
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_SetBits(GPIOE, GPIO_Pin_9);
}

#define HBP               (80)
#define VBP               (40)
#define HSW               (1)
#define VSW               (3)
#define HFP               (200)
#define VFP               (22)

#define LCD_Width         (800)
#define LCD_Height        (480)

#define layer1_w          (800)
#define layer1_h          (480)

#define layer1_color_mode LTDC_Pixelformat_RGB565

#define layer1_pixel_size 2

#define LAYER_BUF_CNT (5)

__attribute__((aligned(32), section(".psram"))) static const uint16_t LTDC_GRAM1[layer1_w * layer1_h];
__attribute__((aligned(32), section(".psram"))) static const uint16_t LTDC_GRAM2[layer1_w * layer1_h];

const void* const LTDC_layer1 = LTDC_GRAM1;
const void* const LTDC_layer2 = LTDC_GRAM2;

__attribute__((aligned(32))) uint8_t* global_layer = (void*)LTDC_layer1;

static volatile uint32_t buffer_switch = 0;
static volatile uint32_t layer1_cnt    = 0;

__attribute__((aligned(32))) uint16_t layer1_buffer0[layer1_w * LAYER_BUF_CNT];
__attribute__((aligned(32))) uint16_t layer1_buffer1[layer1_w * LAYER_BUF_CNT];

/*********************************************************************
 * @fn      LCD_Config
 * 
 * @brief   LCD configuration.
 *
 * @return  none
 */
static void LCD_Config(void)
{

    LTDC_InitTypeDef       LTDC_InitStruct        = {0};
    LTDC_Layer_InitTypeDef LTDC_Layer_1InitStruct = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_LTDC, ENABLE);

    LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AH;
    LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
    LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;
    LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IIPC;

    LTDC_InitStruct.LTDC_HorizontalSync     = HSW - 1;
    LTDC_InitStruct.LTDC_VerticalSync       = VSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedHBP     = HBP + HSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedVBP     = VBP + VSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveW = LCD_Width + HSW + HBP - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveH = LCD_Height + VSW + VBP - 1;
    LTDC_InitStruct.LTDC_TotalWidth         = LCD_Width + HSW + HBP + HFP - 1;
    LTDC_InitStruct.LTDC_TotalHeigh         = LCD_Height + VSW + VBP + VFP - 1;

    LTDC_InitStruct.LTDC_BackgroundRedValue   = 0xff;
    LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
    LTDC_InitStruct.LTDC_BackgroundBlueValue  = 0;

    LTDC_Init(&LTDC_InitStruct);

    // LTDC_layer1
    LTDC_Layer_1InitStruct.LTDC_HorizontalStart = 0;
    LTDC_Layer_1InitStruct.LTDC_HorizontalStop  = layer1_w;
    LTDC_Layer_1InitStruct.LTDC_VerticalStart   = 0;
    LTDC_Layer_1InitStruct.LTDC_VerticalStop    = layer1_h;

    LTDC_Layer_1InitStruct.LTDC_PixelFormat = layer1_color_mode;

    LTDC_Layer_1InitStruct.LTDC_DefaultColorBlue  = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorGreen = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorRed   = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorAlpha = 0;

    LTDC_Layer_1InitStruct.LTDC_CFBStartAdress = (uint32_t)global_layer;

    LTDC_Layer_1InitStruct.LTDC_CFBLineLength = ((layer1_w * layer1_pixel_size) + 31);

    LTDC_Layer_1InitStruct.LTDC_CFBPitch = (layer1_w * layer1_pixel_size);

    LTDC_Layer_1InitStruct.LTDC_CFBLineNumber = layer1_h;

    LTDC_LayerInit(LTDC_Layer, &LTDC_Layer_1InitStruct);

    LTDC_LayerCmd(LTDC_Layer, ENABLE);

    LTDC_ReloadConfig(LTDC_IMReload);

    LTDC_Cmd(ENABLE);
}

void ltdc_it_config()
{

    LTDC_LIPConfig(layer1_h);
    LTDC_ITConfig(LTDC_IT_LI, ENABLE);
    NVIC_EnableIRQ(LTDC_IRQn);
}

void ETH_PLL_CLK_Enalbe(void)
{

    RCC->CTLR &= ~(1 << 22);
    RCC->CTLR |= (1 << 22);
    while ((RCC->CTLR & (uint32_t)RCC_ETHPLLRDY) != (uint32_t)RCC_ETHPLLRDY)
    {
    }
}

/*********************************************************************
 * @fn      PSRAM_INIT
 *
 * @brief   Initializes PSRAM
 *
 * @return  none
 */
void PSRAM_INIT(void)
{
    PSRAMInitTypeDef       PSRAMInitStruct   = {0};
    PSRAMTimingInitTypeDef PSRAMTimingStruct = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_PSRAM, ENABLE);

    PSRAMDeInit();

    PSRAMTimingStruct.PSRAM_trc       = 0x14;
    PSRAMTimingStruct.PSRAM_tcph      = 25;
    PSRAMTimingStruct.PSRAM_txlpd     = 0x7;
    PSRAMInitStruct.PSRAM_cfifo       = PSRAM_CFIFO_BTWWRRD;
    PSRAMInitStruct.PSRAM_cap_cfg     = PSRAM_CAPCFG_32M;
    PSRAMInitStruct.PSRAM_exti_lpmd   = PSRAM_EXIT_LPMD;
    PSRAMInitStruct.PSRAMTimingStruct = &PSRAMTimingStruct;
    PSRAMInit(&PSRAMInitStruct);
    Delay_Ms(1);
    //Set the Read Latency and Write Latency
    SetWrLatency(MR4_Write_200M, Latency_200M);
    SetRdLatency(MR0_Read_200M, Latency_200M, Read_Variable);
}

/*********************************************************************
 * @fn      TIM4_init
 *
 * @brief   Initializes Tim
 *
 * @return  none
 */
void TIM4_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_TIM4, ENABLE);
    TIM_Cmd(TIM4, DISABLE);

    TIM_TimeBaseStructure.TIM_Period        = 500;
    TIM_TimeBaseStructure.TIM_Prescaler     = SystemCoreClock / 100000;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_EnableIRQ(TIM4_IRQn);
    TIM_Cmd(TIM4, ENABLE);
}


/*********************************************************************
 * @fn      LTDC_DMA_Change
 *
 * @brief   ltdc layer control
 *
 * @return  none
 */
static void LTDC_DMA_Change(uint32_t addr)
{
    LTDC->IER |= (1 << 2);
    LTDC_Layer->CFBAR = (uint32_t)addr;

    LTDC_LIPConfig(layer1_cnt);

    LTDC_ReloadConfig(LTDC_IMReload);
    LTDC->IER &= ~(1 << 2);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   main
 *
 * @return  none
 */
int main(void)
{

    SystemCoreClockUpdate();

    USART_Printf_Init(2000000);
    Delay_Init();
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("PSRAM TEST\r\n");
    PSRAM_INIT();

    Delay_Ms(1000);
    Delay_Ms(1000);
    Delay_Ms(1000);

    printf("LTDC_Display\n");

    // LTDC CLK Settings

    RCC->CFGR2 &= ~(0x7F);

    ETH_PLL_CLK_Enalbe();
    RCC->CFGR2 |= (1 << 6);  // use eth_pll
    RCC->CFGR2 |= (15);

    GPIO_Config2();
    LCD_Config();
    ltdc_it_config();

    TIM4_init();

    lv_init();
    lv_port_disp_init();

    // lv_demo_benchmark();
    lv_demo_widgets();

    while (1)
    {
        lv_task_handler();
    }

    while (1)
        ;
}

void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void LTDC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM4_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    lv_tick_inc(5);
}

void LTDC_IRQHandler(void)
{
    LTDC_ClearITPendingBit(LTDC_IT_LI);

    uint16_t* layer16 = (void*)global_layer;

    if (buffer_switch)
    {
        buffer_switch = 0;

        LTDC_DMA_Change((uint32_t)layer1_buffer1);

        void*    dst = layer1_buffer0;
        void*    src = &(layer16[layer1_cnt * layer1_w]);
        uint32_t len = layer1_w * LAYER_BUF_CNT * layer1_pixel_size;

        PSRAMDMARead((void*)dst, (uint32_t)src, len / 4, PSRAM_DMA_SIZE_32, DMA_BRST_NUM2, DMA_PAUSE_TIM0);
    }
    else
    {
        buffer_switch = 1;

        LTDC_DMA_Change((uint32_t)layer1_buffer0);

        void*    dst = layer1_buffer1;
        void*    src = &(layer16[layer1_cnt * layer1_w]);
        uint32_t len = layer1_w * LAYER_BUF_CNT * layer1_pixel_size;

        PSRAMDMARead(dst, (uint32_t)src, len / 4, PSRAM_DMA_SIZE_32, DMA_BRST_NUM2, DMA_PAUSE_TIM0);
    }

    layer1_cnt += LAYER_BUF_CNT;
    layer1_cnt %= layer1_h;
    if (layer1_cnt == 0)
    {

        void ltdc_flushed();
        ltdc_flushed();
    }
}
