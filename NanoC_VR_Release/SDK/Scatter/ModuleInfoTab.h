/*
********************************************************************************
*                   Copyright (c) 2009,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   ModuleInfoTab.h
*
* Description:  define module struct information
*
* History:      <author>          <time>        <version>
*             ZhengYongzhi      2009-02-06         1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _MODULEINFO_H
#define _MODULEINFO_H

#undef  EXT
#ifdef _IN_MODULEINFO_
#define EXT
#else
#define EXT extern
#endif

/*
*-------------------------------------------------------------------------------
*
*                           Module Info
*
*-------------------------------------------------------------------------------
*/
//Module Section ID Define
typedef enum
{
    //SysCode
    MODULE_ID_SYSCODE = (UINT32)0,
    MODULE_ID_SYSINIT,
    MODULE_ID_FLASH_WRIRE,
    MODULE_ID_SDMMC,
#if 0
    MODULE_ID_LCD_1,
    MODULE_ID_LCD_2,
#endif
    //USB
    MODULE_ID_USB,
    MODULE_ID_USBCONTROL,
    MODULE_ID_USB_MSC,
    MODULE_ID_USB_AUDIO,
    MODULE_ID_USB_SERIAL,
    MODULE_ID_USB_HOST,

    //VIDEO
    MODULE_ID_VIDEO,
    MODULE_ID_VIDEO_CONTROL,
    MODULE_ID_MP2_DECODE,
    MODULE_ID_AVI_DECODE,

    //IMAGE
    MODULE_ID_PICTURE,
    MODULE_ID_PICTURE_CONTROL,
    MODULE_ID_JPG_DECODE,
    MODULE_ID_BMP_DECODE,

    //UI
    MODULE_ID_MAINMENU,
    MODULE_ID_MUSIC,
    MODULE_ID_CHARGE_WIN,

    //control level
    MODULE_ID_AUDIO_CONTROL,
    MODULE_ID_AUDIO_RKEQ,
    MODULE_ID_AUDIO_EQ,

    MODULE_ID_AUDIO_FILE,
    MODULE_ID_AUDIO_ID3,
    MODULE_ID_AUDIO_ID3_JPEG,

    MODULE_ID_FILEINFO_SAVE,
    MODULE_ID_FILEINFO_SORT,
    MODULE_ID_FAVO_RESET,

    MODULE_ID_MP3_DECODE,
    MODULE_ID_WMA_COMMON,
    MODULE_ID_WMA_OPEN_CODEC,
    MODULE_ID_WMA_DATA_16Ob,
    MODULE_ID_WMA_DATA_44Qb,
    MODULE_ID_WMA_DATA_44Ob,
    MODULE_ID_WMA_HIGH_LOW_COMMON,
    MODULE_ID_WMA_HIGH_RATE,
    MODULE_ID_WMA_LOW_RATE,
    MODULE_ID_WAV_DECODE,
    MODULE_ID_FLAC_DECODE,
    MODULE_ID_APE_DECODE,

    //FM
    MODULE_ID_FM_CONTROL,
    MODULE_ID_FM_DRIVER1,
    MODULE_ID_FM_DRIVER2,

    //Record
    MODULE_ID_RECORD_CONTROL,
    MODULE_ID_RECORD_MSADPCM,

    //Bluetooth
    MODULE_ID_BTWIN,
    MODULE_ID_BT,
    MODULE_ID_LWBT,
    MODULE_ID_BT_AUDIO,
    MODULE_ID_BT_PHONE,

    MODULE_ID_SBC_DECODE,
    MODULE_ID_AAC_DECODE,
    MODULE_ID_AEC,

    MODULE_ID_VOICE_NOTIFY,

    MODULE_ID_INIT_SCRIPT,
    MODULE_ID_VOICE_CALL_COMING,
    MODULE_ID_VOICE_WAIT_PAIR,
    MODULE_ID_VOICE_PAIRED,
    MODULE_ID_FS_FIRMWARE,
    MAX_MODULE_NUM

} MODULE_ID;

//Code Info Table
typedef	struct _CODE_INFO_T
{
    UINT32 CodeLoadBase;
    UINT32 CodeImageBase;
    UINT32 CodeImageLength;

    UINT32 DataLoadBase;
    UINT32 DataImageBase;
    UINT32 DataImageLength;

    UINT32 BssImageBase;
    UINT32 BssImageLength;

} CODE_INFO_T;

//Module Info
typedef	struct _CODE_INFO_TABLE
{
    UINT32                  ModuleNum;
    #ifndef _JTAG_DEBUG_
    CODE_INFO_T             Module[MAX_MODULE_NUM];
    #endif

} CODE_INFO_TABLE;

/*
*-------------------------------------------------------------------------------
*
*                           FM Driver Info
*
*-------------------------------------------------------------------------------
*/
//FM Driver Index
typedef enum
{
    MODULE_ID_FM_1    = (UINT32)0,
    MODULE_ID_FM_2,

    FM_DRIVER_MAX

} FM_DRIVER_ID;

//FM driver
typedef	struct _FM_DRIVER_TABLE_T
{
    void   (*Tuner_SetInitArea)(UINT8 );
    void   (*Tuner_SetFrequency)(UINT32 , UINT8 , BOOL ,UINT16 );
    void   (*Tuner_SetStereo)(BOOL );
    void   (*Tuner_SetVolume)(uint8);

    void   (*Tuner_PowerOffDeinit)(void);
    UINT16 (*Tuner_SearchByHand)(UINT16 , UINT32 * );
    void   (*Tuner_PowerDown)(void);
    void   (*Tuner_MuteControl)(bool );
    BOOLEAN (*GetStereoStatus)(void);

} FM_DRIVER_TABLE_T;

typedef	struct _FM_DRIVER_INFO_T
{
    //FM function list
    UINT32 FMDriverIndex;
    UINT32 FMDriverMax;
    FM_DRIVER_TABLE_T FmDriver[FM_DRIVER_MAX];

} FM_DRIVER_INFO_T;

/*
*-------------------------------------------------------------------------------
*
*                           LCD Driver Info
*
*-------------------------------------------------------------------------------
*/
//LCD Driver Index
typedef enum
{
    LCD_DRIVER_LCD_1 = (UINT32)0,
    LCD_DRIVER_LCD_2,
    LCD_DRIVER_MAX

} LCD_DRIVER_ID;

//Lcd driver
typedef	struct _LCD_DRIVER_TABLE_T
{
    //Lcd function list
    void(*pLcd_WriteRAM_Prepare)(void);
    void(*pLcd_Init)(void);
    void(*pLcd_SendData)(UINT16);
    void(*pLcd_SetWindow)(UINT16 ,UINT16 ,UINT16,INT16);
    void(*pLcd_Clear)(UINT16);
    void(*pLcd_DMATranfer)(UINT8,UINT8,UINT8,UINT8,UINT16*);
    void(*pLcd_Standby)(void);
    void(*pLcd_WakeUp)(void);
    void(*pLcd_MP4_Init)(void);
    void(*pLcd_MP4_DeInit)(void);
    void(*pLcd_SetPixel)(UINT16 x, UINT16 y, UINT16 data);
    void(*pLcd_Buffer_Display1)(unsigned  int x0,unsigned int y0,unsigned int x1,unsigned int y1);
    void(*pLcd_ClrSrc)(void);
    void(*pLcd_ClrRect)(int x0, int y0, int x1, int y1);
    void(*LCDDEV_FillRect)(int x0, int y0, int x1, int y1);
     // UINT32 reserve;

} LCD_DRIVER_TABLE_T;

typedef	struct _LCD_DRIVER_INFO_T
{
    //Lcd driver function
    UINT32 LcdDriverIndex;
    UINT32 LcdDriverMax;
    LCD_DRIVER_TABLE_T LcdDriver[LCD_DRIVER_MAX];

} LCD_DRIVER_INFO_T;

/*
*-------------------------------------------------------------------------------
*
*                           Default Para
*
*-------------------------------------------------------------------------------
*/
//System Default Para
typedef	struct _SYSTEM_DEFAULT_PARA_T
{
    //system default setting
    UINT32 SDEnable;
    UINT32 FMEnable;
    UINT32 KeyNumIndex;

    UINT32 gLanguage;
    UINT32 Volume;

    UINT32 BLMode;
    UINT32 BLevel;
    UINT32 BLtime;

    UINT32 SettingPowerOffTime;
    UINT32 IdlePowerOffTime;

    //Music default setting
    UINT32 MusicRepMode;
    UINT32 MusicPlayOrder;
    UINT32 MusicEqSel;
    UINT32 MusicPlayFxSel;

    //Video default setting

    //Radio default setting
    UINT32 FMArea;
    UINT32 FMStereo;

    //Record default setting

    //Picture default setting
    UINT32 PicAutoPlayTime;

    //Text default setting
    UINT32 TextAutoPlayTime;

    //Image ID
    uint32 ImageIndex;

    //mutiple language
    uint32 MenuMaxItem;
    uint32 LanguageTotle;   //TOTAL_LANAUAGE_NUM,LANGUAGE_MAX_COUNT
    uint32 LanguagesEnableTbl[64];//support 256 languange

} SYSTEM_DEFAULT_PARA_T;

/*
*-------------------------------------------------------------------------------
*
*                           Module Info
*
*-------------------------------------------------------------------------------
*/
//Firmware Info
typedef	struct _FIRMWARE_INFO_T
{
    UINT32                  LoadStartBase;

    CODE_INFO_TABLE         ModuleInfo;

    SYSTEM_DEFAULT_PARA_T   SysDefaultPara;

    FM_DRIVER_INFO_T        FMDriverTable;

    LCD_DRIVER_INFO_T       LCDDriverTable;

} FIRMWARE_INFO_T;

/*
********************************************************************************
*
*                         End of ModuleInfoTab.h
*
********************************************************************************
*/
#endif

