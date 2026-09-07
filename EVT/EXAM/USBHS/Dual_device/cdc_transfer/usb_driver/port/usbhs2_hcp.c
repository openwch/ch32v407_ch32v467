/********************************** (C) COPYRIGHT *******************************
* File Name          : usbhs2_hcp.c
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb high speed host controller 1 port for ch32v407.
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

#include "usbhs2_hcp.h"
#include "host/usbh_driver_private.h"

#ifdef USB_HOST_DRIVER_EN

/* @function declaration */
static usb_rst_e _open(void);
static usb_rst_e _close(void);
static usb_rst_e _start_transfer(void);
static usb_rst_e _root_set_feature(hub_feature_e feature);
static usb_rst_e _root_get_status(hub_port_status_t *status);

static void _unit_transfer(usbh_xfer_unit_t *xfer);

/* @global */
usbh_handle_t usbhs2h_handle;
static usbh_xfer_unit_t *_current_xfer_unit;

usb_rst_e usbhs2_hch_init(void)
{
    memset(&usbhs2h_handle, 0, sizeof(usbhs2h_handle));

    usbhs2h_handle.open = _open;
    usbhs2h_handle.close = _close;
    usbhs2h_handle.start_transfer = _start_transfer;
    usbhs2h_handle.root_set_feature = _root_set_feature;
    usbhs2h_handle.root_get_status = _root_get_status;

    return USB_RST_OK;
}

void usbhs2_hch_interrupt(void)
{
    uint8_t int_flag = USBHS2H->INT_FLAG;

    if (int_flag & USBHS_UHIF_SOF_ACT)
    {
        usbhs2h_handle.tick++;
        usbhs2h_handle.microframe = (USBHS2H->FRAME >> 16) & 0x07;
    }

    if (int_flag & USBHS_UHIF_TRANSFER)
    {
        if (USBHS2H->PORT_STATUS & USBHS_UHIS_PORT_CONNECT)
        {
            if (_current_xfer_unit->token == USBH_PID_IN)
            {
                _current_xfer_unit->length = USBHS2H->RX_LEN;
            }
            _current_xfer_unit->pid = USBHS2H->INT_ST & 0x0F;
            _current_xfer_unit = _current_xfer_unit->next;
        }
        else
        {
            _current_xfer_unit = NULL;
        }

        if (_current_xfer_unit)
        {
            _unit_transfer(_current_xfer_unit);
        }
        else
        {
            usbhs2h_handle.xfering = USB_FALSE;
        }
    }

    USBHS2H->INT_FLAG = int_flag;

    uint8_t port_change = USBHS2H->PORT_STATUS_CHG;
    uint16_t port_status = USBHS2H->PORT_STATUS;

    if (port_change & USBHS_UHIF_PORT_CONNECT)
    {
        if (port_status & USBHS_UHIS_PORT_CONNECT)
        {
            usbh_device_insert(&usbhs2h_handle, 0, 0);
        }
        else
        {
            usbh_device_remove(&usbhs2h_handle, 0, 0);
        }
    }

    if ((port_change & USBHS_UHIF_PORT_SUSP) && ((port_status & USBHS_UHIS_PORT_SUSP) == 0))
    {
        usbh_device_resume(&usbhs2h_handle, 0, 0);
    }

    USBHS2H->PORT_STATUS_CHG = port_change;
}

static usb_rst_e _open(void)
{
    if ((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
    {
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS2, DISABLE);
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
    RCC_UTMI2cmd(ENABLE);

    /* Enable USBHS Clock */
    RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS2, ENABLE);

    USBHS2H->CFG = USBHS_RST_LINK | USBHS_UH_PHY_SUSPENDM;
    USBHS2H->PORT_CFG = USBHS_UH_PD_EN | USBHS_UH_HOST_EN;
    USBHS2H->FRAME = USBHS_UH_SOF_CNT_EN;
    USBHS2H->CFG = USBHS_UH_SOF_EN | USBHS_UD_DMA_EN | USBHS_UD_PHY_SUSPENDM;
    USBHS2H->INT_EN = USBHS_UHIE_SOF_ACT | USBHS_UHIE_TRANSFER;
    USBHS2H->PORT_INT_EN = USBHS_UHIE_PORT_SUSP | USBHS_UHIE_PORT_CONNECT;
    NVIC_EnableIRQ(USBHS2_IRQn);
    return USB_RST_OK;
}

static usb_rst_e _close(void)
{
    USBHS2H->CFG = USBHS_UD_RST_LINK | USBHS_UD_RST_SIE | USBHS_UD_CLR_ALL;
    NVIC_DisableIRQ(USBHS2_IRQn);

    RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS2, DISABLE);
    RCC_UTMI2cmd(DISABLE);
    RCC->CTLR &= ~RCC_USBHSPLLON;
    return USB_RST_OK;
}

static usb_rst_e _start_transfer(void)
{
    _current_xfer_unit = usbhs2h_handle.xfer_list;
    _unit_transfer(_current_xfer_unit);
    return USB_RST_OK;
}

static usb_rst_e _root_set_feature(hub_feature_e feature)
{
    switch (feature)
    {
    case HUB_FEATURE_PORT_SUSPEND:
        USBHS2H->PORT_CTRL |= USBHS_UH_SET_PORT_SUSP;
        break;

    case HUB_FEATURE_PORT_RESET:
        USBHS2H->PORT_CTRL |= USBHS_UH_SET_PORT_RESET;
        break;

    case HUB_FEATURE_PORT_POWER:
    case HUB_FEATURE_PORT_TEST:
    case HUB_FEATURE_PORT_INDICATOR:
        break;
    }

    return USB_RST_OK;
}

static usb_rst_e _root_get_status(hub_port_status_t *status)
{
    status->port_status = 0;
    status->port_change = 0;

    uint16_t port_status = USBHS2H->PORT_STATUS;
    status->port_status_bits.power = 1;
    if (port_status & USBHS_UHIS_PORT_CONNECT) status->port_status_bits.connect = 1;
    if (port_status & USBHS_UHIS_PORT_EN) status->port_status_bits.enable = 1;
    if (port_status & USBHS_UHIS_PORT_SUSP) status->port_status_bits.suspend = 1;
    if (port_status & USBHS_UHIS_PORT_RST) status->port_status_bits.reset = 1;
    if (port_status & USBHS_UHIS_PORT_LS) status->port_status_bits.low_speed = 1;
    if (port_status & USBHS_UHIS_PORT_HS) status->port_status_bits.high_speed = 1;
    if (port_status & USBHS_UHIS_PORT_TEST) status->port_status_bits.test_mode = 1;

    return USB_RST_OK;
}

static void _unit_transfer(usbh_xfer_unit_t *xfer)
{
    uint32_t supplement = 0;

    USBHS2H->DEV_ADDR = xfer->dev_addr;

    if (xfer->split_data.split_data)
    {
        split_data_t *split = &xfer->split_data;
        USBHS2H->SPLIT = split->split_data;

        if (split->sc == 0)
        {
            if (split->et == XFER_TYPE_ISOC || split->et == XFER_TYPE_INTR)
            {
                supplement = USBHS_UH_SPLIT_VALID | USBHS_UH_TX_NO_RES | USBHS_UH_RX_NO_DATA;
            }
            else
            {
                supplement = USBHS_UH_SPLIT_VALID | USBHS_UH_RX_NO_DATA;
            }
        }
        else
        {
            supplement = USBHS_UH_SPLIT_VALID | USBHS_UH_RX_NO_RES | USBHS_UH_TX_NO_DATA;
        }
    }
    else if (xfer->pre)
    {
        supplement = USBHS_UH_PRE_PID_EN;
    }

    if (xfer->iso)
    {
        supplement |= USBHS_UH_RX_NO_RES | USBHS_UH_TX_NO_RES;
    }

    if (xfer->token == USBH_PID_IN)
    {
        USBHS2H->RX_MAX_LEN = xfer->length;
        USBHS2H->RX_DMA = (uint32_t)xfer->buffer;
        USBHS2H->CONTROL = USBHS_UH_HOST_ACTION | xfer->token | (xfer->endp << 4) | supplement;
    }
    else if (xfer->token == USBH_PID_OUT || xfer->token == USBH_PID_SETUP)
    {
        USBHS2H->TX_LEN = xfer->length;
        USBHS2H->TX_DMA = (uint32_t)xfer->buffer;
        USBHS2H->CONTROL = USBHS_UH_HOST_ACTION | xfer->token | (xfer->endp << 4) | (xfer->toggle << 8) | supplement;
    }
    else if (xfer->token == USBH_PID_PING)
    {
        USBHS2H->TX_LEN = 0;
        USBHS2H->TX_DMA = 0;
        USBHS2H->CONTROL = USBHS_UH_HOST_ACTION | xfer->token | (xfer->endp << 4);
    }
}

#endif // USB_HOST_DRIVER_EN
