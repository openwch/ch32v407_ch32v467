/********************************** (C) COPYRIGHT *******************************
* File Name          : uac.h
* Author             : WCH
* Version            : V1.3
* Date               : 2026/08/25
* Description        : Uac class define.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __UAC_H_
#define __UAC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

/* @enum */
typedef enum
{
    UAC_SUBCLASS_UNDEFINED      = 0x00,
    UAC_SUBCLASS_AUDIOCONTROL   = 0x01,
    UAC_SUBCLASS_AUDIOSTREAMING = 0x02,
    UAC_SUBCLASS_MIDISTREAMING  = 0x03,
} uac_subclass_e;

typedef enum
{
    UAC_PROTOCOL_UNDEFINED    = 0x00,
    UAC_PROTOCOL_IP_VERSION_1 = 0x01,
} uac_protocol_e;

typedef enum
{
    UAC_CS_ITF_DESC_UNDEFINED       = 0x00,
    UAC_CS_ITF_DESC_HEADER          = 0x01,
    UAC_CS_ITF_DESC_INPUT_TERMINAL  = 0x02,
    UAC_CS_ITF_DESC_OUTPUT_TERMINAL = 0x03,
    UAC_CS_ITF_DESC_MIXER_UNIT      = 0x04,
    UAC_CS_ITF_DESC_SELECTOR_UNIT   = 0x05,
    UAC_CS_ITF_DESC_FEATURE_UNIT    = 0x06,
    UAC_CS_ITF_DESC_PROCESSING_UNIT = 0x07,
    UAC_CS_ITF_DESC_EXTENSION_UNIT  = 0x08,
} uac_cs_itf_desc_subtype_e;

typedef enum
{
    UAC_CS_AS_ITF_DESC_UNDEFINED       = 0x00,
    UAC_CS_AS_ITF_DESC_GENERAL         = 0x01,
    UAC_CS_AS_ITF_DESC_FORMAT_TYPE     = 0x02,
    UAC_CS_AS_ITF_DESC_FORMAT_SPECIFIC = 0x03,
} uac_cs_as_itf_desc_subtype_e;

typedef enum
{
    UAC_CS_EP_DESC_UNDEFINED = 0x00,
    UAC_CS_EP_DESC_GENERAL   = 0x01,
} uac_cs_ep_desc_subtype_e;

typedef enum
{
    UAC_TERMINAL_USB_UNDEFINED          = 0x0100,
    UAC_TERMINAL_USB_STREAMING          = 0x0101,
    UAC_TERMINAL_USB_VENDOR_SPEC        = 0x01FF,

    UAC_TERMINAL_IN_UNDEFINED           = 0x0200,
    UAC_TERMINAL_IN_MICROPHONE          = 0x0201,
    UAC_TERMINAL_IN_DESKTOP_MIC         = 0x0202,
    UAC_TERMINAL_IN_PERSONAL_MIC        = 0x0203,
    UAC_TERMINAL_IN_OMNI_MIC            = 0x0204,
    UAC_TERMINAL_IN_MIC_ARRAY           = 0x0205,
    UAC_TERMINAL_IN_PROC_MIC_ARRAY      = 0x0206,

    UAC_TERMINAL_OUT_UNDEFINED          = 0x0300,
    UAC_TERMINAL_OUT_SPEAKER            = 0x0301,
    UAC_TERMINAL_OUT_HEADPHONES         = 0x0302,

    UAC_TERMINAL_BIDIR_UNDEFINED        = 0x0400,
    UAC_TERMINAL_BIDIR_HANDSET          = 0x0401,
    UAC_TERMINAL_BIDIR_HEADSET          = 0x0402,

    UAC_TERMINAL_EXTERNAL_UNDEFINED     = 0x0600,
    UAC_TERMINAL_EXTERNAL_ANALOG_CONN   = 0x0601,
    UAC_TERMINAL_EXTERNAL_DIGITAL_AUDIO = 0x0602,
    UAC_TERMINAL_EXTERNAL_LINE_CONN     = 0x0603,
    UAC_TERMINAL_EXTERNAL_SPDIF         = 0x0605,
} uac_terminal_type_e;

typedef enum
{
    UAC_FORMAT_TYPE_UNDEFINED = 0x00,
    UAC_FORMAT_TYPE_I         = 0x01,
    UAC_FORMAT_TYPE_II        = 0x02,
    UAC_FORMAT_TYPE_III       = 0x03,
} uac_format_type_e;

typedef enum
{
    UAC_FORMAT_TAG_UNDEFINED  = 0x0000,
    UAC_FORMAT_TAG_PCM        = 0x0001,
    UAC_FORMAT_TAG_PCM8       = 0x0002,
    UAC_FORMAT_TAG_IEEE_FLOAT = 0x0003,
    UAC_FORMAT_TAG_ALAW       = 0x0004,
    UAC_FORMAT_TAG_MULAW      = 0x0005,
} uac_format_tag_e;

typedef enum
{
    UAC_REQ_SET_CUR    = 0x01,
    UAC_REQ_GET_CUR    = 0x81,
    UAC_REQ_SET_MIN    = 0x02,
    UAC_REQ_GET_MIN    = 0x82,
    UAC_REQ_SET_MAX    = 0x03,
    UAC_REQ_GET_MAX    = 0x83,
    UAC_REQ_SET_RES    = 0x04,
    UAC_REQ_GET_RES    = 0x84,
    UAC_REQ_SET_MEM    = 0x05,
    UAC_REQ_GET_MEM    = 0x85,
    UAC_REQ_GET_STATUS = 0xFF,
} uac_request_e;

typedef enum
{
    UAC_FU_CONTROL_UNDEFINED         = 0x00,
    UAC_FU_CONTROL_MUTE              = 0x01,
    UAC_FU_CONTROL_VOLUME            = 0x02,
    UAC_FU_CONTROL_BASS              = 0x03,
    UAC_FU_CONTROL_MID               = 0x04,
    UAC_FU_CONTROL_TREBLE            = 0x05,
    UAC_FU_CONTROL_GRAPHIC_EQUALIZER = 0x06,
    UAC_FU_CONTROL_AGC               = 0x07,
    UAC_FU_CONTROL_DELAY             = 0x08,
    UAC_FU_CONTROL_BASS_BOOST        = 0x09,
    UAC_FU_CONTROL_LOUDNESS          = 0x0A,
} uac_fu_control_selector_e;

typedef enum
{
    UAC_EP_CONTROL_UNDEFINED     = 0x00,
    UAC_EP_CONTROL_SAMPLING_FREQ = 0x01,
    UAC_EP_CONTROL_PITCH         = 0x02,
} uac_ep_control_selector_e;

typedef enum
{
    UAC_CHANNEL_LEFT_FRONT      = 0x0001,
    UAC_CHANNEL_RIGHT_FRONT     = 0x0002,
    UAC_CHANNEL_CENTER_FRONT    = 0x0004,
    UAC_CHANNEL_LOW_FREQ        = 0x0008,
    UAC_CHANNEL_LEFT_SURROUND   = 0x0010,
    UAC_CHANNEL_RIGHT_SURROUND  = 0x0020,
    UAC_CHANNEL_LEFT_OF_CENTER  = 0x0040,
    UAC_CHANNEL_RIGHT_OF_CENTER = 0x0080,
    UAC_CHANNEL_SURROUND        = 0x0100,
    UAC_CHANNEL_SIDE_LEFT       = 0x0200,
    UAC_CHANNEL_SIDE_RIGHT      = 0x0400,
    UAC_CHANNEL_TOP             = 0x0800,
} uac_channel_config_e;

/* @struct */

/* Start single-byte alignment */
#pragma pack(1)

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdADC;
    uint16_t wTotalLength;
    uint8_t bInCollection;
    uint8_t baInterfaceNr[];
} uac_ac_header_desc_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bTerminalID;
    uint16_t wTerminalType;
    uint8_t bAssocTerminal;
    uint8_t bNrChannels;
    uint16_t wChannelConfig;
    uint8_t iChannelNames;
    uint8_t iTerminal;
} uac_input_terminal_desc_t;

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
} uac_output_terminal_desc_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bTerminalLink;
    uint8_t bDelay;
    uint16_t wFormatTag;
} uac_as_general_desc_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bFormatType;
    uint8_t bNrChannels;
    uint8_t bSubFrameSize;
    uint8_t bBitResolution;
    uint8_t bSamFreqType;
    uint8_t tSamFreq[];
} uac_format_type_desc_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmAttributes;
    uint8_t bLockDelayUnits;
    uint16_t wLockDelay;
} uac_as_endp_desc_t;

/* End single-byte alignment */
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif
