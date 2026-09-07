/********************************** (C) COPYRIGHT *******************************
* File Name          : hubh.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Hub class host driver.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __HUBH_H_
#define __HUBH_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_driver.h"

#ifdef USB_HUBH_CLASS_CLASS_EN

/* @extern */
extern const usbh_driver_t hubh_driver;

/* @function declaration */
void hubh_drv_init(void);
void hubh_drv_task(void);

#endif // USB_HUBH_CLASS_CLASS_EN

#ifdef __cplusplus
}
#endif

#endif
