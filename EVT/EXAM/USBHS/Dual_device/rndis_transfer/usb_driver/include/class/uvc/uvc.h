/********************************** (C) COPYRIGHT *******************************
* File Name          : uvc.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Uvc class define.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __UVC_H_
#define __UVC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

/* @enum */
typedef enum
{
    UVC_SUBCLASS_UNDEFINED                  = 0x00,
    UVC_SUBCLASS_VIDEOCONTROL               = 0x01,
    UVC_SUBCLASS_VIDEOSTREAMING             = 0x02,
    UVC_SUBCLASS_VIDEO_INTERFACE_COLLECTION = 0x03,
} uvc_subclass_e;

typedef enum
{
    UVC_PROTOCOL_UNDEFINED = 0x00,
    UVC_PROTOCOL_15        = 0x01,
} uvc_protocol_e;

typedef enum
{
    UVC_CS_VC_ITF_DESC_UNDEFINED        = 0x00,
    UVC_CS_VC_ITF_DESC_HEADER           = 0x01,
    UVC_CS_VC_ITF_DESC_INPUT_TERMINAL   = 0x02,
    UVC_CS_VC_ITF_DESC_OUTPUT_TERMINAL  = 0x03,
    UVC_CS_VC_ITF_DESC_SELECTOR_UNIT    = 0x04,
    UVC_CS_VC_ITF_DESC_PROCESSING_UNIT  = 0x05,
    UVC_CS_VC_ITF_DESC_EXTENSION_UNIT   = 0x06,
    UVC_CS_VC_ITF_DESC_ENCODING_UNIT    = 0x07,
} uvc_cs_vc_itf_desc_subtype_e;

typedef enum
{
    UVC_CS_VS_ITF_DESC_UNDEFINED              = 0x00,
    UVC_CS_VS_ITF_DESC_INPUT_HEADER           = 0x01,
    UVC_CS_VS_ITF_DESC_OUTPUT_HEADER          = 0x02,
    UVC_CS_VS_ITF_DESC_STILL_IMAGE_FRAME      = 0x03,
    UVC_CS_VS_ITF_DESC_FORMAT_UNCOMPRESSED    = 0x04,
    UVC_CS_VS_ITF_DESC_FRAME_UNCOMPRESSED     = 0x05,
    UVC_CS_VS_ITF_DESC_FORMAT_MJPEG           = 0x06,
    UVC_CS_VS_ITF_DESC_FRAME_MJPEG            = 0x07,
    UVC_CS_VS_ITF_DESC_FORMAT_MPEG2TS         = 0x0A,
    UVC_CS_VS_ITF_DESC_FORMAT_DV              = 0x0C,
    UVC_CS_VS_ITF_DESC_COLORFORMAT            = 0x0D,
    UVC_CS_VS_ITF_DESC_FORMAT_FRAME_BASED     = 0x10,
    UVC_CS_VS_ITF_DESC_FRAME_FRAME_BASED      = 0x11,
    UVC_CS_VS_ITF_DESC_FORMAT_STREAM_BASED    = 0x12,
    UVC_CS_VS_ITF_DESC_FORMAT_H264            = 0x13,
    UVC_CS_VS_ITF_DESC_FRAME_H264             = 0x14,
    UVC_CS_VS_ITF_DESC_FORMAT_H264_SVC        = 0x15,
    UVC_CS_VS_ITF_DESC_FRAME_H264_SVC         = 0x16,
    UVC_CS_VS_ITF_DESC_FORMAT_H264_MVC        = 0x17,
    UVC_CS_VS_ITF_DESC_FRAME_H264_MVC         = 0x18,
} uvc_cs_vs_itf_desc_subtype_e;

typedef enum
{
    UVC_TERMINAL_USB_UNDEFINED       = 0x0100,
    UVC_TERMINAL_USB_STREAMING       = 0x0101,

    UVC_TERMINAL_IN_UNDEFINED        = 0x0200,
    UVC_TERMINAL_IN_CAMERA           = 0x0201,
    UVC_TERMINAL_IN_MEDIA_TRANSPORT  = 0x0202,

    UVC_TERMINAL_OUT_UNDEFINED       = 0x0300,
    UVC_TERMINAL_OUT_GENERIC_DISPLAY = 0x0301,
    UVC_TERMINAL_OUT_MEDIA_TRANSPORT = 0x0302,

    UVC_TERMINAL_EXTERNAL_UNDEFINED  = 0x0400,
    UVC_TERMINAL_EXTERNAL_COMPOSITE  = 0x0401,
    UVC_TERMINAL_EXTERNAL_SVIDEO     = 0x0402,
    UVC_TERMINAL_EXTERNAL_COMPONENT  = 0x0403,
} uvc_terminal_type_e;

typedef enum
{
    UVC_REQ_SET_CUR  = 0x01,
    UVC_REQ_GET_CUR  = 0x81,
    UVC_REQ_GET_MIN  = 0x82,
    UVC_REQ_GET_MAX  = 0x83,
    UVC_REQ_GET_RES  = 0x84,
    UVC_REQ_GET_LEN  = 0x85,
    UVC_REQ_GET_INFO = 0x86,
    UVC_REQ_GET_DEF  = 0x87,
} uvc_request_e;

typedef enum
{
    UVC_ENTITY_VC_ITF = 0x00,
} uvc_entity_id_e;

typedef enum
{
    UVC_VC_CTRL_VIDEO_POWER_MODE   = 0x01,
    UVC_VC_CTRL_REQUEST_ERROR_CODE = 0x02,
} uvc_vc_ctrl_e;

typedef enum
{
    UVC_VS_CTRL_PROBE                   = 0x01,
    UVC_VS_CTRL_COMMIT                  = 0x02,
    UVC_VS_CTRL_STILL_PROBE             = 0x03,
    UVC_VS_CTRL_STILL_COMMIT            = 0x04,
    UVC_VS_CTRL_STILL_IMAGE_TRIGGER     = 0x05,
    UVC_VS_CTRL_STREAM_ERROR_CODE       = 0x06,
} uvc_vs_ctrl_e;

typedef enum
{
    UVC_CT_CTRL_SCANNING_MODE         = 0x01,
    UVC_CT_CTRL_AUTO_EXPOSURE_MODE    = 0x02,
    UVC_CT_CTRL_AUTO_EXPOSURE_PRIORITY = 0x03,
    UVC_CT_CTRL_EXPOSURE_TIME_ABSOLUTE = 0x04,
    UVC_CT_CTRL_EXPOSURE_TIME_RELATIVE = 0x05,
    UVC_CT_CTRL_FOCUS_ABSOLUTE        = 0x06,
    UVC_CT_CTRL_FOCUS_RELATIVE        = 0x07,
    UVC_CT_CTRL_FOCUS_AUTO            = 0x08,
    UVC_CT_CTRL_IRIS_ABSOLUTE         = 0x09,
    UVC_CT_CTRL_IRIS_RELATIVE         = 0x0A,
    UVC_CT_CTRL_ZOOM_ABSOLUTE         = 0x0B,
    UVC_CT_CTRL_ZOOM_RELATIVE         = 0x0C,
    UVC_CT_CTRL_PANTILT_ABSOLUTE      = 0x0D,
    UVC_CT_CTRL_PANTILT_RELATIVE      = 0x0E,
    UVC_CT_CTRL_ROLL_ABSOLUTE         = 0x0F,
    UVC_CT_CTRL_ROLL_RELATIVE         = 0x10,
    UVC_CT_CTRL_PRIVACY               = 0x11,
    UVC_CT_CTRL_FOCUS_SIMPLE          = 0x12,
    UVC_CT_CTRL_WINDOW                = 0x13,
    UVC_CT_CTRL_REGION_OF_INTEREST    = 0x14,
} uvc_ct_ctrl_e;

typedef enum
{
    UVC_PU_CTRL_BACKLIGHT_COMPENSATION    = 0x01,
    UVC_PU_CTRL_BRIGHTNESS                = 0x02,
    UVC_PU_CTRL_CONTRAST                  = 0x03,
    UVC_PU_CTRL_GAIN                      = 0x04,
    UVC_PU_CTRL_POWER_LINE_FREQUENCY      = 0x05,
    UVC_PU_CTRL_HUE                       = 0x06,
    UVC_PU_CTRL_SATURATION                = 0x07,
    UVC_PU_CTRL_SHARPNESS                 = 0x08,
    UVC_PU_CTRL_GAMMA                     = 0x09,
    UVC_PU_CTRL_WHITE_BALANCE_TEMPERATURE = 0x0A,
    UVC_PU_CTRL_WHITE_BALANCE_COMPONENT   = 0x0B,
    UVC_PU_CTRL_DIGITAL_MULTIPLIER        = 0x0C,
    UVC_PU_CTRL_DIGITAL_MULTIPLIER_LIMIT  = 0x0D,
    UVC_PU_CTRL_HUE_AUTO                  = 0x0E,
    UVC_PU_CTRL_ANALOG_VIDEO_STANDARD     = 0x0F,
    UVC_PU_CTRL_ANALOG_LOCK_STATUS        = 0x10,
    UVC_PU_CTRL_CONTRAST_AUTO             = 0x11,
} uvc_pu_ctrl_e;

typedef enum
{
    UVC_POWER_MODE_READY = 0x00,
    UVC_POWER_MODE_IDLE  = 0x01,
} uvc_power_mode_e;

typedef enum
{
    UVC_CTRL_INFO_GET         = 0x01,
    UVC_CTRL_INFO_SET         = 0x02,
    UVC_CTRL_INFO_DISABLED    = 0x04,
    UVC_CTRL_INFO_AUTOUPDATE  = 0x08,
    UVC_CTRL_INFO_ASYNCHRONOUS = 0x10,
} uvc_ctrl_info_e;

typedef enum
{
    UVC_PROBE_HINT_FRAME_INTERVAL = 0x0001,
    UVC_PROBE_HINT_KEY_FRAME_RATE = 0x0002,
    UVC_PROBE_HINT_PFRAME_RATE    = 0x0004,
    UVC_PROBE_HINT_COMP_QUALITY   = 0x0008,
    UVC_PROBE_HINT_COMP_WINDOW    = 0x0010,
} uvc_probe_hint_e;

typedef enum
{
    UVC_STILL_CAPTURE_NONE     = 0x00,
    UVC_STILL_CAPTURE_METHOD_1 = 0x01,
    UVC_STILL_CAPTURE_METHOD_2 = 0x02,
    UVC_STILL_CAPTURE_METHOD_3 = 0x03,
} uvc_still_capture_method_e;

/* @struct */

/* Start single-byte alignment */
#pragma pack(1)

// VC Interface Header Descriptor (Subtype 0x01)
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdUVC;
    uint16_t wTotalLength;
    uint32_t dwClockFrequency;
    uint8_t bInCollection;
    uint8_t baInterfaceNr[];
} uvc_vc_header_desc_t;

// Camera Terminal Descriptor (Subtype 0x02)
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bTerminalID;
    uint16_t wTerminalType;
    uint8_t bAssocTerminal;
    uint8_t iTerminal;
    uint16_t wObjectiveFocalLengthMin;
    uint16_t wObjectiveFocalLengthMax;
    uint16_t wOcularFocalLength;
    uint8_t bControlSize;
    uint8_t bmControls[];
} uvc_camera_terminal_desc_t;

// Output Terminal Descriptor (Subtype 0x03)
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bTerminalID;
    uint16_t wTerminalType;
    uint8_t bAssocTerminal;
    uint8_t bSourceID;
    uint8_t iTerminal;
} uvc_output_terminal_desc_t;

// VS Interface Input Header Descriptor (Subtype 0x01)
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bNumFormats;
    uint16_t wTotalLength;
    uint8_t bEndpointAddress;
    uint8_t bmInfo;
    uint8_t bTerminalLink;
    uint8_t bStillCaptureMethod;
    uint8_t bTriggerSupport;
    uint8_t bTriggerUsage;
    uint8_t bControlSize;
    uint8_t bmaControls[];
} uvc_vs_input_header_desc_t;

// VS Format Uncompressed Descriptor (Subtype 0x04)
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bFormatIndex;
    uint8_t bNumFrameDescriptors;
    uint8_t guidFormat[16];
    uint8_t bBitsPerPixel;
    uint8_t bDefaultFrameIndex;
    uint8_t bAspectRatioX;
    uint8_t bAspectRatioY;
    uint8_t bmInterlaceFlags;
    uint8_t bCopyProtect;
} uvc_format_uncompressed_desc_t;

// VS Frame Uncompressed Descriptor (Subtype 0x05)
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bFrameIndex;
    uint8_t bmCapabilities;
    uint16_t wWidth;
    uint16_t wHeight;
    uint32_t dwMinBitRate;
    uint32_t dwMaxBitRate;
    uint32_t dwMaxVideoFrameBufferSize;
    uint32_t dwDefaultFrameInterval;
    uint8_t bFrameIntervalType;
    uint8_t tFrameInterval[];
} uvc_frame_uncompressed_desc_t;

// VS Color Format Descriptor (Subtype 0x0D)
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bColorPrimaries;
    uint8_t bTransferCharacteristics;
    uint8_t bMatrixCoefficients;
} uvc_color_format_desc_t;

// Information Structure
typedef struct
{
    uint16_t bmHint;
    uint8_t bFormatIndex;
    uint8_t bFrameIndex;
    uint32_t dwFrameInterval;
    uint16_t wKeyFrameRate;
    uint16_t wPFrameRate;
    uint16_t wCompQuality;
    uint16_t wCompWindowSize;
    uint16_t wDelay;
    uint32_t dwMaxVideoFrameSize;
    uint32_t dwMaxPayloadTransferSize;
    uint32_t dwClockFrequency;
    uint8_t bmFramingInfo;
    uint8_t bPreferedVersion;
    uint8_t bMinVersion;
    uint8_t bMaxVersion;
    uint8_t bUsage;
    uint8_t bBitDepthLuma;
    uint8_t bmSettings;
    uint8_t bMaxNumberOfRefSlicesPlus1;
    uint16_t bmRateControlModes;
    uint64_t bmLayoutPerStream;
} uvc_info_t;

/* End single-byte alignment */
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif
