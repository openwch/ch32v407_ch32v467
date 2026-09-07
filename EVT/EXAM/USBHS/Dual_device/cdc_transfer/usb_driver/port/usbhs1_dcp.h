/********************************** (C) COPYRIGHT *******************************
* File Name          : usbhs1_dcp.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb high speed device controller 1 port for ch32v407.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBHS1_DCP_H_
#define __USBHS1_DCP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include "usb_driver.h"

#ifdef USB_DEVICE_DRIVER_EN

/* @define */
#define USBHS1_DCP_MAX_ENDPOINTS        8
#define USBHS1_DCP_ENDP0_SIZE           64

/* @extern */
extern usbd_handle_t usbhs1d_handle;

/* @function declaration */
usb_rst_e usbhs1_dch_init(void);
void usbhs1_dch_interrupt(void);

#endif // USB_DEVICE_DRIVER_EN

#ifdef __cplusplus
}
#endif

#endif
