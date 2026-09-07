/********************************** (C) COPYRIGHT *******************************
* File Name          : hidh.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Hid class host driver.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __HIDH_H_
#define __HIDH_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_driver.h"

#ifdef USB_HIDH_CLASS_CLASS_EN

/* @define */
#define HIDH_KEY_BYTE(num)          ((num) >> 3)
#define HIDH_KEY_BIT(num)           (1U << ((num) & 0x07))

#define HIDH_KEY_BITMAP_BITS        256
#define HIDH_KEY_BITMAP_SIZE        (HIDH_KEY_BITMAP_BITS / 8)

/* @struct */
typedef struct
{
    usb_bool_t changed;
    uint8_t buttons;
    uint16_t x;
    uint16_t y;
    int8_t wheel;
} hidh_mouse_t;

typedef struct
{
    usb_bool_t changed;
    uint8_t led_status;
    uint8_t key_bitmap[HIDH_KEY_BITMAP_SIZE];
} hidh_keyboard_t;

/* @extern */
extern hidh_mouse_t hidh_mouse;
extern hidh_keyboard_t hidh_keyboard;
extern const usbh_driver_t hidh_driver;

/* @function declaration */
void hidh_drv_init(void);
void hidh_drv_task(void);

#endif // USB_HIDH_CLASS_CLASS_EN

#ifdef __cplusplus
}
#endif

#endif
