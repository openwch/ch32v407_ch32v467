/********************************** (C) COPYRIGHT *******************************
* File Name          : cdcd.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : CDC device class define.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __CDCD_H_
#define __CDCD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "class/cdc/cdc.h"
#include "class/cdc/cdc_rndis.h"

#ifdef USB_CDCD_CLASS_CLASS_EN

/* @struct */
typedef struct
{
    uint8_t itf_num;

    uint8_t ep_notif;
    uint8_t ep_in;
    uint8_t ep_out;

    uint8_t line_state;
    cdc_line_coding_t line_coding;
} cdcd_acm_itf_t;

typedef struct
{
    uint8_t ctrl_itf;
    uint8_t data_itf;

    uint8_t ep_notif;
    uint8_t ep_in;
    uint8_t ep_out;

    uint8_t itf_alt;
} cdcd_ecm_itf_t;

typedef struct
{
    uint8_t ctrl_itf;
    uint8_t data_itf;

    uint8_t ep_notif;
    uint8_t ep_in;
    uint8_t ep_out;

    uint8_t itf_alt;
} cdcd_ncm_itf_t;

typedef struct
{
    uint8_t ctrl_itf;
    uint8_t data_itf;

    uint8_t ep_notif;
    uint8_t ep_in;
    uint8_t ep_out;

    usb_bool_t init_status;
    rndis_message_stage_e msg_stage;
    uint32_t packet_filter;
} cdcd_rndis_itf_t;

#endif // USB_CDCD_CLASS_CLASS_EN

#ifdef __cplusplus
}
#endif

#endif
