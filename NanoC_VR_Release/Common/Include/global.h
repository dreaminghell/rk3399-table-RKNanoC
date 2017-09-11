/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   Main.h
*
* Description:
*
* History:      <author>          <time>        <version>
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef _MUSIC_
#include "effect.h"
#endif

#undef  EXT
#ifdef _IN_MAIN_
#define EXT
#else
#define EXT extern
#endif

/*
*-------------------------------------------------------------------------------
*
*                           Macro define
*
*-------------------------------------------------------------------------------
*/
//section define
#define _ATTR_SYS_CODE_         __attribute__((section("SysCode")))
#define _ATTR_SYS_DATA_         __attribute__((section("SysData")))
#define _ATTR_SYS_BSS_          __attribute__((section("SysBss"),zero_init))

#define _ATTR_SYS_INIT_CODE_    __attribute__((section("SysInitCode")))
#define _ATTR_SYS_INIT_DATA_    __attribute__((section("SysInitData")))
#define _ATTR_SYS_INIT_BSS_     __attribute__((section("SysInitBss"),zero_init))

/*
*-------------------------------------------------------------------------------
*
*                           Struct define
*
*-------------------------------------------------------------------------------
*/
//system keeping information for music module
#ifdef _MUSIC_
typedef __packed struct _MUSICCONFIG
{
    uint16 FileNum;                 //current file number.
    uint32 CurTime;                 //current time.
    UINT32 LrcPreTime;              //last time of lrc
    uint8  HoldOnPlaySaveFlag;
    uint8  RepeatMode;              //repeat mode
    uint8  PlayOrder;               //play order
    RKEffect Eq;                    //Eq
}MUSIC_CONFIG;
#endif



//system keeping information for picture module

#ifdef _USB_
typedef __packed struct _USBCONFIG
{
    UINT8 UsbMode;                  // 0: MSC; 1: USB Audio; 2: USB Host
    #ifdef _USBHOST_
    UINT8 HostDevComplete;
    #endif
}USB_CONFIG;
#endif

//system setting parameter structure definition.
typedef __packed struct _SYSCONFIG
{
    uint16 FirmwareYear;
    uint16 FirmwareDate;
    uint16 MasterVersion;
    uint16 SlaveVersion;
    uint16 SmallVersion;

    uint8  Memory;                  //storage media options.
    UINT8  SysLanguage;             //current system language environment.
    uint8  ImageIndex;

    UINT8  OutputVolume;            //system volume
    UINT8  BLmode;                  //backlight display mode.
    UINT8  BLevel;                  //backlight level
    UINT8  BLtime;                  //backlight time.
    UINT8  LockTime;                //lock keyboard time
    UINT8  ShutTime;                //auto close time
    UINT8  SDEnable;                //sd card select
    UINT8  FMEnable;                //FM setting menu judge.
    UINT8  KeyNum;                  //it is used for 5.6.7 keys judgement.

#ifdef _CDROM_
    UINT8  bShowCdrom;
#endif

#ifdef _MUSIC_
    MUSIC_CONFIG MusicConfig;
#endif

#ifdef _USB_
    USB_CONFIG UsbConfig;
	UINT8 UsbSuspend;
#endif

    uint32 SysClkBack;

    UINT8  Softin;   //shift bit number,it is used for volume attenuate

    int UsbSensor;
	int StartTP;

	int sensor_calibias;
	int sensor_accel_bias[3];
    int sensor_gyro_bias[3];
} SYSCONFIG,*PSYSCONFIG;


/*
*-------------------------------------------------------------------------------
*
*                           Variable define
*
*-------------------------------------------------------------------------------
*/
//Firmware Info
_ATTR_SYS_BSS_  EXT uint32      Font12LogicAddress;
_ATTR_SYS_BSS_  EXT uint32      Font16LogicAddress;

_ATTR_SYS_BSS_  EXT uint32      IDBlockByteAddress;
_ATTR_SYS_BSS_  EXT uint32      CodeLogicAddress;
_ATTR_SYS_BSS_  EXT uint32      FontLogicAddress;
_ATTR_SYS_BSS_  EXT uint32      GBKLogicAddress;
_ATTR_SYS_BSS_  EXT uint32      MenuLogicAddress;
_ATTR_SYS_BSS_  EXT uint32      ImageLogicAddress;

_ATTR_SYS_BSS_  EXT int8        hImageFile;
_ATTR_SYS_BSS_  EXT int8        hMenuFile;
_ATTR_SYS_BSS_  EXT int8        hGBKFile;
_ATTR_SYS_BSS_  EXT int8        hFont12File;
_ATTR_SYS_BSS_  EXT int8        hFont16File;

#ifdef _MULT_DISK_
_ATTR_SYS_BSS_  EXT uint8        bShowFlash1;
#endif

#ifdef _CDROM_
_ATTR_SYS_BSS_  EXT uint8       bShowCdrom;
#endif

_ATTR_SYS_BSS_  EXT uint32      SysTickCounter;  //system clock counter,add one every 10 ms.
_ATTR_SYS_BSS_  EXT SYSCONFIG   gSysConfig;

#ifdef MEDIA_MODULE
   EXT _ATTR_SYS_BSS_ UINT16 gMusicTypeSelID; // add by phc,
   EXT _ATTR_SYS_BSS_ UINT16 ID3TypeSelFlag; // add by phc, for ID3Type select
   EXT _ATTR_SYS_BSS_ UINT16 FileSortUpdateKey; //whether to operate usb disk.
   EXT _ATTR_SYS_BSS_ UINT32 gFavoBlockSectorAddr;
   EXT _ATTR_SYS_BSS_ UINT16 gDisSortedMusicFlag;    //
   EXT _ATTR_SYS_BSS_ UINT32 MediaInfoAddr;  //media information store start address.
#endif

_ATTR_SYS_BSS_  EXT BOOL        IsSetmenu;
_ATTR_SYS_BSS_  EXT BOOL        BroswerFlag;//the flag that is distinguish return to media or explorer
_ATTR_SYS_BSS_  EXT UINT16      GlobalFilenum;

_ATTR_SYS_BSS_  EXT INT8       OutputVolOffset;//it is use to modify volume after eq setting.
_ATTR_SYS_BSS_  EXT UINT8       MaxShuffleAllCount;
#define  MemorySelect           gSysConfig.Memory
#define  gbTextFileNum          gSysConfig.TextConfig.FileNum
#define  gbAudioFileNum         gSysConfig.MusicConfig.FileNum
#define  gbVideoFileNum         gSysConfig.VideoConfig.FileNum
#define  gbPicFileNum           gSysConfig.PicConfig.FileNum //an 4.27 picture mode.
#ifdef _IN_MAIN_
_ATTR_SYS_DATA_ EXT char        FileOpenStringR[]      = "R";
_ATTR_SYS_DATA_ EXT char        ALLFileExtString[]     = "*";
_ATTR_SYS_DATA_ EXT char        AudioFileExtString[]   = "MP1MP2MP3WMAWAVAPEFLAAACM4A";
_ATTR_SYS_DATA_ EXT char        TextFileExtString[]    = "TXTLRC";
_ATTR_SYS_DATA_ EXT char        VideoFileExtString[]   = "AVI";
_ATTR_SYS_DATA_ EXT char        PictureFileExtString[] = "JPGJPEBMP";//JPE it is used judge jpeg picture.
#else
_ATTR_SYS_DATA_ EXT char        FileOpenStringR[];
_ATTR_SYS_DATA_     EXT char    ALLFileExtString[];
_ATTR_SYS_DATA_ EXT char        AudioFileExtString[];
_ATTR_SYS_DATA_ EXT char        TextFileExtString[];
_ATTR_SYS_DATA_ EXT char        VideoFileExtString[];
_ATTR_SYS_DATA_ EXT char        PictureFileExtString[];

#endif

//set language information group of real support in menu.this information is the offset of language in explorer.
_ATTR_SYS_BSS_  EXT UINT8       SetMenuLanguageInfo[21];
_ATTR_SYS_BSS_  EXT UINT8       DefaultLanguage;//default language item number it is used to locate default language cursor in menu display.
_ATTR_SYS_BSS_  EXT UINT8       LanguageNum;//set the real suppot language number.
_ATTR_SYS_BSS_  EXT UINT8       DefaultLanguageID;

/*
********************************************************************************
*
*                         End of Main.h
*
********************************************************************************
*/
#endif
