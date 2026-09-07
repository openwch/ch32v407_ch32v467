/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Main function file.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * 功能说明 / Description:
 *   本示例演示了一个双 USB CDC ACM 设备数据转发功能。板上两个 USB 控制器
 *   分别枚举为 CDC ACM 串口设备，从一个串口接收到的数据会转发到另一个串口
 *   发送出去，实现两个 USB 串口之间的数据透传。
 *
 * 使用方法 / Usage:
 *   1. 编译并烧录固件到目标板
 *   2. 将目标板的两个 USB 接口分别连接到 PC（或两个不同的 PC）
 *   3. PC 设备管理器中应识别到两个 "CDC Transfer Demo" 串口设备
 *   4. 引脚对应请查看 pin_description.md 文件
 *   5. 打开两个串口终端，从一个串口发送的数据将从另一个串口接收
 *
 * 其他说明 / Notes:
 *
 */

/* @include */
#include <string.h>
#include <assert.h>

#include "board.h"
#include "led.h"

#include "usb_driver.h"

#include "descriptor.h"

/* @define */
#ifndef USBD0_INDEX
#define USBD0_INDEX         0
#endif

#if USBD0_INDEX >= BOARD_USBDC_COUNT
#error Not supported this USBD_INDEX!
#endif

#ifndef USBD1_INDEX
#define USBD1_INDEX         1
#endif

#if USBD1_INDEX >= BOARD_USBDC_COUNT
#error Not supported this USBD_INDEX!
#endif

#define LED0                0x01
#define LED1                0x02

#define BUFFER_COUNT        8
#define BUFFER_SIZE         512

/* @struct */
typedef struct
{
    uint8_t *addr;
    uint32_t size;
} pack_t;

typedef struct
{
    uint8_t load;
    uint8_t deal;
    uint8_t stop;
    uint8_t count;
} manage_t;

typedef struct
{
    usbd_handle_t *h;
    cdcd_acm_itf_t itf;
    manage_t xfer_manage;
    pack_t xfer_packs[BUFFER_COUNT];
    __attribute__((aligned(4))) uint8_t xfer_bufs[BUFFER_COUNT][BUFFER_SIZE];
} acm_dev_t;

/* @global */
acm_dev_t acm_devs[2];

/* @function declaration */
void tranxfer_handle(acm_dev_t *dev);
void reset_callback(usbd_handle_t *h, uint32_t parameter);
usb_rst_e cdc_itf_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
endp_resp_e download_callback(usbd_handle_t *h, usb_endp_t ep, void *buf, size_t size);

int main(void)
{
    cdcd_acm_itf_t *itf;

    board_init();

    acm_devs[0].itf.itf_num = 0x00;
    acm_devs[0].itf.ep_notif = 0x81;
    acm_devs[0].itf.ep_in = 0x82;
    acm_devs[0].itf.ep_out = 0x03;
    acm_devs[0].itf.line_state = 0x00;

    assert(led_init() == USB_RST_OK);

    usbd_handle_t *h0 = board_usbd_init(USBD0_INDEX);
    assert(h0 != NULL);

    acm_devs[0].h = h0;
    assert(usbd_drv_init(h0) == USB_RST_OK);
    assert(usbd_enable(h0) == USB_RST_OK);

    itf = &acm_devs[0].itf;
    assert(usbd_register_event_callback(h0, USBD_CB_EVENT_RESET, reset_callback) == USB_RST_OK);
    assert(usbd_register_req_callback(h0, 0x80, USB_REQ_CODE_GET_DESCRIPTOR, get_device_desc, NULL, NULL) == USB_RST_OK);
    assert(usbd_register_itf_callback(h0, itf->itf_num, cdc_itf_setup, NULL, NULL) == USB_RST_OK);
    assert(usbd_register_data_callback(h0, itf->ep_out, download_callback) == USB_RST_OK);

    assert(usbd_open(h0, USB_SPEED_HIGH, USB_FALSE) == USB_RST_OK);

    acm_devs[1].itf.itf_num = 0x00;
    acm_devs[1].itf.ep_notif = 0x81;
    acm_devs[1].itf.ep_in = 0x82;
    acm_devs[1].itf.ep_out = 0x03;
    acm_devs[1].itf.line_state = 0x00;

    assert(led_init() == USB_RST_OK);

    usbd_handle_t *h1 = board_usbd_init(USBD1_INDEX);
    assert(h1 != NULL);

    acm_devs[1].h = h1;
    assert(usbd_drv_init(h1) == USB_RST_OK);
    assert(usbd_enable(h1) == USB_RST_OK);

    itf = &acm_devs[1].itf;
    assert(usbd_register_event_callback(h1, USBD_CB_EVENT_RESET, reset_callback) == USB_RST_OK);
    assert(usbd_register_req_callback(h1, 0x80, USB_REQ_CODE_GET_DESCRIPTOR, get_device_desc, NULL, NULL) == USB_RST_OK);
    assert(usbd_register_itf_callback(h1, itf->itf_num, cdc_itf_setup, NULL, NULL) == USB_RST_OK);
    assert(usbd_register_data_callback(h1, itf->ep_out, download_callback) == USB_RST_OK);

    assert(usbd_open(h1, USB_SPEED_HIGH, USB_FALSE) == USB_RST_OK);

    while (1)
    {
        acm_dev_t *dev;

        for (uint8_t i = 0; i < sizeof(acm_devs) / sizeof(acm_dev_t); i++)
        {
            dev = &acm_devs[i];

            if (usbd_is_enum(dev->h) != USB_TRUE) continue;

            tranxfer_handle(dev);
        }
    }

    return 0;
}

void tranxfer_handle(acm_dev_t *dev)
{
    if (dev->xfer_manage.count)
    {
        acm_dev_t *dst_dev = dev == &acm_devs[0] ? &acm_devs[1] : &acm_devs[0];

        pack_t *pack = &dev->xfer_packs[dev->xfer_manage.deal];

        if (usbd_upload(dst_dev->h, dst_dev->itf.ep_in, pack->addr, pack->size) == USB_RST_OK)
        {
            pack->size = 0;
            dev->xfer_manage.deal = (dev->xfer_manage.deal + 1) % BUFFER_COUNT;

            usbd_interrupt_ctrl(dev->h, USB_FALSE);
            dev->xfer_manage.count--;
            usbd_interrupt_ctrl(dev->h, USB_TRUE);
        }
    }

    if (dev->xfer_manage.stop && dev->xfer_manage.count < BUFFER_COUNT - 2)
    {
        usbd_endp_set_response(dev->h, dev->itf.ep_out, ENDP_RESP_ACK);
        dev->xfer_manage.stop = 0;
    }
}

void reset_callback(usbd_handle_t *h, uint32_t parameter)
{
    acm_dev_t *dev = h == acm_devs[0].h ? &acm_devs[0] : &acm_devs[1];

    memset(&dev->xfer_manage, 0, sizeof(dev->xfer_manage));
    for (uint8_t i = 0; i < BUFFER_COUNT; i++)
    {
        dev->xfer_packs[i].addr = dev->xfer_bufs[i];
        dev->xfer_packs[i].size = 0;
    }
    uint16_t bulk_size = usbd_get_speed(h) == USB_SPEED_HIGH ? 512 : 64;
    usbd_endp_open(h, dev->itf.ep_notif, USBD_ENDP_FEATURE_NORMAL, 8);
    usbd_endp_open(h, dev->itf.ep_in, USBD_ENDP_FEATURE_BURST | USBD_ENDP_FEATURE_ZLP, bulk_size);
    usbd_endp_open(h, dev->itf.ep_out, USBD_ENDP_FEATURE_BURST, bulk_size);
    usbd_download(h, dev->itf.ep_out, dev->xfer_bufs[dev->xfer_manage.load], bulk_size);
}

usb_rst_e cdc_itf_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    acm_dev_t *dev = h == acm_devs[0].h ? &acm_devs[0] : &acm_devs[1];

    switch (req->bmRequestType_bit.type)
    {
    case USB_REQ_TYPE_CLASS:
        switch (req->bRequest)
        {
        case CDC_REQ_SET_LINE_CODING:
        case CDC_REQ_GET_LINE_CODING:
            *buf = (void *)&dev->itf.line_coding;
            *size = USB_MIN(sizeof(dev->itf.line_coding), req->wLength);
            return USB_RST_OK;

        case CDC_REQ_SET_CONTROL_LINE_STATE:
            *size = 0;
            dev->itf.line_state = req->wValue & 0x00FF;
            return USB_RST_OK;
        }
        break;
    }

    return USB_RST_FAILED;
}

endp_resp_e download_callback(usbd_handle_t *h, usb_endp_t ep, void *buf, size_t size)
{
    acm_dev_t *dev = h == acm_devs[0].h ? &acm_devs[0] : &acm_devs[1];
    dev->xfer_packs[dev->xfer_manage.load].addr = (uint8_t *)buf;
    dev->xfer_packs[dev->xfer_manage.load].size = size;
    dev->xfer_manage.load = (dev->xfer_manage.load + 1) % BUFFER_COUNT;
    dev->xfer_manage.count++;
    usbd_endp_set_buf(h, dev->itf.ep_out, dev->xfer_bufs[dev->xfer_manage.load]);

    if (dev->xfer_manage.count >= BUFFER_COUNT - 2)
    {
        dev->xfer_manage.stop = 1;
        return ENDP_RESP_NAK;
    }
    else
    {
        return ENDP_RESP_ACK;
    }
}
