/********************************** (C) COPYRIGHT *******************************
* File Name          : usbhs1_hcp.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb high speed host controller 1 port for ch32v407.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBHS1_HCP_H_
#define __USBHS1_HCP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include "usb_driver.h"

#ifdef USB_HOST_DRIVER_EN

/* @extern */
extern usbh_handle_t usbhs1h_handle;

/* @function declaration */
usb_rst_e usbhs1_hch_init(void);
void usbhs1_hch_interrupt(void);

#endif // USB_HOST_DRIVER_EN

#ifdef __cplusplus
}
#endif

#endif
