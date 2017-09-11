/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  SysReservedOperation.c
*
* Description:
*
* History:      <author>          <time>        <version>
*             ZhengYongzhi      2008-9-13          1.0
*    desc:
********************************************************************************
*/
#define _IN_SYSRESERVED_OP_

#include "SysInclude.h"
#include "FsInclude.h"
#include "SysReservedOperation.h"

#ifdef _MUSIC_
#include "AudioControl.h"
#include "SysFindFile.h"
    #ifdef _RK_EQ_
    extern short UseEqTable[CUSTOMEQ_LEVELNUM] ;//= {7, 8, 9,  10, 11, 12, 13, 14, 15,16,17};
    #endif
#endif

#ifdef _VIDEO_
#include "VideoControl.h"
#endif

#ifdef _USB_
#include "FunUsb.h"
#endif

/*
--------------------------------------------------------------------------------
  Function name : uint32 FindIDBlock(void)
  Author        : ZHengYongzhi
  Description   : find IDBlock

  Input         :
  Return        : IDBlock OR 0xffffffff

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void LoadSysInformation(void)
{
    uint32 i,j=0;
    uint32 Len;
    uint32 SystemDefaultParaAddr;
    uint8  TempBuffer[1024];
    uint8  FlashBuf[512];

    uint16 FirmwareYear;
    uint16 FirmwareDate;
    uint16 MasterVersion;
    uint16 SlaveVersion;
    uint16 SmallVersion;
    uint32 MenuLogicAddress0, MenuLogicAddress1;

    SYSCONFIG             *pSysConfig;
    FIRMWARE_INFO_T       *pFirmwareModuleInfo;
    SYSTEM_DEFAULT_PARA_T *pSystemDefaultPara;
    FIRMWARE_HEADER       *pFWHead;
    BOOL CleanHistortyFlag = FALSE;

    ////////////////////////////////////////////////////////////////////////////
    //read resource module address.
    pFWHead = (FIRMWARE_HEADER *)FlashBuf;
    MDRead(SysDiskID, 0, 1, FlashBuf);

    CodeLogicAddress   = pFWHead->CodeInfo.CodeInfoTbl[SYS_CODE].ModuleOffset;
    GBKLogicAddress    = pFWHead->CharEncInfo.CharEncInfoTbl[GBK].ModuleOffset ;
    MenuLogicAddress0  = pFWHead->MenuInfo.MenuInfoTbl[0].ModuleOffset;
    MenuLogicAddress1  = pFWHead->MenuInfo.MenuInfoTbl[1].ModuleOffset;

    ////////////////////////////////////////////////////////////////////////////
    //read system default parameter.
    MDReadData(SysDiskID, CodeLogicAddress, 512, FlashBuf);
    pFirmwareModuleInfo = (FIRMWARE_INFO_T *)FlashBuf;

    SystemDefaultParaAddr  = CodeLogicAddress + sizeof(pFirmwareModuleInfo -> LoadStartBase);
    SystemDefaultParaAddr += sizeof(pFirmwareModuleInfo -> ModuleInfo.ModuleNum);
    SystemDefaultParaAddr += pFirmwareModuleInfo -> ModuleInfo.ModuleNum * sizeof(CODE_INFO_T);
    MDReadData(SysDiskID, SystemDefaultParaAddr, sizeof(SYSTEM_DEFAULT_PARA_T), FlashBuf);
    pSystemDefaultPara = (SYSTEM_DEFAULT_PARA_T *)FlashBuf;

    ///////////////////////////////////////////////////////////////////////
    //read system setting parameters and judge version number.
    pSysConfig = (SYSCONFIG *)TempBuffer;
    ReadReservedData(SYSDATA_BLK, TempBuffer, 1024);

    GetFirmwareVersion(&FirmwareYear,
                       &FirmwareDate,
                       &MasterVersion,
                       &SlaveVersion,
                       &SmallVersion);

    if ((pSysConfig->FirmwareYear  != FirmwareYear)  ||
        (pSysConfig->FirmwareDate  != FirmwareDate)  ||
        (pSysConfig->MasterVersion != MasterVersion) ||
        (pSysConfig->SlaveVersion  != SlaveVersion)  ||
        (pSysConfig->SmallVersion  != SmallVersion))
    {
        memset(TempBuffer, 0xff, 1024);
        CleanHistortyFlag = TRUE;
    }

    gSysConfig.FirmwareYear  = FirmwareYear;
    gSysConfig.FirmwareDate  = FirmwareDate;
    gSysConfig.MasterVersion = MasterVersion;
    gSysConfig.SlaveVersion  = SlaveVersion;
    gSysConfig.SmallVersion  = SmallVersion;

    gSysConfig.SDEnable = (uint8)pSystemDefaultPara->SDEnable; //sd card founction select.
    gSysConfig.FMEnable = (uint8)pSystemDefaultPara->FMEnable; //FM resource select.
    gSysConfig.KeyNum   = (uint8)pSystemDefaultPara->KeyNumIndex;
    if(gSysConfig.FMEnable)//menu resource address select.
    {
        MenuLogicAddress = MenuLogicAddress0;//FM resource address.
    }
    else
    {
        MenuLogicAddress = MenuLogicAddress1;//FM menu resource address.
    }

    ///////////////////////////////////////////////////////////////////////
    //recover system setting parameter

    //storage media
    gSysConfig.Memory = pSysConfig->Memory;
    if (gSysConfig.Memory > CARD)
    {
        gSysConfig.Memory = FLASH0;
    }

    #ifdef _CDROM_
    gSysConfig.bShowCdrom= pSysConfig->bShowCdrom & 0x01;
    #endif

    //system language.
    gSysConfig.SysLanguage = pSysConfig->SysLanguage;
    if (gSysConfig.SysLanguage > (TOTAL_LANAUAGE_NUM - 1))
    {
        gSysConfig.SysLanguage = (uint8)pSystemDefaultPara->gLanguage;
    }

    for(i=0;i<TOTAL_LANAUAGE_NUM;i++)
    {
        if(pSystemDefaultPara->LanguagesEnableTbl[i])
        {
            SetMenuLanguageInfo[j]= i;
            if(i == gSysConfig.SysLanguage)
                DefaultLanguage = j;
            j++;
        }
    }
    LanguageNum = j;
    DefaultLanguageID = DefaultLanguage;
    gSysConfig.SysLanguage = SetMenuLanguageInfo[DefaultLanguage];

    //picture resource.
    gSysConfig.ImageIndex = pSysConfig->ImageIndex;
    if (gSysConfig.ImageIndex >= (1))
    {
        gSysConfig.ImageIndex = (uint8)pSystemDefaultPara->ImageIndex;
    }

    //system volume
    gSysConfig.OutputVolume = pSysConfig->OutputVolume;
    if (gSysConfig.OutputVolume > 32)
    {
        gSysConfig.OutputVolume = (uint8)pSystemDefaultPara->Volume;
    }

    //backlight mode
    gSysConfig.BLmode = pSysConfig->BLmode;
    if (gSysConfig.BLmode > 1)
    {
        gSysConfig.BLmode = (uint8)pSystemDefaultPara->BLMode;
    }

    //backlight level
    gSysConfig.BLevel = pSysConfig->BLevel;
    if (gSysConfig.BLevel > 5)
    {
        gSysConfig.BLevel = (uint8)pSystemDefaultPara->BLevel;
    }

    //backlight time
    gSysConfig.BLtime = pSysConfig->BLtime;
    if (gSysConfig.BLtime > 5)
    {
        gSysConfig.BLtime = (uint8)pSystemDefaultPara->BLtime;
    }

    //automatic shutdown time
    gSysConfig.ShutTime = pSysConfig->ShutTime;
    if (gSysConfig.ShutTime > 5)
    {
        gSysConfig.ShutTime = (uint8)pSystemDefaultPara->SettingPowerOffTime;
    }

    #ifdef _VIDEO_
    gSysConfig.VideoConfig.FileNum = pSysConfig->VideoConfig.FileNum;

    if (gSysConfig.VideoConfig.FileNum == 0)
    {
        gSysConfig.VideoConfig.FileNum = 1;
    }

    gSysConfig.VideoConfig.HoldOnPlaySaveFlag = pSysConfig->VideoConfig.HoldOnPlaySaveFlag;
    if (gSysConfig.VideoConfig.HoldOnPlaySaveFlag > 1)
    {
        gSysConfig.VideoConfig.HoldOnPlaySaveFlag = 0;
    }

    gSysConfig.VideoConfig.Video_Current_FrameNum = pSysConfig->VideoConfig.Video_Current_FrameNum;

    VideoHoldOnPlayInfo.CurrentFileNum = gSysConfig.VideoConfig.FileNum;
    VideoHoldOnPlayInfo.Video_Current_FrameNum = gSysConfig.VideoConfig.Video_Current_FrameNum;
    VideoHoldOnPlayInfo.HoldVideoGetSign = gSysConfig.VideoConfig.HoldOnPlaySaveFlag;

    #endif

    #ifdef _MUSIC_
    gSysConfig.MusicConfig.LrcPreTime= pSysConfig->MusicConfig.LrcPreTime;

    gSysConfig.MusicConfig.HoldOnPlaySaveFlag = pSysConfig->MusicConfig.HoldOnPlaySaveFlag;

    if (gSysConfig.MusicConfig.HoldOnPlaySaveFlag > 1)
    {
        gSysConfig.MusicConfig.HoldOnPlaySaveFlag = 0;
    }

    gSysConfig.MusicConfig.FileNum = pSysConfig->MusicConfig.FileNum;
    if (gSysConfig.MusicConfig.FileNum == 0)
    {
        gSysConfig.MusicConfig.FileNum = 1;
    }

    gSysConfig.MusicConfig.CurTime = pSysConfig->MusicConfig.CurTime;

    gSysConfig.MusicConfig.RepeatMode = pSysConfig->MusicConfig.RepeatMode;
    if (gSysConfig.MusicConfig.RepeatMode > AUDIO_TRY)
    {
        gSysConfig.MusicConfig.RepeatMode = (uint8)pSystemDefaultPara->MusicRepMode;
    }

    gSysConfig.MusicConfig.PlayOrder = pSysConfig->MusicConfig.PlayOrder;
    if (gSysConfig.MusicConfig.PlayOrder > AUDIO_RAND)
    {
        gSysConfig.MusicConfig.PlayOrder = (uint8)pSystemDefaultPara->MusicPlayOrder;
    }

	#ifdef _RK_EQ_
    gSysConfig.MusicConfig.Eq.Mode = pSysConfig->MusicConfig.Eq.Mode;
    if (gSysConfig.MusicConfig.Eq.Mode > EQ_USER)
    {
        gSysConfig.MusicConfig.Eq.Mode = pSystemDefaultPara->MusicEqSel;
    }
    for(i = 0;i < 5;i++)
    {
        gSysConfig.MusicConfig.Eq.RKCoef.dbGain[i] = pSysConfig->MusicConfig.Eq.RKCoef.dbGain[i];
        if ((UINT16)(gSysConfig.MusicConfig.Eq.RKCoef.dbGain[i]) > 7)
        {
            gSysConfig.MusicConfig.Eq.RKCoef.dbGain[i] = 3;
        }
    }
	#endif

    #ifdef _MS_EQ_
    gSysConfig.MusicConfig.Eq.MSMode = pSysConfig->MusicConfig.Eq.MSMode;
    if ((UINT16)gSysConfig.MusicConfig.Eq.MSMode > AUDIO_BB)
    {
        gSysConfig.MusicConfig.Eq.MSMode = pSystemDefaultPara->MusicPlayFxSel;
    }
    #endif

    HoldOnPlayInfo.CurrentFileNum = gSysConfig.MusicConfig.FileNum;
    HoldOnPlayInfo.HoldCurrentTime = gSysConfig.MusicConfig.CurTime;
    HoldOnPlayInfo.HoldMusicGetSign = gSysConfig.MusicConfig.HoldOnPlaySaveFlag;

    #ifdef MEDIA_MODULE
    //7.30 azg 修改连续两次关机后，断点不能正常播放的bug，以及由断点音乐退回媒体库界面时，光标定位不准的bug
    gSysConfig.MusicConfig.HoldMusicFullInfoSectorAddr = pSysConfig->MusicConfig.HoldMusicFullInfoSectorAddr;
	gSysConfig.MusicConfig.HoldMusicSortInfoSectorAddr =  pSysConfig->MusicConfig.HoldMusicSortInfoSectorAddr ;
    gSysConfig.MusicConfig.HoldMusicPlayType           = pSysConfig->MusicConfig.HoldMusicPlayType;
    gSysConfig.MusicConfig.HoldMusicuiBaseSortId       = pSysConfig->MusicConfig.HoldMusicuiBaseSortId ;
    gSysConfig.MusicConfig.HoldMusicucCurDeep          = pSysConfig->MusicConfig.HoldMusicucCurDeep;
    for(i=0;i<4;i++)
    {
        gSysConfig.MusicConfig.HoldMusicuiCurId[i]     = pSysConfig->MusicConfig.HoldMusicuiCurId[i] ;
	}
	gSysConfig.MusicConfig.HoldMusicuiTitle		       =  pSysConfig->MusicConfig.HoldMusicuiTitle ;

    HoldOnPlayInfo.HoldMusicFullInfoSectorAddr = pSysConfig->MusicConfig.HoldMusicFullInfoSectorAddr;
	HoldOnPlayInfo.HoldMusicSortInfoSectorAddr =  pSysConfig->MusicConfig.HoldMusicSortInfoSectorAddr ;
    HoldOnPlayInfo.HoldMusicPlayType           = pSysConfig->MusicConfig.HoldMusicPlayType;
    HoldOnPlayInfo.HoldMusicuiBaseSortId       = pSysConfig->MusicConfig.HoldMusicuiBaseSortId ;
    HoldOnPlayInfo.HoldMusicucCurDeep          = pSysConfig->MusicConfig.HoldMusicucCurDeep;
    for(i = 0; i < 4; i++)
    {
        HoldOnPlayInfo.HoldMusicuiCurId[i]     = pSysConfig->MusicConfig.HoldMusicuiCurId[i] ;
    }
	HoldOnPlayInfo.HoldMusicTotalFiles		   =  pSysConfig->MusicConfig.HoldMusicuiTitle ;
    #endif
    #endif

    #ifdef _RADIO_
    gSysConfig.RadioConfig.FmArea = pSysConfig->RadioConfig.FmArea;
    if (gSysConfig.RadioConfig.FmArea > 3)
    {
        gSysConfig.RadioConfig.FmArea = (uint8)pSystemDefaultPara->FMArea;
    }

    gSysConfig.RadioConfig.FmStereo = pSysConfig->RadioConfig.FmStereo;
    if (gSysConfig.RadioConfig.FmStereo > 1)
    {
        gSysConfig.RadioConfig.FmStereo = (uint8)pSystemDefaultPara->FMStereo;
    }

    gSysConfig.RadioConfig.FmState= pSysConfig->RadioConfig.FmState;
    if (gSysConfig.RadioConfig.FmState > 1)
    {
        gSysConfig.RadioConfig.FmState = 0;
    }

    gSysConfig.RadioConfig.FmSaveNum = pSysConfig->RadioConfig.FmSaveNum;
    if (gSysConfig.RadioConfig.FmSaveNum > 40)
    {
        gSysConfig.RadioConfig.FmSaveNum = 0;
    }

    gSysConfig.RadioConfig.FmFreq = pSysConfig->RadioConfig.FmFreq;

    for (i = 0; i < 40; i++)
    {
        gSysConfig.RadioConfig.FmFreqArray[i] = pSysConfig->RadioConfig.FmFreqArray[i];
    }
    #endif

    #ifdef _RECORD_
    gSysConfig.RecordConfig.RecordQuality = pSysConfig->RecordConfig.RecordQuality;
    if (gSysConfig.RecordConfig.RecordQuality > 1)
    {
        gSysConfig.RecordConfig.RecordQuality = 1;
    }

    gSysConfig.RecordConfig.RecordVol = pSysConfig->RecordConfig.RecordVol;
    if (gSysConfig.RecordConfig.RecordVol > 5)
    {
        gSysConfig.RecordConfig.RecordVol = 2;
    }
    #endif

    #ifdef _EBOOK_
    gSysConfig.TextConfig.FileNum = pSysConfig->TextConfig.FileNum;
    if (gSysConfig.TextConfig.FileNum == 0)
    {
        gSysConfig.TextConfig.FileNum = 1;
    }

    gSysConfig.TextConfig.AutoPlay = pSysConfig->TextConfig.AutoPlay;
    if (gSysConfig.TextConfig.AutoPlay > 1)
    {
        gSysConfig.TextConfig.AutoPlay = 1;
    }

    gSysConfig.TextConfig.AutoPlayTime = pSysConfig->TextConfig.AutoPlayTime;
    if (gSysConfig.TextConfig.AutoPlayTime > 5)
    {
        gSysConfig.TextConfig.AutoPlayTime = (uint8)pSystemDefaultPara->TextAutoPlayTime;
    }
    #endif

    #ifdef _PICTURE_
    gSysConfig.PicConfig.FileNum = pSysConfig->PicConfig.FileNum;
    if (gSysConfig.PicConfig.FileNum == 0)
    {
        gSysConfig.PicConfig.FileNum = 1;
    }

    gSysConfig.PicConfig.AutoPlaySwitch = pSysConfig->PicConfig.AutoPlaySwitch;
    if (gSysConfig.PicConfig.AutoPlaySwitch > 1)
    {
        gSysConfig.PicConfig.AutoPlaySwitch = 0;
    }

    gSysConfig.PicConfig.AutoPlayTime = pSysConfig->PicConfig.AutoPlayTime;
    if (gSysConfig.PicConfig.AutoPlayTime > 5)
    {
        gSysConfig.PicConfig.AutoPlayTime = (uint8)pSystemDefaultPara->PicAutoPlayTime;
    }
    #endif

    #ifdef _BLUETOOTH_
    gSysConfig.BtConfig.KeyIndex = pSysConfig->BtConfig.KeyIndex;
    if(gSysConfig.BtConfig.KeyIndex >= BT_LINK_KEY_MAX_NUM)
    {
       // gSysConfig.BtConfig.KeyIndex = 0;

        memset(&gSysConfig.BtConfig, 0x00, sizeof(BLUETOOTH_CONFIG));
    }
    else
    {
        memcpy(&gSysConfig.BtConfig,&pSysConfig->BtConfig, sizeof(BLUETOOTH_CONFIG));
    }
    #endif

    #ifdef MEDIA_MODULE //媒体库
    gSysConfig.MedialibPara.gID3AlbumFileNum     = pSysConfig->MedialibPara.gID3AlbumFileNum;
    if(gSysConfig.MedialibPara.gID3AlbumFileNum == 0xffff)
        gSysConfig.MedialibPara.gID3AlbumFileNum = 0;

    gSysConfig.MedialibPara.gID3ArtistFileNum    = pSysConfig->MedialibPara.gID3ArtistFileNum;
    if(gSysConfig.MedialibPara.gID3ArtistFileNum == 0xffff)
        gSysConfig.MedialibPara.gID3ArtistFileNum = 0;

    gSysConfig.MedialibPara.gID3TitleFileNum     = pSysConfig->MedialibPara.gID3TitleFileNum;
    if(gSysConfig.MedialibPara.gID3TitleFileNum == 0xffff)
        gSysConfig.MedialibPara.gID3TitleFileNum = 0;

    gSysConfig.MedialibPara.gID3GenreFileNum     = pSysConfig->MedialibPara.gID3GenreFileNum;
    if(gSysConfig.MedialibPara.gID3GenreFileNum == 0xffff)
        gSysConfig.MedialibPara.gID3GenreFileNum = 0;

    gSysConfig.MedialibPara.gMusicFileNum        = pSysConfig->MedialibPara.gMusicFileNum;
    if(gSysConfig.MedialibPara.gMusicFileNum == 0xffff)
        gSysConfig.MedialibPara.gMusicFileNum = 0;

    gSysConfig.MedialibPara.gMyFavoriteFileNum   = pSysConfig->MedialibPara.gMyFavoriteFileNum;
    if(gSysConfig.MedialibPara.gMyFavoriteFileNum == 0xffff)
        gSysConfig.MedialibPara.gMyFavoriteFileNum = 0;

    gSysConfig.MedialibPara.MediaUpdataFlag = pSysConfig->MedialibPara.MediaUpdataFlag;
    if(gSysConfig.MedialibPara.MediaUpdataFlag>1)
        gSysConfig.MedialibPara.MediaUpdataFlag =1;
    #endif
	#ifdef _USB_
    gSysConfig.UsbConfig.UsbMode = 1; // 0: MSC; 1: USB Audio; 2: USB Host
	#endif

	gSysConfig.sensor_calibias = pSysConfig->sensor_calibias;
	gSysConfig.sensor_accel_bias[0] = pSysConfig->sensor_accel_bias[0];
	gSysConfig.sensor_accel_bias[1] = pSysConfig->sensor_accel_bias[1];
	gSysConfig.sensor_accel_bias[2] = pSysConfig->sensor_accel_bias[2];

	gSysConfig.sensor_gyro_bias[0] = pSysConfig->sensor_gyro_bias[0];
	gSysConfig.sensor_gyro_bias[1] = pSysConfig->sensor_gyro_bias[1];
	gSysConfig.sensor_gyro_bias[2] = pSysConfig->sensor_gyro_bias[2];




}

/*
--------------------------------------------------------------------------------
  Function name : uint32 FindIDBlock(void)
  Author        : ZHengYongzhi
  Description   : find IDBlock

  Input         :
  Return        : IDBlock or 0xffffffff

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYSRESERVED_OP_CODE_
void SaveSysInformation(void)
{
    uint8  TempBuffer[1024];

    memset(TempBuffer, 0xff, 1024);
    memcpy(TempBuffer, (uint8*)&gSysConfig, sizeof(SYSCONFIG));

    WriteReservedData(SYSDATA_BLK, TempBuffer, 1024);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get frimware resource address information

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OVERLAY_CODE_
uint32 ModuleOverlay(uint32 ModuleNum, uint8 Type)
{
    #ifndef _JTAG_DEBUG_
    ModuleOverlayExt(ModuleNum, Type);
    #endif

    return (OK);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get frimware resource address information

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void FlashTotalMemory(void)
{
    //Get flash0 memory Info
    flash0TotalMem = GetTotalMem(FLASH0) / 2;//* 512 /1024;//Sector turn to Kbyte
    if(flash0TotalMem > 0x3e800000)
    {
        flash0TotalMem -= 1000000;
    }

    //Get flash1 memory Info
    #ifdef _MULT_DISK_
    flash1TotalMem = GetTotalMem(FLASH1) / 2;//* 512 /1024;     //Sector turn to Kbyte
    if(flash1TotalMem > 0x3e800000)
    {
       flash1TotalMem -= 1000000;
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get frimware resource address information

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void FlashFreeMemory(void)
{
    if (GetMsg(MSG_FLASH_MEM_UPDATE))
    {
        sysFreeMemeryFlash0 = GetFreeMem(FLASH0);
        #ifdef _MULT_DISK_
        sysFreeMemeryFlash1 = GetFreeMem(FLASH1);
        #endif
    }
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get frimware resource address information

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#ifdef _SDCARD_
void SDCardInfoUpdate(void)
{
    uint32 ImaskTemp;

    if (GetMsg(MSG_SDCARD_MEM_UPDATE))
    {
        if (CheckCard() != 0)
        {
            ImaskTemp = UserIsrDisable();
            if (MemorySelect != CARD)
            {
                SDCardEnable();
                MDScanDev(1,MEM_DEV_SCAN_ALL_FLASH);
            }
            cardTotalMem      = GetTotalMem(CARD) / 2;//* 512 /1024; //Sector turn to Kbyte
            sysFreeMemeryCard = GetFreeMem(CARD);

            if (MemorySelect != CARD)//slove the bug:it can not continue play the sd card file,
            {
                SDCardDisable();
            }
            UserIsrEnable(ImaskTemp);
        }
        else
        {
            cardTotalMem      = 0;
            sysFreeMemeryCard = 0;
        }
    }
}
#endif

/*
********************************************************************************
*
*                         End of SysReservedOperation.c
*
********************************************************************************
*/

