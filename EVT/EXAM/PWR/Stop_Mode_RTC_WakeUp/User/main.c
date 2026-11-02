/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/10/14
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 * RTC timed wake up stop mode routine:
 * This example demonstrates periodic wake-up from Stop mode using the RTC timer. 
 * After setting the alarm time, the system enters Stop mode. Approximately 5 seconds later, 
 * the RTC automatically wakes up the system, which resumes execution, and the cycle continues. 
 * The internal LSI oscillator is used as the RTC clock source.
 * For the small package model of the chip, there are some pins that have not been 
 * led out compared to the largest package, or some pins that have been packaged but 
 * not used. These pins need to be set as pull-down inputs to reduce current consumption. 
 */

#include "debug.h"

/* Global define */

/* definition of execute with WFI or WFE */
#define  Entry_WFI   0
#define  Entry_WFE   1
// #define  Entry_Method  Entry_WFI
#define  Entry_Method  Entry_WFE

/* Global Variable */

/*********************************************************************
 * @fn      RTC_Alarm_WakeUp
 *
 * @brief  Initializes RTC alarm wake-up function.
 *
 * @return  none
 */
void RTC_Alarm_WakeUp(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_PWR | RCC_PB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_ClearITPendingBit(RTC_IT_ALR);
    RTC_ClearITPendingBit(RTC_IT_SEC);
    BKP_DeInit();
    /* Enable LSI oscillator */
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    /* Enable the RTC clock */
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForLastTask();
    RTC_WaitForSynchro();
    /* Enable alarm interrupt */
    RTC_ITConfig( RTC_IT_ALR, ENABLE );
    RTC_WaitForLastTask();
    RTC_EnterConfigMode();
    /* Set the RTC prescaler value */
    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();
    RTC_SetCounter(0);
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();
#if(Entry_Method == Entry_WFI)
    /* Enable the RTC alarm interrupt */
    NVIC_SetPriority(RTCAlarm_IRQn, 1);
    NVIC_EnableIRQ(RTCAlarm_IRQn);
#endif
    /* Configure EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
#if(Entry_Method == Entry_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/*********************************************************************
 * @fn      All_GPIO_IPD_Init
 *
 * @brief   To reduce power consumption, unused I/O is configured in pull-down input mode.
 *
 * @return  none
 */
void All_GPIO_IPD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC |
                          RCC_PB2Periph_GPIOD | RCC_PB2Periph_GPIOE | RCC_PB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    GPIO_Init( GPIOC, &GPIO_InitStructure );
    GPIO_Init( GPIOD, &GPIO_InitStructure );
    GPIO_Init( GPIOE, &GPIO_InitStructure );
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
    Delay_Ms(500);
    All_GPIO_IPD_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%u\r\n", SystemClock);
    printf("SystemCoreClk:%u\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Ms(500);
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_PWR, ENABLE);
    /* If PSRAM is not used,Turn off the VDD18 power supply to reduce the power consumption current */
    PWR_VDD18Cmd(DISABLE);
    RTC_Alarm_WakeUp();
    
    while(1)
    {
        printf(">>> enter stop mode\r\n");
        Delay_Ms(10);
        /* Set the RTC alarm value */
        RTC_SetAlarm(RTC_GetCounter()+5);
        RTC_WaitForLastTask();
        #if(Entry_Method == Entry_WFI)
           PWR_EnterSTOPMode_RAM_LV(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        #else
            PWR_EnterSTOPMode_RAM_LV(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
            SystemInit();
        #endif
        printf("wake up\r\n\r\n");
        Delay_Ms(2000);
    }
}

#if(Entry_Method == Entry_WFI)
/*********************************************************************
 * @fn      RTCAlarm_IRQHandler
 *
 * @brief   This function handles RTCAlarm Handler.
 *
 * @return  none
 */
void RTCAlarm_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void RTCAlarm_IRQHandler(void)
{
    SystemInit();
    if(RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET)
    {
        /* Clears EXTI line and alarm clock flag */
        EXTI_ClearITPendingBit(EXTI_Line17);
        RTC_ClearITPendingBit(RTC_IT_ALR); 
        RTC_WaitForLastTask();
    }
}
#endif