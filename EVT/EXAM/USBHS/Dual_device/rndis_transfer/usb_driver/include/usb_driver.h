/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_driver.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb driver headfile.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USB_DRIVER_H_
#define __USB_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
// include common headfile.
#include "usb_define.h"
#include "usb_config.h"

// include usb device driver headfile.
#ifdef USB_DEVICE_DRIVER_EN
#include "device/usbd_driver_public.h"
#endif

// include usb host driver headfile.
#ifdef USB_HOST_DRIVER_EN
#include "host/usbh_driver_public.h"
#endif

// include usb class driver headfile.
#include "class/hub/hub.h"
#include "class/hid/hid.h"
#include "class/cdc/cdc.h"
#include "class/cdc/cdc_rndis.h"
#include "class/msc/msc.h"
#include "class/uac/uac.h"
#include "class/uvc/uvc.h"

// include usb device class driver headfile.
#include "class/hid/hidd.h"
#include "class/cdc/cdcd.h"
#include "class/msc/mscd.h"
#include "class/uac/uacd.h"
#include "class/uvc/uvcd.h"

// include usb host class driver headfile.
#include "class/hub/hubh.h"
#include "class/hid/hidh.h"
#include "class/cdc/cdch.h"
#include "class/uac/uach.h"

/* @define */
#ifndef USB_LOG_TAG
#define USB_LOG_TAG                     "?"
#endif

#ifdef USB_DRIVER_LOG_INFO_EN
#define USB_LOGI(format, ...)           USB_LOG_OUTPUT("[" USB_LOG_TAG "][I]: " format "\r\n", ##__VA_ARGS__)
#else
#define USB_LOGI(format, ...)
#endif

#ifdef USB_DRIVER_LOG_WARNING_EN
#define USB_LOGW(format, ...)           USB_LOG_OUTPUT("[" USB_LOG_TAG "][W]: " format "\r\n", ##__VA_ARGS__)
#else
#define USB_LOGW(format, ...)
#endif

#ifdef USB_DRIVER_LOG_ERROR_EN
#define USB_LOGE(format, ...)           USB_LOG_OUTPUT("[" USB_LOG_TAG "][E]: " format "\r\n", ##__VA_ARGS__)
#else
#define USB_LOGE(format, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif
