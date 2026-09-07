/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_define.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Usb driver define.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USB_DEFINE_H_
#define __USB_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

/* @define */
#define USB_DRIVER_VERSION_STRING       "v1.3"
#define USB_DRIVER_VERSION_NUMBER       (0x0130)

#define USB_MAX_EP_NUM                  (16)

#define ENDP_DIR(endp)                  ((endp) & 0x80)
#define ENDP_NUM(endp)                  ((endp) & 0x7F)
#define ENDP_TYPE(endp)                 ((endp) & 0x03)
#define ENDP_MAX_SIZE(size)             ((size) & 0x07FF)
#define ENDP_ADDITIONAL(size)           (((size) >> 11) & 0x03)

#define USB_MIN(a, b)                   ((a) < (b) ? (a) : (b))
#define USB_MAX(a, b)                   ((a) > (b) ? (a) : (b))
#define USB_RANGE(a, min, max)          (USB_MAX((min), USB_MIN((a), (max))))

#define USB_REQ_BM(dir, type, rcpt)     (((dir) << 7) | ((type) << 5) | (rcpt))
#define USB_ARRAY_SIZE(a)               (sizeof(a) / sizeof((a)[0]))

#define U16_HIGH(_u16)                  ((uint8_t)(((_u16) >> 8) & 0x00FF))
#define U16_LOW(_u16)                   ((uint8_t)(((_u16) >> 0) & 0x00FF))
#define U16_TO_U8S_BE(_u16)             U16_HIGH(_u16), U16_LOW(_u16)
#define U16_TO_U8S_LE(_u16)             U16_LOW(_u16), U16_HIGH(_u16)

#define U32_BYTE3(_u32)                 ((uint8_t) ((((uint32_t) _u32) >> 24) & 0x000000FF))
#define U32_BYTE2(_u32)                 ((uint8_t) ((((uint32_t) _u32) >> 16) & 0x000000FF))
#define U32_BYTE1(_u32)                 ((uint8_t) ((((uint32_t) _u32) >>  8) & 0x000000FF))
#define U32_BYTE0(_u32)                 ((uint8_t) ((((uint32_t) _u32) >>  0) & 0x000000FF))

#define U32_TO_U8S_BE(_u32)             U32_BYTE3(_u32), U32_BYTE2(_u32), U32_BYTE1(_u32), U32_BYTE0(_u32)
#define U32_TO_U8S_LE(_u32)             U32_BYTE0(_u32), U32_BYTE1(_u32), U32_BYTE2(_u32), U32_BYTE3(_u32)

/* @typedef */
typedef uint8_t                         usb_bool_t;
typedef uint8_t                         usb_endp_t;

/* Return result enum type */
typedef enum
{
    USB_RST_OK,
    USB_RST_FAILED,
    USB_RST_NULL_PTR,
    USB_RST_OVERFLOW,
    USB_RST_EXISTS,
    USB_RST_ERROR_PARAMETER,
    USB_RST_ENDP_BUSY,
    USB_RST_ENDP_STALL,
    USB_RST_UNSUPPORTED_ENDP,
    USB_RST_UNSUPPORTED_EVENT,
    USB_RST_UNSUPPORTED_SOF,
} usb_rst_e;

typedef enum
{
    USB_FALSE,
    USB_TRUE,
} usb_bool_e;

typedef enum
{
    USB_SPEED_FULL,
    USB_SPEED_LOW,
    USB_SPEED_HIGH,
    USB_SPEED_SUPER,
    USB_SPEED_UNKNOW,
} usb_speed_e;

typedef enum
{
    USB_DIR_OUT,
    USB_DIR_IN,
} usb_dir_e;

typedef enum
{
    XFER_TYPE_CTRL,
    XFER_TYPE_ISOC,
    XFER_TYPE_BULK,
    XFER_TYPE_INTR,
} xfer_type_e;

typedef enum
{
    ENDP_RESP_NONE,
    ENDP_RESP_ACK,
    ENDP_RESP_NYET,
    ENDP_RESP_NAK,
    ENDP_RESP_STALL,
} endp_resp_e;

typedef enum
{
    ENDP_TOG_DATA0,
    ENDP_TOG_DATA1,
    ENDP_TOG_DATA2,
    ENDP_TOG_MDATA,
} endp_tog_e;

typedef enum
{
    ENDP_STA_IDLE,
    ENDP_STA_BUSY,
    ENDP_STA_STALL,
} endp_sta_e;

typedef enum
{
    CTRL_STAGE_SETUP,
    CTRL_STAGE_DATA,
    CTRL_STAGE_STATUS,
} ctrl_stage_e;

typedef enum
{
    USB_CLASS_PER_INTERFACE         = 0x00,
    USB_CLASS_AUDIO                 = 0x01,
    USB_CLASS_COMM                  = 0x02,
    USB_CLASS_HID                   = 0x03,
    USB_CLASS_PHYSICAL              = 0x05,
    USB_CLASS_IMAGE                 = 0x06,
    USB_CLASS_PRINTER               = 0x07,
    USB_CLASS_MSC                   = 0x08,
    USB_CLASS_HUB                   = 0x09,
    USB_CLASS_CDC_DATA              = 0x0A,
    USB_CLASS_SMART_CARD            = 0x0B,
    USB_CLASS_CONTENT_SEC           = 0x0D,
    USB_CLASS_VIDEO                 = 0x0E,
    USB_CLASS_PERSONAL_HEALTHCARE   = 0x0F,
    USB_CLASS_AUDIO_VIDEO           = 0x10,
    USB_CLASS_BILLBOARD             = 0x11,
    USB_CLASS_TYPE_C_BRIDGE         = 0x12,
    USB_CLASS_BULK_DISPLAY          = 0x13,
    USB_CLASS_MCTP_DEVICE           = 0x14,
    USB_CLASS_I3C_DEVICE            = 0x3C,
    USB_CLASS_DIAGNOSTIC_DEVICE     = 0xDC,
    USB_CLASS_WIRELESS_CONTROLLER   = 0xE0,
    USB_CLASS_MISC                  = 0xEF,
    USB_CLASS_APP_SPEC              = 0xFE,
    USB_CLASS_VENDOR_SPEC           = 0xFF,
} usb_class_code_e;

typedef enum
{
    USB_REQ_CODE_GET_STATUS         = 0x00,
    USB_REQ_CODE_CLEAR_FEATURE      = 0x01,
    USB_REQ_CODE_SET_FEATURE        = 0x03,
    USB_REQ_CODE_SET_ADDRESS        = 0x05,
    USB_REQ_CODE_GET_DESCRIPTOR     = 0x06,
    USB_REQ_CODE_SET_DESCRIPTOR     = 0x07,
    USB_REQ_CODE_GET_CONFIGURATION  = 0x08,
    USB_REQ_CODE_SET_CONFIG         = 0x09,
    USB_REQ_CODE_GET_INTERFACE      = 0x0A,
    USB_REQ_CODE_SET_INTERFACE      = 0x0B,
    USB_REQ_CODE_SYNCH_FRAME        = 0x0C,
} usb_req_code_e;

typedef enum
{
    USB_REQ_FEATURE_ENDPOINT_HALT,
    USB_REQ_FEATURE_REMOTE_WAKEUP,
    USB_REQ_FEATURE_TEST_MODE,
} usb_req_feature_e;

typedef enum
{
    USB_REQ_TEST_SELECT_RESERVED,
    USB_REQ_TEST_SELECT_J,
    USB_REQ_TEST_SELECT_K,
    USB_REQ_TEST_SELECT_SE0_NAK,
    USB_REQ_TEST_SELECT_PACKET,
    USB_REQ_TEST_SELECT_FORCE_ENABLE,
} usb_req_test_select_e;

typedef enum
{
    USB_REQ_TYPE_STANDARD,
    USB_REQ_TYPE_CLASS,
    USB_REQ_TYPE_VENDOR,
    USB_REQ_TYPE_INVALID,
} usb_req_type_e;

typedef enum
{
    USB_REQ_RCPT_DEVICE,
    USB_REQ_RCPT_INTERFACE,
    USB_REQ_RCPT_ENDPOINT,
    USB_REQ_RCPT_OTHER,
} usb_req_recipient_e;

typedef enum
{
    USB_DESC_DEVICE                 = 0x01,
    USB_DESC_CONFIGURATION          = 0x02,
    USB_DESC_STRING                 = 0x03,
    USB_DESC_INTERFACE              = 0x04,
    USB_DESC_ENDPOINT               = 0x05,
    USB_DESC_DEVICE_QUALIFIER       = 0x06,
    USB_DESC_OTHER_SPEED_CONFIG     = 0x07,
    USB_DESC_INTERFACE_POWER        = 0x08,
    USB_DESC_OTG                    = 0x09,
    USB_DESC_DEBUG                  = 0x0A,
    USB_DESC_INTERFACE_ASSOCIATION  = 0x0B,

    USB_DESC_BOS                    = 0x0F,
    USB_DESC_DEVICE_CAPABILITY      = 0x10,

    USB_DESC_FUNCTIONAL             = 0x21,

    // Class Specific Descriptor
    USB_DESC_CS_DEVICE              = 0x21,
    USB_DESC_CS_CONFIGURATION       = 0x22,
    USB_DESC_CS_STRING              = 0x23,
    USB_DESC_CS_INTERFACE           = 0x24,
    USB_DESC_CS_ENDPOINT            = 0x25,
} usb_desc_type_e;

typedef enum
{
    HUB_FEATURE_C_HUB_LOCAL_POWER   = 0x00,
    HUB_FEATURE_C_HUB_OVER_CURRENT  = 0x01,

    HUB_FEATURE_PORT_CONNECTION     = 0x00,
    HUB_FEATURE_PORT_ENABLE         = 0x01,
    HUB_FEATURE_PORT_SUSPEND        = 0x02,
    HUB_FEATURE_PORT_OVER_CURRENT   = 0x03,
    HUB_FEATURE_PORT_RESET          = 0x04,
    HUB_FEATURE_PORT_POWER          = 0x08,
    HUB_FEATURE_PORT_LOW_SPEED      = 0x09,
    HUB_FEATURE_C_PORT_CONNECTION   = 0x10,
    HUB_FEATURE_C_PORT_ENABLE       = 0x11,
    HUB_FEATURE_C_PORT_SUSPEND      = 0x12,
    HUB_FEATURE_C_PORT_OVER_CURRENT = 0x13,
    HUB_FEATURE_C_PORT_RESET        = 0x14,
    HUB_FEATURE_PORT_TEST           = 0x15,
    HUB_FEATURE_PORT_INDICATOR      = 0x16,
} hub_feature_e;

/* @struct */
typedef struct
{
    union
    {
        uint8_t bmRequestType;

        struct
        {
            uint8_t recipient : 5;
            uint8_t type : 2;
            uint8_t direction : 1;
        } bmRequestType_bit;
    };

    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_req_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} desc_device_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t bNumConfigurations;
    uint8_t bReserved;
} desc_qua_t;

/* Start single-byte alignment */
#pragma pack(1)

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} desc_config_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} desc_interface_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} desc_endpoint_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
    uint8_t bRefresh;
    uint8_t bSynchAddress;
} desc_endp_iso_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
} desc_iad_t;

/* End single-byte alignment */
#pragma pack()

typedef struct
{
    union
    {
        uint32_t split_data;

        struct
        {
            uint32_t hub_addr : 7;
            uint32_t sc : 1;
            uint32_t port : 7;
            uint32_t s : 1;
            uint32_t e : 1;
            uint32_t et : 2;
            uint32_t reserved : 13;
        };
    };

    uint16_t iso_out_length;
    uint16_t iso_out_offset;
} split_data_t;

typedef struct
{
    union
    {
        uint16_t port_status;

        struct
        {
            uint16_t connect : 1;
            uint16_t enable : 1;
            uint16_t suspend : 1;
            uint16_t over_current : 1;
            uint16_t reset : 1;
            uint16_t reserved1 : 3;
            uint16_t power : 1;
            uint16_t low_speed : 1;
            uint16_t high_speed : 1;
            uint16_t test_mode : 1;
            uint16_t port_indicator : 1;
            uint16_t reserved2 : 3;
        } port_status_bits;
    };

    union
    {
        uint16_t port_change;

        struct
        {
            uint16_t connect : 1;
            uint16_t enable : 1;
            uint16_t suspend : 1;
            uint16_t over_current : 1;
            uint16_t reset : 1;
        } port_change_bits;
    };
} hub_port_status_t;

#ifdef __cplusplus
}
#endif

#endif
