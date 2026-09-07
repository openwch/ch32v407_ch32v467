/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/08/10
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


#include "debug.h"
#include "string.h"
#include "PSRAM.h"
#include <stdlib.h>

#define PSRAM_ADDR      0X80000000
#define BUFFER_SIZE     1024
__attribute__((aligned(1))) uint32_t TxBuffer[BUFFER_SIZE];
__attribute__((aligned(1))) uint32_t RxBuffer[BUFFER_SIZE];

/*********************************************************************
 * @fn      PSRAM_INIT
 *
 * @brief   Initializes PSRAM
 *
 * @return  none
 */
void PSRAM_INIT(void)
{
    PSRAMInitTypeDef PSRAMInitStruct = {0};
    PSRAMTimingInitTypeDef PSRAMTimingStruct= {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_PSRAM, ENABLE);
    
    PSRAMDeInit();

    PSRAMTimingStruct.PSRAM_trc=0x14;
    PSRAMTimingStruct.PSRAM_tcph=0xC;
    PSRAMTimingStruct.PSRAM_txlpd=0x7;
    PSRAMInitStruct.PSRAM_cfifo=PSRAM_CFIFO_BTWWRRD;
    #if(PSRAM_Memory==Memory32Mb)
    PSRAMInitStruct.PSRAM_cap_cfg=PSRAM_CAPCFG_32M;
    #elif (PSRAM_Memory==Memory64Mb) 
     PSRAMInitStruct.PSRAM_cap_cfg=PSRAM_CAPCFG_64M;
    #endif
    PSRAMInitStruct.PSRAM_exti_lpmd=PSRAM_EXIT_LPMD;
    PSRAMInitStruct.PSRAMTimingStruct=&PSRAMTimingStruct;
    PSRAMInit(&PSRAMInitStruct); 
    Delay_Ms(1);
	//Set the Read Latency and Write Latency
    SetWrLatency(MR4_Write_300M,Latency_300M);
    SetRdLatency(MR0_Read_300M,Latency_300M,Read_Variable);    
}

/*********************************************************************
 * @fn      PSRAM_300MHz_HSE
 *
 * @brief   Sets PSRAM clock frequency to 300MHz.
 *
 * @return  none
 */
static void PSRAM_300MHz_HSE(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0 , FLASH_Temp = 0;
    RCC->CTLR |= (uint32_t)0x00000001;
    RCC->CFGR0 &= (uint32_t)0x80FF0000;
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x00)
    {
    }

    RCC->CTLR &= (uint32_t)0xFEA6FFFF;
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;

    RCC->CFGR0 &= (uint32_t)0xFFC0FFFF;
    RCC->CFGR2 &= (uint32_t)0xFFFFEFFF;
    RCC->CFGR2 &= (uint32_t)0x03F11800;

    RCC->CFGR0 |= (uint32_t)(0x20000000);
    RCC->CFGR3 = 0x00000000;
    RCC->INTR = 0x008F0000;   
    RCC->CTLR |= ((uint32_t)RCC_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
      HSEStatus = RCC->CTLR & RCC_HSERDY;
      StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CTLR & RCC_HSERDY) != RESET)
    {
      HSEStatus = (uint32_t)0x01;
    }
    else
    {
      HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
      /* HCLK = SYSCLK/2 */
      RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
      /* PCLK2 = HCLK */
      RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
      /* PCLK1 = HCLK */
      RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV1;

      /* PLL configuration: HCLK = (HSE * 12) / 2 = 150 MHz */
      RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLMULL12);

      /* Enable PLL */
      RCC->CTLR |= RCC_PLLON;
      /* Wait till PLL is ready */
      while((RCC->CTLR & RCC_PLLRDY) == 0)
      {
      }

      RCC->CFGR2 |= (uint32_t)(RCC_SYSPLLSRC_PLL | RCC_SYSPLLDIV1);
      /* Wait till PLL clock is used as SYSPLLSRC clock source */
      while ((RCC->CFGR2 & (uint32_t)RCC_SYSPLLSRC) != (uint32_t)RCC_SYSPLLSRC_PLL)
      {
      }     

      /* Select FLASH clock frequency*/
      FLASH->KEYR = ((uint32_t)0x45670123);
      FLASH->KEYR = ((uint32_t)0xCDEF89AB);
      FLASH->MODEKEYR = ((uint32_t)0x45670123);
      FLASH->MODEKEYR = ((uint32_t)0xCDEF89AB);

      FLASH_Temp = FLASH->CTLR ;
      FLASH_Temp &= ~((uint32_t)FLASH_CTLR_SCKMOD);
      FLASH_Temp |= FLASH_CTLR_SCKMOD_Div2;
      FLASH->CTLR = FLASH_Temp;
      FLASH->CTLR |= ((uint32_t)0x00008080);

      /* Select PLL as system clock source */
      RCC->CFGR2 |= (uint32_t)RCC_SYSPLL_GATE;
      RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
      /* Wait till PLL is used as system clock source */
      while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
      {
      }
    }
    else
    {
      /*
       * If HSE fails to start-up, the application will have wrong clock
       * configuration. User can add here some code to deal with this error
       */
    }
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
    u32 i=0;
    u32 times=0;
    SystemCoreClockUpdate();
    PSRAM_300MHz_HSE();
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);	
    Delay_Init();
    printf("SystemClk:%d\r\n", SystemClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("PSRAM TEST\r\n");
	PWR_VDD18LevelConfig(PWR_VDD18_Level1);
    PSRAM_INIT();
    while(1){
    for(int j=0; j<BUFFER_SIZE; j++)
    {
      TxBuffer[j] = random();
      RxBuffer[j]=0;
	  }
    /*     1.  CPU write  CPU read */
    printf("CPU write data\r\n");
    for(i=0; i<BUFFER_SIZE; i++)
    {
      *(u32*)(PSRAM_ADDR + 4*i) = TxBuffer[i];
	  }
    printf("CPU Read data\r\n");
    for(i=0; i<BUFFER_SIZE; i++)
    {
      RxBuffer[i]= *(vu32*)(PSRAM_ADDR + 4*i);  
    } 
    if(memcmp(TxBuffer, RxBuffer, BUFFER_SIZE*4))
    {
      printf("  fail\n\r");
      for(int k=0;k<BUFFER_SIZE;k++)
      {
        if(TxBuffer[k]!=RxBuffer[k])
        {
          printf(" TxBuffer[%d]=%x\n\r",k,TxBuffer[k]);
          printf(" RxBuffer[%d]=%x\n\r",k,RxBuffer[k]);
        }
      }
      break;
    }
    else
    {
      times++;
      printf("sucess times:%d\r\n",times);  
    }

    /*     2. DMA write  DMA read */
    for(int j=0; j<BUFFER_SIZE; j++)
    {
      TxBuffer[j] = random();
      RxBuffer[j]=0;
	  }
    printf("DMA write data\r\n");
    PSRAMDMAWrite(TxBuffer,PSRAM_ADDR,BUFFER_SIZE,PSRAM_MEMORYSIZE_32bit,DMA_BRST_NUM2,DMA_PAUSE_TIM0 );
    printf("DMA Read data\r\n");
    PSRAMDMARead(RxBuffer,PSRAM_ADDR,BUFFER_SIZE,PSRAM_MEMORYSIZE_32bit,DMA_BRST_NUM2,DMA_PAUSE_TIM0 );
    if(memcmp(TxBuffer, RxBuffer, BUFFER_SIZE*4))
    {
      printf("  fail\n\r");
      for(int k=0;k<BUFFER_SIZE;k++)
      {
        if(TxBuffer[k]!=RxBuffer[k])
        {
          printf(" TxBuffer[%d]=%x\n\r",k,TxBuffer[k]);
          printf(" RxBuffer[%d]=%x\n\r",k,RxBuffer[k]);
        }
      }
      break;
    }
    else
    {
      times++;
      printf("sucess times:%d\r\n",times);  
    }

    /*     3.DMA write CPU read */
    for(int j=0; j<BUFFER_SIZE; j++)
    {
      TxBuffer[j] = random();
      RxBuffer[j]=0;
	  }
    printf("DMA write data\r\n");
    PSRAMDMAWrite(TxBuffer,PSRAM_ADDR,BUFFER_SIZE,PSRAM_MEMORYSIZE_32bit,DMA_BRST_NUM2,DMA_PAUSE_TIM0 );
    printf("CPU Read data\r\n");
    for(i=0; i<BUFFER_SIZE; i++)
    {
      RxBuffer[i]= *(vu32*)(PSRAM_ADDR + 4*i);  
    } 
    if(memcmp(TxBuffer, RxBuffer, BUFFER_SIZE*4))
    {
      printf("  fail\n\r");
      for(int k=0;k<BUFFER_SIZE;k++)
      {
        if(TxBuffer[k]!=RxBuffer[k])
        {
          printf(" TxBuffer[%d]=%x\n\r",k,TxBuffer[k]);
          printf(" RxBuffer[%d]=%x\n\r",k,RxBuffer[k]);
        }
      }
      break;
    }
    else
    {
      times++;
      printf("sucess times:%d\r\n",times);  
    }

    /*     4.CPU write DMA read */
    for(int j=0; j<BUFFER_SIZE; j++)
    {
      TxBuffer[j] = random();
      RxBuffer[j]=0;
	  }
    printf("CPU write data\r\n");
    for(i=0; i<BUFFER_SIZE; i++)
    {
      *(u32*)(PSRAM_ADDR + 4*i) = TxBuffer[i];
	  }
    printf("DMA Read data\r\n");
    PSRAMDMARead(RxBuffer,PSRAM_ADDR,BUFFER_SIZE,PSRAM_MEMORYSIZE_32bit,DMA_BRST_NUM2,DMA_PAUSE_TIM0 );
    if(memcmp(TxBuffer, RxBuffer, BUFFER_SIZE*4))
    {
      printf("  fail\n\r");
      for(int k=0;k<BUFFER_SIZE;k++)
      {
        if(TxBuffer[k]!=RxBuffer[k])
        {
          printf(" TxBuffer[%d]=%x\n\r",k,TxBuffer[k]);
          printf(" RxBuffer[%d]=%x\n\r",k,RxBuffer[k]);
        }
      }
      break;
    }
    else
    {
      times++;
      printf("sucess times:%d\r\n",times);  
    }
	}
  while(1);
}
