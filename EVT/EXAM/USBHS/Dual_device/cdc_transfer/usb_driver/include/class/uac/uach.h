/********************************** (C) COPYRIGHT *******************************
* File Name          : uach.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Uac class host driver.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __UACH_H_
#define __UACH_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_driver.h"

#ifdef USB_UACH_CLASS_CLASS_EN

/* @struct */
typedef struct
{
    uint8_t channels;
    uint8_t channel_config;
    uint8_t bit_resolution;
    uint8_t sample_freq_count;
    size_t data_buf_size;
    const uint32_t *sample_freqs;
    const void *handle;
} uach_stream_info_t;

/* @typedef */
typedef struct uach_device uach_device_t;

/* @callback */
typedef void (*uach_stream_cb)(void *buf, size_t length);

/* @extern */
extern const usbh_driver_t uach_driver;

/* @function declaration */

/**
 * @brief Initializes the UAC class host driver.
 *
 */
void uach_drv_init(void);

/**
 * @brief Performs the UAC class host driver task.
 *
 */
void uach_drv_task(void);

/**
 * @brief Gets the count of UAC devices.
 *
 * @return size_t The count of UAC devices.
 */
size_t uach_get_device_count(void);

/**
 * @brief Gets the UAC device at the specified index.
 *
 * @param index The index of the UAC device.
 * @return uach_device_t* The pointer to the UAC device, or NULL if not found.
 */
uach_device_t *uach_get_device(size_t index);

/**
 * @brief Gets the stream information for the specified UAC device and direction.
 *
 * @param uac The pointer to the UAC device.
 * @param dir The direction of the stream (USB_DIR_IN or USB_DIR_OUT).
 * @param max_count The maximum number of stream information entries to retrieve.
 * @param stream_info The pointer to an array of uach_stream_info_t structures to store the stream information.
 * @return size_t The number of stream information entries retrieved.
 */
size_t uach_get_stream_info(uach_device_t *uac, usb_dir_e dir, size_t max_count, uach_stream_info_t *stream_info);

usb_rst_e uach_open_stream(uach_device_t *uac, const void *handle, uint32_t sample_freq, void *buf, uach_stream_cb cb);
usb_rst_e uach_close_stream(uach_device_t *uac, const void *handle);
usb_rst_e uach_set_mute(uach_device_t *uac, const void *handle, usb_bool_t mute);
usb_rst_e uach_set_volume(uach_device_t *uac, const void *handle, uint8_t volume);

#endif // USB_UACH_CLASS_CLASS_EN

#ifdef __cplusplus
}
#endif

#endif
