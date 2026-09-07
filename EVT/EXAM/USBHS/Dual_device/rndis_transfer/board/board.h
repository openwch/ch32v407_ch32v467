/********************************** (C) COPYRIGHT *******************************
* File Name          : board.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Board for ch32v407.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __BOARD_H_
#define __BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include "usb_driver.h"

/* @define */
#define BOARD_USBDC_COUNT       2
#define BOARD_USBHC_COUNT       2

/* @function declaration */
void board_init(void);
void board_delay_ms(uint32_t ms);
void board_get_mac(uint8_t *mac);

#ifdef USB_DEVICE_DRIVER_EN
usbd_handle_t *board_usbd_init(uint8_t index);
#endif

#ifdef USB_HOST_DRIVER_EN
usbh_handle_t *board_usbh_init(uint8_t index);
#endif

#ifdef __cplusplus
}
#endif

#endif
