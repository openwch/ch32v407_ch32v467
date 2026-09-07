/********************************** (C) COPYRIGHT *******************************
* File Name          : uvcd.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Uvc device class define.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __UVCD_H_
#define __UVCD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_define.h"
#include "class/uvc/uvc.h"

#ifdef USB_UVCD_CLASS_CLASS_EN

/* @struct */
typedef struct
{
    uint8_t vc_itf_num;
    uint8_t vs_itf_num;

    usb_endp_t vs_ep;

    uint8_t vs_alt_setting;
    uint8_t power_mode;
    uint8_t stream_error_code;

    uvc_info_t info;
} uvcd_interface_t;

#endif // USB_UVCD_CLASS_CLASS_EN

#ifdef __cplusplus
}
#endif

#endif
