/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   StartLoadTab.c
*
* Description:  定义模块信息，在模块调度时需要使用
*
* History:      <author>          <time>        <version>
*             ZhengYongzhi      2009-02-06         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_STARTLOAD_

#include "SysInclude.h"

extern uint32 Load$$MODULE_INFO$$Base;

//SysCode
extern uint32  Load$$SYS_CODE$$Base;
extern uint32 Image$$SYS_CODE$$Base;
extern uint32 Image$$SYS_CODE$$Length;
extern uint32  Load$$SYS_DATA$$Base;
extern uint32 Image$$SYS_DATA$$RW$$Base;
extern uint32 Image$$SYS_DATA$$RW$$Length;
extern uint32 Image$$SYS_DATA$$ZI$$Base;
extern uint32 Image$$SYS_DATA$$ZI$$Length;

#ifndef _JTAG_DEBUG_
extern uint32  Load$$SYS_INIT_CODE$$Base;
extern uint32 Image$$SYS_INIT_CODE$$Base;
extern uint32 Image$$SYS_INIT_CODE$$Length;
extern uint32  Load$$SYS_INIT_DATA$$Base;
extern uint32 Image$$SYS_INIT_DATA$$RW$$Base;
extern uint32 Image$$SYS_INIT_DATA$$RW$$Length;
extern uint32 Image$$SYS_INIT_DATA$$ZI$$Base;
extern uint32 Image$$SYS_INIT_DATA$$ZI$$Length;

//SD
extern uint32  Load$$SD_MMC_CODE$$Base;
extern uint32 Image$$SD_MMC_CODE$$Base;
extern uint32 Image$$SD_MMC_CODE$$Length;
extern uint32  Load$$SD_MMC_DATA$$Base;
extern uint32 Image$$SD_MMC_DATA$$RW$$Base;
extern uint32 Image$$SD_MMC_DATA$$RW$$Length;
extern uint32 Image$$SD_MMC_DATA$$ZI$$Base;
extern uint32 Image$$SD_MMC_DATA$$ZI$$Length;

//#ifdef MEDIA_MODULE
//fileinfosave
extern uint32  Load$$FILE_INFO_SAVE_CODE$$Base;
extern uint32 Image$$FILE_INFO_SAVE_CODE$$Base;
extern uint32 Image$$FILE_INFO_SAVE_CODE$$Length;
extern uint32  Load$$FILE_INFO_SAVE_DATA$$Base;
extern uint32 Image$$FILE_INFO_SAVE_DATA$$RW$$Base;
extern uint32 Image$$FILE_INFO_SAVE_DATA$$RW$$Length;
extern uint32 Image$$FILE_INFO_SAVE_DATA$$ZI$$Base;
extern uint32 Image$$FILE_INFO_SAVE_DATA$$ZI$$Length;

//fileinfosort
extern uint32  Load$$FILE_INFO_SORT_CODE$$Base;
extern uint32 Image$$FILE_INFO_SORT_CODE$$Base;
extern uint32 Image$$FILE_INFO_SORT_CODE$$Length;
extern uint32  Load$$FILE_INFO_SORT_DATA$$Base;
extern uint32 Image$$FILE_INFO_SORT_DATA$$RW$$Base;
extern uint32 Image$$FILE_INFO_SORT_DATA$$RW$$Length;
extern uint32 Image$$FILE_INFO_SORT_DATA$$ZI$$Base;
extern uint32 Image$$FILE_INFO_SORT_DATA$$ZI$$Length;

//FavoReset
extern uint32  Load$$FAVORESET_CODE$$Base;
extern uint32 Image$$FAVORESET_CODE$$Base;
extern uint32 Image$$FAVORESET_CODE$$Length;
extern uint32  Load$$FAVORESET_DATA$$Base;
extern uint32 Image$$FAVORESET_DATA$$RW$$Base;
extern uint32 Image$$FAVORESET_DATA$$RW$$Length;
extern uint32 Image$$FAVORESET_DATA$$ZI$$Base;
extern uint32 Image$$FAVORESET_DATA$$ZI$$Length;

//USB Window UI code & data
extern uint32 Load$$USBWIN_CODE$$Base;
extern uint32 Image$$USBWIN_CODE$$Base;
extern uint32 Image$$USBWIN_CODE$$Length;
extern uint32 Load$$USBWIN_DATA$$Base;
extern uint32 Image$$USBWIN_DATA$$RW$$Base;
extern uint32 Image$$USBWIN_DATA$$RW$$Length;
extern uint32 Image$$USBWIN_DATA$$ZI$$Base;
extern uint32 Image$$USBWIN_DATA$$ZI$$Length;

//USB Control & driver code & data
extern uint32 Load$$USBCONTROL_CODE$$Base;
extern uint32 Image$$USBCONTROL_CODE$$Base;
extern uint32 Image$$USBCONTROL_CODE$$Length;
extern uint32 Load$$USBCONTROL_DATA$$Base;
extern uint32 Image$$USBCONTROL_DATA$$RW$$Base;
extern uint32 Image$$USBCONTROL_DATA$$RW$$Length;
extern uint32 Image$$USBCONTROL_DATA$$ZI$$Base;
extern uint32 Image$$USBCONTROL_DATA$$ZI$$Length;

//USB Audio code & data
extern uint32 Load$$USBAUDIO_CODE$$Base;
extern uint32 Image$$USBAUDIO_CODE$$Base;
extern uint32 Image$$USBAUDIO_CODE$$Length;
extern uint32 Load$$USBAUDIO_DATA$$Base;
extern uint32 Image$$USBAUDIO_DATA$$RW$$Base;
extern uint32 Image$$USBAUDIO_DATA$$RW$$Length;
extern uint32 Image$$USBAUDIO_DATA$$ZI$$Base;
extern uint32 Image$$USBAUDIO_DATA$$ZI$$Length;

//USB MSC code & data
extern uint32 Load$$USBMSC_CODE$$Base;
extern uint32 Image$$USBMSC_CODE$$Base;
extern uint32 Image$$USBMSC_CODE$$Length;
extern uint32 Load$$USBMSC_DATA$$Base;
extern uint32 Image$$USBMSC_DATA$$RW$$Base;
extern uint32 Image$$USBMSC_DATA$$RW$$Length;
extern uint32 Image$$USBMSC_DATA$$ZI$$Base;
extern uint32 Image$$USBMSC_DATA$$ZI$$Length;

//USB Serial code & data
extern uint32 Load$$USB_SERIAL_CODE$$Base;
extern uint32 Image$$USB_SERIAL_CODE$$Base;
extern uint32 Image$$USB_SERIAL_CODE$$Length;
extern uint32 Load$$USB_SERIAL_DATA$$Base;
extern uint32 Image$$USB_SERIAL_DATA$$RW$$Base;
extern uint32 Image$$USB_SERIAL_DATA$$RW$$Length;
extern uint32 Image$$USB_SERIAL_DATA$$ZI$$Base;
extern uint32 Image$$USB_SERIAL_DATA$$ZI$$Length;

//USB HOST code & data
extern uint32 Load$$USBHOST_CODE$$Base;
extern uint32 Image$$USBHOST_CODE$$Base;
extern uint32 Image$$USBHOST_CODE$$Length;
extern uint32 Load$$USBHOST_DATA$$Base;
extern uint32 Image$$USBHOST_DATA$$RW$$Base;
extern uint32 Image$$USBHOST_DATA$$RW$$Length;
extern uint32 Image$$USBHOST_DATA$$ZI$$Base;
extern uint32 Image$$USBHOST_DATA$$ZI$$Length;

//Audio Decode
extern uint32  Load$$AUDIO_CONTROL_CODE$$Base;
extern uint32 Image$$AUDIO_CONTROL_CODE$$Base;
extern uint32 Image$$AUDIO_CONTROL_CODE$$Length;
extern uint32  Load$$AUDIO_CONTROL_DATA$$Base;
extern uint32 Image$$AUDIO_CONTROL_DATA$$RW$$Base;
extern uint32 Image$$AUDIO_CONTROL_DATA$$RW$$Length;
extern uint32 Image$$AUDIO_CONTROL_DATA$$ZI$$Base;
extern uint32 Image$$AUDIO_CONTROL_DATA$$ZI$$Length;

//Audio MsEq
extern uint32  Load$$BBE_EQ_CODE$$Base;
extern uint32 Image$$BBE_EQ_CODE$$Base;
extern uint32 Image$$BBE_EQ_CODE$$Length;
extern uint32  Load$$BBE_EQ_DATA$$Base;
extern uint32 Image$$BBE_EQ_DATA$$RW$$Base;
extern uint32 Image$$BBE_EQ_DATA$$RW$$Length;
extern uint32 Image$$BBE_EQ_DATA$$ZI$$Base;
extern uint32 Image$$BBE_EQ_DATA$$ZI$$Length;

//Audio MsEq
extern uint32  Load$$MS_EQ_CODE$$Base;
extern uint32 Image$$MS_EQ_CODE$$Base;
extern uint32 Image$$MS_EQ_CODE$$Length;
extern uint32  Load$$MS_EQ_DATA$$Base;
extern uint32 Image$$MS_EQ_DATA$$RW$$Base;
extern uint32 Image$$MS_EQ_DATA$$RW$$Length;
extern uint32 Image$$MS_EQ_DATA$$ZI$$Base;
extern uint32 Image$$MS_EQ_DATA$$ZI$$Length;

//Audio RKEq
extern uint32  Load$$RK_EQ_CODE$$Base;
extern uint32 Image$$RK_EQ_CODE$$Base;
extern uint32 Image$$RK_EQ_CODE$$Length;
extern uint32  Load$$RK_EQ_DATA$$Base;
extern uint32 Image$$RK_EQ_DATA$$RW$$Base;
extern uint32 Image$$RK_EQ_DATA$$RW$$Length;
extern uint32 Image$$RK_EQ_DATA$$ZI$$Base;
extern uint32 Image$$RK_EQ_DATA$$ZI$$Length;

//Audio EQ Table
extern uint32  Load$$RKEQ_TABLE_DATA$$Base;
extern uint32 Image$$RKEQ_TABLE_DATA$$Base;
extern uint32 Image$$RKEQ_TABLE_DATA$$Length;
extern uint32 Image$$RKEQ_TABLE_DATA$$RW$$Base;
extern uint32 Image$$RKEQ_TABLE_DATA$$RW$$Length;
extern uint32 Image$$RKEQ_TABLE_DATA$$ZI$$Base;
extern uint32 Image$$RKEQ_TABLE_DATA$$ZI$$Length;

//Audio file
extern uint32  Load$$AUDIO_FILE_CODE$$Base;
extern uint32 Image$$AUDIO_FILE_CODE$$Base;
extern uint32 Image$$AUDIO_FILE_CODE$$Length;
extern uint32  Load$$AUDIO_FILE_DATA$$Base;
extern uint32 Image$$AUDIO_FILE_DATA$$RW$$Base;
extern uint32 Image$$AUDIO_FILE_DATA$$RW$$Length;
extern uint32 Image$$AUDIO_FILE_DATA$$ZI$$Base;
extern uint32 Image$$AUDIO_FILE_DATA$$ZI$$Length;

extern uint32  Load$$AUDIO_ID3_CODE$$Base;
extern uint32 Image$$AUDIO_ID3_CODE$$Base;
extern uint32 Image$$AUDIO_ID3_CODE$$Length;
extern uint32  Load$$AUDIO_ID3_DATA$$Base;
extern uint32 Image$$AUDIO_ID3_DATA$$RW$$Base;
extern uint32 Image$$AUDIO_ID3_DATA$$RW$$Length;
extern uint32 Image$$AUDIO_ID3_DATA$$ZI$$Base;
extern uint32 Image$$AUDIO_ID3_DATA$$ZI$$Length;

//Audio ID3JPEG
extern uint32  Load$$AUDIO_ID3JPEG_CODE$$Base;
extern uint32 Image$$AUDIO_ID3JPEG_CODE$$Base;
extern uint32 Image$$AUDIO_ID3JPEG_CODE$$Length;
extern uint32  Load$$AUDIO_ID3JPEG_DATA$$Base;
extern uint32 Image$$AUDIO_ID3JPEG_DATA$$RW$$Base;
extern uint32 Image$$AUDIO_ID3JPEG_DATA$$RW$$Length;
extern uint32 Image$$AUDIO_ID3JPEG_DATA$$ZI$$Base;
extern uint32 Image$$AUDIO_ID3JPEG_DATA$$ZI$$Length;

//MP3
extern uint32  Load$$MP3_DECODE_CODE$$Base;
extern uint32 Image$$MP3_DECODE_CODE$$Base;
extern uint32 Image$$MP3_DECODE_CODE$$Length;
extern uint32  Load$$MP3_DECODE_DATA$$Base;
extern uint32 Image$$MP3_DECODE_DATA$$RW$$Base;
extern uint32 Image$$MP3_DECODE_DATA$$RW$$Length;
extern uint32 Image$$MP3_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$MP3_DECODE_DATA$$ZI$$Length;
//WMA
//common
extern uint32  Load$$WMA_COMMON_CODE$$Base;
extern uint32 Image$$WMA_COMMON_CODE$$Base;
extern uint32 Image$$WMA_COMMON_CODE$$Length;
extern uint32  Load$$WMA_COMMON_DATA$$Base;
extern uint32 Image$$WMA_COMMON_DATA$$RW$$Base;
extern uint32 Image$$WMA_COMMON_DATA$$RW$$Length;
extern uint32 Image$$WMA_COMMON_DATA$$ZI$$Base;
extern uint32 Image$$WMA_COMMON_DATA$$ZI$$Length;

//open codec
extern uint32  Load$$WMA_OPEN_CODEC_CODE$$Base;
extern uint32 Image$$WMA_OPEN_CODEC_CODE$$Base;
extern uint32 Image$$WMA_OPEN_CODEC_CODE$$Length;
extern uint32  Load$$WMA_OPEN_CODEC_DATA$$Base;
extern uint32 Image$$WMA_OPEN_CODEC_DATA$$RW$$Base;
extern uint32 Image$$WMA_OPEN_CODEC_DATA$$RW$$Length;
extern uint32 Image$$WMA_OPEN_CODEC_DATA$$ZI$$Base;
extern uint32 Image$$WMA_OPEN_CODEC_DATA$$ZI$$Length;

//DATA_16Ob
extern uint32  Load$$WMA_DATA_16Ob_CODE$$Base;
extern uint32 Image$$WMA_DATA_16Ob_CODE$$Base;
extern uint32 Image$$WMA_DATA_16Ob_CODE$$Length;
extern uint32  Load$$WMA_DATA_16Ob_DATA$$Base;
extern uint32 Image$$WMA_DATA_16Ob_DATA$$RW$$Base;
extern uint32 Image$$WMA_DATA_16Ob_DATA$$RW$$Length;
extern uint32 Image$$WMA_DATA_16Ob_DATA$$ZI$$Base;
extern uint32 Image$$WMA_DATA_16Ob_DATA$$ZI$$Length;

//DATA_44Qb
extern uint32  Load$$WMA_DATA_44Qb_CODE$$Base;
extern uint32 Image$$WMA_DATA_44Qb_CODE$$Base;
extern uint32 Image$$WMA_DATA_44Qb_CODE$$Length;
extern uint32  Load$$WMA_DATA_44Qb_DATA$$Base;
extern uint32 Image$$WMA_DATA_44Qb_DATA$$RW$$Base;
extern uint32 Image$$WMA_DATA_44Qb_DATA$$RW$$Length;
extern uint32 Image$$WMA_DATA_44Qb_DATA$$ZI$$Base;
extern uint32 Image$$WMA_DATA_44Qb_DATA$$ZI$$Length;

//DATA_44Ob
extern uint32  Load$$WMA_DATA_44Ob_CODE$$Base;
extern uint32 Image$$WMA_DATA_44Ob_CODE$$Base;
extern uint32 Image$$WMA_DATA_44Ob_CODE$$Length;
extern uint32  Load$$WMA_DATA_44Ob_DATA$$Base;
extern uint32 Image$$WMA_DATA_44Ob_DATA$$RW$$Base;
extern uint32 Image$$WMA_DATA_44Ob_DATA$$RW$$Length;
extern uint32 Image$$WMA_DATA_44Ob_DATA$$ZI$$Base;
extern uint32 Image$$WMA_DATA_44Ob_DATA$$ZI$$Length;

//high rate decode and mid-low rate decode
extern uint32  Load$$WMA_HIGH_LOW_COMMON_CODE$$Base;
extern uint32 Image$$WMA_HIGH_LOW_COMMON_CODE$$Base;
extern uint32 Image$$WMA_HIGH_LOW_COMMON_CODE$$Length;
extern uint32  Load$$WMA_HIGH_LOW_COMMON_DATA$$Base;
extern uint32 Image$$WMA_HIGH_LOW_COMMON_DATA$$RW$$Base;
extern uint32 Image$$WMA_HIGH_LOW_COMMON_DATA$$RW$$Length;
extern uint32 Image$$WMA_HIGH_LOW_COMMON_DATA$$ZI$$Base;
extern uint32 Image$$WMA_HIGH_LOW_COMMON_DATA$$ZI$$Length;

//high rate decode
extern uint32  Load$$WMA_HIGH_RATE_CODE$$Base;
extern uint32 Image$$WMA_HIGH_RATE_CODE$$Base;
extern uint32 Image$$WMA_HIGH_RATE_CODE$$Length;
extern uint32  Load$$WMA_HIGH_RATE_DATA$$Base;
extern uint32 Image$$WMA_HIGH_RATE_DATA$$RW$$Base;
extern uint32 Image$$WMA_HIGH_RATE_DATA$$RW$$Length;
extern uint32 Image$$WMA_HIGH_RATE_DATA$$ZI$$Base;
extern uint32 Image$$WMA_HIGH_RATE_DATA$$ZI$$Length;

//low rate decode
extern uint32  Load$$WMA_LOW_RATE_CODE$$Base;
extern uint32 Image$$WMA_LOW_RATE_CODE$$Base;
extern uint32 Image$$WMA_LOW_RATE_CODE$$Length;
extern uint32  Load$$WMA_LOW_RATE_DATA$$Base;
extern uint32 Image$$WMA_LOW_RATE_DATA$$RW$$Base;
extern uint32 Image$$WMA_LOW_RATE_DATA$$RW$$Length;
extern uint32 Image$$WMA_LOW_RATE_DATA$$ZI$$Base;
extern uint32 Image$$WMA_LOW_RATE_DATA$$ZI$$Length;

extern uint32  Load$$WAV_DECODE_CODE$$Base;
extern uint32 Image$$WAV_DECODE_CODE$$Base;
extern uint32 Image$$WAV_DECODE_CODE$$Length;
extern uint32  Load$$WAV_DECODE_DATA$$Base;
extern uint32 Image$$WAV_DECODE_DATA$$RW$$Base;
extern uint32 Image$$WAV_DECODE_DATA$$RW$$Length;
extern uint32 Image$$WAV_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$WAV_DECODE_DATA$$ZI$$Length;

//FLAC
extern uint32  Load$$FLAC_DECODE_CODE$$Base;
extern uint32 Image$$FLAC_DECODE_CODE$$Base;
extern uint32 Image$$FLAC_DECODE_CODE$$Length;
extern uint32  Load$$FLAC_DECODE_DATA$$Base;
extern uint32 Image$$FLAC_DECODE_DATA$$RW$$Base;
extern uint32 Image$$FLAC_DECODE_DATA$$RW$$Length;
extern uint32 Image$$FLAC_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$FLAC_DECODE_DATA$$ZI$$Length;

//AAC
extern uint32  Load$$AAC_DECODE_CODE$$Base;
extern uint32 Image$$AAC_DECODE_CODE$$Base;
extern uint32 Image$$AAC_DECODE_CODE$$Length;
extern uint32  Load$$AAC_DECODE_DATA$$Base;
extern uint32 Image$$AAC_DECODE_DATA$$RW$$Base;
extern uint32 Image$$AAC_DECODE_DATA$$RW$$Length;
extern uint32 Image$$AAC_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$AAC_DECODE_DATA$$ZI$$Length;

//APE
extern uint32  Load$$APE_DECODE_CODE$$Base;
extern uint32 Image$$APE_DECODE_CODE$$Base;
extern uint32 Image$$APE_DECODE_CODE$$Length;
extern uint32  Load$$APE_DECODE_DATA$$Base;
extern uint32 Image$$APE_DECODE_DATA$$RW$$Base;
extern uint32 Image$$APE_DECODE_DATA$$RW$$Length;
extern uint32 Image$$APE_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$APE_DECODE_DATA$$ZI$$Length;

//Encode MSADPCM
extern uint32  Load$$ENCODE_MSADPCM_CODE$$Base;
extern uint32 Image$$ENCODE_MSADPCM_CODE$$Base;
extern uint32 Image$$ENCODE_MSADPCM_CODE$$Length;
extern uint32  Load$$ENCODE_MSADPCM_DATA$$Base;
extern uint32 Image$$ENCODE_MSADPCM_DATA$$RW$$Base;
extern uint32 Image$$ENCODE_MSADPCM_DATA$$RW$$Length;
extern uint32 Image$$ENCODE_MSADPCM_DATA$$ZI$$Base;
extern uint32 Image$$ENCODE_MSADPCM_DATA$$ZI$$Length;

//RecordControl
extern uint32  Load$$RECORD_CONTROL_CODE$$Base;
extern uint32 Image$$RECORD_CONTROL_CODE$$Base;
extern uint32 Image$$RECORD_CONTROL_CODE$$Length;
extern uint32  Load$$RECORD_CONTROL_DATA$$Base;
extern uint32 Image$$RECORD_CONTROL_DATA$$RW$$Base;
extern uint32 Image$$RECORD_CONTROL_DATA$$RW$$Length;
extern uint32 Image$$RECORD_CONTROL_DATA$$ZI$$Base;
extern uint32 Image$$RECORD_CONTROL_DATA$$ZI$$Length;

//UI MainMenu
extern uint32  Load$$MAINMENU_CODE$$Base;
extern uint32 Image$$MAINMENU_CODE$$Base;
extern uint32 Image$$MAINMENU_CODE$$Length;
extern uint32  Load$$MAINMENU_DATA$$Base;
extern uint32 Image$$MAINMENU_DATA$$RW$$Base;
extern uint32 Image$$MAINMENU_DATA$$RW$$Length;
extern uint32 Image$$MAINMENU_DATA$$ZI$$Base;
extern uint32 Image$$MAINMENU_DATA$$ZI$$Length;

//UI Music
extern uint32  Load$$MUSICWIN_CODE$$Base;
extern uint32 Image$$MUSICWIN_CODE$$Base;
extern uint32 Image$$MUSICWIN_CODE$$Length;
extern uint32  Load$$MUSICWIN_DATA$$Base;
extern uint32 Image$$MUSICWIN_DATA$$RW$$Base;
extern uint32 Image$$MUSICWIN_DATA$$RW$$Length;
extern uint32 Image$$MUSICWIN_DATA$$ZI$$Base;
extern uint32 Image$$MUSICWIN_DATA$$ZI$$Length;

//UI Video
extern uint32  Load$$VIDEOWIN_CODE$$Base;
extern uint32 Image$$VIDEOWIN_CODE$$Base;
extern uint32 Image$$VIDEOWIN_CODE$$Length;
extern uint32  Load$$VIDEOWIN_DATA$$Base;
extern uint32 Image$$VIDEOWIN_DATA$$RW$$Base;
extern uint32 Image$$VIDEOWIN_DATA$$RW$$Length;
extern uint32 Image$$VIDEOWIN_DATA$$ZI$$Base;
extern uint32 Image$$VIDEOWIN_DATA$$ZI$$Length;

//Picture
extern uint32  Load$$PICWIN_CODE$$Base;
extern uint32 Image$$PICWIN_CODE$$Base;
extern uint32 Image$$PICWIN_CODE$$Length;
extern uint32  Load$$PICWIN_DATA$$Base;
extern uint32 Image$$PICWIN_DATA$$RW$$Base;
extern uint32 Image$$PICWIN_DATA$$RW$$Length;
extern uint32 Image$$PICWIN_DATA$$ZI$$Base;
extern uint32 Image$$PICWIN_DATA$$ZI$$Length;
//Picture 进程
extern uint32  Load$$IMAGE_CONTROL_CODE$$Base;
extern uint32 Image$$IMAGE_CONTROL_CODE$$Base;
extern uint32 Image$$IMAGE_CONTROL_CODE$$Length;
extern uint32  Load$$IMAGE_CONTROL_DATA$$Base;
extern uint32 Image$$IMAGE_CONTROL_DATA$$RW$$Base;
extern uint32 Image$$IMAGE_CONTROL_DATA$$RW$$Length;
extern uint32 Image$$IMAGE_CONTROL_DATA$$ZI$$Base;
extern uint32 Image$$IMAGE_CONTROL_DATA$$ZI$$Length;
//JPG
extern uint32  Load$$JPG_DECODE_CODE$$Base;
extern uint32 Image$$JPG_DECODE_CODE$$Base;
extern uint32 Image$$JPG_DECODE_CODE$$Length;
extern uint32  Load$$JPG_DECODE_DATA$$Base;
extern uint32 Image$$JPG_DECODE_DATA$$RW$$Base;
extern uint32 Image$$JPG_DECODE_DATA$$RW$$Length;
extern uint32 Image$$JPG_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$JPG_DECODE_DATA$$ZI$$Length;
//videocontrol
extern uint32 Load$$VIDEO_CONTROL_CODE$$Base;
extern uint32 Image$$VIDEO_CONTROL_CODE$$Base;
extern uint32 Image$$VIDEO_CONTROL_CODE$$Length;
extern uint32 Load$$VIDEO_CONTROL_DATA$$Base;
extern uint32 Image$$VIDEO_CONTROL_DATA$$RW$$Base;
extern uint32 Image$$VIDEO_CONTROL_DATA$$RW$$Length;
extern uint32 Image$$VIDEO_CONTROL_DATA$$ZI$$Base;
extern uint32 Image$$VIDEO_CONTROL_DATA$$ZI$$Length;

// VIDEO_MP2
extern uint32  Load$$MP2_DECODE_CODE$$Base;
extern uint32 Image$$MP2_DECODE_CODE$$Base;
extern uint32 Image$$MP2_DECODE_CODE$$Length;
extern uint32  Load$$MP2_DECODE_DATA$$Base;
extern uint32 Image$$MP2_DECODE_DATA$$RW$$Base;
extern uint32 Image$$MP2_DECODE_DATA$$RW$$Length;
extern uint32 Image$$MP2_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$MP2_DECODE_DATA$$ZI$$Length;

//AVI
extern uint32  Load$$AVI_DECODE_CODE$$Base;
extern uint32 Image$$AVI_DECODE_CODE$$Base;
extern uint32 Image$$AVI_DECODE_CODE$$Length;
extern uint32  Load$$AVI_DECODE_DATA$$Base;
extern uint32 Image$$AVI_DECODE_DATA$$RW$$Base;
extern uint32 Image$$AVI_DECODE_DATA$$RW$$Length;
extern uint32 Image$$AVI_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$AVI_DECODE_DATA$$ZI$$Length;


//BMP
extern uint32  Load$$BMP_DECODE_CODE$$Base;
extern uint32 Image$$BMP_DECODE_CODE$$Base;
extern uint32 Image$$BMP_DECODE_CODE$$Length;
extern uint32  Load$$BMP_DECODE_DATA$$Base;
extern uint32 Image$$BMP_DECODE_DATA$$RW$$Base;
extern uint32 Image$$BMP_DECODE_DATA$$RW$$Length;
extern uint32 Image$$BMP_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$BMP_DECODE_DATA$$ZI$$Length;

//FM
extern uint32 Load$$FM_CONTROL_CODE$$Base;
extern uint32 Image$$FM_CONTROL_CODE$$Base;
extern uint32 Image$$FM_CONTROL_CODE$$Length;
extern uint32 Load$$FM_CONTROL_DATA$$Base;
extern uint32 Image$$FM_CONTROL_DATA$$RW$$Base;
extern uint32 Image$$FM_CONTROL_DATA$$RW$$Length;
extern uint32 Image$$FM_CONTROL_DATA$$ZI$$Base;
extern uint32 Image$$FM_CONTROL_DATA$$ZI$$Length;


// FM DRIVER
extern uint32 Load$$FM_DRIVER1_CODE$$Base;
extern uint32 Image$$FM_DRIVER1_CODE$$Base;
extern uint32 Image$$FM_DRIVER1_CODE$$Length;
extern uint32 Load$$FM_DRIVER1_DATA$$Base;
extern uint32 Image$$FM_DRIVER1_DATA$$RW$$Base;
extern uint32 Image$$FM_DRIVER1_DATA$$RW$$Length;
extern uint32 Image$$FM_DRIVER1_DATA$$ZI$$Base;
extern uint32 Image$$FM_DRIVER1_DATA$$ZI$$Length;

extern uint32 Load$$FM_DRIVER2_CODE$$Base;
extern uint32 Image$$FM_DRIVER2_CODE$$Base;
extern uint32 Image$$FM_DRIVER2_CODE$$Length;
extern uint32 Load$$FM_DRIVER2_DATA$$Base;
extern uint32 Image$$FM_DRIVER2_DATA$$RW$$Base;
extern uint32 Image$$FM_DRIVER2_DATA$$RW$$Length;
extern uint32 Image$$FM_DRIVER2_DATA$$ZI$$Base;
extern uint32 Image$$FM_DRIVER2_DATA$$ZI$$Length;


//系统保留区读写操作
extern uint32  Load$$SYSAREA_OP_CODE$$Base;
extern uint32 Image$$SYSAREA_OP_CODE$$Base;
extern uint32 Image$$SYSAREA_OP_CODE$$Length;
extern uint32  Load$$SYSAREA_OP_DATA$$Base;
extern uint32 Image$$SYSAREA_OP_DATA$$RW$$Base;
extern uint32 Image$$SYSAREA_OP_DATA$$RW$$Length;
extern uint32 Image$$SYSAREA_OP_DATA$$ZI$$Base;
extern uint32 Image$$SYSAREA_OP_DATA$$ZI$$Length;

//CHARGE WIN
extern uint32  Load$$CHARGEWIN_CODE$$Base;
extern uint32 Image$$CHARGEWIN_CODE$$Base;
extern uint32 Image$$CHARGEWIN_CODE$$Length;
extern uint32  Load$$CHARGEWIN_DATA$$Base;
extern uint32 Image$$CHARGEWIN_DATA$$RW$$Base;
extern uint32 Image$$CHARGEWIN_DATA$$RW$$Length;
extern uint32 Image$$CHARGEWIN_DATA$$ZI$$Base;
extern uint32 Image$$CHARGEWIN_DATA$$ZI$$Length;

//BT
extern uint32  Load$$BTCONTROL_CODE$$Base;
extern uint32 Image$$BTCONTROL_CODE$$Base;
extern uint32 Image$$BTCONTROL_CODE$$Length;
extern uint32  Load$$BTCONTROL_DATA$$Base;
extern uint32 Image$$BTCONTROL_DATA$$RW$$Base;
extern uint32 Image$$BTCONTROL_DATA$$RW$$Length;
extern uint32 Image$$BTCONTROL_DATA$$ZI$$Base;
extern uint32 Image$$BTCONTROL_DATA$$ZI$$Length;

//BT WIN
extern uint32  Load$$BTWIN_CODE$$Base;
extern uint32 Image$$BTWIN_CODE$$Base;
extern uint32 Image$$BTWIN_CODE$$Length;
extern uint32  Load$$BTWIN_DATA$$Base;
extern uint32 Image$$BTWIN_DATA$$RW$$Base;
extern uint32 Image$$BTWIN_DATA$$RW$$Length;
extern uint32 Image$$BTWIN_DATA$$ZI$$Base;
extern uint32 Image$$BTWIN_DATA$$ZI$$Length;

//LWBT
extern uint32  Load$$LWBT_CODE$$Base;
extern uint32 Image$$LWBT_CODE$$Base;
extern uint32 Image$$LWBT_CODE$$Length;
extern uint32  Load$$LWBT_DATA$$Base;
extern uint32 Image$$LWBT_DATA$$RW$$Base;
extern uint32 Image$$LWBT_DATA$$RW$$Length;
extern uint32 Image$$LWBT_DATA$$ZI$$Base;
extern uint32 Image$$LWBT_DATA$$ZI$$Length;

//BT Audio
extern uint32  Load$$BT_AUDIO_CODE$$Base;
extern uint32 Image$$BT_AUDIO_CODE$$Base;
extern uint32 Image$$BT_AUDIO_CODE$$Length;
extern uint32  Load$$BT_AUDIO_DATA$$Base;
extern uint32 Image$$BT_AUDIO_DATA$$RW$$Base;
extern uint32 Image$$BT_AUDIO_DATA$$RW$$Length;
extern uint32 Image$$BT_AUDIO_DATA$$ZI$$Base;
extern uint32 Image$$BT_AUDIO_DATA$$ZI$$Length;

//BT Phone
extern uint32  Load$$BT_PHONE_CODE$$Base;
extern uint32 Image$$BT_PHONE_CODE$$Base;
extern uint32 Image$$BT_PHONE_CODE$$Length;
extern uint32  Load$$BT_PHONE_DATA$$Base;
extern uint32 Image$$BT_PHONE_DATA$$RW$$Base;
extern uint32 Image$$BT_PHONE_DATA$$RW$$Length;
extern uint32 Image$$BT_PHONE_DATA$$ZI$$Base;
extern uint32 Image$$BT_PHONE_DATA$$ZI$$Length;

//SBC
extern uint32  Load$$SBC_DECODE_CODE$$Base;
extern uint32 Image$$SBC_DECODE_CODE$$Base;
extern uint32 Image$$SBC_DECODE_CODE$$Length;
extern uint32  Load$$SBC_DECODE_DATA$$Base;
extern uint32 Image$$SBC_DECODE_DATA$$RW$$Base;
extern uint32 Image$$SBC_DECODE_DATA$$RW$$Length;
extern uint32 Image$$SBC_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$SBC_DECODE_DATA$$ZI$$Length;

//AEC
extern uint32  Load$$AEC_ALG_CODE$$Base;
extern uint32 Image$$AEC_ALG_CODE$$Base;
extern uint32 Image$$AEC_ALG_CODE$$Length;
extern uint32  Load$$AEC_ALG_DATA$$Base;
extern uint32 Image$$AEC_ALG_DATA$$RW$$Base;
extern uint32 Image$$AEC_ALG_DATA$$RW$$Length;
extern uint32 Image$$AEC_ALG_DATA$$ZI$$Base;
extern uint32 Image$$AEC_ALG_DATA$$ZI$$Length;

//cc2564 init script
extern uint32  Load$$BT_INIT_SCRIPT_CODE$$Base;
extern uint32 Image$$BT_INIT_SCRIPT_CODE$$Base;
extern uint32 Image$$BT_INIT_SCRIPT_CODE$$Length;


//voice notify
extern uint32  Load$$BT_VOICENOTIFY_CODE$$Base;
extern uint32 Image$$BT_VOICENOTIFY_CODE$$Base;
extern uint32 Image$$BT_VOICENOTIFY_CODE$$Length;
extern uint32  Load$$BT_VOICENOTIFY_DATA$$Base;
extern uint32 Image$$BT_VOICENOTIFY_DATA$$RW$$Base;
extern uint32 Image$$BT_VOICENOTIFY_DATA$$RW$$Length;
extern uint32 Image$$BT_VOICENOTIFY_DATA$$ZI$$Base;
extern uint32 Image$$BT_VOICENOTIFY_DATA$$ZI$$Length;

// voice call coming data
extern uint32  Load$$VOCIE_NOTIFY_CALL_COMING_CODE$$Base;
extern uint32 Image$$VOCIE_NOTIFY_CALL_COMING_CODE$$Base;
extern uint32 Image$$VOCIE_NOTIFY_CALL_COMING_CODE$$Length;

// voice call wait pair
extern uint32  Load$$VOCIE_NOTIFY_WAIT_PAIR$$Base;
extern uint32 Image$$VOCIE_NOTIFY_WAIT_PAIR$$Base;
extern uint32 Image$$VOCIE_NOTIFY_WAIT_PAIR$$Length;

// voice call paired
extern uint32  Load$$VOCIE_NOTIFY_PAIRED$$Base;
extern uint32 Image$$VOCIE_NOTIFY_PAIRED$$Base;
extern uint32 Image$$VOCIE_NOTIFY_PAIRED$$Length;



// gslx680 firmware
extern uint32 Load$$FS_FIRMWARE$$Base;
extern uint32 Image$$FS_FIRMWARE$$Base;
extern uint32 Image$$FS_FIRMWARE$$Length;


#endif

extern void ST7637_WriteRAM_Prepare(void);
extern void ST7637_Init(void);
extern void ST7637_SendData(UINT16 data);
extern void ST7637_SetWindow(UINT16 x0,UINT16 y0,UINT16 x1,INT16 y1);
extern void ST7637_Clear(UINT16 color);
extern void ST7637_DMATranfer (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *pSrc);
extern void ST7637_Standby(void);
extern void ST7637_WakeUp(void);
extern void ST7637_MP4_Init(void);
extern void ST7637_MP4_DeInit(void);

extern void ST7735_WriteRAM_Prepare(void);
extern void ST7735_Init(void);
extern void ST7735_SendData(UINT16 data);
extern void ST7735_SetWindow(UINT16 x0,UINT16 y0,UINT16 x1,INT16 y1);
extern void ST7735_Clear(UINT16 color);
extern void ST7735_DMATranfer (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *pSrc);
extern void ST7735_Standby(void);
extern void ST7735_WakeUp(void);
extern void ST7735_MP4_Init(void);
extern void ST7735_MP4_DeInit(void);

extern void Qn8035_Tuner_SetInitArea(UINT8 area);
extern void Qn8035_Tuner_SetFrequency(UINT32 n10KHz, UINT8 HILO, BOOL ForceMono,UINT16 Area);
extern void Qn8035_Tuner_SetStereo(BOOL bStereo);
extern void Qn8035_Tuner_Vol_Set(UINT8 gain);
extern void Qn8035_Tuner_PowerOffDeinit(void);
extern UINT16 Qn8035_Tuner_SearchByHand(UINT16 direct, UINT32 *FmFreq);
extern void Qn8035_Tuner_PowerDown(void);
extern void Qn8035_Tuner_MuteControl(bool active);
extern BOOLEAN Qn8035_GetStereoStatus(void);

extern void FM5807_Tuner_SetInitArea(UINT8 area);
extern void FM5807_Tuner_SetFrequency(UINT32 n10KHz, UINT8 HILO, BOOL ForceMono,UINT16 Area);
extern void FM5807_Tuner_SetStereo(BOOL bStereo);
extern void FM5807_Tuner_Vol_Set(UINT8 gain);
extern void FM5807_Tuner_PowerOffDeinit(void);
extern UINT16 FM5807_Tuner_SearchByHand(UINT16 direct, UINT32 *FmFreq);
extern void FM5807_Tuner_PowerDown(void);
extern void FM5807_Tuner_MuteControl(bool active);
extern BOOLEAN FM5807_GetStereoStatus(void);


/*
--------------------------------------------------------------------------------
  Function name :
  Author        : ZHengYongzhi
  Description   : 模块信息表，生成固件模块头信息，用于代码调度

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
__attribute__((section("ModuleInfo")))
FIRMWARE_INFO_T const ModuleInfo = {

    (uint32)(&Load$$MODULE_INFO$$Base),

    //CODE_INFO_TABLE
    {
        (uint32)(MAX_MODULE_NUM),
#ifndef _JTAG_DEBUG_
        {
            //系统段
            {
                (uint32)(&Load$$SYS_CODE$$Base),
                (uint32)(&Image$$SYS_CODE$$Base),
                (uint32)(&Image$$SYS_CODE$$Length),

                (uint32)(&Load$$SYS_DATA$$Base),
                (uint32)(&Image$$SYS_DATA$$RW$$Base),
                (uint32)(&Image$$SYS_DATA$$RW$$Length),

                (uint32)(&Image$$SYS_DATA$$ZI$$Base),
                (uint32)(&Image$$SYS_DATA$$ZI$$Length),
            },

            //系统初始化段
            {
                (uint32)(&Load$$SYS_INIT_CODE$$Base),
                (uint32)(&Image$$SYS_INIT_CODE$$Base),
                (uint32)(&Image$$SYS_INIT_CODE$$Length),

                (uint32)(&Load$$SYS_INIT_DATA$$Base),
                (uint32)(&Image$$SYS_INIT_DATA$$RW$$Base),
                (uint32)(&Image$$SYS_INIT_DATA$$RW$$Length),

                (uint32)(&Image$$SYS_INIT_DATA$$ZI$$Base),
                (uint32)(&Image$$SYS_INIT_DATA$$ZI$$Length),
            },

			//Flash Write
            {
                (uint32)0,
                (uint32)0,
                (uint32)0,

                (uint32)0,
                (uint32)0,
                (uint32)0,

                (uint32)0,
                (uint32)0,
            },

            //SD MMC
            {
                (uint32)(&Load$$SD_MMC_CODE$$Base),
                (uint32)(&Image$$SD_MMC_CODE$$Base),
                (uint32)(&Image$$SD_MMC_CODE$$Length),

                (uint32)(&Load$$SD_MMC_DATA$$Base),
                (uint32)(&Image$$SD_MMC_DATA$$RW$$Base),
                (uint32)(&Image$$SD_MMC_DATA$$RW$$Length),

                (uint32)(&Image$$SD_MMC_DATA$$ZI$$Base),
                (uint32)(&Image$$SD_MMC_DATA$$ZI$$Length),
            },
            #if 0
            //lcd驱动段driver1
            {
                (uint32)(&Load$$LCD_DRIVER1_CODE$$Base),
                (uint32)(&Image$$LCD_DRIVER1_CODE$$Base),
                (uint32)(&Image$$LCD_DRIVER1_CODE$$Length),

                (uint32)(&Load$$LCD_DRIVER1_DATA$$Base),
                (uint32)(&Image$$LCD_DRIVER1_DATA$$RW$$Base),
                (uint32)(&Image$$LCD_DRIVER1_DATA$$RW$$Length),

                (uint32)(&Image$$LCD_DRIVER1_DATA$$ZI$$Base),
                (uint32)(&Image$$LCD_DRIVER1_DATA$$ZI$$Length),
            },

            {
                (uint32)(&Load$$LCD_DRIVER2_CODE$$Base),
                (uint32)(&Image$$LCD_DRIVER2_CODE$$Base),
                (uint32)(&Image$$LCD_DRIVER2_CODE$$Length),

                (uint32)(&Load$$LCD_DRIVER2_DATA$$Base),
                (uint32)(&Image$$LCD_DRIVER2_DATA$$RW$$Base),
                (uint32)(&Image$$LCD_DRIVER2_DATA$$RW$$Length),

                (uint32)(&Image$$LCD_DRIVER2_DATA$$ZI$$Base),
                (uint32)(&Image$$LCD_DRIVER2_DATA$$ZI$$Length),
            },
            #endif

            //------------------------------------------------------------------
            //USB window UI
            {
                (uint32)(&Load$$USBWIN_CODE$$Base),
                (uint32)(&Image$$USBWIN_CODE$$Base),
                (uint32)(&Image$$USBWIN_CODE$$Length),

                (uint32)(&Load$$USBWIN_DATA$$Base),
                (uint32)(&Image$$USBWIN_DATA$$RW$$Base),
                (uint32)(&Image$$USBWIN_DATA$$RW$$Length),

                (uint32)(&Image$$USBWIN_DATA$$ZI$$Base),
                (uint32)(&Image$$USBWIN_DATA$$ZI$$Length),
            },

            //USB Control & driver
            {
                (uint32)(&Load$$USBCONTROL_CODE$$Base),
                (uint32)(&Image$$USBCONTROL_CODE$$Base),
                (uint32)(&Image$$USBCONTROL_CODE$$Length),

                (uint32)(&Load$$USBCONTROL_DATA$$Base),
                (uint32)(&Image$$USBCONTROL_DATA$$RW$$Base),
                (uint32)(&Image$$USBCONTROL_DATA$$RW$$Length),

                (uint32)(&Image$$USBCONTROL_DATA$$ZI$$Base),
                (uint32)(&Image$$USBCONTROL_DATA$$ZI$$Length),
            },

            //USB MSC
            {
                (uint32)(&Load$$USBMSC_CODE$$Base),
                (uint32)(&Image$$USBMSC_CODE$$Base),
                (uint32)(&Image$$USBMSC_CODE$$Length),

                (uint32)(&Load$$USBMSC_DATA$$Base),
                (uint32)(&Image$$USBMSC_DATA$$RW$$Base),
                (uint32)(&Image$$USBMSC_DATA$$RW$$Length),

                (uint32)(&Image$$USBMSC_DATA$$ZI$$Base),
                (uint32)(&Image$$USBMSC_DATA$$ZI$$Length),
            },

            //USB Audio
            {
                (uint32)(&Load$$USBAUDIO_CODE$$Base),
                (uint32)(&Image$$USBAUDIO_CODE$$Base),
                (uint32)(&Image$$USBAUDIO_CODE$$Length),

                (uint32)(&Load$$USBAUDIO_DATA$$Base),
                (uint32)(&Image$$USBAUDIO_DATA$$RW$$Base),
                (uint32)(&Image$$USBAUDIO_DATA$$RW$$Length),

                (uint32)(&Image$$USBAUDIO_DATA$$ZI$$Base),
                (uint32)(&Image$$USBAUDIO_DATA$$ZI$$Length),
            },

            //USB Serial
            {
                (uint32)(&Load$$USB_SERIAL_CODE$$Base),
                (uint32)(&Image$$USB_SERIAL_CODE$$Base),
                (uint32)(&Image$$USB_SERIAL_CODE$$Length),

                (uint32)(&Load$$USB_SERIAL_DATA$$Base),
                (uint32)(&Image$$USB_SERIAL_DATA$$RW$$Base),
                (uint32)(&Image$$USB_SERIAL_DATA$$RW$$Length),

                (uint32)(&Image$$USB_SERIAL_DATA$$ZI$$Base),
                (uint32)(&Image$$USB_SERIAL_DATA$$ZI$$Length),
            },

            //USB Host
            {
                (uint32)(&Load$$USBHOST_CODE$$Base),
                (uint32)(&Image$$USBHOST_CODE$$Base),
                (uint32)(&Image$$USBHOST_CODE$$Length),

                (uint32)(&Load$$USBHOST_DATA$$Base),
                (uint32)(&Image$$USBHOST_DATA$$RW$$Base),
                (uint32)(&Image$$USBHOST_DATA$$RW$$Length),

                (uint32)(&Image$$USBHOST_DATA$$ZI$$Base),
                (uint32)(&Image$$USBHOST_DATA$$ZI$$Length),
            },

            //------------------------------------------------------------------
            //VIDEO
            {
                (uint32)(&Load$$VIDEOWIN_CODE$$Base),
                (uint32)(&Image$$VIDEOWIN_CODE$$Base),
                (uint32)(&Image$$VIDEOWIN_CODE$$Length),

                (uint32)(&Load$$VIDEOWIN_DATA$$Base),
                (uint32)(&Image$$VIDEOWIN_DATA$$RW$$Base),
                (uint32)(&Image$$VIDEOWIN_DATA$$RW$$Length),

                (uint32)(&Image$$VIDEOWIN_DATA$$ZI$$Base),
                (uint32)(&Image$$VIDEOWIN_DATA$$ZI$$Length),
            },

            //VIDEO CONTROL
            {
                (uint32)(&Load$$VIDEO_CONTROL_CODE$$Base),
                (uint32)(&Image$$VIDEO_CONTROL_CODE$$Base),
                (uint32)(&Image$$VIDEO_CONTROL_CODE$$Length),

                (uint32)(&Load$$VIDEO_CONTROL_DATA$$Base),
                (uint32)(&Image$$VIDEO_CONTROL_DATA$$RW$$Base),
                (uint32)(&Image$$VIDEO_CONTROL_DATA$$RW$$Length),

                (uint32)(&Image$$VIDEO_CONTROL_DATA$$ZI$$Base),
                (uint32)(&Image$$VIDEO_CONTROL_DATA$$ZI$$Length),
            },

            //VIDEO_Mp2 Decode
            {
                (uint32)(&Load$$MP2_DECODE_CODE$$Base),
                (uint32)(&Image$$MP2_DECODE_CODE$$Base),
                (uint32)(&Image$$MP2_DECODE_CODE$$Length),

                (uint32)(&Load$$MP2_DECODE_DATA$$Base),
                (uint32)(&Image$$MP2_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$MP2_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$MP2_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$MP2_DECODE_DATA$$ZI$$Length),
            },

            //AVI Decode
            {
                (uint32)(&Load$$AVI_DECODE_CODE$$Base),
                (uint32)(&Image$$AVI_DECODE_CODE$$Base),
                (uint32)(&Image$$AVI_DECODE_CODE$$Length),

                (uint32)(&Load$$AVI_DECODE_DATA$$Base),
                (uint32)(&Image$$AVI_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$AVI_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$AVI_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$AVI_DECODE_DATA$$ZI$$Length),
            },

            //------------------------------------------------------------------
            //PicWin
            {
                (uint32)(&Load$$PICWIN_CODE$$Base),
                (uint32)(&Image$$PICWIN_CODE$$Base),
                (uint32)(&Image$$PICWIN_CODE$$Length),

                (uint32)(&Load$$PICWIN_DATA$$Base),
                (uint32)(&Image$$PICWIN_DATA$$RW$$Base),
                (uint32)(&Image$$PICWIN_DATA$$RW$$Length),

                (uint32)(&Image$$PICWIN_DATA$$ZI$$Base),
                (uint32)(&Image$$PICWIN_DATA$$ZI$$Length),
            },

            //PicWin jin cheng
            {
                (uint32)(&Load$$IMAGE_CONTROL_CODE$$Base),
                (uint32)(&Image$$IMAGE_CONTROL_CODE$$Base),
                (uint32)(&Image$$IMAGE_CONTROL_CODE$$Length),

                (uint32)(&Load$$IMAGE_CONTROL_DATA$$Base),
                (uint32)(&Image$$IMAGE_CONTROL_DATA$$RW$$Base),
                (uint32)(&Image$$IMAGE_CONTROL_DATA$$RW$$Length),

                (uint32)(&Image$$IMAGE_CONTROL_DATA$$ZI$$Base),
                (uint32)(&Image$$IMAGE_CONTROL_DATA$$ZI$$Length),
            },

            //JPG Decode
            {
                (uint32)( &Load$$JPG_DECODE_CODE$$Base),
                (uint32)(&Image$$JPG_DECODE_CODE$$Base),
                (uint32)(&Image$$JPG_DECODE_CODE$$Length),

                (uint32)( &Load$$JPG_DECODE_DATA$$Base),
                (uint32)(&Image$$JPG_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$JPG_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$JPG_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$JPG_DECODE_DATA$$ZI$$Length),
            },

            //BMP Decode
            {
                (uint32)( &Load$$BMP_DECODE_CODE$$Base),
                (uint32)(&Image$$BMP_DECODE_CODE$$Base),
                (uint32)(&Image$$BMP_DECODE_CODE$$Length),

                (uint32)( &Load$$BMP_DECODE_DATA$$Base),
                (uint32)(&Image$$BMP_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$BMP_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$BMP_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$BMP_DECODE_DATA$$ZI$$Length),
            },

            //------------------------------------------------------------------
            //UI MainMenu
            {
                (uint32)(&Load$$MAINMENU_CODE$$Base),
                (uint32)(&Image$$MAINMENU_CODE$$Base),
                (uint32)(&Image$$MAINMENU_CODE$$Length),

                (uint32)(&Load$$MAINMENU_DATA$$Base),
                (uint32)(&Image$$MAINMENU_DATA$$RW$$Base),
                (uint32)(&Image$$MAINMENU_DATA$$RW$$Length),

                (uint32)(&Image$$MAINMENU_DATA$$ZI$$Base),
                (uint32)(&Image$$MAINMENU_DATA$$ZI$$Length),
            },

            //Music Win
            {
                (uint32)(&Load$$MUSICWIN_CODE$$Base),
                (uint32)(&Image$$MUSICWIN_CODE$$Base),
                (uint32)(&Image$$MUSICWIN_CODE$$Length),

                (uint32)(&Load$$MUSICWIN_DATA$$Base),
                (uint32)(&Image$$MUSICWIN_DATA$$RW$$Base),
                (uint32)(&Image$$MUSICWIN_DATA$$RW$$Length),

                (uint32)(&Image$$MUSICWIN_DATA$$ZI$$Base),
                (uint32)(&Image$$MUSICWIN_DATA$$ZI$$Length),
            },


            //Charge Win
            {
                (uint32)(&Load$$CHARGEWIN_CODE$$Base),
                (uint32)(&Image$$CHARGEWIN_CODE$$Base),
                (uint32)(&Image$$CHARGEWIN_CODE$$Length),

                (uint32)(&Load$$CHARGEWIN_DATA$$Base),
                (uint32)(&Image$$CHARGEWIN_DATA$$RW$$Base),
                (uint32)(&Image$$CHARGEWIN_DATA$$RW$$Length),

                (uint32)(&Image$$CHARGEWIN_DATA$$ZI$$Base),
                (uint32)(&Image$$CHARGEWIN_DATA$$ZI$$Length),
            },

            //AudioControl
            {
                (uint32)(&Load$$AUDIO_CONTROL_CODE$$Base),
                (uint32)(&Image$$AUDIO_CONTROL_CODE$$Base),
                (uint32)(&Image$$AUDIO_CONTROL_CODE$$Length),

                (uint32)(&Load$$AUDIO_CONTROL_DATA$$Base),
                (uint32)(&Image$$AUDIO_CONTROL_DATA$$RW$$Base),
                (uint32)(&Image$$AUDIO_CONTROL_DATA$$RW$$Length),

                (uint32)(&Image$$AUDIO_CONTROL_DATA$$ZI$$Base),
                (uint32)(&Image$$AUDIO_CONTROL_DATA$$ZI$$Length),
            },

            //RK EQ
            {
                (uint32)(&Load$$RK_EQ_CODE$$Base),
                (uint32)(&Image$$RK_EQ_CODE$$Base),
                (uint32)(&Image$$RK_EQ_CODE$$Length),

                (uint32)(&Load$$RK_EQ_DATA$$Base),
                (uint32)(&Image$$RK_EQ_DATA$$RW$$Base),
                (uint32)(&Image$$RK_EQ_DATA$$RW$$Length),

                (uint32)(&Image$$RK_EQ_DATA$$ZI$$Base),
                (uint32)(&Image$$RK_EQ_DATA$$ZI$$Length),
            },

            //EQ Table
            {
                (uint32)(0),
                (uint32)(0),
                (uint32)(0),

                (uint32)(&Load$$RKEQ_TABLE_DATA$$Base),
                (uint32)(&Image$$RKEQ_TABLE_DATA$$RW$$Base),
                (uint32)(&Image$$RKEQ_TABLE_DATA$$RW$$Length),

                (uint32)(&Image$$RKEQ_TABLE_DATA$$ZI$$Base),
                (uint32)(&Image$$RKEQ_TABLE_DATA$$ZI$$Length),
            },

            //Audio File
            {
                (uint32)(&Load$$AUDIO_FILE_CODE$$Base),
                (uint32)(&Image$$AUDIO_FILE_CODE$$Base),
                (uint32)(&Image$$AUDIO_FILE_CODE$$Length),

                (uint32)(&Load$$AUDIO_FILE_DATA$$Base),
                (uint32)(&Image$$AUDIO_FILE_DATA$$RW$$Base),
                (uint32)(&Image$$AUDIO_FILE_DATA$$RW$$Length),

                (uint32)(&Image$$AUDIO_FILE_DATA$$ZI$$Base),
                (uint32)(&Image$$AUDIO_FILE_DATA$$ZI$$Length),
            },

            //ID3
            {
                (uint32)(&Load$$AUDIO_ID3_CODE$$Base),
                (uint32)(&Image$$AUDIO_ID3_CODE$$Base),
                (uint32)(&Image$$AUDIO_ID3_CODE$$Length),

                (uint32)(&Load$$AUDIO_ID3_DATA$$Base),
                (uint32)(&Image$$AUDIO_ID3_DATA$$RW$$Base),
                (uint32)(&Image$$AUDIO_ID3_DATA$$RW$$Length),

                (uint32)(&Image$$AUDIO_ID3_DATA$$ZI$$Base),
                (uint32)(&Image$$AUDIO_ID3_DATA$$ZI$$Length),
            },

            //ID3 Jpeg
            {
                (uint32)(&Load$$AUDIO_ID3JPEG_CODE$$Base),
                (uint32)(&Image$$AUDIO_ID3JPEG_CODE$$Base),
                (uint32)(&Image$$AUDIO_ID3JPEG_CODE$$Length),

                (uint32)(&Load$$AUDIO_ID3JPEG_DATA$$Base),
                (uint32)(&Image$$AUDIO_ID3JPEG_DATA$$RW$$Base),
                (uint32)(&Image$$AUDIO_ID3JPEG_DATA$$RW$$Length),

                (uint32)(&Image$$AUDIO_ID3JPEG_DATA$$ZI$$Base),
                (uint32)(&Image$$AUDIO_ID3JPEG_DATA$$ZI$$Length),
            },

            //MediaLib File info Save
            {
                (uint32)(&Load$$FILE_INFO_SAVE_CODE$$Base),
                (uint32)(&Image$$FILE_INFO_SAVE_CODE$$Base),
                (uint32)(&Image$$FILE_INFO_SAVE_CODE$$Length),

                (uint32)(&Load$$FILE_INFO_SAVE_DATA$$Base),
                (uint32)(&Image$$FILE_INFO_SAVE_DATA$$RW$$Base),
                (uint32)(&Image$$FILE_INFO_SAVE_DATA$$RW$$Length),

                (uint32)(&Image$$FILE_INFO_SAVE_DATA$$ZI$$Base),
                (uint32)(&Image$$FILE_INFO_SAVE_DATA$$ZI$$Length),
            },

            //MediaLib Sort
            {
                (uint32)(&Load$$FILE_INFO_SORT_CODE$$Base),
                (uint32)(&Image$$FILE_INFO_SORT_CODE$$Base),
                (uint32)(&Image$$FILE_INFO_SORT_CODE$$Length),

                (uint32)(&Load$$FILE_INFO_SORT_DATA$$Base),
                (uint32)(&Image$$FILE_INFO_SORT_DATA$$RW$$Base),
                (uint32)(&Image$$FILE_INFO_SORT_DATA$$RW$$Length),

                (uint32)(&Image$$FILE_INFO_SORT_DATA$$ZI$$Base),
                (uint32)(&Image$$FILE_INFO_SORT_DATA$$ZI$$Length),
            },

            //MediaLib Favorate Set
            {
                (uint32)(&Load$$FAVORESET_CODE$$Base),
                (uint32)(&Image$$FAVORESET_CODE$$Base),
                (uint32)(&Image$$FAVORESET_CODE$$Length),

                (uint32)(&Load$$FAVORESET_DATA$$Base),
                (uint32)(&Image$$FAVORESET_DATA$$RW$$Base),
                (uint32)(&Image$$FAVORESET_DATA$$RW$$Length),

                (uint32)(&Image$$FAVORESET_DATA$$ZI$$Base),
                (uint32)(&Image$$FAVORESET_DATA$$ZI$$Length),
            },

            //CONSOLE MP3
            {
                (uint32)(&Load$$MP3_DECODE_CODE$$Base),
                (uint32)(&Image$$MP3_DECODE_CODE$$Base),
                (uint32)(&Image$$MP3_DECODE_CODE$$Length),

                (uint32)(&Load$$MP3_DECODE_DATA$$Base),
                (uint32)(&Image$$MP3_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$MP3_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$MP3_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$MP3_DECODE_DATA$$ZI$$Length),
            },

    		//CONSOLE WMA
    		{
    			(uint32)(& Load$$WMA_COMMON_CODE$$Base),
    			(uint32)(&Image$$WMA_COMMON_CODE$$Base),
    			(uint32)(&Image$$WMA_COMMON_CODE$$Length),
    			(uint32)(& Load$$WMA_COMMON_DATA$$Base),
    			(uint32)(&Image$$WMA_COMMON_DATA$$RW$$Base),
    			(uint32)(&Image$$WMA_COMMON_DATA$$RW$$Length),
    			(uint32)(&Image$$WMA_COMMON_DATA$$ZI$$Base),
    			(uint32)(&Image$$WMA_COMMON_DATA$$ZI$$Length),
    		},

    		{
    			(uint32)(& Load$$WMA_OPEN_CODEC_CODE$$Base),
    			(uint32)(&Image$$WMA_OPEN_CODEC_CODE$$Base),
    			(uint32)(&Image$$WMA_OPEN_CODEC_CODE$$Length),
    			(uint32)(& Load$$WMA_OPEN_CODEC_DATA$$Base),
    			(uint32)(&Image$$WMA_OPEN_CODEC_DATA$$RW$$Base),
    			(uint32)(&Image$$WMA_OPEN_CODEC_DATA$$RW$$Length),
    			(uint32)(&Image$$WMA_OPEN_CODEC_DATA$$ZI$$Base),
    			(uint32)(&Image$$WMA_OPEN_CODEC_DATA$$ZI$$Length),
    		},

            {
    			(uint32)(& Load$$WMA_DATA_16Ob_CODE$$Base),
    			(uint32)(&Image$$WMA_DATA_16Ob_CODE$$Base),
    			(uint32)(&Image$$WMA_DATA_16Ob_CODE$$Length),
    			(uint32)(& Load$$WMA_DATA_16Ob_DATA$$Base),
    			(uint32)(&Image$$WMA_DATA_16Ob_DATA$$RW$$Base),
    			(uint32)(&Image$$WMA_DATA_16Ob_DATA$$RW$$Length),
    			(uint32)(&Image$$WMA_DATA_16Ob_DATA$$ZI$$Base),
    			(uint32)(&Image$$WMA_DATA_16Ob_DATA$$ZI$$Length),
    		},

            {
    			(uint32)(& Load$$WMA_DATA_44Qb_CODE$$Base),
    			(uint32)(&Image$$WMA_DATA_44Qb_CODE$$Base),
    			(uint32)(&Image$$WMA_DATA_44Qb_CODE$$Length),
    			(uint32)(& Load$$WMA_DATA_44Qb_DATA$$Base),
    			(uint32)(&Image$$WMA_DATA_44Qb_DATA$$RW$$Base),
    			(uint32)(&Image$$WMA_DATA_44Qb_DATA$$RW$$Length),
    			(uint32)(&Image$$WMA_DATA_44Qb_DATA$$ZI$$Base),
    			(uint32)(&Image$$WMA_DATA_44Qb_DATA$$ZI$$Length),
    		},

    		{
    			(uint32)(& Load$$WMA_DATA_44Ob_CODE$$Base),
    			(uint32)(&Image$$WMA_DATA_44Ob_CODE$$Base),
    			(uint32)(&Image$$WMA_DATA_44Ob_CODE$$Length),
    			(uint32)(& Load$$WMA_DATA_44Ob_DATA$$Base),
    			(uint32)(&Image$$WMA_DATA_44Ob_DATA$$RW$$Base),
    			(uint32)(&Image$$WMA_DATA_44Ob_DATA$$RW$$Length),
    			(uint32)(&Image$$WMA_DATA_44Ob_DATA$$ZI$$Base),
    			(uint32)(&Image$$WMA_DATA_44Ob_DATA$$ZI$$Length),
    		},

    		{
    			(uint32)(& Load$$WMA_HIGH_LOW_COMMON_CODE$$Base),
    			(uint32)(&Image$$WMA_HIGH_LOW_COMMON_CODE$$Base),
    			(uint32)(&Image$$WMA_HIGH_LOW_COMMON_CODE$$Length),
    			(uint32)(& Load$$WMA_HIGH_LOW_COMMON_DATA$$Base),
    			(uint32)(&Image$$WMA_HIGH_LOW_COMMON_DATA$$RW$$Base),
    			(uint32)(&Image$$WMA_HIGH_LOW_COMMON_DATA$$RW$$Length),
    			(uint32)(&Image$$WMA_HIGH_LOW_COMMON_DATA$$ZI$$Base),
    			(uint32)(&Image$$WMA_HIGH_LOW_COMMON_DATA$$ZI$$Length),
    		},

    		{
    			(uint32)(& Load$$WMA_HIGH_RATE_CODE$$Base),
    			(uint32)(&Image$$WMA_HIGH_RATE_CODE$$Base),
    			(uint32)(&Image$$WMA_HIGH_RATE_CODE$$Length),
    			(uint32)(& Load$$WMA_HIGH_RATE_DATA$$Base),
    			(uint32)(&Image$$WMA_HIGH_RATE_DATA$$RW$$Base),
    			(uint32)(&Image$$WMA_HIGH_RATE_DATA$$RW$$Length),
    			(uint32)(&Image$$WMA_HIGH_RATE_DATA$$ZI$$Base),
    			(uint32)(&Image$$WMA_HIGH_RATE_DATA$$ZI$$Length),
    		},

    		{
    			(uint32)(& Load$$WMA_LOW_RATE_CODE$$Base),
    			(uint32)(&Image$$WMA_LOW_RATE_CODE$$Base),
    			(uint32)(&Image$$WMA_LOW_RATE_CODE$$Length),
    			(uint32)(& Load$$WMA_LOW_RATE_DATA$$Base),
    			(uint32)(&Image$$WMA_LOW_RATE_DATA$$RW$$Base),
    			(uint32)(&Image$$WMA_LOW_RATE_DATA$$RW$$Length),
    			(uint32)(&Image$$WMA_LOW_RATE_DATA$$ZI$$Base),
    			(uint32)(&Image$$WMA_LOW_RATE_DATA$$ZI$$Length),
    		},

            //WAV
            {
                (uint32)(&Load$$WAV_DECODE_CODE$$Base),
                (uint32)(&Image$$WAV_DECODE_CODE$$Base),
                (uint32)(&Image$$WAV_DECODE_CODE$$Length),

                (uint32)(&Load$$WAV_DECODE_DATA$$Base),
                (uint32)(&Image$$WAV_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$WAV_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$WAV_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$WAV_DECODE_DATA$$ZI$$Length),
            },

            //CONSOLE FLAC
            {
                (uint32)(&Load$$FLAC_DECODE_CODE$$Base),
                (uint32)(&Image$$FLAC_DECODE_CODE$$Base),
                (uint32)(&Image$$FLAC_DECODE_CODE$$Length),

                (uint32)(&Load$$FLAC_DECODE_DATA$$Base),
                (uint32)(&Image$$FLAC_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$FLAC_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$FLAC_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$FLAC_DECODE_DATA$$ZI$$Length),
            },

            //CONSOLE APE
            {
                (uint32)(&Load$$APE_DECODE_CODE$$Base),
                (uint32)(&Image$$APE_DECODE_CODE$$Base),
                (uint32)(&Image$$APE_DECODE_CODE$$Length),

                (uint32)(&Load$$APE_DECODE_DATA$$Base),
                (uint32)(&Image$$APE_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$APE_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$APE_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$APE_DECODE_DATA$$ZI$$Length),
            },

            //FM Control
            {
                (uint32)(&Load$$FM_CONTROL_CODE$$Base),
                (uint32)(&Image$$FM_CONTROL_CODE$$Base),
                (uint32)(&Image$$FM_CONTROL_CODE$$Length),

                (uint32)(&Load$$FM_CONTROL_DATA$$Base),
                (uint32)(&Image$$FM_CONTROL_DATA$$RW$$Base),
                (uint32)(&Image$$FM_CONTROL_DATA$$RW$$Length),

                (uint32)(&Image$$FM_CONTROL_DATA$$ZI$$Base),
                (uint32)(&Image$$FM_CONTROL_DATA$$ZI$$Length),
            },

            //FM Driver1
            {
                (uint32)(&Load$$FM_DRIVER1_CODE$$Base),
                (uint32)(&Image$$FM_DRIVER1_CODE$$Base),
                (uint32)(&Image$$FM_DRIVER1_CODE$$Length),

                (uint32)(&Load$$FM_DRIVER1_DATA$$Base),
                (uint32)(&Image$$FM_DRIVER1_DATA$$RW$$Base),
                (uint32)(&Image$$FM_DRIVER1_DATA$$RW$$Length),

                (uint32)(&Image$$FM_DRIVER1_DATA$$ZI$$Base),
                (uint32)(&Image$$FM_DRIVER1_DATA$$ZI$$Length),
            },

            //FM Driver2
            {
                (uint32)(&Load$$FM_DRIVER2_CODE$$Base),
                (uint32)(&Image$$FM_DRIVER2_CODE$$Base),
                (uint32)(&Image$$FM_DRIVER2_CODE$$Length),

                (uint32)(&Load$$FM_DRIVER2_DATA$$Base),
                (uint32)(&Image$$FM_DRIVER2_DATA$$RW$$Base),
                (uint32)(&Image$$FM_DRIVER2_DATA$$RW$$Length),

                (uint32)(&Image$$FM_DRIVER2_DATA$$ZI$$Base),
                (uint32)(&Image$$FM_DRIVER2_DATA$$ZI$$Length),
            },

            //Record control
            {
                (uint32)(&Load$$RECORD_CONTROL_CODE$$Base),
                (uint32)(&Image$$RECORD_CONTROL_CODE$$Base),
                (uint32)(&Image$$RECORD_CONTROL_CODE$$Length),

                (uint32)(&Load$$RECORD_CONTROL_DATA$$Base),
                (uint32)(&Image$$RECORD_CONTROL_DATA$$RW$$Base),
                (uint32)(&Image$$RECORD_CONTROL_DATA$$RW$$Length),

                (uint32)(&Image$$RECORD_CONTROL_DATA$$ZI$$Base),
                (uint32)(&Image$$RECORD_CONTROL_DATA$$ZI$$Length),
            },

            //MS ADPCM
            {
                (uint32)(&Load$$ENCODE_MSADPCM_CODE$$Base),
                (uint32)(&Image$$ENCODE_MSADPCM_CODE$$Base),
                (uint32)(&Image$$ENCODE_MSADPCM_CODE$$Length),

                (uint32)(&Load$$ENCODE_MSADPCM_DATA$$Base),
                (uint32)(&Image$$ENCODE_MSADPCM_DATA$$RW$$Base),
                (uint32)(&Image$$ENCODE_MSADPCM_DATA$$RW$$Length),

                (uint32)(&Image$$ENCODE_MSADPCM_DATA$$ZI$$Base),
                (uint32)(&Image$$ENCODE_MSADPCM_DATA$$ZI$$Length),
            },

            //BTWIN
            {
                (uint32)(&Load$$BTWIN_CODE$$Base),
                (uint32)(&Image$$BTWIN_CODE$$Base),
                (uint32)(&Image$$BTWIN_CODE$$Length),

                (uint32)(&Load$$BTWIN_DATA$$Base),
                (uint32)(&Image$$BTWIN_DATA$$RW$$Base),
                (uint32)(&Image$$BTWIN_DATA$$RW$$Length),

                (uint32)(&Image$$BTWIN_DATA$$ZI$$Base),
                (uint32)(&Image$$BTWIN_DATA$$ZI$$Length),
            },

            //BT Control
            {
                (uint32)(&Load$$BTCONTROL_CODE$$Base),
                (uint32)(&Image$$BTCONTROL_CODE$$Base),
                (uint32)(&Image$$BTCONTROL_CODE$$Length),

                (uint32)(&Load$$BTCONTROL_DATA$$Base),
                (uint32)(&Image$$BTCONTROL_DATA$$RW$$Base),
                (uint32)(&Image$$BTCONTROL_DATA$$RW$$Length),

                (uint32)(&Image$$BTCONTROL_DATA$$ZI$$Base),
                (uint32)(&Image$$BTCONTROL_DATA$$ZI$$Length),
            },

            //lwbt
            {
                (uint32)(&Load$$LWBT_CODE$$Base),
                (uint32)(&Image$$LWBT_CODE$$Base),
                (uint32)(&Image$$LWBT_CODE$$Length),

                (uint32)(&Load$$LWBT_DATA$$Base),
                (uint32)(&Image$$LWBT_DATA$$RW$$Base),
                (uint32)(&Image$$LWBT_DATA$$RW$$Length),

                (uint32)(&Image$$LWBT_DATA$$ZI$$Base),
                (uint32)(&Image$$LWBT_DATA$$ZI$$Length),
            },

            //BT Audio
            {
                (uint32)(&Load$$BT_AUDIO_CODE$$Base),
                (uint32)(&Image$$BT_AUDIO_CODE$$Base),
                (uint32)(&Image$$BT_AUDIO_CODE$$Length),

                (uint32)(&Load$$BT_AUDIO_DATA$$Base),
                (uint32)(&Image$$BT_AUDIO_DATA$$RW$$Base),
                (uint32)(&Image$$BT_AUDIO_DATA$$RW$$Length),

                (uint32)(&Image$$BT_AUDIO_DATA$$ZI$$Base),
                (uint32)(&Image$$BT_AUDIO_DATA$$ZI$$Length),
            },

            //BT Phone
            {
                (uint32)(&Load$$BT_PHONE_CODE$$Base),
                (uint32)(&Image$$BT_PHONE_CODE$$Base),
                (uint32)(&Image$$BT_PHONE_CODE$$Length),

                (uint32)(&Load$$BT_PHONE_DATA$$Base),
                (uint32)(&Image$$BT_PHONE_DATA$$RW$$Base),
                (uint32)(&Image$$BT_PHONE_DATA$$RW$$Length),

                (uint32)(&Image$$BT_PHONE_DATA$$ZI$$Base),
                (uint32)(&Image$$BT_PHONE_DATA$$ZI$$Length),
            },

            //SBC
            {
                (uint32)(&Load$$SBC_DECODE_CODE$$Base),
                (uint32)(&Image$$SBC_DECODE_CODE$$Base),
                (uint32)(&Image$$SBC_DECODE_CODE$$Length),

                (uint32)(&Load$$SBC_DECODE_DATA$$Base),
                (uint32)(&Image$$SBC_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$SBC_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$SBC_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$SBC_DECODE_DATA$$ZI$$Length),
            },

            //AAC
            {
                (uint32)(&Load$$AAC_DECODE_CODE$$Base),
                (uint32)(&Image$$AAC_DECODE_CODE$$Base),
                (uint32)(&Image$$AAC_DECODE_CODE$$Length),

                (uint32)(&Load$$AAC_DECODE_DATA$$Base),
                (uint32)(&Image$$AAC_DECODE_DATA$$RW$$Base),
                (uint32)(&Image$$AAC_DECODE_DATA$$RW$$Length),

                (uint32)(&Image$$AAC_DECODE_DATA$$ZI$$Base),
                (uint32)(&Image$$AAC_DECODE_DATA$$ZI$$Length),
            },

            //AEC
            {
                (uint32)(&Load$$AEC_ALG_CODE$$Base),
                (uint32)(&Image$$AEC_ALG_CODE$$Base),
                (uint32)(&Image$$AEC_ALG_CODE$$Length),

                (uint32)(&Load$$AEC_ALG_DATA$$Base),
                (uint32)(&Image$$AEC_ALG_DATA$$RW$$Base),
                (uint32)(&Image$$AEC_ALG_DATA$$RW$$Length),

                (uint32)(&Image$$AEC_ALG_DATA$$ZI$$Base),
                (uint32)(&Image$$AEC_ALG_DATA$$ZI$$Length),
            },

            //voice notify
            {
                (uint32)(&Load$$BT_VOICENOTIFY_CODE$$Base),
                (uint32)(&Image$$BT_VOICENOTIFY_CODE$$Base),
                (uint32)(&Image$$BT_VOICENOTIFY_CODE$$Length),

                (uint32)(&Load$$BT_VOICENOTIFY_DATA$$Base),
                (uint32)(&Image$$BT_VOICENOTIFY_DATA$$RW$$Base),
                (uint32)(&Image$$BT_VOICENOTIFY_DATA$$RW$$Length),

                (uint32)(&Image$$BT_VOICENOTIFY_DATA$$ZI$$Base),
                (uint32)(&Image$$BT_VOICENOTIFY_DATA$$ZI$$Length),
            },

            //BT init script
            {
                (uint32)(&Load$$BT_INIT_SCRIPT_CODE$$Base),
                (uint32)(&Image$$BT_INIT_SCRIPT_CODE$$Base),
                (uint32)(&Image$$BT_INIT_SCRIPT_CODE$$Length),

                (uint32)(0),
                (uint32)(0),
                (uint32)(0),

                (uint32)(0),
                (uint32)(0),
            },

            {
                (uint32)(&Load$$VOCIE_NOTIFY_CALL_COMING_CODE$$Base),
                (uint32)(&Image$$VOCIE_NOTIFY_CALL_COMING_CODE$$Base),
                (uint32)(&Image$$VOCIE_NOTIFY_CALL_COMING_CODE$$Length),

                (uint32)0,
                (uint32)0,
                (uint32)0,

                (uint32)0,
                (uint32)0,
            },

            {
                (uint32)(&Load$$VOCIE_NOTIFY_WAIT_PAIR$$Base),
                (uint32)(&Image$$VOCIE_NOTIFY_WAIT_PAIR$$Base),
                (uint32)(&Image$$VOCIE_NOTIFY_WAIT_PAIR$$Length),

                (uint32)0,
                (uint32)0,
                (uint32)0,

                (uint32)0,
                (uint32)0,
            },


            {
                (uint32)(&Load$$VOCIE_NOTIFY_PAIRED$$Base),
                (uint32)(&Image$$VOCIE_NOTIFY_PAIRED$$Base),
                (uint32)(&Image$$VOCIE_NOTIFY_PAIRED$$Length),

                (uint32)0,
                (uint32)0,
                (uint32)0,

                (uint32)0,
                (uint32)0,
            },
            {
                (uint32)(&Load$$FS_FIRMWARE$$Base),
                (uint32)(&Image$$FS_FIRMWARE$$Base),
                (uint32)(&Image$$FS_FIRMWARE$$Length),

                (uint32)0,
                (uint32)0,
                (uint32)0,

                (uint32)0,
                (uint32)0,
            },
        },
#endif
    },

    //系统默认参数
    {
        //系统参数
        #ifdef _SDCARD_
        (uint32)1,      //UINT32 SDEnable;
        #else
        (uint32)0,
        #endif

        #ifdef _RADIO_
        (uint32)1,      //UINT32 FMEnable;
        #else
        (uint32)0,
        #endif

        (uint32)KEY_NUM,//UINT32 KeyNum


        (uint32)2,      //UINT32 Langule;
        (uint32)24,     //UINT32 Volume;

        (uint32)0,      //UINT32 BLMode;
        (uint32)2,      //UINT32 BLevel;
        (uint32)5,      //UINT32 BLtime;

        (uint32)0,      //UINT32 SetPowerOffTime;
        (uint32)0,      //UINT32 IdlePowerOffTime;

        //Music参数
        (uint32)5,      //UINT32 MusicRepMode;
        (uint32)0,      //UINT32 MusicPlayOrder;
        (uint32)0,      //UINT32 MusicEqSel;
        (uint32)0,      //UINT32 MusicPlayFxSel;

        //Video参数

        //Radio参数
        (uint32)0,      //UINT32 FMArea;
        (uint32)0,      //UINT32 FMStereo;

        //Record参数

        //Picture参数
        (uint32)2,      //UINT32 PicAutoPlayTime;

        //Text参数
        (uint32)2,      //UINT32 TextAutoPlayTime;

        //Image ID
        (uint32)0,

        //多国语言选择
        (uint32)TOTAL_MENU_ITEM,    //最大菜单项
        (uint32)TOTAL_LANAUAGE_NUM, //uint32 LanguageTotle, 实际最大多国语言个数
        //语言配置表                //最多可支持64国语言
        {
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //8
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //16
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //24
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //32
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //40
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //48
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //56
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //64
        },
    },

    //FM 参数及驱动程序列表
    {
        (uint32)1,      //FMDriverID;
        (uint32)FM_DRIVER_MAX,

        //FM Driver List
        #ifdef _RADIO_
        {
            //FM5767
            {
                Qn8035_Tuner_SetInitArea,
                Qn8035_Tuner_SetFrequency,
                Qn8035_Tuner_SetStereo,
                Qn8035_Tuner_Vol_Set,

                Qn8035_Tuner_PowerOffDeinit,
                Qn8035_Tuner_SearchByHand,
                Qn8035_Tuner_PowerDown,
                Qn8035_Tuner_MuteControl,
                Qn8035_GetStereoStatus,
            },

            //FM5807
            {
                FM5807_Tuner_SetInitArea,
                FM5807_Tuner_SetFrequency,
                FM5807_Tuner_SetStereo,
                FM5807_Tuner_Vol_Set,

                FM5807_Tuner_PowerOffDeinit,
                FM5807_Tuner_SearchByHand,
                FM5807_Tuner_PowerDown,
                FM5807_Tuner_MuteControl,
                FM5807_GetStereoStatus,

            },
        },
        #else
        {
            //FM5767
            {
                0,
                0,
                0,
                0,

                0,
                0,
                0,
                0,
                0,
            },

            //FM5807
            {
                0,
                0,
                0,
                0,

                0,
                0,
                0,
                0,
                0,

            },
        },
        #endif
    },

    //LCD 参数及驱动程序列表
    {
        (uint32)1,      //LcdDriverID;
        (uint32)LCD_DRIVER_MAX,

        //#ifdef DISPLAY_ENABLE
        #if 0
        //LCD Driver List
        {
            //DRIVER1
            {
                ST7637_WriteRAM_Prepare,
                ST7637_Init,
                ST7637_SendData,
                ST7637_SetWindow,
                ST7637_Clear,
                ST7637_DMATranfer,
                ST7637_Standby,
                ST7637_WakeUp,
                ST7637_MP4_Init,
                ST7637_MP4_DeInit,
                0,
                0,
                0,
                0,
				0,
            },

            {
                ST7735_WriteRAM_Prepare,
                ST7735_Init,
                ST7735_SendData,
                ST7735_SetWindow,
                ST7735_Clear,
                ST7735_DMATranfer,
                ST7735_Standby,
                ST7735_WakeUp,
                ST7735_MP4_Init,
                ST7735_MP4_DeInit,
                0,
                0,
                0,
                0,
                0,
            },
        },
        #else
        //LCD Driver List
        {
            //DRIVER1
            {
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
				0,
            },

            {
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
            },
        },
        #endif
    }
};

/*
--------------------------------------------------------------------------------
  Function name :
  Author        : ZHengYongzhi
  Description   : 模块信息表

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
__attribute__((section("START_CODE")))
void ScatterLoader(void)
{
    uint32 i,len;
    uint8  *pDest;

    //清除Bss段
    pDest = (uint8*)((uint32)(&Image$$SYS_DATA$$ZI$$Base));
    len = (uint32)((uint32)(&Image$$SYS_DATA$$ZI$$Length));
    for (i = 0; i < len; i++)
    {
        *pDest++ = 0;
    }
}

/*
********************************************************************************
*
*                         End of StartLoadTab.c
*
********************************************************************************
*/
