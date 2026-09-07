#include "debug.h"
#include "PSRAM.h"

/*********************************************************************
 * @fn      PSRAM_INIT
 *
 * @brief   Initializes PSRAM
 *
 * @return  none
 */
void PSRAM_INIT (void) {
    PSRAMInitTypeDef PSRAMInitStruct = {0};
    PSRAMTimingInitTypeDef PSRAMTimingStruct = {0};
    RCC_HBPeriphClockCmd (RCC_HBPeriph_PSRAM, ENABLE);

    PSRAMDeInit();

    PSRAMTimingStruct.PSRAM_trc = 0x14;
    PSRAMTimingStruct.PSRAM_tcph = 0xC;
    PSRAMTimingStruct.PSRAM_txlpd = 0x7;
    PSRAMInitStruct.PSRAM_cfifo = PSRAM_CFIFO_BTWWRRD;
    PSRAMInitStruct.PSRAM_cap_cfg = PSRAM_CAPCFG_32M;
    PSRAMInitStruct.PSRAM_exti_lpmd = PSRAM_EXIT_LPMD;
    PSRAMInitStruct.PSRAMTimingStruct = &PSRAMTimingStruct;
    PSRAMInit (&PSRAMInitStruct);
    Delay_Ms (1);
    // Set the Read Latency and Write Latency
    SetWrLatency (MR4_Write_200M, Latency_200M);
    SetRdLatency (MR0_Read_200M, Latency_200M, Read_Variable);
}

/*********************************************************************
 * @fn      PSRAM_200MHz_HSE
 *
 * @brief   Sets PSRAM clock frequency to 200MHz.
 *
 * @return  none
 */
static void PSRAM_200MHz_HSE (void) {
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0, FLASH_Temp = 0;
    RCC->CTLR |= (uint32_t)0x00000001;
    RCC->CFGR0 &= (uint32_t)0x80FF0000;
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x00) {
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
    do {
        HSEStatus = RCC->CTLR & RCC_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CTLR & RCC_HSERDY) != RESET) {
        HSEStatus = (uint32_t)0x01;
    } else {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01) {
        /* HCLK = SYSCLK  */
        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
        /* PCLK2 = HCLK */
        RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
        /* PCLK1 = HCLK */
        RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV1;

        /* PLL configuration: HCLK = (HSE * 8) / 1 = 200 MHz */
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLMULL8);

        /* Enable PLL */
        RCC->CTLR |= RCC_PLLON;
        /* Wait till PLL is ready */
        while ((RCC->CTLR & RCC_PLLRDY) == 0) {
        }

        RCC->CFGR2 |= (uint32_t)(RCC_SYSPLLSRC_PLL | RCC_SYSPLLDIV1);
        /* Wait till PLL clock is used as SYSPLLSRC clock source */
        while ((RCC->CFGR2 & (uint32_t)RCC_SYSPLLSRC) != (uint32_t)RCC_SYSPLLSRC_PLL) {
        }

        /* Select FLASH clock frequency*/
        FLASH->KEYR = ((uint32_t)0x45670123);
        FLASH->KEYR = ((uint32_t)0xCDEF89AB);
        FLASH->MODEKEYR = ((uint32_t)0x45670123);
        FLASH->MODEKEYR = ((uint32_t)0xCDEF89AB);

        FLASH_Temp = FLASH->CTLR;
        FLASH_Temp &= ~((uint32_t)FLASH_CTLR_SCKMOD);
        FLASH_Temp |= FLASH_CTLR_SCKMOD_Div2;
        FLASH->CTLR = FLASH_Temp;
        FLASH->CTLR |= ((uint32_t)0x00008080);

        /* Select PLL as system clock source */
        RCC->CFGR2 |= (uint32_t)RCC_SYSPLL_GATE;
        RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08) {
        }
    } else {
        /*
         * If HSE fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error
         */
    }
}

extern uint32_t _psram_lma_end[];
extern uint32_t _psram_lma[];
extern uint32_t _psram_vma[];

void PSRAM_Prepare() {

    SystemCoreClockUpdate();
    PSRAM_200MHz_HSE();
    SystemCoreClockUpdate();
    Delay_Init();
    PSRAM_INIT();
    USART_Printf_Init(115200);
    uint32_t *rp = _psram_lma, *wp = _psram_vma;
    printf("%#x %#x %#x\n",rp,wp,_psram_lma_end);
    Delay_Ms(10);
    while (rp < _psram_lma_end) {
        *wp = *rp;
        wp ++ ;
        rp ++ ;
    }
}