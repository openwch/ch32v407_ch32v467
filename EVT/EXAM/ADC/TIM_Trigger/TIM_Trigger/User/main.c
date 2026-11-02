/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.2
* Date               : 2026/10/18
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *TIM trigger ADC conversion routine:
 * ADC channel 2 (PA2) - injection group channel or injection group channel
 * injection group channel  TIM1 CH4  TIM trigger In this mode, an ADC conversion is triggered by an event on
 * TIM1 CH4 TIM trigger, and a JEOC interrupt is generated afterthe conversion is completed.
 * rule group channel  TIM1 CH3  TIM trigger In this mode, an ADC conversion is triggered by an event on
 * TIM1 CH3 TIM trigger, and a EOC interrupt is generated afterthe conversion is completed.
 *
 */

#include "debug.h"
/* ADC Conversion Mode Definition*/
#define ADC_trig_Mode     0
#define ADC_inje_Mode     1

/* ADC Conversion Mode Selection*/
#define ADC_MODE_Conversion   ADC_trig_Mode
// #define ADC_MODE_Conversion   ADC_inje_Mode

/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    #if  (ADC_MODE_Conversion == ADC_inje_Mode)
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
#else
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;
#endif
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    #if  (ADC_MODE_Conversion == ADC_inje_Mode)
 	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_CyclesMode7);
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);  
	ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
#else
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_CyclesMode7);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
#endif
    NVIC_EnableIRQ(ADC1_2_IRQn);
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
#else
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
#endif
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}


/*********************************************************************
 * @fn      TIM1_PWM_Init
 *
 * @brief   Initializes PWM.
 *
 * @return  none
 */

void TIM1_PWM_Init(u16 arr, u16 psc, u16 ccp)
{
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

   RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
#else
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
#endif
 
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
  TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
#else
  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);
#endif
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

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
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    ADC_Function_Init();
    TIM1_PWM_Init(480-1,1000-1,2400);
    while(1);
}



void ADC1_2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      ADC1_2_IRQHandler
 *
 * @brief   ADC1_2 Interrupt Service Function.
 *
 * @return  none
 */
void ADC1_2_IRQHandler()
{
	u16 ADC_val;

#if  (ADC_MODE_Conversion == ADC_inje_Mode)
    if(ADC_GetITStatus( ADC1, ADC_IT_JEOC)){
        ADC_val = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        #if 0
        printf("ADC Extline trigger inje conversion...\r\n");
        printf("JADC %04d\r\n", ADC_val);
       #endif
    }

    ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
#else
    if(ADC_GetITStatus( ADC1, ADC_IT_EOC)){
        ADC_val = ADC_GetConversionValue(ADC1);
         #if 0
        printf("ADC Extline trigger conversion...\r\n");
        printf("ADC %04d\r\n", ADC_val);
        #endif
    }

    ADC_ClearITPendingBit( ADC1, ADC_IT_EOC);
#endif
}





