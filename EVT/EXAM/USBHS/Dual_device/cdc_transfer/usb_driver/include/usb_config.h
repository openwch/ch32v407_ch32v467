/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_config.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb driver config headfile.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USB_CONFIG_H_
#define __USB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdio.h>

#include "usb_define.h"

/* @define */
// usb driver common configuration macro definitions.
#define USB_DEVICE_DRIVER_EN
// #define USB_HOST_DRIVER_EN

// usb class driver configuration macro definitions.
#ifdef USB_DEVICE_DRIVER_EN
#define USB_CDCD_CLASS_CLASS_EN
#define USB_HIDD_CLASS_CLASS_EN
#define USB_HUBD_CLASS_CLASS_EN
#define USB_MSCD_CLASS_CLASS_EN
#define USB_UACD_CLASS_CLASS_EN
#define USB_UVCD_CLASS_CLASS_EN
#endif

#ifdef USB_HOST_DRIVER_EN
#define USB_CDCH_CLASS_CLASS_EN
#define USB_HIDH_CLASS_CLASS_EN
#define USB_HUBH_CLASS_CLASS_EN
#define USB_MSCH_CLASS_CLASS_EN
#define USB_UACH_CLASS_CLASS_EN
#define USB_UVCH_CLASS_CLASS_EN
#endif

// usb driver log configuration macro definitions.
#define USB_DRIVER_LOG_INFO_EN
#define USB_DRIVER_LOG_WARNING_EN
#define USB_DRIVER_LOG_ERROR_EN

// usb driver log output interface macro definitions.
#ifndef USB_LOG_OUTPUT
#define USB_LOG_OUTPUT(format, ...)     printf(format, ##__VA_ARGS__)
#endif

// usb device driver configuration macro definitions.
#define USBD_REQ_CB_MAX_ITEMS           16
#define USBD_ITF_CB_MAX_ITEMS           8

#define USBD_SELF_POWERED               USB_FALSE

// usb host driver configuration macro definitions.
#define USBH_MAX_DEVICES                6
#define USBH_MAX_DRIVERS                6
#define USBH_MAX_ENDPOINTS              32
#define USBH_ENUM_BUFFER_SIZE           1024

#ifdef __cplusplus
}
#endif

#endif
