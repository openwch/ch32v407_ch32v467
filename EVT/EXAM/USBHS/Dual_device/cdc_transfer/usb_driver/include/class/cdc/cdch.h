/********************************** (C) COPYRIGHT *******************************
* File Name          : cdch.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Cdc class host driver.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __CDCH_H_
#define __CDCH_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_driver.h"

#ifdef USB_CDCH_CLASS_CLASS_EN

/* @typedef */
typedef struct cdch_device cdch_device_t;

/* @callback */
typedef void (*cdch_open_cb)(cdch_device_t *cdc, usb_bool_t rst, const cdc_line_coding_t *line_coding);

/* @extern */
extern const usbh_driver_t cdch_ctrl_driver;
extern const usbh_driver_t cdch_data_driver;

/* @function declaration */

/**
 * @brief Initializes the CDC class host driver.
 *
 */
void cdch_drv_init(void);

/**
 * @brief Performs the CDC class host driver task.
 *
 */
void cdch_drv_task(void);

/**
 * @brief Gets the count of CDC devices.
 *
 * @return uint32_t The count of CDC devices.
 */
uint32_t cdch_get_device_count(void);

/**
 * @brief Gets the CDC device at the specified index.
 *
 * @param index The index of the CDC device.
 * @return cdch_device_t* The pointer to the CDC device, or NULL if not found.
 */
cdch_device_t *cdch_get_device(uint32_t index);

/**
 * @brief Opens the specified CDC device with the given line coding and callback.
 *
 * @param cdc The pointer to the CDC device.
 * @param line_coding The pointer to the line coding configuration.
 * @param cb The callback function to be called upon completion of the open operation.
 * @return usb_bool_t USB_TRUE if the operation was successful, USB_FALSE otherwise.
 */
usb_bool_t cdch_open(cdch_device_t *cdc, const cdc_line_coding_t *line_coding, cdch_open_cb cb);

/**
 * @brief Closes the specified CDC device.
 *
 * @param cdc The pointer to the CDC device.
 * @return usb_bool_t USB_TRUE if the operation was successful, USB_FALSE otherwise.
 */
usb_bool_t cdch_close(cdch_device_t *cdc);

/**
 * @brief Checks if the specified CDC device is open.
 *
 * @param cdc The pointer to the CDC device.
 * @return usb_bool_t USB_TRUE if the device is open, USB_FALSE otherwise.
 */
usb_bool_t cdch_is_open(cdch_device_t *cdc);

/**
 * @brief Reads data from the specified CDC device.
 *
 * @param cdc The pointer to the CDC device.
 * @param data The buffer to store the read data.
 * @param length The length of data to read.
 * @return usb_bool_t USB_TRUE if the read operation was successful, USB_FALSE otherwise.
 */
usb_bool_t cdch_read(cdch_device_t *cdc, void *data, size_t length);

/**
 * @brief Writes data to the specified CDC device.
 *
 * @param cdc The pointer to the CDC device.
 * @param data The data to write.
 * @param length The length of data to write.
 * @return usb_bool_t USB_TRUE if the write operation was successful, USB_FALSE otherwise.
 */
usb_bool_t cdch_write(cdch_device_t *cdc, const void *data, size_t length);

#endif // USB_CDCH_CLASS_CLASS_EN

#ifdef __cplusplus
}
#endif

#endif
