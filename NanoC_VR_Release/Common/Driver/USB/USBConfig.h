/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   FsConfig.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                               2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#ifndef _USBCONFIG_H
#define _USBCONFIG_H

/*
*-------------------------------------------------------------------------------
*  
*                           FileSystem Configer
*  
*-------------------------------------------------------------------------------
*/

/*
*-------------------------------------------------------------------------------
*  
*                           Section define
*  
*-------------------------------------------------------------------------------
*/

    
#include <stdio.h>
#include <string.h>
#include "SysConfig.h"
#include "typedef.h"
#include "Macro.h"

#include "hw_nvic.h"
#include "hw_scu.h"

#include "hw_memap.h"
#include "hook.h"


#define     USB_DRM_EN
#define     DMA_BULKIN_EN
#define     DMA_BULKOUT_EN

//section defines
//usb ui 
#define     _ATTR_USB_UI_CODE_				__attribute__((section("UsbUICode")))
#define     _ATTR_USB_UI_DATA_           	__attribute__((section("UsbUIData")))
#define     _ATTR_USB_UI_BSS_            	__attribute__((section("UsbUIBss"), zero_init))

//usb audio
#define     _ATTR_USB_AUDIO_CODE_			__attribute__((section("UsbAudioCode")))
#define     _ATTR_USB_AUDIO_DATA_           __attribute__((section("UsbAudioData")))
#define     _ATTR_USB_AUDIO_BSS_            __attribute__((section("UsbAudioBss"), zero_init))

//usb msc
#define     _ATTR_USB_MSC_CODE_				__attribute__((section("UsbMSCCode")))
#define     _ATTR_USB_MSC_DATA_           	__attribute__((section("UsbMSCData")))
#define     _ATTR_USB_MSC_BSS_            	__attribute__((section("UsbMSCBss"), zero_init))

//usb msc
#define     _ATTR_USB_SRL_CODE_				__attribute__((section("UsbSerialCode")))
#define     _ATTR_USB_SRL_DATA_           	__attribute__((section("UsbSerialData")))
#define     _ATTR_USB_SRL_BSS_            	__attribute__((section("UsbSerialBss"), zero_init))

//usb control
#define 	_ATTR_USBCONTROL_CODE_     		__attribute__((section("UsbControlCode")))
#define 	_ATTR_USBCONTROL_DATA_     		__attribute__((section("UsbControlData")))
#define 	_ATTR_USBCONTROL_BSS_      		__attribute__((section("UsbControlBss"),zero_init))

//usb driver
#define 	_ATTR_USB_DRIVER_CODE_     		__attribute__((section("UsbDriverCode")))
#define 	_ATTR_USB_DRIVER_DATA_     		__attribute__((section("UsbDriverData")))
#define 	_ATTR_USB_DRIVER_BSS_      		__attribute__((section("UsbDriverBss"),zero_init))

//usb host
#define     _ATTR_USB_HOST_CODE_            __attribute__((section("UsbHostCode")))
#define     _ATTR_USB_HOST_DATA_            __attribute__((section("UsbHostData")))
#define     _ATTR_USB_HOST_BSS_             __attribute__((section("UsbHostBss"), zero_init))


#if 0
//USB module compile switch.
#ifdef _USB_
#define USB_MSC
//#define USB_AUDIO
//#define USB_SERIAL
#define USB_IF_TEST                 //Ҫ��USB��֤������Ҫ�򿪴˶���, �򿪴˶���ֻҪUSB��������,�Ͳ��˳�USB
//#define USB_PHY_TEST                //USB PHY���ԣ�������ͼ������
#define USB_SUSPEND_EN           1
#endif
#endif


#define USB_DEBUG
#ifdef  USB_DEBUG
#define UDEBUG(...)  printf(__VA_ARGS__)
#else
#define UDEBUG(...)   
#endif

/*
*-------------------------------------------------------------------------------
*  
*                           Macro define
*  
*-------------------------------------------------------------------------------
*/
#include "OsInclude.h"

#include "FSconfig.h"
#include "FSInclude.h"

#include "USBreg.h"
#include "USBComm.h"
#include "chap9.h"

#include "USBDevice.h"

#include "storage.h"
#include "USBAudio.h"
#include "USBSerial.h"

#include "MDConfig.h"

#ifdef  _USBHOST_
#include "USBD.h"
#include "HCD.h"
#include "MSC.h"
#endif

/*
********************************************************************************
*
*                         End of FsConfig.h
*
********************************************************************************
*/
#endif

