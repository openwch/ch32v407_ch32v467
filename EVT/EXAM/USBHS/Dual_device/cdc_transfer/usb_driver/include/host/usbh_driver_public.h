/********************************** (C) COPYRIGHT *******************************
* File Name          : usbh_driver_public.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb host driver public headfile.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBH_DRIVER_PUBLIC_H_
#define __USBH_DRIVER_PUBLIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "usb_define.h"
#include "usb_config.h"

#ifdef USB_HOST_DRIVER_EN

/* @struct */
typedef struct usbh_device usbh_device_t;
typedef struct usbh_driver usbh_driver_t;
typedef struct usbh_handle usbh_handle_t;

/* @function declaration */

/**
 * @brief Usb host driver initialization.
 *
 * @note This function should be called before any other USB host driver functions.
 */
void usbh_drv_init(void);

/**
 * @brief Usb host driver core task.
 *
 * @param h Usb host driver handle object.
 * @note This function is executed in the main loop.
 */
void usbh_drv_task(usbh_handle_t *h);

/**
 * @brief Open usb host controller.
 *
 * @param h Usb host driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbh_open(usbh_handle_t *h);

/**
 * @brief Close usb host controller.
 *
 * @param h Usb host driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbh_close(usbh_handle_t *h);

/**
 * @brief Register usb host driver.
 *
 * @param h Usb host driver handle object.
 * @param driver Usb host driver object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbh_register_driver(usbh_handle_t *h, const usbh_driver_t *driver);

/**
 * @brief Unregister usb host driver.
 *
 * @param h Usb host driver handle object.
 * @param driver Usb host driver object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbh_unregister_driver(usbh_handle_t *h, const usbh_driver_t *driver);

/**
 * @brief Get the timestamp of usb host driver.
 *
 * @param h Usb host driver handle object.
 * @return uint32_t Timestamp value in milliseconds.
 */
uint32_t usbh_get_timestamp(usbh_handle_t *h);

#endif // USB_HOST_DRIVER_EN

#ifdef __cplusplus
}
#endif

#endif
