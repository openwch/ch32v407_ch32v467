/********************************** (C) COPYRIGHT *******************************
* File Name          : usbh_driver_private.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb host driver private headfile.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBH_DRIVER_PRIVATE_H_
#define __USBH_DRIVER_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>
#include <stddef.h>

#include "usb_define.h"
#include "usb_config.h"
#include "host/usbh_driver_public.h"

#ifdef USB_HOST_DRIVER_EN

/* @define */
#define USBH_MATCH_ALL_ITF      0xFF

/* @enum */
typedef enum
{
    USBH_PID_OUT      = 0x01,
    USBH_PID_IN       = 0x09,
    USBH_PID_SOF      = 0x05,
    USBH_PID_SETUP    = 0x0D,

    USBH_PID_DATA0    = 0x03,
    USBH_PID_DATA1    = 0x0B,
    USBH_PID_DATA2    = 0x07,
    USBH_PID_MDATA    = 0x0F,

    USBH_PID_ACK      = 0x02,
    USBH_PID_NAK      = 0x0A,
    USBH_PID_STALL    = 0x0E,
    USBH_PID_NYET     = 0x06,

    USBH_PID_PRE_ERR  = 0x0C,
    USBH_PID_SPLIT    = 0x08,
    USBH_PID_PING     = 0x04,
    USBH_PID_RESERVED = 0x00,
} usbh_pid_e;

/* @callback */
typedef usb_rst_e(*usbh_drv_bind_cb)(usbh_handle_t *h, usbh_device_t *dev, const uint8_t *desc, uint16_t length);
typedef usb_rst_e(*usbh_drv_unbind_cb)(usbh_handle_t *h, usbh_device_t *dev, uint8_t itf_num);

typedef void(*usbh_data_xfer_cb)(void *drv, usb_bool_t rst, usb_endp_t ep, void *buf, size_t length);
typedef void(*usbh_ctrl_xfer_cb)(void *drv, usb_bool_t rst, usb_req_t *req, void *buf, uint16_t length);

typedef void(*usbh_wakeup_cb)(usbh_handle_t *h, usbh_device_t *dev);

/* @struct */
typedef struct usbh_endpoint usbh_endpoint_t;

typedef struct usbh_xfer_unit
{
    uint8_t pid;
    uint8_t pre;
    uint8_t iso;
    uint8_t endp;
    uint8_t token;
    uint8_t toggle;
    uint8_t dev_addr;
    uint8_t split_retry;
    uint32_t split_tick;
    split_data_t split_data;
    size_t length;
    void *buffer;
    usbh_endpoint_t *endp_ptr;
    struct usbh_xfer_unit *next;
} usbh_xfer_unit_t;

typedef struct
{
    usb_bool_t xfering;
    uint8_t toggle;
    uint8_t ctrl_stage;
    size_t retry;
    size_t length;
    size_t offset;
    uint32_t tick;
    usb_req_t *request;
    void *buffer;
    void *driver;
    void *callback;
    usbh_xfer_unit_t xfer_unit;
} usbh_xfer_ctx_t;

struct usbh_endpoint
{
    struct usbh_endpoint *next;

    uint8_t type;
    usb_endp_t endp;
    usb_bool_t stall;
    usb_bool_t ping_en;
    uint16_t interval;
    uint16_t max_size;
    usbh_xfer_ctx_t xfer_ctx;
};

struct usbh_device
{
    struct usbh_device *next;

    uint8_t address;
    uint8_t hub_addr;
    uint8_t hub_port;
    uint8_t ep0_size;
    uint8_t enum_stage;
    uint8_t reset_retry;
    uint8_t suspend_stage;
    usb_bool_t self_powered;
    usb_bool_t remote_wakeup;
    usb_bool_t is_suspend;
    uint16_t language_id;
    usb_speed_e speed;
    usbh_wakeup_cb wakeup_cb;

    desc_device_t device_desc;
    usbh_endpoint_t ctrl_endp;
    usbh_endpoint_t *endp_array[2][USB_MAX_EP_NUM];
};

struct usbh_driver
{
    uint16_t vid;
    uint16_t pid;

    uint8_t dev_class;
    uint8_t dev_subclass;
    uint8_t dev_protocol;

    uint8_t itf_class;
    uint8_t itf_subclass;
    uint8_t itf_protocol;

    uint16_t match_vid : 1;
    uint16_t match_pid : 1;
    uint16_t match_dev_class : 1;
    uint16_t match_dev_subclass : 1;
    uint16_t match_dev_protocol : 1;
    uint16_t match_itf_class : 1;
    uint16_t match_itf_subclass : 1;
    uint16_t match_itf_protocol : 1;
    uint16_t reserved : 8;

    const usbh_drv_bind_cb bind_cb;
    const usbh_drv_unbind_cb unbind_cb;
};

struct usbh_handle
{
    usb_bool_t xfering;
    usb_bool_t is_suspend;
    usb_speed_e speed;
    uint8_t microframe;
    uint32_t tick;
    uint32_t enum_tick;
    uint32_t xfer_tick;
    uint32_t bus_address_bitmap[4];

    usb_req_t enum_request;
    __attribute__((aligned(4))) uint8_t enum_buffer[USBH_ENUM_BUFFER_SIZE];

    usbh_xfer_unit_t *xfer_list;
    usbh_device_t *device_list;
    const usbh_driver_t *driver_list[USBH_MAX_DRIVERS];
    usbh_endpoint_t *endp_list[4];

    usb_rst_e(*open)(void);
    usb_rst_e(*close)(void);
    usb_rst_e(*start_transfer)(void);
    usb_rst_e(*root_set_feature)(hub_feature_e feature);
    usb_rst_e(*root_get_status)(hub_port_status_t *status);
    usb_rst_e(*port_set_feature)(usbh_handle_t *h, usbh_device_t *dev, hub_feature_e feature);
    usb_rst_e(*port_get_status)(usbh_handle_t *h, usbh_device_t *dev, hub_port_status_t *status);
};

/* @function declaration */

/**
 * @brief Usb host driver open device endpoint.
 *
 * @param h Usb host driver handle object.
 * @param dev Usb host driver device object.
 * @param desc Usb endpoint descriptor.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbh_endp_open(usbh_handle_t *h, usbh_device_t *dev, const desc_endpoint_t *desc);

/**
 * @brief Usb host driver close device endpoint.
 *
 * @param h Usb host driver handle object.
 * @param dev Usb host driver device object.
 * @param ep Usb endpoint address.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbh_endp_close(usbh_handle_t *h, usbh_device_t *dev, usb_endp_t ep);

/**
 * @brief Usb host driver control transfer.
 *
 * @param dev Usb host driver device object.
 * @param drv Usb host driver object.
 * @param req Usb control transfer request.
 * @param buf Usb control transfer buffer.
 * @param cb Usb control transfer callback function.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbh_ctrl_xfer(usbh_device_t *dev, void *drv, usb_req_t *req, void *buf, usbh_ctrl_xfer_cb cb);

/**
 * @brief Usb host driver data transfer.
 *
 * @param dev Usb host driver device object.
 * @param drv Usb host driver object.
 * @param ep Usb endpoint address.
 * @param buf Usb data transfer buffer.
 * @param length Usb data transfer length.
 * @param cb Usb data transfer callback function.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbh_data_xfer(usbh_device_t *dev, void *drv, usb_endp_t ep, void *buf, size_t length, usbh_data_xfer_cb cb);

/**
 * @brief Check if the control transfer is in progress.
 *
 * @param dev Usb host driver device object.
 * @return usb_bool_t USB_TRUE if the control transfer is in progress, otherwise USB_FALSE.
 */
usb_bool_t usbh_is_ctrl_xfering(usbh_device_t *dev);

/**
 * @brief Check if the data transfer is in progress.
 *
 * @param dev Usb host driver device object.
 * @param ep Usb endpoint address.
 * @return usb_bool_t USB_TRUE if the data transfer is in progress, otherwise USB_FALSE.
 */
usb_bool_t usbh_is_data_xfering(usbh_device_t *dev, usb_endp_t ep);

/**
 * @brief Insert a device into the USB host driver.
 *
 * @param h Usb host driver handle object.
 * @param hub_addr Usb hub address.
 * @param hub_port Usb hub port.
 */
void usbh_device_insert(usbh_handle_t *h, uint8_t hub_addr, uint8_t hub_port);

/**
 * @brief Remove a device from the USB host driver.
 *
 * @param h Usb host driver handle object.
 * @param hub_addr Usb hub address.
 * @param hub_port Usb hub port.
 */
void usbh_device_remove(usbh_handle_t *h, uint8_t hub_addr, uint8_t hub_port);

/**
 * @brief Reset a device in the USB host driver.
 *
 * @param h Usb host driver handle object.
 * @param dev Usb host driver device object.
 */
void usbh_device_reset(usbh_handle_t *h, usbh_device_t *dev);

/**
 * @brief Suspend a device in the USB host driver.
 *
 * @param h Usb host driver handle object.
 * @param dev Usb host driver device object.
 * @param remote_wakeup USB_TRUE to enable remote wakeup, otherwise USB_FALSE.
 * @param wakeup_cb Usb wakeup callback function.
 */
void usbh_device_suspend(usbh_handle_t *h, usbh_device_t *dev, usb_bool_t remote_wakeup, usbh_wakeup_cb wakeup_cb);

/**
 * @brief Resume a device in the USB host driver.
 *
 * @param h Usb host driver handle object.
 * @param hub_addr Usb hub address.
 * @param hub_port Usb hub port.
 */
void usbh_device_resume(usbh_handle_t *h, uint8_t hub_addr, uint8_t hub_port);

#endif // USB_HOST_DRIVER_EN

#ifdef __cplusplus
}
#endif

#endif
