/********************************** (C) COPYRIGHT *******************************
* File Name          : usbhs1_dcp.c
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb high speed device controller 1 port for ch32v407.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>

#include "debug.h"
#include "ch32v4x7.h"
#include "ch32v4x7_usb.h"

#include "usbhs1_dcp.h"
#include "device/usbd_driver_private.h"

#ifdef USB_DEVICE_DRIVER_EN

/* @define */
#define ENDP_MAX_LEN(ep)                *((volatile uint32_t *)&(USBHS1D->UEP0_MAX_LEN) + (ep))
#define ENDP_TX_LEN(ep)                 *((volatile uint16_t *)&(USBHS1D->UEP0_TX_LEN) + (ep) * 2)
#define ENDP_RX_LEN(ep)                 *((volatile uint16_t *)&(USBHS1D->UEP0_RX_LEN) + (ep) * 2)
#define ENDP_RX_SIZE(ep)                *((volatile uint16_t *)&(USBHS1D->UEP1_RX_SIZE) + (ep - 1) * 2)
#define ENDP_TX_CTRL(ep)                *((volatile uint8_t *)&(USBHS1D->UEP0_TX_CTRL) + (ep) * 4)
#define ENDP_RX_CTRL(ep)                *((volatile uint8_t *)&(USBHS1D->UEP0_RX_CTRL) + (ep) * 4)
#define ENDP_TX_DMA_ADDR(ep)            *((volatile uint32_t *)&(USBHS1D->UEP1_TX_DMA) + (ep - 1))
#define ENDP_RX_DMA_ADDR(ep)            *((volatile uint32_t *)&(USBHS1D->UEP1_RX_DMA) + (ep - 1))

/* @global */
usbd_handle_t usbhs1d_handle;
static uint16_t _endp_tx_size[USBHS1_DCP_MAX_ENDPOINTS];

/* @function declaration */
static usb_rst_e _enable(void);
static usb_rst_e _disable(void);
static usb_rst_e _open(usb_speed_e speed, usb_bool_t sof_en);
static usb_rst_e _close(void);
static usb_rst_e _resume(void);
static usb_rst_e _set_address(uint8_t addr);
static usb_rst_e _interrupt_ctrl(usb_bool_t status);
static usb_rst_e _test_mode_ctrl(usb_req_test_select_e test_select);
static usb_rst_e _endp_open(usb_endp_t ep, uint8_t feature, uint16_t max_size);
static usb_rst_e _endp_close(usb_endp_t ep);
static usb_rst_e _set_endp_buf(usb_endp_t ep, void *buf);
static usb_rst_e _set_endp_size(usb_endp_t ep, size_t size);
static usb_rst_e _set_endp_toggle(usb_endp_t ep, endp_tog_e tog);
static usb_rst_e _set_endp_response(usb_endp_t ep, endp_resp_e resp);

usb_rst_e usbhs1_dch_init(void)
{
    memset(&usbhs1d_handle, 0, sizeof(usbhs1d_handle));
    memset(_endp_tx_size, 0, sizeof(_endp_tx_size));

    usbhs1d_handle.feature.ep0_size = USBHS1_DCP_ENDP0_SIZE;

    usbhs1d_handle.enable = _enable;
    usbhs1d_handle.disable = _disable;
    usbhs1d_handle.open = _open;
    usbhs1d_handle.close = _close;
    usbhs1d_handle.resume = _resume;
    usbhs1d_handle.set_address = _set_address;
    usbhs1d_handle.interrupt_ctrl = _interrupt_ctrl;
    usbhs1d_handle.test_mode_ctrl = _test_mode_ctrl;
    usbhs1d_handle.endp_open = _endp_open;
    usbhs1d_handle.endp_close = _endp_close;
    usbhs1d_handle.set_endp_buf = _set_endp_buf;
    usbhs1d_handle.set_endp_size = _set_endp_size;
    usbhs1d_handle.set_endp_toggle = _set_endp_toggle;
    usbhs1d_handle.set_endp_response = _set_endp_response;

    return USB_RST_OK;
}

void usbhs1_dch_interrupt(void)
{
    usbd_event_t e;

    uint8_t en = USBHS1D->INT_EN;
    uint8_t flag = USBHS1D->INT_FG;
    uint8_t status = USBHS1D->INT_ST;

    if (flag & USBHS_UDIF_TRANSFER)
    {
        uint8_t endp = status & USBHS_UDIS_EP_ID_MASK;
        uint8_t dir = status & USBHS_UDIS_EP_DIR;

        if (endp == 0x00 && !dir && (ENDP_RX_CTRL(0) & USBHS_UEP_R_SETUP_IS))
        {
            e.e = USBD_EVENT_SETUP;
            ENDP_TX_CTRL(0) = (ENDP_TX_CTRL(0) & ~USBHS_UEP_T_TOG_MASK) | USBHS_UEP_T_TOG_DATA1;
            ENDP_RX_CTRL(0) = (ENDP_RX_CTRL(0) & ~USBHS_UEP_R_TOG_MASK) | USBHS_UEP_R_TOG_DATA1;
        }
        else if (dir)
        {
            e.e = USBD_EVENT_XFER;
            e.xfer.ep = endp | 0x80;
            e.xfer.size = _endp_tx_size[endp];
            if (endp == 0)
            {
                e.xfer.buf = (void *)USBHS1D->UEP0_DMA;
                ENDP_TX_CTRL(0) ^= USBHS_UEP_T_TOG_DATA1;
            }
            else
            {
                e.xfer.buf = (void *)ENDP_TX_DMA_ADDR(endp);
            }
        }
        else if (ENDP_RX_CTRL(endp) & USBHS_UEP_R_TOG_MATCH)
        {
            e.e = USBD_EVENT_XFER;
            e.xfer.ep = endp;
            e.xfer.size = ENDP_RX_LEN(endp);
            ENDP_RX_LEN(endp) = 0;
            if (endp == 0)
            {
                e.xfer.buf = (void *)USBHS1D->UEP0_DMA;
                ENDP_RX_CTRL(0) ^= USBHS_UEP_R_TOG_DATA1;
            }
            else
            {
                e.xfer.buf = (void *)ENDP_RX_DMA_ADDR(endp);
            }
        }
        else
        {
            ENDP_RX_CTRL(endp) = (ENDP_RX_CTRL(endp) & ~(USBHS_UEP_R_RES_MASK | USBHS_UEP_R_DONE)) |
                                 USBHS_UEP_R_RES_ACK;
            return;
        }
    }
    else if (en & flag & USBHS_UDIF_RX_SOF)
    {
        e.e = USBD_EVENT_SOF;
        uint16_t frame_no = USBHS1D->FRAME_NO;
        e.sof.frame_no = frame_no & 0x07FF;
        e.sof.microframe_no = frame_no >> 13;
    }
    else if (flag & USBHS_UDIF_BUS_RST)
    {
        e.e = USBD_EVENT_RESET;
        Delay_Ms(10);
        usb_speed_e link_speed = USB_SPEED_FULL;
        if (USBHS1D->MIS_ST & USBHS_UDMS_HS_MOD)
        {
            link_speed = USB_SPEED_HIGH;
        }
        else if ((USBHS1D->BASE_MODE & USBHS_UD_SPEED_TYPE) == USBHS_UD_SPEED_LOW)
        {
            link_speed = USB_SPEED_LOW;
        }
        e.reset.link_speed = link_speed;
    }
    else if (flag & USBHS_UDIF_SUSPEND)
    {
        if (USBHS1D->MIS_ST & USBHS_UDMS_SUSPEND)
        {
            e.e = USBD_EVENT_SUSPEND;
        }
        else
        {
            USBHS1D->INT_FG = USBHS_UDIF_SUSPEND;
            return;
        }
    }
    else
    {
        e.e = USBD_EVENT_NONE;
        USBHS1D->INT_FG = flag;
        return;
    }

    usbd_event_handle(&usbhs1d_handle, &e);

    switch (e.e)
    {
    case USBD_EVENT_RESET:
        USBHS1D->INT_FG = USBHS_UDIF_BUS_RST;
        break;

    case USBD_EVENT_SUSPEND:
        USBHS1D->INT_FG = USBHS_UDIF_SUSPEND;
        break;

    case USBD_EVENT_SOF:
        USBHS1D->INT_FG = USBHS_UDIF_RX_SOF;
        break;

    case USBD_EVENT_SETUP:
        USBHS1D->UEP0_RX_CTRL &= ~USBHS_UEP_R_DONE;
        break;

    case USBD_EVENT_XFER:
        if (ENDP_DIR(e.xfer.ep))
        {
            ENDP_TX_CTRL(ENDP_NUM(e.xfer.ep)) &= ~USBHS_UEP_T_DONE;
        }
        else
        {
            ENDP_RX_CTRL(ENDP_NUM(e.xfer.ep)) &= ~USBHS_UEP_R_DONE;
        }
        break;
    }
}

static usb_rst_e _enable(void)
{
    if ((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
    {
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS1, DISABLE);
        RCC->CTLR &= ~RCC_USBHSPLLON;
        if (RCC->CTLR & RCC_HSEON)
        {
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLCLKSource_HSE);
            RCC_USBHSPLLReferConfig(RCC_USBHSPLLCKREFCLK_25M);
        }
        else
        {
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLCLKSource_HSI);
            RCC_USBHSPLLReferConfig(RCC_USBHSPLLCKREFCLK_20M);
        }
        RCC->CTLR |= RCC_USBHSPLLON;
        while (!(RCC->CTLR & RCC_USBHSPLLRDY));
    }

    /* Enable UTMI Clock */
    RCC_UTMI1cmd(ENABLE);

    /* Enable USBHS Clock */
    RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS1, ENABLE);

    USBHS1D->CONTROL = USBHS_UD_RST_LINK | USBHS_UD_PHY_SUSPENDM;
    USBHS1D->INT_EN = USBHS_UDIE_BUS_RST | USBHS_UDIE_SUSPEND | USBHS_UDIE_BUS_SLEEP | USBHS_UDIE_LPM_ACT |
                      USBHS_UDIE_TRANSFER | USBHS_UDIE_LINK_RDY;
    USBHS1D->UEP_TX_EN = 0;
    USBHS1D->UEP_RX_EN = 0;
    USBHS1D->CONTROL = USBHS_UD_DMA_EN | USBHS_UD_LPM_EN | USBHS_UD_PHY_SUSPENDM;

    NVIC_EnableIRQ(USBHS1_IRQn);

    return USB_RST_OK;
}

static usb_rst_e _disable(void)
{
    NVIC_DisableIRQ(USBHS1_IRQn);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS1, DISABLE);
    RCC_UTMI1cmd(DISABLE);
    RCC->CTLR &= ~RCC_USBHSPLLON;
    return USB_RST_OK;
}

static usb_rst_e _open(usb_speed_e speed, usb_bool_t sof_en)
{
    uint8_t speed_val[] = {USBHS_UD_SPEED_FULL, USBHS_UD_SPEED_LOW, USBHS_UD_SPEED_HIGH, USBHS_UD_SPEED_HIGH};

    USBHS1D->INT_EN = sof_en ? USBHS1D->INT_EN | USBHS_UDIE_SOF_ACT : USBHS1D->INT_EN & ~USBHS_UDIE_SOF_ACT;
    USBHS1D->BASE_MODE = (USBHS1D->BASE_MODE & ~USBHS_UD_SPEED_TYPE) | speed_val[speed];
    USBHS1D->CONTROL |= USBHS_UD_DEV_EN;
    return USB_RST_OK;
}

static usb_rst_e _close(void)
{
    USBHS1D->CONTROL &= ~USBHS_UD_DEV_EN;
    return USB_RST_OK;
}

static usb_rst_e _resume(void)
{
    USBHS1D->WAKE_CTRL |= USBHS_UD_REMOTE_WKUP;
    return USB_RST_OK;
}

static usb_rst_e _set_address(uint8_t addr)
{
    USBHS1D->DEV_AD = addr & 0x7F;
    return USB_RST_OK;
}

static usb_rst_e _interrupt_ctrl(usb_bool_t status)
{
    if (status == USB_TRUE)
    {
        NVIC_EnableIRQ(USBHS1_IRQn);
    }
    else
    {
        NVIC_DisableIRQ(USBHS1_IRQn);
        __asm volatile("fence.i");
    }

    return USB_RST_OK;
}

static usb_rst_e _test_mode_ctrl(usb_req_test_select_e test_select)
{
#define TEST_MASK           0x0F

    __attribute__((aligned(4))) static uint8_t test_packet_buf[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
        0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
        0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF, 0xEF,
        0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0x7E,
    };

    switch (test_select)
    {
    case USB_REQ_TEST_SELECT_J:
        USBHS1D->TEST_MODE = USBHS1D->TEST_MODE & ~TEST_MASK | USBHS_UD_TEST_J;
        return USB_RST_OK;

    case USB_REQ_TEST_SELECT_K:
        USBHS1D->TEST_MODE = USBHS1D->TEST_MODE & ~TEST_MASK | USBHS_UD_TEST_K;

        return USB_RST_OK;

    case USB_REQ_TEST_SELECT_SE0_NAK:
        USBHS1D->TEST_MODE = USBHS1D->TEST_MODE & ~TEST_MASK | USBHS_UD_TEST_SE0NAK;
        return USB_RST_OK;

    case USB_REQ_TEST_SELECT_PACKET:
        USBHS1D->TEST_MODE &= ~TEST_MASK;
        USBHS1D->UEP4_TX_DMA = (uint32_t)test_packet_buf;
        USBHS1D->UEP4_TX_LEN = sizeof(test_packet_buf);
        USBHS1D->UEP4_TX_CTRL = USBHS_UEP_T_RES_ACK;
        USBHS1D->TEST_MODE |= USBHS_UD_TEST_PKT;
        return USB_RST_OK;

    default:
        return USB_RST_FAILED;
    }

    return USB_RST_OK;
}

static usb_rst_e _endp_open(usb_endp_t ep, uint8_t feature, uint16_t max_size)
{
    uint8_t dir = ENDP_DIR(ep);
    uint8_t num = ENDP_NUM(ep);

    if (num >= USBHS1_DCP_MAX_ENDPOINTS) return USB_RST_UNSUPPORTED_ENDP;

    uint32_t bit = 1 << num;

    if (dir)
    {
        USBHS1D->UEP_TX_EN |= bit;

        if (feature & USBD_ENDP_FEATURE_ISO)
        {
            USBHS1D->UEP_TX_ISO |= bit;
            USBHS1D->UEP_TX_TOG_AUTO &= ~bit;
        }
        else
        {
            USBHS1D->UEP_TX_ISO &= ~bit;
            USBHS1D->UEP_TX_TOG_AUTO |= bit;
        }

        if (feature & USBD_ENDP_FEATURE_BURST)
        {
            ENDP_MAX_LEN(num) = max_size;
            USBHS1D->UEP_TX_BURST |= bit;
            usbhs1d_handle.endp_params[USB_DIR_IN][num].feature |= USBD_ENDP_FEATURE_BURST;
        }
        else
        {
            USBHS1D->UEP_TX_BURST &= ~bit;
        }

        if (feature & USBD_ENDP_FEATURE_ZLP)
        {
            USBHS1D->UEP_TX_BURST_MODE &= ~bit;
            usbhs1d_handle.endp_params[USB_DIR_IN][num].feature &= ~USBD_ENDP_FEATURE_ZLP;
        }
        else
        {
            USBHS1D->UEP_TX_BURST_MODE |= bit;
        }
    }
    else
    {
        USBHS1D->UEP_RX_EN |= bit;
        ENDP_MAX_LEN(num) = max_size;

        if (num > 0)
        {
            ENDP_RX_SIZE(num) = max_size;
        }

        if (feature & USBD_ENDP_FEATURE_ISO)
        {
            USBHS1D->UEP_RX_ISO |= bit;
            USBHS1D->UEP_RX_TOG_AUTO &= ~bit;
        }
        else
        {
            USBHS1D->UEP_RX_ISO &= ~bit;
            USBHS1D->UEP_RX_TOG_AUTO |= bit;
        }

        if (feature & USBD_ENDP_FEATURE_BURST)
        {
            ENDP_RX_LEN(num) = 0;
            USBHS1D->UEP_RX_BURST |= bit;
            USBHS1D->UEP_RX_RES_MODE |= bit;
            usbhs1d_handle.endp_params[USB_DIR_OUT][num].feature |= USBD_ENDP_FEATURE_BURST;
        }
        else
        {
            USBHS1D->UEP_RX_BURST &= ~bit;
            USBHS1D->UEP_RX_RES_MODE &= ~bit;
        }
    }

    return USB_RST_OK;
}

static usb_rst_e _endp_close(usb_endp_t ep)
{
    uint8_t dir = ENDP_DIR(ep);
    uint8_t num = ENDP_NUM(ep);

    if (num >= USBHS1_DCP_MAX_ENDPOINTS) return USB_RST_UNSUPPORTED_ENDP;

    uint32_t bit = 1 << num;

    if (dir)
    {
        USBHS1D->UEP_TX_EN &= ~bit;
        USBHS1D->UEP_TX_ISO &= ~bit;
        USBHS1D->UEP_TX_BURST &= ~bit;
        USBHS1D->UEP_TX_TOG_AUTO &= ~bit;
        USBHS1D->UEP_TX_BURST_MODE &= ~bit;
    }
    else
    {
        USBHS1D->UEP_RX_EN &= ~bit;
        USBHS1D->UEP_RX_ISO &= ~bit;
        USBHS1D->UEP_RX_BURST &= ~bit;
        USBHS1D->UEP_RX_TOG_AUTO &= ~bit;
        USBHS1D->UEP_RX_RES_MODE &= ~bit;
    }

    return USB_RST_OK;
}

static usb_rst_e _set_endp_buf(usb_endp_t ep, void *buf)
{
    uint8_t num = ENDP_NUM(ep);

    if (num == 0)
    {
        USBHS1D->UEP0_DMA = (uint32_t)buf;
    }
    else if (ENDP_DIR(ep))
    {
        ENDP_TX_DMA_ADDR(num) = (uint32_t)buf;
    }
    else
    {
        ENDP_RX_DMA_ADDR(num) = (uint32_t)buf;
    }

    return USB_RST_OK;
}

static usb_rst_e _set_endp_size(usb_endp_t ep, size_t size)
{
    uint8_t num = ENDP_NUM(ep);

    if (ENDP_DIR(ep))
    {
        ENDP_TX_LEN(num) = size;
        _endp_tx_size[num] = size;
    }
    else if (num > 0)
    {
        ENDP_RX_SIZE(num) = size;
    }

    return USB_RST_OK;
}

static usb_rst_e _set_endp_toggle(usb_endp_t ep, endp_tog_e tog)
{
    uint8_t num = ENDP_NUM(ep);
    const uint8_t tog_val[] =
    {
        USBHS_UEP_T_TOG_DATA0, USBHS_UEP_T_TOG_DATA1, USBHS_UEP_T_TOG_DATA2,
        USBHS_UEP_T_TOG_MDATA,
    };

    if (ENDP_DIR(ep))
    {
        USBHS1D->UEP_TX_TOG_AUTO &= ~(1 << num);
        ENDP_TX_CTRL(num) = (ENDP_TX_CTRL(num) & ~USBHS_UEP_T_TOG_MASK) | tog_val[tog];
        USBHS1D->UEP_TX_TOG_AUTO |= 1 << num;
    }
    else
    {
        USBHS1D->UEP_RX_TOG_AUTO &= ~(1 << num);
        ENDP_RX_CTRL(num) = (ENDP_RX_CTRL(num) & ~USBHS_UEP_R_TOG_MASK) | tog_val[tog];
        USBHS1D->UEP_RX_TOG_AUTO |= 1 << num;
    }

    return USB_RST_OK;
}

static usb_rst_e _set_endp_response(usb_endp_t ep, endp_resp_e resp)
{
    uint8_t num = ENDP_NUM(ep);
    static const uint8_t resp_val[] =
    {
        USBHS_UEP_T_RES_ACK, USBHS_UEP_T_RES_ACK, USBHS_UEP_T_RES_ACK, USBHS_UEP_T_RES_NAK,
        USBHS_UEP_T_RES_STALL,
    };

    if (ENDP_DIR(ep))
    {
        ENDP_TX_CTRL(num) = (ENDP_TX_CTRL(num) & ~USBHS_UEP_T_RES_MASK) | resp_val[resp];
    }
    else
    {
        ENDP_RX_CTRL(num) = (ENDP_RX_CTRL(num) & ~USBHS_UEP_R_RES_MASK) | resp_val[resp];
    }

    return USB_RST_OK;
}

#endif // USB_DEVICE_DRIVER_EN
