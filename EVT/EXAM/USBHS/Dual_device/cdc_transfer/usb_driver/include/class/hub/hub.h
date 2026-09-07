/********************************** (C) COPYRIGHT *******************************
* File Name          : hub.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Hub class define.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __HUB_H_
#define __HUB_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

/* @define */
#define USB_DESC_HUB            0x29

/* @struct */
typedef struct
{
    union
    {
        uint16_t hub_status;

        struct
        {
            uint16_t local_power : 1;
            uint16_t over_current : 1;
            uint16_t reserved : 14;
        } hub_status_bits;
    };

    union
    {
        uint16_t hub_change;

        struct
        {
            uint16_t local_power : 1;
            uint16_t over_current : 1;
            uint16_t reserved : 14;
        } hub_change_bits;
    };
} hub_status_t;

/* Start single-byte alignment */
#pragma pack(1)

typedef struct
{
    uint8_t bDescLength;
    uint8_t bDescriptorType;
    uint8_t bNbrPorts;
    uint16_t wHubCharacteristics;
    uint8_t bPwrOn2PwrGood;
    uint8_t bHubContrCurrent;
    uint8_t DeviceRemovable;
    uint8_t PortPwrCtrlMask;
} desc_hub_t;

/* End single-byte alignment */
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif
