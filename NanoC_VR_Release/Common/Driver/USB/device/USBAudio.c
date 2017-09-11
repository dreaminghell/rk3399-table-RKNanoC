/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: USBAudio.c
Desc:

Author:
Date: 13-07-26
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/
#include "SysInclude.h"
#include "USBConfig.h"

#ifdef USB_AUDIO

#include <stdio.h>
#include <string.h>
#include "USBAudioBuffer.h"
#include "AudioControl.h"
#include "usbhidrkvrdata.h"

//_ATTR_USB_AUDIO_DATA_   DMA_CFGX AudioControlDmaCfg1={DMA_CTLL_I2S_TX, DMA_CFGL_I2S_TX, DMA_CFGH_I2S_TX,0};

extern void UsbAudioAudioInit(void);
extern void usbAudio_buf_puts(const unsigned char *s, unsigned int len);
extern unsigned int usbAudio_buf_len(void);



/*------------------------------------ Defines -------------------------------*/

/// Playback input terminal ID.
#define USB_AUDIO_INPUT_TERMINAL      1
/// Playback output terminal ID.
#define USB_AUDIO_OUTPUT_TERMINAL     2
/// Playback feature unit ID.
#define USB_AUDIO_FEATURE_UNIT        3


#define USB_AUDIO_EPOUT_NUM           2
#define USB_HID_EPIN_NUM              3
//#define USB_HID_EPOUT_NUM             2

#define MAX_USB_AUDIO_CUR             0x80f0
#define MAX_USB_AUDIO_VOL             0x80ff //根据音量表进行映射调整
#define MIN_USB_AUDIO_VOL             0x8000
#define MAX_USB_AUDIO_RES             0x0001

/* Audio Class-Specific Request Codes */
#define UAC_RC_UNDEFINED				0x00
#define UAC_SET_CUR					    0x01
#define UAC_GET_INT                     0x0A
#define UAC_GET_CUR					    0x81
#define UAC_GET_MIN						0x82
#define UAC_GET_MAX					    0x83
#define UAC_GET_RES				        0x84

//HID 类请求协议补充:
#define HID_REQ_GET_REPORT              0x01
#define HID_REQ_SET_REPORT              0x09
#define HID_SET_IDLE                    0x0A
extern int vr_nanoc_suspend;
/*
#define SENSOR_CONFIG                   0x02 //(A1)
#define SENSOR_FACTORY_CALIBRATION      0x03 //(A1)
#define SENSOR_RANGE                    0x04 //(21)
#define SENSOR_DATA                     0x06 //(A1)

#define SENSOR_SERIAL                   0x05 //CMY 设置的0X0A
#define SENSOR_UUID                     0x13 //()
#define SENSOR_TEMPERATURE              0x14 //()
#define SENSOR_DISPLAY_INFO             0x09 //()
*/





/*----------------------------------- Typedefs -------------------------------*/

typedef volatile struct tagUAS_DEVICE
{
    uint8  LastVolume;
    uint8  ConSel;
    uint8  SensorSetReg;
    uint8  connected;
    uint8  AlterInterface;
    USB_DEVICE   *pDev;

} UAS_DEVICE, *pUAS_DEVICE;

typedef enum _CONTROL_STATUS
{
    AUDIO_NO_CONTROL,
    AUDIO_VOLUME_SET,
    AUDIO_MUTE_STATUS
}
CONTROL_STATUS;
/*-------------------------- Forward Declarations ----------------------------*/

static int32 USBAudioSetup(USB_CTRL_REQUEST *ctrl, USB_DEVICE *pDev);
static void USBAudioReqest(uint8 epnum, uint32 event, uint32 param, USB_DEVICE *pDev);
static void USBAudioResume(USB_DEVICE *pDev);
static void USBAudioSuspend(USB_DEVICE *pDev);
static void USBAudioDisconnect(USB_DEVICE *pDev);


/* ------------------------------- Globals ---------------------------------- */


/*-------------------------------- Local Statics: ----------------------------*/

_ATTR_USB_AUDIO_DATA_
USB_AUDIO_CONFIGS_DESCRIPTOR HSAudioConfigs =
{
    {
    sizeof(USB_CONFIGURATION_DESCRIPTOR),                   // size of the descriptor 9
    USB_DT_CONFIG,                                          // type of the configuration descriptor02(1B)
    sizeof(USB_AUDIO_CONFIGS_DESCRIPTOR),                   //  wTotalLength  110 bytes
    0x03,                                                   // bNumInterfaces
    0x1,                                                    // bConfigurationValue
    0x0,                                                    // iConfiguration
    0xC0,                                                   // bmAttributes  BUS Powred
    0x32                                                    // bMaxPower = 100 mA
    },

    //Audio Control  interface descriptor
    {
    sizeof(USB_INTERFACE_DESCRIPTOR),                               //size of  usb interface
    USB_DT_INTERFACE,                                               //type of USB standard descriptor
    0x00,                                                           //interface number
    0x00,                                                           //bAlternateSetting
    0x00,                                                           //bNumEndpoints
    USB_DEVICE_CLASS_AUDIO,                                         //bInterfaceClass
    USB_SUBCLASS_CODE_AUDIO_CONTROL,                                //bInterfaceSubClass
    USB_AUDIO_CODE_INTERFACE_PROTOCOL_UNDEFINED,                    //bInterfaceProtocol
    0x00                                                            //iInterface
    },
    //String Descriptor Index

    //Audio Control specific header interface descriptor
    {
    sizeof(USB_CS_AC_INTERFACE_DESCRIPTOR_HEADER),                  // size of USB audio class specified interface header descriptor
    USB_AUDIO_CLASS_SPECIFIC_DESCRIPTOR_TYPE_INTERFACE,             // type of USB audio class specialied descriptor
    USB_AUDIO_CLASS_CS_AC_ID_SUBTYPE_HEADER,                        // subtype of usb audio class  control
    0x0100,                                                         // bcdADC
    sizeof(USB_CS_AC_INTERFACE_DESCRIPTOR_HEADER)+                  // 9 size of control descriptor
    sizeof(USB_CS_AC_INTERFACE_DESCRIPTOR_INPUT_TERMINAL)+          // 12
    sizeof(USB_CS_AC_INTERFACE_DESCRIPTOR_FEATURE_UNIT)+            // 10
    sizeof(USB_CS_AC_INTERFACE_DESCRIPTOR_OUTPUT_TERMINAL),         // 9                                                    //wTotalLength
    0x01,                                                           //Number of Streaming interfaces
    0x01                                                            //bmControls the interface belong to this control
    },

    //Audio Control Specific input terminal interface descriptor
    {
    sizeof(USB_CS_AC_INTERFACE_DESCRIPTOR_INPUT_TERMINAL),          // 1 size of USB audio class specified interface input terminal descriptor
    USB_AUDIO_CLASS_SPECIFIC_DESCRIPTOR_TYPE_INTERFACE,             // 1 type of USB audio class specified   descriptor
    USB_AUDIO_CLASS_CS_AC_ID_SUBTYPE_INPUT_TERMINAL,                // 1 subtype of usb audio class control
    USB_AUDIO_INPUT_TERMINAL,                                       // 1 bTerminalID
    0x0101,                                                         // 2 wTerminalType
    0x00,                                                           // 1 bAssocTerminal
    0x02,                                                           // 1 bNrChannels
    0x0003,                                                         // 2 bmChannelConfig
    0x00,                                                           // 1 iChannelNames
    0x00                                                            // 1 iTerminal
    },
    //iTerminal Index of a string descriptor

    //Audio Control Specific output terminal interface descriptor
    {
    sizeof(USB_CS_AC_INTERFACE_DESCRIPTOR_OUTPUT_TERMINAL),         // 1 bLength
    USB_AUDIO_CLASS_SPECIFIC_DESCRIPTOR_TYPE_INTERFACE,             // 1 bDescriptorType
    USB_AUDIO_CLASS_CS_AC_ID_SUBTYPE_OUTPUT_TERMINAL,               // 1 bDescriptorSubType
    USB_AUDIO_OUTPUT_TERMINAL,                                      // 1 bTerminalID
    0x0301,                                                         // 2 wTerminalType
    0x00,                                                           // 1 bAssocTerminal
    USB_AUDIO_FEATURE_UNIT,                                         // 1 bSourceID
    0x00                                                            // 1 iTerminal
    },

    // Audio Control Feature Unit
    //Audio Control Specific feature terminal interface descriptor
    {
    sizeof(USB_CS_AC_INTERFACE_DESCRIPTOR_FEATURE_UNIT),            // 1 bLength
    USB_AUDIO_CLASS_SPECIFIC_DESCRIPTOR_TYPE_INTERFACE,             // 1 bDescriptorType
    USB_AUDIO_CLASS_CS_AC_ID_SUBTYPE_FEATURE_UNIT,                  // 1 bDescriptorSubType
    USB_AUDIO_FEATURE_UNIT,                                         // 1 bUnitID
    USB_AUDIO_INPUT_TERMINAL,                                       // bSourceId
    0x01,                                                           // 1 bControlSize
    0x03,                                                           // 2 bmaControls0
    0X00,                                                           // bmaControls1
    0X00,                                                           // bmaControls2
    0x00                                                            // 1 iFeature
    },

    //Audio streaming interface with 0 endpoints
    //Interface 1, Alternate Setting 0
    {
    sizeof(USB_INTERFACE_DESCRIPTOR),                               // size of  usb interface
    USB_DT_INTERFACE,                                               // type of USB standard descriptor
    0x01,                                                           // number of endpoint
    0x00,                                                           // replace of up
    0x00,                                                           // endpoint it used
    USB_DEVICE_CLASS_AUDIO,                                         // Class Type ,USB_DEVICE_CLASS_STORAGE=Mass Storage
    USB_SUBCLASS_CODE_AUDIO_STREAMING,                              // Class Sub Type(1B),"0x06=Reduced Block Commands(RBC)"
    USB_AUDIO_CODE_INTERFACE_PROTOCOL_UNDEFINED,                    // USB protocol type(1B),"0X50=Mass Storage Class Bulk-Only Transport"
    0x00                                                            // String Descriptor Index
    },

    //Audio streaming interface with data endpoint
    //Interface 1, Alternate Setting 1
    {
    sizeof(USB_INTERFACE_DESCRIPTOR),                               // size of  usb interface
    USB_DT_INTERFACE,                                               // type of USB standard descriptor
    0x01,                                                           // number of endpoint
    0x01,                                                           // replace of up
    0x01,                                                           // endpoint it used
    USB_DEVICE_CLASS_AUDIO,                                         // Class Type ,USB_DEVICE_CLASS_STORAGE=Mass Storage
    USB_SUBCLASS_CODE_AUDIO_STREAMING,                              // Class Sub Type(1B),"0x06=Reduced Block Commands(RBC)"
    USB_AUDIO_CODE_INTERFACE_PROTOCOL_UNDEFINED,                    // USB protocol type(1B),"0X50=Mass Storage Class Bulk-Only Transport"
    0x00                                                            //String Descriptor Index
    },

    // Audio Stream General
    //Audio Stream Specific general  interface descriptor
    {
    sizeof(USB_CS_AS_INTERFACE_GENERAL_DESCRIPTOR),                 // 1 bLength
    USB_AUDIO_CLASS_SPECIFIC_DESCRIPTOR_TYPE_INTERFACE,             // 1 bDescriptorType
    USB_AUDIO_CLASS_CS_AS_ID_SUBTYPE_AS_GENERAL,                    // 1 bDescriptorSubtype
    USB_AUDIO_INPUT_TERMINAL,                                       // bTerminalLink
    0x01,//00                                                       // No internal delay because of data path
    0x0001                                                          // wFormatTag PCM format
    },

    //Audio Stream Specific format  interface descriptor
    {
    sizeof(USB_CS_AS_INTERFACE_FORMAT_DESCRIPTOR),                  // 1 bLength
    USB_AUDIO_CLASS_SPECIFIC_DESCRIPTOR_TYPE_INTERFACE,             // 1 bDescriptorType
    USB_AUDIO_CLASS_CS_AS_ID_SUBTYPE_FORMAT_TYPE,                   // 1 bDescriptorSubType
    USB_AC_FORMAT_TYPE_I,                                           // 1 bFormatType
    0x02,                                                           // 1 bNrChannels
    0x02,                                                           // 1 bSubframeSize
    0x10,                                                           // 1 bBitResolution
    0x01,                                                           // 1 bSamFreqType
    {0x80,0xBB,0x00}                                                // 3 tSamFreq
    },

    // Audio streaming endpoint standard descriptor
    {
    sizeof(USB_AS_ENDPOINT_DESCRIPTOR),                             // 9bytes
    USB_DT_ENDPOINT,                                                // usb endpoint descriptor
    AS_OUT_EP,                                                      // endpoint number
    (USB_EPTYPE_ISOC | 0x08),                                       // type isochronous |0x08 for SynchronisationType
    192,                                                            // HS_isochronous_RX_SIZE,
    1,                                                              // Polling interval = 2^(x-1) milliseconds (1 ms)
    0,                                                              // This is not a synchronization endpoint
    0                                                               // No associated synchronization endpoint                                                         // bulk trans invailed
    },

    //Audio Stream Specific   endpoint descriptor
    {
    sizeof(USB_AS_DATA_ENDPOINT_DESCRIPTOR),                                                               // bLength
    USB_AUDIO_CLASS_SPECIFIC_DESCRIPTOR_TYPE_ENDPOINT,              // bDescriptorType
    0x01,                                                           // Descriptor subtype for an Audio data endpoint.
    0x01,//0                                                                       // bmAttributes
    0x01,//0                                                                       // bLockDelayUnits
    0x0001,                                                         // wLockDelay

    },
#if 1
    /******Sensor HID Device*****/
    //HID Interface
    {
    sizeof(USB_INTERFACE_DESCRIPTOR),                               // blength
    USB_DT_INTERFACE,                                               // interface type
    0x02,                                                           // bInterfaceNumber: Number of Interface
    0x00,                                                           // bAlternateSetting: Alternate setting
    0x02,                                                           // bNumEndpoints
    0x03,                                                           // bInterfaceClass: HID
    0x00,                                                           // unkown  sub class
    0x00,                                                           // interface protol
    0x00                                                            // index
    },

    //HID Class Descriptor
    {
    sizeof(USB_HID_CLASS_DESCRIPTOR),                               // blength
    0x21,                                                           // bDescriptorType: HID
    0x0201,                                                         // bcdHid 1.2
    0x00,                                                           // bCountryCode: Hardware target country
    0x01,                                                           // bNumDescriptors: Number of HID class descriptors to follow
    0x22,                                                           // report descriptor type
    sizeof(USB_HID_REPORT_DESCRIPTOR_KEY)                        // report descriptor length
    },

    //HID In Endpoint
    {
    sizeof(USB_ENDPOINT_DESCRIPTOR),                                // blength
    USB_DT_ENDPOINT,                                                // endpoint type
    (HID_IN_EP_SENSOR|0x80),                                        // bEndpointAddress: Endpoint Address (IN)
    USB_EPTYPE_INTR,                                                // bmAttributes: Interrupt endpoint
    0x0040,                                                         // wMaxPacketSize: 2 Bytes max (0x0002)
    0x01,                                                           // bInterval: Polling Interval (64 ms 0x40)
    },

    //HID Out Endpoint
    {
    sizeof(USB_ENDPOINT_DESCRIPTOR),                                // blength
    USB_DT_ENDPOINT,                                                // endpoint type
    HID_IN_EP_SENSOR,                                               // bEndpointAddress: Endpoint Address (OUT)
    USB_EPTYPE_INTR,                                                // bmAttributes: Interrupt endpoint
    0x0040,                                                         // wMaxPacketSize: 2 Bytes max(0x0002)
    0x01,                                                           // bInterval: Polling Interval (10 ms)
    },
#endif
#if 0
    /******Key HID Device*****/
    //HID Interface
    {
    sizeof(USB_INTERFACE_DESCRIPTOR),                               // blength
    USB_DT_INTERFACE,                                               // interface type
    0x03,                                                           // bInterfaceNumber: Number of Interface
    0x00,                                                           // bAlternateSetting: Alternate setting
    0x02,                                                           // bNumEndpoints
    0x03,                                                           // bInterfaceClass: HID
    0x00,                                                           // unkown  sub class
    0x00,                                                           // interface protol
    0x00                                                            // index
    },

    //HID Class Descriptor
    {
    sizeof(USB_HID_CLASS_DESCRIPTOR),                               // blength
    0x21,                                                           // bDescriptorType: HID
    0x0201,                                                         // bcdHid 1.2
    0x00,                                                           // bCountryCode: Hardware target country
    0x01,                                                           // bNumDescriptors: Number of HID class descriptors to follow
    0x22,                                                           // report descriptor type
    sizeof(USB_HID_REPORT_DESCRIPTOR_KEY)                           // report descriptor length
    },

    //HID In Endpoint
    {
    sizeof(USB_ENDPOINT_DESCRIPTOR),                                // blength
    USB_DT_ENDPOINT,                                                // endpoint type
    (HID_IN_EP_KEY|0x80),                                              // bEndpointAddress: Endpoint Address (IN)
    USB_EPTYPE_INTR,                                                // bmAttributes: Interrupt endpoint
    0x0040,                                                         // wMaxPacketSize: 2 Bytes max (0x0002)
    0x01,                                                           // bInterval: Polling Interval (64 ms 0x40)
    },

    {
    sizeof(USB_ENDPOINT_DESCRIPTOR),                                // blength
    USB_DT_ENDPOINT,                                                // endpoint type
    HID_IN_EP_KEY,                                                     // bEndpointAddress: Endpoint Address (OUT)
    USB_EPTYPE_INTR,                                                // bmAttributes: Interrupt endpoint
    0x0040,                                                         // wMaxPacketSize: 2 Bytes max(0x0002)
    0x01,                                                           // bInterval: Polling Interval (64 ms)
    }
#endif
};

#if 1
//Key Report Descriptor
_ATTR_USB_AUDIO_DATA_
USB_HID_REPORT_DESCRIPTOR_KEY USBHidReport_Key =
{
#if 0
    0x0c05,     //usage page(Consumer)
    0x0109,     //usage(consumer control)
    0x01a1,     //collection

            0x0175,     //report size(1)
            0x0895,     //report cnt(8)
            0x0015,     //logical minimum(0)
            0x0125,     //logical maxmum(1)
            0xe909,     //usage(volume up)
            0xea09,     //usage(volume down)
            0xe209,     //usage(volume mute)
            0xcd09,     //usage(play and stop)
            0xb509,     //scan (next track)
            0xb609,     //scan (prev track)
            0xb309,     //fast forward
            0xb709,     //usage (stop)
            0x4281,     //input
    0xc0,       //end collection
#endif
    0x0c05,     //usage page(Consumer)
    0x0109,     //usage(consumer control)
    0x01a1,     //collection
            0x0875,     //report size(8)
            0x4095,     //report cnt(64)
            0x0015,     //logical minimum(0)
            0x0125,     //logical maxmum(1)
            0x0109,     //usage(volume up)
            0x0109,     //usage(volume down)
            0x0109,     //usage(volume mute 0xe209)
            0x0109,     //usage(play and stop)
            0x0109,     //scan (next track)
            0x0109,     //scan (prev track)
            0x0109,     //fast forward
            0x0109,     //usage (stop)
            0x4281,     //input
    0xc0,       //end collection

};


//Sensor Report Descriptor
_ATTR_USB_AUDIO_DATA_
USB_HID_REPORT_DESCRIPTOR_SENSOR USBHidReport_Sensor =
{
    0x05, 0x03, //(usage page: VR control)   [Global Item]
    0x09, 0x05, //(usage: Head Tracker)      [Local Item]
    0xa1, 0x01, //(Collection: application)  [Main Item] 0xa1, 0x01
              0x06, 0x02, 0xff, //(usage page: Reserved)               [Global Item]
              0x09, 0x01,       //(usage:  belt)                       [Local Item]0x09, 0x01
              0xa1, 0x00,       //(Collection:Physical(group of axes)) [Main Item]
              0x19, 0x01,       //(usage minimum: 0x01)                [Local Item]
              0x29, 0x20,       //(usage maximum: 0x20)                [Local Item]
              0x15, 0x00,       //(Logical minimum: 0x00)              [Global Item]
              0x25, 0x01,       //(Logical maximum: 0x01)              [Global Item]
              0x95, 0x20,       //(Report count: 0x20)                 [Global Item]
              0x75, 0x08,       //(Report size: 0x08 单位:bit)         [Global Item]
              //Input:(Volatile|No Null Position|Preferred state|Linear|No Wrap|Absolute|Variable|Data)
              0x81, 0x02,       //(Input)                              [Main Item]
              0xc0,             //(End Collection)                     [Main Item]
    0xc0,       //(End Collection)           [Main Item]
};
#endif

_ATTR_USB_AUDIO_DATA_
USB_DEVICE_DESCRIPTOR HSAudioDeviceDescr =
{
    sizeof(USB_DEVICE_DESCRIPTOR),              //descriptor's size 18(1B)
    USB_DT_DEVICE,                              //descriptor's type 01(1B)
    0x0200,                                      //USB plan distorbution number (2B)
    0,                                          //1type code (point by USB)(1B),0x00
    0, 0,                                       //child type and protocal (usb alloc)(2B)
    EP0_PACKET_SIZE,                            //endpoint 0 max package length(1B)
    0x071b,
    0x3205,
    0x0200,								        // device serial number
    USB_STRING_MANUFACTURER,
    USB_STRING_PRODUCT,
    USB_STRING_SERIAL,                          //producter,produce,device serial number index(3B)
    1                                           //feasible configuration parameter(1B)
};


_ATTR_USB_AUDIO_BSS_
static UAS_DEVICE USBAudioDev;

_ATTR_USB_AUDIO_DATA_
static USB_DEVICE USBAudioDriver =
{
    //USB_SPEED_HIGH,
    USB_SPEED_FULL,
    USB_CLASS_TYPE_AUDIO,
    sizeof(USB_DEVICE_DESCRIPTOR),
    &HSAudioDeviceDescr,
    sizeof(USB_AUDIO_CONFIGS_DESCRIPTOR),
    &HSAudioConfigs,
    &HSAudioConfigs,
    USBAudioSetup,
    USBAudioSuspend,
    USBAudioResume,
    USBAudioDisconnect,
    USBAudioReqest,
    &USBAudioDev,
    NULL,
    NULL
};

_ATTR_USB_AUDIO_BSS_ static uint16 I2SbufferIndex;
_ATTR_USB_AUDIO_BSS_ static uint16 USBbufferIndex;
_ATTR_USB_AUDIO_BSS_ static uint32 DMAStarted;
_ATTR_USB_AUDIO_BSS_ static uint32 USBOutputBuf;
_ATTR_USB_AUDIO_BSS_ static uint32 I2SInputBuf;
_ATTR_USB_AUDIO_BSS_ static uint32 USBOutputLen;

_ATTR_USB_AUDIO_BSS_    uint8 UsbAudioControlStatus;
_ATTR_USB_AUDIO_BSS_    uint8 AddOdd ;
_ATTR_USB_AUDIO_BSS_    uint8 AudioEnumState;

_ATTR_USB_AUDIO_BSS_
static __align(4) uint8  UASBuffer[3*0x0f*192/4];

_ATTR_USB_AUDIO_DATA_
static DMA_CFGX UASDmaCfg1={DMA_CTLL_I2S_TX, DMA_CFGL_I2S_TX, DMA_CFGH_I2S_TX,0};


/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

/*
Name:       USBAudioOutPkt
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioReInitOutEp(void)
{
    DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;

    if(AddOdd)
    {
        dev_regs->out_ep[AS_OUT_EP].doepctl &= ~(1ul<<29);
        dev_regs->out_ep[AS_OUT_EP].doepctl |= (1ul<<28);
        AddOdd = 0;
    }
    else
    {
        dev_regs->out_ep[AS_OUT_EP].doepctl |= (1ul<<29);
        dev_regs->out_ep[AS_OUT_EP].doepctl &= ~(1ul<<28);
        AddOdd = 1;
    }

    dev_regs->out_ep[AS_OUT_EP].doeptsiz |= (1ul<<19) | 192;
    dev_regs->out_ep[AS_OUT_EP].doepctl|= (1ul<<26);
}

/*
Name:       USBAudioOutPkt
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioOutPkt(pUAS_DEVICE pAud, uint16 len, uint8 SOF)
{
    USBReadEp(AS_OUT_EP, len, (void *)USBOutputBuf);
    USBAudioReInitOutEp();
    usbAudio_buf_puts((void *)USBOutputBuf, len);
}

/*
Name:       USBAudioGetDesc
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static int32 USBAudioGetDesc(USB_CTRL_REQUEST *ctrl, USB_DEVICE *pDev)
{
    uint8 bTarget = (ctrl->bRequestType & 0x0F);
    int32 ret = -1;

    if (bTarget == USB_RECIP_INTERFACE)
    {
        uint8 type = (ctrl->wValue >> 8) & 0xff;
        uint8 IntfNum =  (ctrl->wIndex) & 0xff;

        if(IntfNum == 0x02) //HID Interface //sensor
        {
            if(type == 0x22) //report descriptor
            {
                uint32 len;
                USB_EP0_REQ *ep0req = USBGetEp0Req();

                len = MIN(sizeof(USB_HID_REPORT_DESCRIPTOR_KEY), ctrl->wLength);
                printf ("HID Sensor Report Des len=%d\n",len);
                memcpy(ep0req->buf, (uint8*)&USBHidReport_Key, len);
                ep0req->dir = 1;
                ep0req->NeedLen = len;
                USBStartEp0Xfer(ep0req);

                ret = 0;
            }
        }
        if(IntfNum == 0x03) //HID Interface key
        {
            if(type == 0x22) //report descriptor
            {
                uint32 len;
                USB_EP0_REQ *ep0req = USBGetEp0Req();

                len = MIN(sizeof(USB_HID_REPORT_DESCRIPTOR_KEY), ctrl->wLength);
                printf ("HID KEY Report Des len=%d\n",len);
                memcpy(ep0req->buf, (uint8*)&USBHidReport_Key, len);
                ep0req->dir = 1;
                ep0req->NeedLen = len;
                USBStartEp0Xfer(ep0req);
                ret = 0;
            }
        }
    }


    return ret;
}

/*
Name:       USBAudioSetConfig
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioSetConfig(UAS_DEVICE *pAud, uint8 config)
{
    if (config)
    {
        USBEnableEp(AS_OUT_EP, (USB_ENDPOINT_DESCRIPTOR *)&HSAudioConfigs.ASOutEndp);
        USBEnableEp(HID_IN_EP_SENSOR|0x80, &HSAudioConfigs.HIDInEndp); //from Nanoc->Host
        //USBEnableEp(HID_IN_EP_SENSOR, &HSAudioConfigs.HIDOutEndp); //from Host->Nanoc
        //USBEnableEp(HID_IN_EP_KEY|0x80, &HSAudioConfigs.HIDInEndp_Key); //from Nanoc->Host
        //USBEnableEp(HID_IN_EP_KEY, &HSAudioConfigs.HIDOutEndp_Key); //from Nanoc->Host
        pAud->connected = 1;
    }
    USBInEp0Ack();
}

/*
Name:       USBAudioStandardReq
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static int32 USBAudioStandardReq(USB_CTRL_REQUEST *ctrl, USB_DEVICE *pDev)
{
    UAS_DEVICE *pAud = (UAS_DEVICE *)pDev->pFunDev;
    int32 ret = 0;

    switch (ctrl->bRequest)
    {
        case USB_REQ_GET_DESCRIPTOR:
            ret = USBAudioGetDesc(ctrl, pDev);
            break;

        /* One config, two speeds */
        case USB_REQ_SET_CONFIGURATION:
            USBAudioSetConfig(pAud, ctrl->wValue & 0x1);
            printf ("\nSET_CONFIGURATION(wValue)=0x%x wIndex=0x%x\n",ctrl->wValue, ctrl->wIndex);
            break;

         case USB_REQ_SET_INTERFACE:
            pAud->AlterInterface = ctrl->wValue & 0x1;
            printf ("\nSET_INTERFACE(wValue)=0x%x wIndex=0x%x\n",ctrl->wValue, ctrl->wIndex);
            USBInEp0Ack();
            break;


        case USB_REQ_GET_INTERFACE:
    	    {
                uint8 intf = pAud->AlterInterface;
                printf ("\nGET_INTERFACE=%d\n",intf);
                USBWriteEp0(1, &intf);
            }
            break;
        default:
            break;
    }

    return ret;
}

/*
Name:       USBAudioEp0Cmpl
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioEp0Cmpl(void *ptr)
{
    USB_DEVICE *pDev = (USB_DEVICE *)ptr;
    pUAS_DEVICE pAud = (pUAS_DEVICE)pDev->pFunDev;
    USB_EP0_REQ *ep0req = USBGetEp0Req();
    int32 value = 0;
    int32 ValueLow = 0;
    if (pAud->ConSel == 0x02)   //VOLUME_CONTROL
    {
        value = __get_unaligned_be16(ep0req->buf);
        ValueLow = (value & 0x00ff);
        value = ((value  >> 8) | (ValueLow <<8));              //high and low exchange
        // DEBUG("the value is %x",value);
        if(value ==  MIN_USB_AUDIO_VOL || value ==  0x8000)
        {
            pAud->LastVolume = 0;
            gSysConfig.OutputVolume = 0;
            Codec_SetVolumet(0);
            SendMsg(MSG_USB_DISPFLAG_VOL);
        }
        else if(value < MIN_USB_AUDIO_VOL || value > MAX_USB_AUDIO_VOL)
        {
            return;
        }
        else if(value ==  MAX_USB_AUDIO_VOL)
        {

            pAud->LastVolume = MAX_VOLUME;
            gSysConfig.OutputVolume = MAX_VOLUME;
            Codec_SetVolumet(MAX_VOLUME);
            SendMsg(MSG_USB_DISPFLAG_VOL);
        }
        else
        {
            value = ((value - MIN_USB_AUDIO_VOL)*1000 /(MAX_USB_AUDIO_VOL - MIN_USB_AUDIO_VOL)*MAX_VOLUME/1000) + 1;
            gSysConfig.OutputVolume = value;
            pAud->LastVolume = value;
            Codec_SetVolumet(value);
            SendMsg(MSG_USB_DISPFLAG_VOL);
        }

    }
    else if (pAud->ConSel == 0x01) //MUTE_CONTROL
    {
        value = ep0req->buf[0];
        if(value)//mute
        {

            Codec_DACMute();
        }
    else //unmute
    {

        Codec_DACUnMute();
    }
    }

}
int vr_nanoc_suspend;
_ATTR_USB_AUDIO_CODE_
static void USBMPUEp0Cmpl(void *ptr)
{
    USB_DEVICE *pDev = (USB_DEVICE *)ptr;
    pUAS_DEVICE pAud = (pUAS_DEVICE)pDev->pFunDev;
    USB_EP0_REQ *ep0req = USBGetEp0Req();

    if(pAud->SensorSetReg&& ep0req->NeedLen > 2) {
		if(ep0req->buf[0] == HID_REPORT_ID_RKVR) {
			switch(ep0req->buf[1]){
				case RKVR_ID_IDLE:
					if(ep0req->buf[2] == 1) {
						printf ("nanoc resume...\n");
						vr_nanoc_suspend = 0;
                        SendMsg(MSG_VR_RESUME);
					} else {
						printf ("nanoc suspend...\n");
						vr_nanoc_suspend = 1;
						ClearMsg(MSG_USB_SUSPEND);
                        SendMsg(MSG_VR_SUSPEND);
					}
					break;
				case RKVR_ID_SYNC:
					if (!sync_check(&ep0req->buf[2], ep0req->NeedLen - 2 )) {
						printf ("sync check ok\n");
					} else {
						printf ("sync check fail\n");
					}
					break;
				default:
					printf ("default OK\n");
					break;	
			}
		} else if(ep0req->buf[0] == HID_REPORT_ID_W && ep0req->buf[1]&0x80) {
            MPU6500_Write(ep0req->buf[2],ep0req->buf[3]);
        }
        pAud->SensorSetReg = 0;
    } 
}

/*
Name:       USBAudioClassReq
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioClassReq(USB_CTRL_REQUEST *ctrl, USB_DEVICE *pDev)
{
    uint32 tmp = 0;
    uint8 Sensor_data[64] = {0};
    pUAS_DEVICE pAud = (pUAS_DEVICE)pDev->pFunDev;
    USB_EP0_REQ *ep0req = USBGetEp0Req();

    int16 accel_data[3]={0};
    uint16 accel_tmp[3]={0};

    int16 gyro_data[3] ={0};
    uint16 gyro_tmp[3] ={0};
    uint8 temperature[2] ={0};

    //printf ("USBAudioClassReq bRequest=0x%x\n",ctrl->bRequest);
    switch (ctrl->bRequestType)
    {
        case (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE): //0x21 Data(Host->Device)
        {
            switch(ctrl->bRequest)
            {
                case UAC_SET_CUR|HID_REQ_GET_REPORT:
                {
				    uint8 ConSel = (ctrl->wValue >> 8) & 0xFF;
                    if (ConSel == 0x02 || ConSel == 0x01)           //VOLUME_CONTROL || MUTE_CONTROL
                    {
                        pAud->ConSel = ConSel;
                        ep0req->dir = 0;
                        ep0req->complete = USBAudioEp0Cmpl;
                        ep0req->NeedLen = ctrl->wLength;
                        USBStartEp0Xfer(ep0req);
					}
                    break;
                }

                case UAC_GET_INT|HID_SET_IDLE:
                {
                    uint8 Value = (ctrl->wValue >> 8) & 0xFF;
                    if (ctrl->wIndex == 0x0002) //Sesnor HID(接口2)
                    {
                        if (Value != 0)
                        {
                            //打开Sensor
                            printf ("Sensor Enable\n");
                            #ifdef _MPU6500_
                            gSysConfig.UsbSensor = 1;
                            //MPU6500_GyroEnable(1);
                            //MPU6500_AccelEnable(1);
                            #endif
                        }
                        USBInEp0Ack();
                    }
                    else if (ctrl->wIndex == 0x0003) //Key HID(接口3)
                    {
                        USBInEp0Ack();
                    }
                    break;
                }
                case HID_REQ_SET_REPORT: /*bRequest **/
                {
                    if (0x0002 == ctrl->wIndex ) { /*binterfacenumber sesnor hid **/
						uint8 id = ctrl->wValue & 0xFF;
						if (id== HID_REPORT_ID_RKVR) {
                            ep0req->dir = 0;
                            ep0req->complete = USBMPUEp0Cmpl;
                            ep0req->NeedLen = ctrl->wLength;
                            USBStartEp0Xfer(ep0req);
                            pAud->SensorSetReg = 1;
						}
                    } else {
                        USBInEp0Ack();
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }

        case (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE)://0xA1 Data(Device->Host)
        {
            switch(ctrl->bRequest)
            {
                //HID 自定义协议补充:
                case HID_REQ_GET_REPORT:
                {
				    uint8 Value = (ctrl->wValue >> 8) & 0xFF;
                    uint8 data[64]={0};
                    if (Value == HID_REGR_REPORT) {/* 0x01 INPUT REPORT0x02 OUTPUT REPORT0x03 FEATURE REPORT **/
                        uint8 reg = ctrl->wValue & 0xFF;
                        int len = ctrl->wLength - 1;
                        if(0 < len) {
                            printf ("HID_OUTPUT_READREG %d,%d\n", reg, len);
                            if(OK == MPU6500_Read(reg|0x80, &data[1], len) ){
                                USBWriteEp0(ctrl->wLength, data);
                            }
                        }
                    } else if (Value == HID_MISC_REPORT) {
						printf("---->USB_DIR_IN:HID_MISC_REPORT\n");
						if ( RKVR_ID_CAPS == (ctrl->wValue & 0xFF)) {
							printf("HID_MISC_REPORT:nanoc support suspend\n");
							data[1] = 1;
							USBWriteEp0(ctrl->wLength, data);
						}
					} else if(Value == HID_ENCRYPTR_REPORT) {
						printf("---->USB_DIR_IN:HID_ENCRYPT_REPORT\n");
						USBWriteEp0(ctrl->wLength, data);
                    } else if (Value == 0x01) { //report Input
                        if (ctrl->wIndex == 0x0002) //Sesnor HID(接口2)
                        {
                            //打开Sensor
                            #ifdef _MPU6500_
								printf ("start sensor\n");
	                            //MPU6500_GyroEnable(1);
	                            //MPU6500_AccelEnable(1);
                            #endif
							USBInEp0Ack();
							/*
                            //获取Sensor 数据,第一次采样
                            MPU6500_Read_Data(accel_data,1,gyro_data,1);
                            printf ("\n1:accel %4d %4d %4d gyro %8d %8d %8d\n",
                                     accel_data[0],
                                     accel_data[1],
                                     accel_data[2],
                                     gyro_data[0],
                                     gyro_data[1],
                                     gyro_data[2]);
                            accel_tmp[0] = accel_data[0];
                            accel_tmp[1] = accel_data[1];
                            accel_tmp[2] = accel_data[2];

                            gyro_tmp[0] = gyro_data[0];
                            gyro_tmp[1] = gyro_data[1];
                            gyro_tmp[2] = gyro_data[2];

                            MPU6500_Read_Temperature(temperature);

                            //Message_Sensors
                            data[0] = 0x01; //No.ReportID is Sensor
                            //SampleCount
                            data[1] = 0x02;
                            //Timestamp
                            data[2] = 0x00; //Time Low bit
                            data[3] = 0x00; //Time High bit
                            //KEY STATUS
                            data[4] = 0x00;
                            data[5] = 0x00;
                            //Temperature
                            data[6] = temperature[0];//:Temperature 低字节
                            data[7] = temperature[1];//:Temperature 高字节

                            data[8]  = (accel_tmp[0]&0x00ff); //accel_x_l
                            data[9]  = (accel_tmp[0]>>8);     //accel_x_h
                            data[10] = (accel_tmp[1]&0x00ff); //accel_y_l
                            data[11] = (accel_tmp[1]>>8);     //accel_y_h
                            data[12] = (accel_tmp[2]&0x00ff); //accel_z_l
                            data[13] = (accel_tmp[2]>>8);     //accel_z_h

                            data[14] = (gyro_tmp[0]&0x00ff);  //gyro_x_l
                            data[15] = (gyro_tmp[0]>>8);      //gyro_x_h
                            data[16] = (gyro_tmp[1]&0x00ff);  //gyro_y_l
                            data[17] = (gyro_tmp[1]>>8);      //gyro_y_h
                            data[18] = (gyro_tmp[2]&0x00ff);  //gyro_z_l
                            data[19] = (gyro_tmp[2]>>8);      //gyro_z_h

                            //获取Sensor 数据,SampleCount=2,表示第二次采样
                            memset (accel_data, 0, 6);
                            memset (gyro_data, 0, 6);
                            MPU6500_Read_Data(accel_data,1,gyro_data,1);
                            accel_tmp[0] = accel_data[0];
                            accel_tmp[1] = accel_data[1];
                            accel_tmp[2] = accel_data[2];

                            gyro_tmp[0] = gyro_data[0];
                            gyro_tmp[1] = gyro_data[1];
                            gyro_tmp[2] = gyro_data[2];
                            printf ("\n2:accel %4d %4d %4d gyro %8d %8d %8d\n",
                                     accel_data[0],
                                     accel_data[1],
                                     accel_data[2],
                                     gyro_data[0],
                                     gyro_data[1],
                                     gyro_data[2]);

                            data[20]  = (accel_tmp[0]&0x00ff); //accel_x_l
                            data[21]  = (accel_tmp[0]>>8);     //accel_x_h
                            data[22] = (accel_tmp[1]&0x00ff); //accel_y_l
                            data[23] = (accel_tmp[1]>>8);     //accel_y_h
                            data[24] = (accel_tmp[2]&0x00ff); //accel_z_l
                            data[25] = (accel_tmp[2]>>8);     //accel_z_h
                            data[26] = (gyro_tmp[0]&0x00ff);  //gyro_x_l
                            data[27] = (gyro_tmp[0]>>8);      //gyro_x_h
                            data[28] = (gyro_tmp[1]&0x00ff);  //gyro_y_l
                            data[29] = (gyro_tmp[1]>>8);      //gyro_y_h
                            data[30] = (gyro_tmp[2]&0x00ff);  //gyro_z_l
                            data[31] = (gyro_tmp[2]>>8);      //gyro_z_h
                            USBWriteEp0(ctrl->wLength, data);*/
                        }
                        else if (ctrl->wIndex == 0x0003) //Key HID(接口3)
                        {
                            USBWriteEp0(ctrl->wLength, data);
                        }

                    }
                    break;
                }
                case UAC_GET_CUR:
                {
                    if(ctrl->wLength == 0x01)
                        tmp = 0x00;
                    else if(ctrl->wLength == 0x02)
                        tmp = MAX_USB_AUDIO_CUR;                //当前值，留下根据系统的音量设置
                    USBWriteEp0(ctrl->wLength, &tmp);
                    break;
                }
                case UAC_GET_RES:                               //分辨率属性
                {
                    tmp = MAX_USB_AUDIO_RES;
                    USBWriteEp0(2, &tmp);
                    break;
                }
                case UAC_GET_MAX:
                {
                    //max volume
                    tmp = MAX_USB_AUDIO_VOL;
                    USBWriteEp0(2, &tmp);
                    break;
                }
                case UAC_GET_MIN:
                {
                    // min volume
                    tmp = MIN_USB_AUDIO_VOL;
                    USBWriteEp0(2, &tmp);
                    break;
                }
                case 0x0A:
                {
                    USBInEp0Ack();
                    break;
                }

                default:
                {
                    break;
                }
            }
            break;
        }
        case (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_ENDPOINT): //0x82
        {
            USBInEp0Ack();
            break;
        }

        case (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_ENDPOINT): //0xA2
        {
            switch(ctrl->bRequest)
            {
                case UAC_GET_CUR|HID_REQ_GET_REPORT:
                {
                    if (ctrl->wIndex == 0x0002) //Sesnor HID(接口2)
                    {
                        if(ctrl->wLength == 0x03)
                            tmp = 0xFFFF80;
                        USBWriteEp0(ctrl->wLength, &tmp);
                    }
                    else if (ctrl->wIndex == 0x0003) //Key HID(接口3)
                    {
                        //USBWriteEp0(ctrl->wLength, &tmp);
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }

        default:
            break;
    }

}

/*
Name:       USBAudioSetup
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static int32 USBAudioSetup(USB_CTRL_REQUEST *ctrl, USB_DEVICE *pDev)
{
    uint8 type = ctrl->bRequestType & USB_TYPE_MASK;
    int32 ret = 0;

	if (type == USB_TYPE_STANDARD)
    {
    	ret = USBAudioStandardReq(ctrl, pDev);
    }
	else if (type == USB_TYPE_CLASS)
	{
		USBAudioClassReq(ctrl, pDev);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

/*
Name:       USBAudioReqest
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioReqest(uint8 epnum, uint32 event, uint32 param, USB_DEVICE *pDev)
{
    pUAS_DEVICE pAud = (pUAS_DEVICE)pDev->pFunDev;

    if (epnum == AS_OUT_EP)
    {
        if (event == UDC_EV_OUT_PKT_RCV)
        {
            USBAudioOutPkt(pAud, (uint16)param, (uint8)(param>>16));
        }
        return;
    }

    if (event == UDC_EV_ENUM_DONE)
    {
        if ((uint8)param == USB_SPEED_FULL)
        {
            USB_AUDIO_CONFIGS_DESCRIPTOR *ptr = &HSAudioConfigs;
            ptr->ASOutEndp.wMaxPacketSize = FS_BULK_TX_SIZE;
            AudioEnumState = USB_AUDIO_ENUM_COMPLETED;
        }
    }
}

/*
Name:       USBAudioResume
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioResume(USB_DEVICE *pDev)
{
//    SendMsg(MSG_USB_RESUMED);
}

/*
Name:       USBAudioSuspend
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioSuspend(USB_DEVICE *pDev)
{
//    pUAS_DEVICE pAud = (pUAS_DEVICE)pDev->pFunDev;
//    pAud->connected = 0;
//    SendMsg(MSG_USB_DISCONNECT);
    SendMsg(MSG_USB_SUSPEND);
}

/*
Name:       USBAudioDisconnect
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
static void USBAudioDisconnect(USB_DEVICE *pDev)
{
    pUAS_DEVICE pAud = (pUAS_DEVICE)pDev->pFunDev;

    pAud->connected = 0;
	printf("send MSG_USB_DISCONNECT...\n");
    SendMsg(MSG_USB_DISCONNECT);
}

/*
Name:       USBAudioOperCmd
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
#if 1
_ATTR_USB_AUDIO_CODE_
void USBAudioOperCmd(uint8 bCmd)
{
    uint8 AudioOper = 0;
    int i = 0;

    switch(bCmd)
    {
        case AUDIO_CMD_NONE:
            printf ("CMD_NONE\n");
            AudioOper = 0;
            USBWriteEp(HID_IN_EP_KEY, 1, &AudioOper);
            break;

        case AUDIO_CMD_PLAY_SKIP:
            printf ("A\n");

            break;

        case AUDIO_CMD_VOLUME_UP:
            printf ("b\n");
            AudioOper |=  (1 <<AUDIO_CMD_VOLUME_UP);
            USBWriteEp(HID_IN_EP_KEY, 1, &AudioOper);

            break;

        case AUDIO_CMD_VOLUME_DOWN:
            printf ("c\n");
            break;

        case AUDIO_CMD_VOLUME_MUTE:
            printf ("d\n");
            break;

        case AUDIO_CMD_VOLUME_CD:
            printf ("e\n");
            break;

        case AUDIO_CMD_SCAN_NEXT_TRACK:
            printf ("f\n");
            break;

        case AUDIO_CMD_SCAN_PREV_TRACK:
            printf ("g\n");
            AudioOper |=  (1 <<AUDIO_CMD_VOLUME_DOWN);
            USBWriteEp(HID_IN_EP_KEY, 1, &AudioOper);
            break;

        case AUDIO_CMD_FAST_FORWARD:
            printf ("h\n");
            break;

        case AUDIO_CMD_STOP:
            printf ("CMD_STOP 0\n");
            AudioOper |=  (1 <<AUDIO_CMD_VOLUME_CD);
            USBWriteEp(HID_IN_EP_KEY, 1, &AudioOper);
            break;
    }

}
#endif

/*
Name:       USBAudioInit
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
int32 USBAudioInit(void *arg)
{
    uint32 i;
    pUAS_DEVICE pAud = &USBAudioDev;

    memset(pAud, 0, sizeof(UAS_DEVICE));

    AddOdd          = 0;
    USBOutputLen     = 0;
    DMAStarted       = 0;
    AudioEnumState = USB_AUDIO_ENUM_UNFINISH;
    UsbAudioControlStatus = AUDIO_NO_CONTROL;
    USBbufferIndex   = I2SbufferIndex = 0;
    USBOutputBuf     = I2SInputBuf =(uint32)UASBuffer;

    pAud->pDev = &USBAudioDriver;
    USBDriverProbe(&USBAudioDriver);

    return 0;
}

/*
Name:       USBAudioDeInit
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
void USBAudioDeInit(void)
{
    if (TRUE == ThreadCheck(pMainThread, &MusicThread))
    {
        ThreadDelete(&pMainThread, &MusicThread);
    }
    AudioEnumState = USB_AUDIO_ENUM_UNFINISH;
}

/*
Name:       USBAudioDeInit
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
int32 USBAudioThread(void)
{
    TASK_ARG TaskArg;
    uint32 ret = 0;

    if (TRUE != ThreadCheck(pMainThread, &MusicThread))
    {
        if(usbAudio_buf_len() > (USBAUDIO_BUFSIZE*3/4))
        {
            AUDIO_THREAD_ARG AudioArg;
            DEBUG("usb audio start");
            ModuleOverlay(MODULE_ID_AUDIO_CONTROL, MODULE_OVERLAY_ALL);
            AudioArg.pAudioOpsInit = UsbAudioAudioInit;
            ThreadCreat(&pMainThread, &MusicThread, &AudioArg);
        }
    }

    return ret;
}

/*
Name:       USBAudioCheck
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
uint8 USBAudioEnumCheck(void)
{
    if(AudioEnumState != USB_AUDIO_ENUM_COMPLETED)
        AudioEnumState = USB_AUDIO_ENUM_UNFINISH;

    return  AudioEnumState;
}

/*
Name:       USBAuidoTypeCheck
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_AUDIO_CODE_
uint32 UsbAudioHidControl(uint32 CtrKey)
{
    if(USBAudioEnumCheck() == USB_AUDIO_ENUM_COMPLETED)
    {
        //7 key function
        switch (CtrKey)
        {
            case KEY_VAL_ESC_SHORT_UP:
            {
                //printf ("PLAY->STOP\n");
                USBAudioOperCmd(AUDIO_CMD_PLAY_SKIP);
                USBAudioOperCmd(AUDIO_CMD_NONE);  //发送0清除标志
                break;
            }

            case KEY_VAL_MENU_SHORT_UP:
            {
                USBAudioOperCmd(AUDIO_CMD_STOP);
                USBAudioOperCmd(AUDIO_CMD_NONE);  //发送0清除标志
                break;
            }
            case KEY_VAL_PLAY_SHORT_UP:
            {
                //printf ("play/pause ++\n");
                USBAudioOperCmd(AUDIO_CMD_VOLUME_CD);
                USBAudioOperCmd(AUDIO_CMD_NONE);  //发送0清除标志
                break;
            }

            case KEY_VAL_FFD_SHORT_UP:
            {
                //Next
                USBAudioOperCmd(AUDIO_CMD_SCAN_NEXT_TRACK);
                USBAudioOperCmd(AUDIO_CMD_NONE);
                break;
            }


            case KEY_VAL_FFW_SHORT_UP:
            {
                //Prev
                USBAudioOperCmd( AUDIO_CMD_SCAN_PREV_TRACK);
                USBAudioOperCmd(AUDIO_CMD_NONE);
                break;
            }

            case KEY_VAL_UP_DOWN:                                   //volume increse
            case KEY_VAL_UP_PRESS:
            {
                //printf ("vol ++\n");
                USBAudioOperCmd(AUDIO_CMD_VOLUME_UP);
                USBAudioOperCmd(AUDIO_CMD_NONE);
                break;
            }
            case KEY_VAL_DOWN_DOWN:                                 //volume reduce
            case KEY_VAL_DOWN_PRESS:
            {
                //printf ("vol --\n");
                USBAudioOperCmd(AUDIO_CMD_VOLUME_DOWN);
                USBAudioOperCmd(AUDIO_CMD_NONE);
                break;
            }

            default:
                //printf ("Get USB key 2\n");
                break;

        }
    }


    return RETURN_OK;
}

#endif

