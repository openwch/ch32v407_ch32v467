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
  LTDC static display: JPEG logo.jpg is embedded as a C array,
  decoded by libjpeg , written to sram_frame (internal SRAM
  framebuffer) and shown top-left at native size.  
*/

#include <setjmp.h>
#include <stdlib.h>
#include "debug.h"
#include "jpeglib.h"

extern const unsigned char JPEG_IMAGE[];
extern const unsigned int  JPEG_IMAGE_SIZE;

/* Global define */
#define HBP               (43)
#define VBP               (12)
#define HSW               (4)
#define VSW               (4)
#define HFP               (8)
#define VFP               (8)

#define LCD_Width         (480)
#define LCD_Height        (272)

#define layer1_w          (320)
#define layer1_h          (240)

#define layer1_color_mode LTDC_Pixelformat_RGB565
#define layer1_pixel_size 2

__attribute__((aligned(32))) uint16_t sram_frame[layer1_w * layer1_h];
/*********************************************************************
 * @fn      GPIO_Config2
 *
 * @brief   Configure the GPIO pins for LTDC
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

    // 背光灯使能 PE9
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_SetBits(GPIOE, GPIO_Pin_9);
}

/*********************************************************************
 * @fn      LCD_Config
 *
 * @brief   LCD configuration (LTDC drives internal SRAM framebuffer).
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

    LTDC_Layer_1InitStruct.LTDC_CFBStartAdress = (uint32_t)sram_frame;

    LTDC_Layer_1InitStruct.LTDC_CFBLineLength = ((layer1_w * layer1_pixel_size) + 31);

    LTDC_Layer_1InitStruct.LTDC_CFBPitch = (layer1_w * layer1_pixel_size);

    LTDC_Layer_1InitStruct.LTDC_CFBLineNumber = layer1_h;

    LTDC_LayerInit(LTDC_Layer, &LTDC_Layer_1InitStruct);

    LTDC_LayerCmd(LTDC_Layer, ENABLE);

    LTDC_ReloadConfig(LTDC_IMReload);

    LTDC_Cmd(ENABLE);
}

/*********************************************************************
 * @fn      ETH_PLL_CLK_Enalbe
 *
 * @brief   Enable the ETH PLL that feeds the LTDC pixel clock.
 *
 * @return  none
 */
void ETH_PLL_CLK_Enalbe(void)
{
    RCC->CTLR &= ~(1 << 22);
    RCC->CTLR |= (1 << 22);
    while ((RCC->CTLR & (uint32_t)RCC_ETHPLLRDY) != (uint32_t)RCC_ETHPLLRDY)
    {
    }
}

/*********************************************************************
 * @fn      TIM4_init
 *
 * @brief   Initializes Tim
 */
void TIM4_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_TIM4, ENABLE);
    TIM_Cmd(TIM4, DISABLE);

    TIM_TimeBaseStructure.TIM_Period        = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler     = (SystemCoreClock / 100000) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);

    TIM_Cmd(TIM4, ENABLE);
}

/* Custom libjpeg error manager: the standard manager plus a jmp_buf, so a
   fatal decode error can jump back to the caller of the decoder. */
typedef struct
{
    struct jpeg_error_mgr pub;
    jmp_buf               setjmp_buffer;
} my_error_mgr_t;
typedef my_error_mgr_t* my_error_ptr_t;

/*********************************************************************
 * @fn      my_output_message
 *
 * @brief   Discard libjpeg messages so they do not pollute the output.
 *
 * @param   cinfo - pointer to the libjpeg common structure (unused)
 *
 * @return  none
 */
METHODDEF(void)
my_output_message(j_common_ptr cinfo)
{
    (void)cinfo;
}

/*********************************************************************
 * @fn      my_error_exit
 *
 * @brief   libjpeg fatal error handler, reports the error and returns control
 *          to the caller of jpeg_decode_to_sram().
 *
 * @param   cinfo - pointer to the libjpeg common structure
 *
 * @return  none (control returns to the setjmp point via longjmp)
 */
METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr_t myerr = (my_error_ptr_t)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

/*********************************************************************
 * @fn      rgb_565
 *
 * @brief   Pack three 8-bit colour components into one RGB565 pixel value.
 *
 * @param   r - red component   (8 bits, the high 5 bits are kept)
 *           g - green component (8 bits, the high 6 bits are kept)
 *           b - blue component  (8 bits, the high 5 bits are kept)
 *
 * @return  16-bit RGB565 pixel value
 */
static inline uint16_t rgb_565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

/*********************************************************************
 * @fn      jpeg_decode_to_sram
 *
 * @brief   Decode the embedded JPEG image and write it into sram_frame
 *          (internal SRAM framebuffer) as RGB565 pixels.
 *
 * @return  0 on success, 1 if the decoder reports a fatal error
 */
static int jpeg_decode_to_sram(void)
{
    struct jpeg_decompress_struct cinfo;

    my_error_mgr_t jerr;
    uint32_t       w, h, x, y;
    JSAMPARRAY     row_buf;

    cinfo.err               = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit     = my_error_exit;
    jerr.pub.output_message = my_output_message;

    if (setjmp(jerr.setjmp_buffer))
    {
        printf("JPG decode ERROR\r\n");
        jpeg_destroy_decompress(&cinfo);
        return 1;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (unsigned char*)JPEG_IMAGE, JPEG_IMAGE_SIZE); /* pic src */
    jpeg_read_header(&cinfo, TRUE);

    w = cinfo.image_width;
    h = cinfo.image_height;

    cinfo.out_color_space     = JCS_RGB;
    cinfo.dct_method          = JDCT_IFAST;
    cinfo.do_fancy_upsampling = FALSE; /* 4:2:0 */
    jpeg_start_decompress(&cinfo);

#define JPG_STRIP 32 /* scanlines decoded per strip */

    row_buf = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo,
                                         JPOOL_IMAGE,
                                         cinfo.output_width * (size_t)cinfo.output_components,
                                         JPG_STRIP);

    while (cinfo.output_scanline < cinfo.output_height)
    {

        unsigned int got = jpeg_read_scanlines(&cinfo, row_buf, JPG_STRIP);

        for (unsigned int i = 0; i < got; i++)
        {

            y = (unsigned int)(cinfo.output_scanline - got) + i;
            if (y >= h)
                continue;

            const unsigned char* row = row_buf[i];
            uint16_t*            d   = &sram_frame[(size_t)y * layer1_w];

            for (x = 0; x + 3 < w; x += 4)
            {
                const unsigned char* rp = &row[x * 3];
                d[x + 0]                = rgb_565(rp[0], rp[1], rp[2]);
                d[x + 1]                = rgb_565(rp[3], rp[4], rp[5]);
                d[x + 2]                = rgb_565(rp[6], rp[7], rp[8]);
                d[x + 3]                = rgb_565(rp[9], rp[10], rp[11]);
            }

            for (; x < w; x++)
            {
                const unsigned char* rp = &row[x * 3];
                d[x]                    = rgb_565(rp[0], rp[1], rp[2]);
            }
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return 0;
}

/*********************************************************************
 * @fn      main
 *
 * @brief   main
 */
int main(void)
{
    SystemCoreClockUpdate();
    USART_Printf_Init(2000000);
    Delay_Init();
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("JPEG static display \r\n");

    // wait for download not remove!!!
    Delay_Ms(1000);
    Delay_Ms(1000);

    // LTDC CLK Settings
    RCC->CFGR2 &= ~(0x7F);
    ETH_PLL_CLK_Enalbe();
    RCC->CFGR2 |= (1 << 6);  // use eth_pll
    RCC->CFGR2 |= (50);      // div 40

    GPIO_Config2();
    TIM4_init();
    LCD_Config();

    TIM_SetCounter(TIM4, 0);
    uint32_t t_start = TIM4->CNT;

    jpeg_decode_to_sram();

    uint32_t dt_us = (TIM4->CNT - t_start) * 10u;
    printf("decode time: %lu us = %lums %luus\r\n",
           (unsigned long)dt_us,
           (unsigned long)(dt_us / 1000u),
           (unsigned long)(dt_us % 1000u));

    while (1)
    {
        Delay_Ms(1000);
    }
}