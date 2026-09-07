/********************************** (C) COPYRIGHT *******************************
* File Name          : usbd_driver_public.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb device driver public headfile.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBD_DRIVER_PUBLIC_H_
#define __USBD_DRIVER_PUBLIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "usb_define.h"
#include "usb_config.h"

#ifdef USB_DEVICE_DRIVER_EN

/* @define */
#define USBD_ENDP_FEATURE_NORMAL            (0x00)
#define USBD_ENDP_FEATURE_ISO               (0x01)
#define USBD_ENDP_FEATURE_BURST             (0x02)
#define USBD_ENDP_FEATURE_ZLP               (0x04)

/* @enum */
typedef enum
{
    USBD_CB_EVENT_RESET,
    USBD_CB_EVENT_SUSPEND,
    USBD_CB_EVENT_SOF,
    USBD_CB_EVENT_ENUM_COMPLETED,
    USBD_CB_EVENT_COUNT,
} usbd_cb_event_e;

/* @struct */
typedef struct usbd_handle usbd_handle_t;

/* @callback function */
typedef void (*usbd_event_cb)(usbd_handle_t *h, uint32_t parameter);
typedef usb_rst_e(*usbd_ctrl_setup_cb)(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
typedef usb_rst_e(*usbd_ctrl_data_cb)(usbd_handle_t *h, const usb_req_t *req, void *buf, size_t size);
typedef void (*usbd_ctrl_status_cb)(usbd_handle_t *h, const usb_req_t *req, void *buf, size_t size);
typedef endp_resp_e(*usbd_data_xfer_cb)(usbd_handle_t *h, usb_endp_t ep, void *buf, size_t size);

/* @function declaration */

/**
 * @brief Usb device driver initialization.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 * @note Make sure the handle has been initialized before executing this function.
 */
usb_rst_e usbd_drv_init(usbd_handle_t *h);

/**
 * @brief Enable usb device controller.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_enable(usbd_handle_t *h);

/**
 * @brief disable usb device controller.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_disable(usbd_handle_t *h);

/**
 * @brief Open usb device controller.
 *
 * @param h Usb device driver handle object.
 * @param speed Usb device expected connection speed.
 * @param sof_en Enable SOF event or not.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_open(usbd_handle_t *h, usb_speed_e speed, usb_bool_t sof_en);

/**
 * @brief Close usb device controller.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_close(usbd_handle_t *h);

/**
 * @brief Usb device driver enumeration status.
 *
 * @param h Usb device driver handle object.
 * @return usb_bool_t USB_FALSE: is not enumerated, USB_TRUE: is enumerated.
 */
usb_bool_t usbd_is_enum(usbd_handle_t *h);

/**
 * @brief Usb device connection speed.
 *
 * @param h Usb device driver handle object.
 * @return usb_speed_e Return usb device connection speed enumeration.
 */
usb_speed_e usbd_get_speed(usbd_handle_t *h);

/**
 * @brief Send resume signal.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_resume(usbd_handle_t *h);

/**
 * @brief Usb device interrupt control.
 *
 * @param h Usb device driver handle object.
 * @param status Interrupt status.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_interrupt_ctrl(usbd_handle_t *h, usb_bool_t status);

/**
 * @brief Open usb controller endpoint.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param feature Endpoint feature.
 * @param max_size Endpoint maximum size.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_open(usbd_handle_t *h, usb_endp_t ep, uint8_t feature, uint16_t max_size);

/**
 * @brief Close usb controller endpoint.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_close(usbd_handle_t *h, usb_endp_t ep);

/**
 * @brief Set endpoint response.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param resp Endpoint response.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_set_response(usbd_handle_t *h, usb_endp_t ep, endp_resp_e resp);

/**
 * @brief Set endpoint size.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param size Endpoint size.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_set_size(usbd_handle_t *h, usb_endp_t ep, size_t size);

/**
 * @brief Set endpoint buffer.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param buf Endpoint data buffer.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_set_buf(usbd_handle_t *h, usb_endp_t ep, void *buf);

/**
 * @brief Register usb device driver event callback.
 *
 * @param h Usb device driver handle object.
 * @param event Usb device driver event type.
 * @param cb Callback function.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_register_event_callback(usbd_handle_t *h, usbd_cb_event_e event, usbd_event_cb cb);

/**
 * @brief Register usb device driver request control transfer callback.
 *
 * @param h Usb device driver handle object.
 * @param bmRequestType Setup packet bmRequestType field.
 * @param bRequest Setup packet bRequest field.
 * @param setup Setup stage callback function.
 * @param data Data stage callback function.
 * @param status Status stage callback function.
 * @return usb_rst_e Execution result.
 * @note If the setup callback function does not return USB_RST_OK, the driver will respond with a STALL.
 */
usb_rst_e usbd_register_req_callback(usbd_handle_t *h, uint8_t bmRequestType, uint8_t bRequest,
                                     usbd_ctrl_setup_cb setup, usbd_ctrl_data_cb data, usbd_ctrl_status_cb status);

/**
 * @brief Register usb device driver interface control transfer callback.
 *
 * @param h Usb device driver handle object.
 * @param itf Interface number.
 * @param setup Setup stage callback function.
 * @param data Data stage callback function.
 * @param status Status stage callback function.
 * @return usb_rst_e Execution result.
 * @note If the setup callback function does not return USB_RST_OK, the driver will respond with a STALL.
 */
usb_rst_e usbd_register_itf_callback(usbd_handle_t *h, uint16_t itf, usbd_ctrl_setup_cb setup, usbd_ctrl_data_cb data,
                                     usbd_ctrl_status_cb status);

/**
 * @brief Register usb device driver data transfer callback.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param cb Data transfer callback function.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_register_data_callback(usbd_handle_t *h, usb_endp_t ep, usbd_data_xfer_cb cb);

/**
 * @brief Usb device driver data upload.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param buf Data buffer.
 * @param size Expected data length to send.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_upload(usbd_handle_t *h, usb_endp_t ep, void *buf, size_t size);

/**
 * @brief Usb device driver data download.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param buf Data buffer.
 * @param size Expected data length to receive.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_download(usbd_handle_t *h, usb_endp_t ep, void *buf, size_t size);

#endif // USB_DEVICE_DRIVER_EN

#ifdef __cplusplus
}
#endif

#endif
