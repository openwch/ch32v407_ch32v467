/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/05/15
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 SPI interface operation flash peripheral routine:
 Master:SPI1_SCK(PA5)、SPI1_MISO(PA6)、SPI1_MOSI(PA7).
 This example demonstrates SPI operation Winbond W25Qxx SPIFLASH with FATFS.
 pins:
    CS  -- PA2
    DO  -- PA6(SPI1_MISO)
    WP   -- 3.3V
    DI   -- PA7(SPI1_MOSI)
    CLK  -- PA5(SPI1_SCK)
    HOLD -- 3.3V

*/

#include "debug.h"
#include "string.h"
#include "SPI.h"
#include "ff.h"

FATFS fs;
FIL fnew;
UINT fnum;
FRESULT res_sd;
BYTE work[FF_MAX_SS];
u8 WriteBuf[]={"Hello WCH!"};
u8 ReadSDBuf[11]={0};
MKFS_PARM opt = {0};



/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
  
    u16 Flash_Model;
    opt.fmt = FM_FAT;
    opt.n_fat = 1;
    opt.align = 0;
    opt.au_size = 0;
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    SPI_Flash_Init();

    Flash_Model = SPI_Flash_ReadID();

    switch(Flash_Model)
    {
        case W25Q80:
            printf("W25Q80 OK!\r\n");

            break;

        case W25Q16:
            printf("W25Q16 OK!\r\n");

            break;

        case W25Q32:
            printf("W25Q32 OK!\r\n");

            break;

        case W25Q64:
            printf("W25Q64 OK!\r\n");

            break;

        case W25Q128:
            printf("W25Q128 OK!\r\n");

            break;

        default:
            printf("Fail!\r\n");

            break;
    }
    Delay_Ms(500);
    printf("Start Erase W25Qxx....\r\n");
    SPI_Flash_Erase_Sector(0);
    printf("W25Qxx Erase Finished!\r\n");
    Delay_Ms(500);
	res_sd=f_mount(&fs, "0:", 1);//SD mount
	if(res_sd==FR_OK)
	{
	    printf("Disk mounted successfully\r\n");
	}
	if(res_sd == FR_NO_FILESYSTEM)
	{
	    printf("Disk formatting\r\n");
	    res_sd=f_mkfs("0:", &opt, work, sizeof(work));
	    if (res_sd == FR_OK)
	    {
	        printf("Disk formatted successfully\r\n");
	        res_sd = f_mount(&fs, "0:", 1);
	        if (res_sd == FR_OK)
	        {
	            printf("Disk mounted successfully\r\n");
	        }
	        else
	        {
	            printf("Disk mounting failed\r\n");
	        }
	    }
	    else {
	        printf("Disk formatting failed!(%d)\r\n", res_sd);
	    }
	}
    res_sd= f_open(&fnew,(const char*)"0:/testWCH.txt",FA_CREATE_ALWAYS|FA_WRITE);
      if(res_sd!=FR_OK)
      {
          printf("Create file error\r\n");
      }
      else
      {
          printf("Create file successfully\r\n");
      }
      printf("Writing......................\r\n");
      res_sd= f_write(&fnew,WriteBuf,11,&fnum);
      for(int j=0;j<sizeof(WriteBuf);j++)
      {
          printf("WriteBuf[%d]=%d\r\n",j,WriteBuf[j]);
      }
      if(res_sd!=FR_OK)
      {
          printf("Write error\r\n");
      }
      f_close(&fnew);
      res_sd= f_open(&fnew,(const char*)"0:/testWCH.txt",FA_OPEN_EXISTING|FA_READ);
      if(res_sd!=FR_OK)
      {
          printf("Open file error\r\n");
      }
      printf("Reading......................\r\n");
      res_sd= f_read(&fnew,ReadSDBuf,11,(UINT*)&fnum);
      if(res_sd!=FR_OK)
      {
          printf("Read error\r\n");
      }
      for(int i=0;i<sizeof(ReadSDBuf);i++)
      {
          printf("ReadBuf[%d]=%d\r\n",i,ReadSDBuf[i]) ;
      }
    while(1)
        ;
}
