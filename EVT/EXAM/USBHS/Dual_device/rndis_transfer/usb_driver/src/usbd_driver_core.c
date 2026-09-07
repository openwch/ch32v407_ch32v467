/********************************** (C) COPYRIGHT *******************************
* File Name          : usbd_driver_core.c
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb device driver core file.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#define USB_LOG_TAG                 "USBD"

/* @include */
#include <string.h>

#include "usb_driver.h"
#include "device/usbd_driver_private.h"

#ifdef USB_DEVICE_DRIVER_EN

/* @function declaration */
static void _reset_parameter(usbd_handle_t *h);
static void _setup_process(usbd_handle_t *h, usbd_event_t *e);
static void _data_process(usbd_handle_t *h, usbd_event_t *e);
static void _ctrl_xfer_process(usbd_handle_t *h, usbd_event_t *e);
static usb_rst_e _set_endp_response(usbd_handle_t *h, usb_endp_t ep, endp_resp_e resp);

static usb_rst_e _register_standard_request(usbd_handle_t *h);
static usb_rst_e _set_address_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _clear_feature_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _get_config_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _get_status_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _set_config_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _set_feature_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static void _set_address_status(usbd_handle_t *h, const usb_req_t *req, void *buf, size_t size);
static void _set_config_status(usbd_handle_t *h, const usb_req_t *req, void *buf, size_t size);
static void _set_feature_status(usbd_handle_t *h, const usb_req_t *req, void *buf, size_t size);

usb_rst_e usbd_drv_init(usbd_handle_t *h)
{
    if (!h || !h->enable || !h->disable || !h->open || !h->close || !h->resume || !h->set_address ||
        !h->interrupt_ctrl || !h->endp_open || !h->endp_close || !h->set_endp_buf || !h->set_endp_size ||
        !h->set_endp_response || !h->set_endp_toggle)
    {
        return USB_RST_NULL_PTR;
    }

    h->feature.self_powered = USBD_SELF_POWERED;

    h->ctrl_req_cb_count = 0;
    memset(&h->ctrl_req_cb_items, 0, sizeof(h->ctrl_req_cb_items));
    memset(&h->ctrl_itf_cb_items, 0, sizeof(h->ctrl_itf_cb_items));
    _reset_parameter(h);

    usb_rst_e rst = _register_standard_request(h);
    if (rst != USB_RST_OK)
    {
        return rst;
    }

    return USB_RST_OK;
}

usb_rst_e usbd_enable(usbd_handle_t *h)
{
    return h->enable();
}

usb_rst_e usbd_disable(usbd_handle_t *h)
{
    return h->disable();
}

usb_rst_e usbd_open(usbd_handle_t *h, usb_speed_e speed, usb_bool_t sof_en)
{
    usb_rst_e rst;

    rst = h->open(speed, sof_en);
    if (rst != USB_RST_OK) return rst;

    rst = h->endp_open(0x80, USBD_ENDP_FEATURE_NORMAL, h->feature.ep0_size);
    if (rst != USB_RST_OK) return rst;

    rst = h->endp_open(0x00, USBD_ENDP_FEATURE_NORMAL, h->feature.ep0_size);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_buf(0x80, h->ctrl_ctx.buf);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_buf(0x00, h->ctrl_ctx.buf);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_size(0x00, sizeof(usb_req_t));
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_response(0x80, ENDP_RESP_NAK);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_response(0x00, ENDP_RESP_ACK);
    if (rst != USB_RST_OK) return rst;

    return USB_RST_OK;
}

usb_rst_e usbd_close(usbd_handle_t *h)
{
    return h->close();
}

usb_bool_t usbd_is_enum(usbd_handle_t *h)
{
    return h->feature.config_num ? USB_TRUE : USB_FALSE;
}

usb_speed_e usbd_get_speed(usbd_handle_t *h)
{
    return h->feature.link_speed;
}

usb_rst_e usbd_resume(usbd_handle_t *h)
{
    if (!h->feature.remote_wakeup) return USB_RST_FAILED;

    return h->resume();
}

usb_rst_e usbd_interrupt_ctrl(usbd_handle_t *h, usb_bool_t status)
{
    return h->interrupt_ctrl(status);
}

usb_rst_e usbd_endp_open(usbd_handle_t *h, usb_endp_t ep, uint8_t feature, uint16_t max_size)
{
    usb_rst_e rst;
    uint8_t num = ENDP_NUM(ep);

    if (num == 0) return USB_RST_UNSUPPORTED_ENDP;

    usbd_endp_param_t *param = &h->endp_params[ENDP_DIR(ep) ? USB_DIR_IN : USB_DIR_OUT][num];
    param->status = ENDP_STA_IDLE;
    param->feature = feature & ~USBD_ENDP_FEATURE_BURST;
    param->max_size = max_size;
    param->xfer_length = 0;
    param->xfer_offset = 0;

    rst = h->set_endp_toggle(ep, ENDP_TOG_DATA0);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_response(ep, ENDP_RESP_NAK);
    if (rst != USB_RST_OK) return rst;

    rst = h->endp_open(ep, feature, max_size);
    if (rst != USB_RST_OK) return rst;

    USB_LOGI("Endpoint 0x%02X opened, feature: 0x%02X, max size: %d", ep, feature, max_size);
    return USB_RST_OK;
}

usb_rst_e usbd_endp_close(usbd_handle_t *h, usb_endp_t ep)
{
    USB_LOGI("Endpoint 0x%02X closed", ep);
    return h->endp_close(ep);
}

usb_rst_e usbd_endp_set_response(usbd_handle_t *h, usb_endp_t ep, endp_resp_e resp)
{
    return _set_endp_response(h, ep, resp);
}

usb_rst_e usbd_endp_set_size(usbd_handle_t *h, usb_endp_t ep, size_t size)
{
    return h->set_endp_size(ep, size);
}

usb_rst_e usbd_endp_set_buf(usbd_handle_t *h, usb_endp_t ep, void *buf)
{
    h->endp_params[ENDP_DIR(ep) ? USB_DIR_IN : USB_DIR_OUT][ENDP_NUM(ep)].xfer_buffer = buf;
    return h->set_endp_buf(ep, buf);
}

usb_rst_e usbd_register_event_callback(usbd_handle_t *h, usbd_cb_event_e event, usbd_event_cb cb)
{
    if (event >= USBD_CB_EVENT_COUNT) return USB_RST_UNSUPPORTED_EVENT;

    h->event_cbs[event] = cb;
    return USB_RST_OK;
}

usb_rst_e usbd_register_req_callback(usbd_handle_t *h, uint8_t bmRequestType, uint8_t bRequest,
                                     usbd_ctrl_setup_cb setup, usbd_ctrl_data_cb data, usbd_ctrl_status_cb status)
{
    if (h->ctrl_req_cb_count >= USBD_REQ_CB_MAX_ITEMS)
    {
        USB_LOGW("Request callback items overflow, cannot register callback: bmRequestType=0x%02X, bRequest=0x%02X",
                 bmRequestType, bRequest);
        return USB_RST_OVERFLOW;
    }

    size_t i;
    for (i = 0; i < h->ctrl_req_cb_count; i++)
    {
        if (h->ctrl_req_cb_items[i].bmRequestType == bmRequestType &&
            h->ctrl_req_cb_items[i].bRequest == bRequest)
        {
            /* Existing item will be overwritten */
            break;
        }
    }
    if (i >= h->ctrl_req_cb_count)
    {
        h->ctrl_req_cb_count++;
    }

    usbd_req_cb_item_t *item = &h->ctrl_req_cb_items[i];
    item->bmRequestType = bmRequestType;
    item->bRequest = bRequest;
    item->setup = setup;
    item->data = data;
    item->status = status;
    return USB_RST_OK;
}

usb_rst_e usbd_register_itf_callback(usbd_handle_t *h, uint16_t itf, usbd_ctrl_setup_cb setup, usbd_ctrl_data_cb data,
                                     usbd_ctrl_status_cb status)
{
    if (itf >= USB_ARRAY_SIZE(h->ctrl_itf_cb_items))
    {
        USB_LOGW("Interface callback items overflow, cannot register callback for interface %d", itf);
        return USB_RST_OVERFLOW;
    }

    h->ctrl_itf_cb_items[itf].setup = setup;
    h->ctrl_itf_cb_items[itf].data = data;
    h->ctrl_itf_cb_items[itf].status = status;
    return USB_RST_OK;
}

usb_rst_e usbd_register_data_callback(usbd_handle_t *h, usb_endp_t ep, usbd_data_xfer_cb cb)
{
    uint8_t num = ENDP_NUM(ep);
    if (num >= USB_MAX_EP_NUM) return USB_RST_UNSUPPORTED_ENDP;

    usbd_endp_param_t *param = &h->endp_params[ENDP_DIR(ep) ? USB_DIR_IN : USB_DIR_OUT][num];
    param->xfer_cb = cb;

    return USB_RST_OK;
}

usb_rst_e usbd_upload(usbd_handle_t *h, usb_endp_t ep, void *buf, size_t size)
{
    usbd_endp_param_t *param = &h->endp_params[USB_DIR_IN][ENDP_NUM(ep)];

    switch (param->status)
    {
    case ENDP_STA_IDLE:
        param->status = ENDP_STA_BUSY;
        param->xfer_buffer = buf;
        param->xfer_length = size;
        param->xfer_offset = 0;

        h->set_endp_buf(ep, buf);
        h->set_endp_size(ep, param->feature & USBD_ENDP_FEATURE_BURST ? size : USB_MIN(size, param->max_size));
        _set_endp_response(h, ep, param->feature & USBD_ENDP_FEATURE_ISO ? ENDP_RESP_NONE : ENDP_RESP_ACK);
        return USB_RST_OK;

    case ENDP_STA_BUSY:
        return USB_RST_ENDP_BUSY;

    case ENDP_STA_STALL:
        return USB_RST_ENDP_STALL;
    }

    return USB_RST_FAILED;
}

usb_rst_e usbd_download(usbd_handle_t *h, usb_endp_t ep, void *buf, size_t size)
{
    usbd_endp_param_t *param = &h->endp_params[USB_DIR_OUT][ENDP_NUM(ep)];

    switch (param->status)
    {
    case ENDP_STA_IDLE:
    case ENDP_STA_BUSY:
        param->status = ENDP_STA_BUSY;
        param->xfer_length = size;
        param->xfer_offset = 0;
        param->xfer_buffer = buf;

        h->set_endp_buf(ep, buf);
        h->set_endp_size(ep, param->feature & USBD_ENDP_FEATURE_BURST ? size : USB_MIN(size, param->max_size));
        _set_endp_response(h, ep, param->feature & USBD_ENDP_FEATURE_ISO ? ENDP_RESP_NONE : ENDP_RESP_ACK);
        return USB_RST_OK;

    case ENDP_STA_STALL:
        return USB_RST_ENDP_STALL;
    }

    return USB_RST_FAILED;
}

void usbd_event_handle(usbd_handle_t *h, usbd_event_t *e)
{
    switch (e->e)
    {
    case USBD_EVENT_NONE:
        break;

    case USBD_EVENT_RESET:
        _reset_parameter(h);
        h->set_address(0x00);
        h->feature.link_speed = e->reset.link_speed;
        USB_LOGI("USB reset event, link speed: %s",
                 e->reset.link_speed == USB_SPEED_LOW ? "Low" :
                 e->reset.link_speed == USB_SPEED_FULL ? "Full" :
                 e->reset.link_speed == USB_SPEED_HIGH ? "High" : "Unknown");
        if (h->event_cbs[USBD_CB_EVENT_RESET])
        {
            h->event_cbs[USBD_CB_EVENT_RESET](h, e->reset.link_speed);
        }
        break;

    case USBD_EVENT_SUSPEND:
        USB_LOGI("USB suspend event");
        if (h->event_cbs[USBD_CB_EVENT_SUSPEND] && h->feature.address)
        {
            h->event_cbs[USBD_CB_EVENT_SUSPEND](h, 0);
        }
        break;

    case USBD_EVENT_SOF:
        if (h->event_cbs[USBD_CB_EVENT_SOF])
        {
            h->event_cbs[USBD_CB_EVENT_SOF](h, e->sof.frame_no);
        }
        break;

    case USBD_EVENT_SETUP:
        _setup_process(h, e);
        break;

    case USBD_EVENT_XFER:
        if (ENDP_NUM(e->xfer.ep) == 0x00)
        {
            _ctrl_xfer_process(h, e);
        }
        else
        {
            _data_process(h, e);
        }
        break;
    }
}

static void _reset_parameter(usbd_handle_t *h)
{
    for (uint8_t i = 0; i < USB_ARRAY_SIZE(h->endp_params); i++)
    {
        for (uint8_t j = 0; j < USB_ARRAY_SIZE(h->endp_params[0]); j++)
        {
            h->endp_params[i][j].status = ENDP_STA_IDLE;
            h->endp_params[i][j].feature = 0;
            h->endp_params[i][j].max_size = 0;
            h->endp_params[i][j].xfer_length = 0;
            h->endp_params[i][j].xfer_offset = 0;
            h->endp_params[i][j].xfer_buffer = NULL;
        }
    }

    h->feature.address = 0x00;
    h->feature.link_speed = USB_SPEED_FULL;
    h->feature.config_num = 0x00;
    h->feature.remote_wakeup = USB_FALSE;
}

static void _setup_process(usbd_handle_t *h, usbd_event_t *e)
{
    usb_rst_e rst = USB_RST_FAILED;
    usbd_ctrl_ctx_t *ctx = &h->ctrl_ctx;

    memcpy(&ctx->req, ctx->buf, sizeof(ctx->req));
    usbd_ctrl_data_cb data_cb = NULL;
    usbd_ctrl_status_cb status_cb = NULL;

    /* Priority query interface control requests */
    if (ctx->req.bmRequestType_bit.recipient == USB_REQ_RCPT_INTERFACE &&
        ctx->req.wIndex < USB_ARRAY_SIZE(h->ctrl_itf_cb_items) && h->ctrl_itf_cb_items[ctx->req.wIndex].setup)
    {
        rst = h->ctrl_itf_cb_items[ctx->req.wIndex].setup(h, &ctx->req, &ctx->buf_ptr, &ctx->size);
        data_cb = h->ctrl_itf_cb_items[ctx->req.wIndex].data;
        status_cb = h->ctrl_itf_cb_items[ctx->req.wIndex].status;
    }
    else
    {
        usbd_req_cb_item_t *item;
        for (size_t i = 0; i < h->ctrl_req_cb_count; i++)
        {
            item = &h->ctrl_req_cb_items[i];

            if (item->bmRequestType == ctx->req.bmRequestType && item->bRequest == ctx->req.bRequest)
            {
                rst = item->setup ? item->setup(h, &ctx->req, &ctx->buf_ptr, &ctx->size) : USB_RST_NULL_PTR;
                data_cb = item->data;
                status_cb = item->status;
                break;
            }
        }
    }

    if (rst == USB_RST_OK)
    {
        USB_LOGI("Control request: bmRequestType=%02XH, bRequest=%02XH, wValue=%04XH, wIndex=%04XH, wLength=%04XH",
                 ctx->req.bmRequestType, ctx->req.bRequest, ctx->req.wValue, ctx->req.wIndex, ctx->req.wLength);
        ctx->offset = 0;
        ctx->stage = CTRL_STAGE_SETUP;
        ctx->dir = ctx->req.bmRequestType_bit.direction ? USB_DIR_IN : USB_DIR_OUT;
        ctx->data_cb = data_cb;
        ctx->status_cb = status_cb;
        _ctrl_xfer_process(h, e);
    }
    else
    {
        /* Unsupported control transmission */
        USB_LOGW("Unsupported request: bmRequestType=%02XH, bRequest=%02XH, wValue=%04XH, wIndex=%04XH, wLength=%04XH",
                 ctx->req.bmRequestType, ctx->req.bRequest, ctx->req.wValue, ctx->req.wIndex, ctx->req.wLength);
        h->set_endp_response(0x00, ENDP_RESP_STALL);
        h->set_endp_response(0x80, ENDP_RESP_STALL);
    }
}

static void _data_process(usbd_handle_t *h, usbd_event_t *e)
{
    uint8_t ep = e->xfer.ep;
    size_t size = e->xfer.size;

    usbd_endp_param_t *param = &h->endp_params[ENDP_DIR(ep) ? USB_DIR_IN : USB_DIR_OUT][ENDP_NUM(ep)];

    param->xfer_offset += size;

    usb_bool_t need_zlp = (param->feature & USBD_ENDP_FEATURE_ZLP) ? USB_TRUE : USB_FALSE;
    usb_bool_t xfer_done = (param->xfer_offset >= param->xfer_length) ? USB_TRUE : USB_FALSE;
    usb_bool_t short_packet = (size % param->max_size || size == 0) ? USB_TRUE : USB_FALSE;

    if (short_packet || (xfer_done && !need_zlp))
    {
        param->status = ENDP_STA_IDLE;

        h->set_endp_buf(ep, param->xfer_buffer);
        h->set_endp_size(ep, param->feature & USBD_ENDP_FEATURE_BURST ? param->xfer_length :
                         USB_MIN(param->xfer_length, param->max_size));

        endp_resp_e resp = ENDP_RESP_NAK;
        if (param->xfer_cb)
        {
            resp = param->xfer_cb(h, ep, param->xfer_buffer, param->xfer_offset);
        }

        param->xfer_offset = 0;
        _set_endp_response(h, ep, resp);
    }
    else
    {
        h->set_endp_buf(ep, (uint8_t *)param->xfer_buffer + param->xfer_offset);
        h->set_endp_size(ep, USB_MIN(param->xfer_length - param->xfer_offset, param->max_size));
        _set_endp_response(h, ep, ENDP_RESP_ACK);
    }
}

static void _ctrl_xfer_process(usbd_handle_t *h, usbd_event_t *e)
{
    size_t size;
    usbd_ctrl_ctx_t *ctx = &h->ctrl_ctx;

    switch (ctx->stage)
    {
    case CTRL_STAGE_SETUP:
    case CTRL_STAGE_DATA:
        /* Device to host control transfer */
        if (ctx->dir)
        {
            if (ctx->size > 0)
            {
                uint8_t ep0_size = h->feature.link_speed == USB_SPEED_LOW ? 8 : h->feature.ep0_size;
                size = USB_MIN(ctx->size, ep0_size);
                memcpy(ctx->buf, (uint8_t *)ctx->buf_ptr + ctx->offset, size);
                ctx->offset += size;
                ctx->size -= size;
                h->set_endp_size(0x80, size);
                h->set_endp_response(0x80, ENDP_RESP_ACK);
                ctx->stage = CTRL_STAGE_DATA;
            }
            else
            {
                usb_rst_e rst = ctx->data_cb ? ctx->data_cb(h, &ctx->req, ctx->buf_ptr, ctx->offset) : USB_RST_OK;
                h->set_endp_size(0x80, 0);
                h->set_endp_size(0x00, sizeof(usb_req_t));
                h->set_endp_response(0x80, ENDP_RESP_ACK);
                h->set_endp_response(0x00, rst == USB_RST_OK ? ENDP_RESP_ACK : ENDP_RESP_STALL);
                ctx->stage = CTRL_STAGE_STATUS;
            }
        }
        /* Host to device control transfer */
        else
        {
            if (ctx->stage == CTRL_STAGE_DATA)
            {
                uint8_t ep0_size = h->feature.link_speed == USB_SPEED_LOW ? 8 : h->feature.ep0_size;
                size = USB_MIN(ctx->size, ep0_size);
                memcpy((uint8_t *)ctx->buf_ptr + ctx->offset, ctx->buf, size);
                ctx->offset += size;
                ctx->size -= size;
            }

            if (ctx->size > 0)
            {
                uint8_t ep0_size = h->feature.link_speed == USB_SPEED_LOW ? 8 : h->feature.ep0_size;
                size = USB_MIN(ctx->size, ep0_size);
                h->set_endp_size(0x00, size);
                h->set_endp_response(0x00, ENDP_RESP_ACK);
                ctx->stage = CTRL_STAGE_DATA;
            }
            else
            {
                usb_rst_e rst = ctx->data_cb ? ctx->data_cb(h, &ctx->req, ctx->buf_ptr, ctx->offset) : USB_RST_OK;
                h->set_endp_size(0x80, 0);
                h->set_endp_size(0x00, sizeof(usb_req_t));
                h->set_endp_response(0x00, ENDP_RESP_ACK);
                h->set_endp_response(0x80, rst == USB_RST_OK ? ENDP_RESP_ACK : ENDP_RESP_STALL);
                ctx->stage = CTRL_STAGE_STATUS;
            }
        }
        break;

    case CTRL_STAGE_STATUS:
        if (ctx->status_cb)
        {
            ctx->status_cb(h, &ctx->req, ctx->buf_ptr, ctx->offset);
        }
        break;
    }
}

static usb_rst_e _set_endp_response(usbd_handle_t *h, usb_endp_t ep, endp_resp_e resp)
{
    usbd_endp_param_t *param = &h->endp_params[ENDP_DIR(ep) ? USB_DIR_IN : USB_DIR_OUT][ENDP_NUM(ep)];

    switch (resp)
    {
    case ENDP_RESP_NAK:
        param->status = ENDP_STA_IDLE;
        break;

    case ENDP_RESP_STALL:
        param->status = ENDP_STA_STALL;
        break;

    default:
        param->status = ENDP_STA_BUSY;
        break;
    }

    return h->set_endp_response(ep, resp);
}

static usb_rst_e _register_standard_request(usbd_handle_t *h)
{
    usb_rst_e rst;

    rst = usbd_register_req_callback(h, 0x00, USB_REQ_CODE_SET_ADDRESS, _set_address_setup, NULL, _set_address_status);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_req_callback(h, 0x00, USB_REQ_CODE_SET_FEATURE, _set_feature_setup, NULL, _set_feature_status);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_req_callback(h, 0x00, USB_REQ_CODE_CLEAR_FEATURE, _clear_feature_setup, NULL, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_req_callback(h, 0x00, USB_REQ_CODE_SET_CONFIG, _set_config_setup, NULL, _set_config_status);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_req_callback(h, 0x80, USB_REQ_CODE_GET_STATUS, _get_status_setup, NULL, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_req_callback(h, 0x80, USB_REQ_CODE_GET_CONFIGURATION, _get_config_setup, NULL, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_req_callback(h, 0x02, USB_REQ_CODE_SET_FEATURE, _set_feature_setup, NULL, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_req_callback(h, 0x02, USB_REQ_CODE_CLEAR_FEATURE, _clear_feature_setup, NULL, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_req_callback(h, 0x82, USB_REQ_CODE_GET_STATUS, _get_status_setup, NULL, NULL);
    if (rst != USB_RST_OK) return rst;

    return USB_RST_OK;
}

static usb_rst_e _clear_feature_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE &&
        req->wValue == USB_REQ_FEATURE_REMOTE_WAKEUP)
    {
        h->feature.remote_wakeup = USB_FALSE;
        USB_LOGI("Remote wakeup feature cleared");
        return USB_RST_OK;
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_ENDPOINT &&
        req->wValue == USB_REQ_FEATURE_ENDPOINT_HALT)
    {
        usb_endp_t ep = req->wIndex & 0x00FF;
        uint8_t dir = ENDP_DIR(ep);

        usbd_endp_param_t *param = &h->endp_params[dir ? USB_DIR_IN : USB_DIR_OUT][ENDP_NUM(ep)];
        param->status = ENDP_STA_IDLE;
        param->xfer_length = 0;
        param->xfer_offset = 0;
        param->xfer_buffer = NULL;
        h->set_endp_toggle(ep, ENDP_TOG_DATA0);
        _set_endp_response(h, ep, dir ? ENDP_RESP_NAK : ENDP_RESP_ACK);
        USB_LOGI("Endpoint 0x%02X halt feature cleared", ep);
        return USB_RST_OK;
    }

    return USB_RST_FAILED;
}

static usb_rst_e _get_config_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    h->ctrl_ctx.temp_data = h->feature.config_num;
    *buf = (void *)&h->ctrl_ctx.temp_data;
    *size = USB_MIN(sizeof(uint8_t), req->wLength);
    USB_LOGI("Get configuration request: config_num=%d", h->feature.config_num);
    return USB_RST_OK;
}

static usb_rst_e _get_status_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE)
    {
        h->ctrl_ctx.temp_data = (h->feature.self_powered ? 0x0001 : 0x0000) |
                                (h->feature.remote_wakeup ? 0x0002 : 0x0000);
        *buf = (void *)&h->ctrl_ctx.temp_data;
        *size = USB_MIN(sizeof(uint16_t), req->wLength);
        USB_LOGI("Get device status request: self_powered=%s, remote_wakeup=%s",
                 h->feature.self_powered ? "true" : "false",
                 h->feature.remote_wakeup ? "true" : "false");
        return USB_RST_OK;
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_ENDPOINT)
    {
        usb_endp_t ep = req->wIndex & 0x00FF;
        uint8_t status = h->endp_params[ENDP_DIR(ep) ? USB_DIR_IN : USB_DIR_OUT][ENDP_NUM(ep)].status;
        h->ctrl_ctx.temp_data = status == ENDP_STA_STALL ? 0x0001 : 0x0000;
        *buf = (void *)&h->ctrl_ctx.temp_data;
        *size = USB_MIN(sizeof(uint16_t), req->wLength);
        USB_LOGI("Get endpoint status request: endpoint=0x%02X, status=%s", ep,
                 status == ENDP_STA_STALL ? "stalled" : "not stalled");
        return USB_RST_OK;
    }

    return USB_RST_FAILED;
}

static usb_rst_e _set_address_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    *size = 0;
    h->feature.address = req->wValue & 0x00FF;
    return USB_RST_OK;
}

static usb_rst_e _set_config_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    *size = 0;
    h->feature.config_num = req->wValue & 0x00FF;
    return USB_RST_OK;
}

static usb_rst_e _set_feature_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE &&
        req->wValue == USB_REQ_FEATURE_REMOTE_WAKEUP)
    {
        h->feature.remote_wakeup = USB_TRUE;
        USB_LOGI("Remote wakeup feature set");
        return USB_RST_OK;
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE &&
        req->wValue == USB_REQ_FEATURE_TEST_MODE)
    {
        uint8_t test_select = (req->wIndex >> 8) & 0x00FF;
        if ((req->wIndex & 0x00FF) == 0x0000 && test_select > USB_REQ_TEST_SELECT_RESERVED &&
            test_select <= USB_REQ_TEST_SELECT_FORCE_ENABLE)
        {
            USB_LOGI("Test mode feature set: test_select=0x%02X", test_select);
            return USB_RST_OK;
        }
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_ENDPOINT &&
        req->wValue == USB_REQ_FEATURE_ENDPOINT_HALT)
    {
        usb_endp_t ep = req->wIndex & 0x00FF;
        h->endp_params[ENDP_DIR(ep) ? USB_DIR_IN : USB_DIR_OUT][ENDP_NUM(ep)].status = ENDP_STA_STALL;
        _set_endp_response(h, ep, ENDP_RESP_STALL);
        USB_LOGI("Endpoint 0x%02X halt feature set", ep);
        return USB_RST_OK;
    }

    return USB_RST_FAILED;
}

static void _set_address_status(usbd_handle_t *h, const usb_req_t *req, void *buf, size_t size)
{
    USB_LOGI("Set address request: address=0x%02X", h->feature.address);
    h->set_address(h->feature.address);
}

static void _set_config_status(usbd_handle_t *h, const usb_req_t *req, void *buf, size_t size)
{
    USB_LOGI("Set configuration request: config_num=%d", h->feature.config_num);
    if (h->event_cbs[USBD_CB_EVENT_ENUM_COMPLETED])
    {
        h->event_cbs[USBD_CB_EVENT_ENUM_COMPLETED](h, h->feature.config_num);
    }
}

static void _set_feature_status(usbd_handle_t *h, const usb_req_t *req, void *buf, size_t size)
{
    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE &&
        req->wValue == USB_REQ_FEATURE_TEST_MODE)
    {
        uint8_t test_select = (req->wIndex >> 8) & 0x00FF;

        switch (test_select)
        {
        case USB_REQ_TEST_SELECT_J:
            USB_LOGI("Test mode: TEST_J selected");
            break;

        case USB_REQ_TEST_SELECT_K:
            USB_LOGI("Test mode: TEST_K selected");
            break;

        case USB_REQ_TEST_SELECT_SE0_NAK:
            USB_LOGI("Test mode: TEST_SE0_NAK selected");
            break;

        case USB_REQ_TEST_SELECT_PACKET:
            USB_LOGI("Test mode: TEST_PACKET selected");
            break;

        case USB_REQ_TEST_SELECT_FORCE_ENABLE:
            USB_LOGI("Test mode: TEST_FORCE_ENABLE selected");
            break;
        }

        if (h->test_mode_ctrl)
        {
            h->test_mode_ctrl(test_select);
        }
    }
}

#endif // USB_DEVICE_DRIVER_EN
