/********************************** (C) COPYRIGHT *******************************
* File Name          : usbd_driver_private.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb device driver private headfile.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBD_DRIVER_PRIVATE_H_
#define __USBD_DRIVER_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>
#include <stddef.h>

#include "usb_define.h"
#include "usb_config.h"
#include "device/usbd_driver_public.h"

#ifdef USB_DEVICE_DRIVER_EN

/* @enum */
typedef enum
{
    USBD_EVENT_NONE,
    USBD_EVENT_RESET,
    USBD_EVENT_SUSPEND,
    USBD_EVENT_SOF,
    USBD_EVENT_SETUP,
    USBD_EVENT_XFER,
    USBD_EVENT_COUNT,
} usbd_event_e;

/* @struct */
typedef struct
{
    usbd_event_e e;

    union
    {
        struct
        {
            usb_speed_e link_speed;
        } reset;

        struct
        {
            uint16_t frame_no;
            uint16_t microframe_no;
        } sof;

        struct
        {
            usb_endp_t ep;
            size_t size;
            void *buf;
        } xfer;
    };
} usbd_event_t;

typedef struct
{
    uint8_t address;
    uint8_t link_speed;
    uint8_t config_num;
    uint8_t ep0_size;
    usb_bool_t self_powered;
    usb_bool_t remote_wakeup;
} usbd_feature_t;

typedef struct
{
    uint8_t dir;
    uint8_t stage;
    size_t size;
    size_t offset;
    void *buf_ptr;
    usbd_ctrl_data_cb data_cb;
    usbd_ctrl_status_cb status_cb;
    uint32_t temp_data;
    usb_req_t req;
    __attribute__((aligned(4))) uint8_t buf[64];
} usbd_ctrl_ctx_t;

typedef struct
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    usbd_ctrl_setup_cb setup;
    usbd_ctrl_data_cb data;
    usbd_ctrl_status_cb status;
} usbd_req_cb_item_t;

typedef struct
{
    usbd_ctrl_setup_cb setup;
    usbd_ctrl_data_cb data;
    usbd_ctrl_status_cb status;
} usbd_itf_cb_item_t;

typedef struct
{
    uint8_t status;
    uint8_t feature;
    uint16_t max_size;

    void *xfer_buffer;
    size_t xfer_length;
    size_t xfer_offset;
    usbd_data_xfer_cb xfer_cb;
} usbd_endp_param_t;

struct usbd_handle
{
    usbd_feature_t feature;

    usbd_ctrl_ctx_t ctrl_ctx;

    size_t ctrl_req_cb_count;
    usbd_req_cb_item_t ctrl_req_cb_items[USBD_REQ_CB_MAX_ITEMS];
    usbd_itf_cb_item_t ctrl_itf_cb_items[USBD_ITF_CB_MAX_ITEMS];

    usbd_endp_param_t endp_params[2][USB_MAX_EP_NUM];

    usbd_event_cb event_cbs[USBD_CB_EVENT_COUNT];

    usb_rst_e(*enable)(void);
    usb_rst_e(*disable)(void);

    usb_rst_e(*open)(usb_speed_e speed, usb_bool_t sof_en);
    usb_rst_e(*close)(void);

    usb_rst_e(*resume)(void);

    usb_rst_e(*set_address)(uint8_t addr);
    usb_rst_e(*interrupt_ctrl)(usb_bool_t status);
    usb_rst_e(*test_mode_ctrl)(usb_req_test_select_e test_select);

    usb_rst_e(*endp_open)(usb_endp_t ep, uint8_t feature, uint16_t max_size);
    usb_rst_e(*endp_close)(usb_endp_t ep);

    usb_rst_e(*set_endp_buf)(usb_endp_t ep, void *buf);
    usb_rst_e(*set_endp_size)(usb_endp_t ep, size_t size);
    usb_rst_e(*set_endp_toggle)(usb_endp_t ep, endp_tog_e tog);
    usb_rst_e(*set_endp_response)(usb_endp_t ep, endp_resp_e resp);
};

/* @function declaration */

/**
 * @brief Usb device driver event handle.
 *
 * @param h Usb device driver handle object.
 * @param e Usb device driver event object.
 * @note This function is executed in the USB device interrupt handler.
 */
void usbd_event_handle(usbd_handle_t *h, usbd_event_t *e);

#endif // USB_DEVICE_DRIVER_EN

#ifdef __cplusplus
}
#endif

#endif
