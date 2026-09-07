/********************************** (C) COPYRIGHT *******************************
* File Name          : board.c
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Board for ch32v407.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <stdio.h>
#include <string.h>

#include "ch32v4x7.h"
#include "debug.h"

#include "usbhs1_dcp.h"
#include "usbhs2_dcp.h"
#include "usbhs1_hcp.h"
#include "usbhs2_hcp.h"

#include "usb_driver.h"

/* @define */
#define ROM_CFG_USERADR_ID              0x1FFFF7E8

/* @enum */
typedef enum
{
    USB_MODE_DEVICE,
    USB_MODE_HOST,
} usb_mode_e;

/* @global */
static usb_mode_e usbhs1_mode = USB_MODE_DEVICE;
static usb_mode_e usbhs2_mode = USB_MODE_DEVICE;

/**
 * @brief Board initialization.
 *
 */
void board_init(void)
{
    SystemCoreClockUpdate();
    Delay_Init();

    USART_Printf_Init(115200);

    printf("======== Startup Information ========\r\n");
    printf("Compiled Time: %s %s\n", __DATE__, __TIME__);
    printf("RISC-V Compiler: %s\r\n", __VERSION__);
    printf("System Clock: %ld\r\n", SystemCoreClock);
    printf("=====================================\r\n\r\n");
}

/**
 * @brief Board delay in milliseconds.
 *
 * @param ms Milliseconds to delay.
 */
void board_delay_ms(uint32_t ms)
{
    Delay_Ms(ms);
}

/**
 * @brief Board get mac address.
 *
 * @param mac mac address buffer.
 */
void board_get_mac(uint8_t *mac)
{
    if (mac == NULL) return;

    uint8_t *mac_addr = (uint8_t *)(ROM_CFG_USERADR_ID + 5);

    for (uint8_t i = 0; i < 6; i++)
    {
        *mac++ = *mac_addr--;
    }
}

#ifdef USB_DEVICE_DRIVER_EN

/**
 * @brief Board USB device initialization.
 *
 * @param index USB device index.
 * @return usbd_handle_t* USB device handle.
 */
usbd_handle_t *board_usbd_init(uint8_t index)
{
    switch (index)
    {
    case 0:
        if (usbhs1_dch_init() == USB_RST_OK)
        {
            usbhs1_mode = USB_MODE_DEVICE;
            return &usbhs1d_handle;
        }
        break;

    case 1:
        if (usbhs2_dch_init() == USB_RST_OK)
        {
            usbhs2_mode = USB_MODE_DEVICE;
            return &usbhs2d_handle;
        }
        break;
    }

    return NULL;
}

#endif

#ifdef USB_HOST_DRIVER_EN

/**
 * @brief Board USB host initialization.
 *
 * @param index USB host index.
 * @return usbh_handle_t* USB host handle.
 */
usbh_handle_t *board_usbh_init(uint8_t index)
{
    switch (index)
    {
    case 0:
        if (usbhs1_hch_init() == USB_RST_OK)
        {
            usbhs1_mode = USB_MODE_HOST;
            return &usbhs1h_handle;
        }
        break;

    case 1:
        if (usbhs2_hch_init() == USB_RST_OK)
        {
            usbhs2_mode = USB_MODE_HOST;
            return &usbhs2h_handle;
        }
        break;
    }

    return NULL;
}

#endif

__attribute__((interrupt("WCH-Interrupt-fast"))) void USBHS1_IRQHandler(void)
{
    if (usbhs1_mode == USB_MODE_DEVICE)
    {
#ifdef USB_DEVICE_DRIVER_EN
        usbhs1_dch_interrupt();
#endif
    }
    else
    {
#ifdef USB_HOST_DRIVER_EN
        usbhs1_hch_interrupt();
#endif
    }
}

__attribute__((interrupt("WCH-Interrupt-fast"))) void USBHS2_IRQHandler(void)
{
    if (usbhs2_mode == USB_MODE_DEVICE)
    {
#ifdef USB_DEVICE_DRIVER_EN
        usbhs2_dch_interrupt();
#endif
    }
    else
    {
#ifdef USB_HOST_DRIVER_EN
        usbhs2_hch_interrupt();
#endif
    }
}
