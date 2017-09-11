/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  AudioControl.C
*
* Description:
*
* History:      <author>          <time>        <version>
*                 ZS      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#include "SysInclude.h"

#ifdef _MUSIC_

#include "FileInfoSaveMacro.h"
#include "audio_globals.h"
#include "audio_file_access.h"
#include "HoldonPlay.h"
#include "AudioControl.h"
#include "fade.h"

#ifdef MEDIA_MODULE
#include "MediaBroWin.h"
#include "AddrSaveMacro.h"
#endif

extern void CloseTrack(void);
extern void AudioEQUpdate(void);
extern void AudioEQProcess(short *outptr, int32 len);
extern BOOLEAN GetMusicUIState(void);

extern AUDIO_CONTROL_OPS AudioControlOps;
extern DMA_CFGX AudioControlDmaCfg;

_ATTR_AUDIOFILE_BSS_  uint32   DecodingError;
_ATTR_AUDIOFILE_DATA_ BOOLEAN  FileNumGlobalOrFolder = 1;//0是目录内的文件号，1是全局文件号// by zs 09.07.03

#ifdef _RK_ID3_

_ATTR_AUDIOFILE_BSS_  FILE_SAVE_STRUCT AudioID3Info;
/*
--------------------------------------------------------------------------------
  Function name :uint32 AudioGetID3Info(void)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int  AudioGetID3Info(void)
{
    FILE_SAVE_STRUCT ID3FileInfo;
    int   ret = 0;

    HANDLE Id3Handle;

    ModuleOverlay(MODULE_ID_AUDIO_ID3,MODULE_OVERLAY_ALL);

    #ifdef _RK_ID3_JPEG_
    ModuleOverlay(MODULE_ID_AUDIO_ID3_JPEG,MODULE_OVERLAY_ALL);
    ID3AlbumArtInit(ID3_PICTUTER_WIDTH,ID_3PICTUTER_HEIGTH);
    ret =  ID3_GetID3((int)pRawFileCache);
    if(ret == -2)
    {
        return -2 ;
    }
    #endif

    #ifdef _RK_ID3_
    if(CurrentCodec == CODEC_MP3)
    {
        Id3Handle = FileOpen(AudioFileInfo.Path, AudioFileInfo.Fdt.Name, FileOpenStringR);
        if(Id3Handle!=-1)
        {
            GetAudioId3Info((FILE*)Id3Handle , &ID3FileInfo, &AudioFileInfo.Fdt.Name[8]);
            memcpy(&AudioID3Info, &ID3FileInfo, sizeof(FILE_SAVE_STRUCT));
        }
        FileClose(Id3Handle);
        return ret;
    }
    memset(&ID3FileInfo, 0, sizeof(FILE_SAVE_STRUCT));
    FileSeek(0, 0 ,(HANDLE)pRawFileCache);
    GetAudioId3Info(pRawFileCache , &ID3FileInfo, &AudioFileInfo.Fdt.Name[8]);
    memcpy(&AudioID3Info, &ID3FileInfo, sizeof(FILE_SAVE_STRUCT));
    FileSeek(0, 0 ,(HANDLE)pRawFileCache);
    #endif

    return ret;
}

#endif //_RK_ID3_

/*
--------------------------------------------------------------------------------
  Function name :  UINT16  DiskChange(void)
  Author        :  zs
  Description   :  if no file in current disk, when enter the audio module in the situation
                   of more than one disk(no media libary),then poll the following disk.
  Input         :  null
  Return        :  0 -- find file from disk， 1-- no fine in disk
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 DiskChangeToFlash0(void)
{
    gSysConfig.Memory = FLASH0;//to switch to flash1 at frist.
    FileSysSetup(gSysConfig.Memory);
    AudioFileInfo.TotalFiles = BuildDirInfo((UINT8*)AudioFileExtString);
    if (AudioFileInfo.TotalFiles != 0)
    {
        return OK;
    }
    return ERROR;
}

#ifdef _MULT_DISK_
_ATTR_AUDIOFILE_TEXT_
int32 DiskChangeToFlash1(void)
{
    if(bShowFlash1)
    {
        gSysConfig.Memory = FLASH1;//to switch to flash1 at frist.
        FileSysSetup(gSysConfig.Memory);
        AudioFileInfo.TotalFiles = BuildDirInfo((UINT8*)AudioFileExtString);
        if (AudioFileInfo.TotalFiles != 0)
        {
            return OK;
        }
    }
    return ERROR;
}
#endif

#ifdef _SDCARD_
_ATTR_AUDIOFILE_TEXT_
int32 DiskChangeToSDCard(void)
{
    if(gSysConfig.SDEnable)
    {
        ModuleOverlay(MODULE_ID_SDMMC, MODULE_OVERLAY_ALL);
        if (CheckCard() == 1) //have sd card.
        {
            MemDevInit(MEM_DEV_MASK_SD);

            gSysConfig.Memory = CARD;
            FileSysSetup(gSysConfig.Memory);
            AudioFileInfo.TotalFiles = BuildDirInfo((UINT8*)AudioFileExtString);
            if (AudioFileInfo.TotalFiles != 0)
            {
                return OK;
            }
            MDDeInit(MEM_DEV_MASK_SD);
        }
    }
    return ERROR;
}
#endif

#ifdef _USBHOST_
_ATTR_AUDIOFILE_TEXT_
int32 DiskChangeToHostMsc(void)
{
    if (gSysConfig.UsbConfig.UsbMode == 2)
    {
        ModuleOverlay(MODULE_ID_USB_HOST, MODULE_OVERLAY_ALL);
        {
            MemDevInit(MEM_DEV_MASK_HOST);

            gSysConfig.Memory = UDISK0;
            FileSysSetup(gSysConfig.Memory);
            AudioFileInfo.TotalFiles = BuildDirInfo((UINT8*)AudioFileExtString);
            if (AudioFileInfo.TotalFiles != 0)
            {
                return OK;
            }
            MDDeInit(MEM_DEV_MASK_HOST);
        }
    }
    return ERROR;
}
#endif

/*
--------------------------------------------------------------------------------
  Function name :int32 AudioFileInit(void *pArg)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 AudioFileInit(void *pArg)
{
    int32 ret = RETURN_OK;

    PMU_EnterModule(PMU_MAX);
    gbAudioFileNum = ((AUDIO_THREAD_ARG*)pArg)->FileNum;

    if((gbAudioFileNum == 0xFFFF)||(gbAudioFileNum == 0))//by zs 05.13 fix bug
    {
        gbAudioFileNum = 1;
    }

    #ifdef MEDIA_MODULE
    if(AudioFileInfo.ucSelPlayType == SORT_TYPE_SEL_BROWSER)
    #endif
    {
        if(0 == (AudioFileInfo.TotalFiles = BuildDirInfo((UINT8*)AudioFileExtString)))
        {
            if (gSysConfig.Memory != FLASH0)
            {
                if (OK == DiskChangeToFlash0())
                    goto DiskChangeOK;
            }

            #ifdef _MULT_DISK_
            if (gSysConfig.Memory != FLASH1)
            {
                if (OK == DiskChangeToFlash1())
                    goto DiskChangeOK;
            }
            #endif

            #ifdef _SDCARD_
            if (gSysConfig.Memory != CARD)
            {
                if (OK == DiskChangeToSDCard())
                    goto DiskChangeOK;
            }
            #endif

            #ifdef _USBHOST_
            if (gSysConfig.Memory != UDISK0)
            {
                if (OK == DiskChangeToHostMsc())
                    goto DiskChangeOK;
            }
            #endif
            //find file error
            gSysConfig.Memory = FLASH0;
            FileSysSetup(gSysConfig.Memory);
            SendMsg(MSG_AUDIO_NOFILE);
            ret = RETURN_FAIL;
            goto error;
DiskChangeOK:
        }
    }

    pAudioRegKey->RepeatMode    = gSysConfig.MusicConfig.RepeatMode;
    if((pAudioRegKey->RepeatMode == AUDIO_FOLDER_ONCE)||(pAudioRegKey->RepeatMode == AUIDO_FOLDER_REPEAT))
    {
         pAudioRegKey->AudioFileDirOrAll =  FIND_FILE_RANGE_DIR;
    }

    pAudioRegKey->PlayOrder     = gSysConfig.MusicConfig.PlayOrder;
    #ifdef MEDIA_MODULE
    if(AudioFileInfo.ucSelPlayType != SORT_TYPE_SEL_BROWSER)
    {
        AudioFileInfo.PlayOrder     = gSysConfig.MusicConfig.PlayOrder;
        if(GetMsg(MSG_MEDIA_BREAKPOINT_PLAY)==FALSE)
        {
            memcpy(MusicLongFileName,&LongFileName,MAX_FILENAME_LEN*2);
        }
    }
    else
    #endif
    {
        SysFindFileInit(&AudioFileInfo, gbAudioFileNum, pAudioRegKey->AudioFileDirOrAll,
			             pAudioRegKey->PlayOrder, (UINT8*)AudioFileExtString);
    }

    if (AudioFileInfo.TotalFiles > SORT_FILENUM_DEFINE)
    {
        AudioFileInfo.TotalFiles = SORT_FILENUM_DEFINE;
        if (AudioFileInfo.CurrentFileNum > AudioFileInfo.TotalFiles)
            AudioFileInfo.CurrentFileNum = AudioFileInfo.TotalFiles - 1;
    }

    //CreateRandomList(AudioFileInfo.TotalFiles,AudioFileInfo.CurrentFileNum - 1, MaxShuffleAllCount);

error:
    PMU_ExitModule(PMU_MAX);

    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name :int32 AudioFileInit(void *pArg)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 AudioFileDeInit(void)
{
    gSysConfig.MusicConfig.RepeatMode = pAudioRegKey->RepeatMode;
    gSysConfig.MusicConfig.PlayOrder  = pAudioRegKey->PlayOrder;

    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name :int32 AudioFileInit(void *pArg)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void AudioFileHoldOnInit(void)
{
    #ifdef AUDIOHOLDONPLAY
    #ifdef MEDIA_MODULE
    if(((HoldOnPlayInfo.HoldMusicGetSign == 1)||(gSysConfig.MusicConfig.HoldOnPlaySaveFlag == 1))&&(HoldOnPlayInfo.CurrentFileNum ==gbAudioFileNum )&&(AudioFileInfo.ucSelPlayType==HoldOnPlayInfo.HoldMusicPlayType))
    #else
    if(((HoldOnPlayInfo.HoldMusicGetSign == 1)||(gSysConfig.MusicConfig.HoldOnPlaySaveFlag == 1))&&(HoldOnPlayInfo.CurrentFileNum ==gbAudioFileNum ))
    #endif
    {
        HoldMusicStart();
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name :int32 AudioFileInit(void *pArg)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void AudioFileHoldOnProcess(void)
{
    #ifdef AUDIOHOLDONPLAY
    MusicHoldOnPlay();
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name :int32 AudioFileInit(void *pArg)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void AudioFileHoldOnDeInit(void)
{
    #ifdef AUDIOHOLDONPLAY
    #ifdef MEDIA_MODULE
    if (gSysConfig.Memory == FLASH0) //the music can not breakpoint play when the music in sd card.
    #endif
    {
	    MusicHoldOnInforSave(pAudioRegKey->CurrentTime, &AudioFileInfo);
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name : void AudioSetEQ(void)
  Author        : zs
  Description   : set current codec type.
  Input         : Codec_Type
  Return        : null
  History       : <author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void AudioCodec(UINT8 *pBuffer, UINT8 *pStr)
{
    UINT8 temp;
    temp = GetFileType(pBuffer, pStr);

    switch(temp)
   {
    case 1:     //mp3
    case 2:     //mp2
    case 3:     //mp1
        #ifdef MP3_INCLUDE
        CurrentCodec = CODEC_MP3;
        #endif
        break;

    case 4:     //wma
        #ifdef  WMA_INCLUDE
        CurrentCodec = CODEC_WMA;
        #endif
        break;

    case 5:     //wav
        #ifdef WAV_INCLUDE
        CurrentCodec = CODEC_WAV;
        #endif
        break;

    case 6:     //ape
        #ifdef APE_DEC_INCLUDE
        CurrentCodec = CODEC_APE_DEC;
        #endif
        break;

    case 7:     //flac
        #ifdef FLAC_DEC_INCLUDE
        CurrentCodec = CODEC_FLAC_DEC;
        #endif
        break;
    case 8:     //AAC
    case 9:     //M4A
        #ifdef AAC_INCLUDE
        CurrentCodec = CODEC_AAC;
        #endif
        break;

    default:
        break;
    }
}

/*
--------------------------------------------------------------------------------
  Function name :uint32 AudioFileOpen(void)
  Author        : zyz
  Description   :
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 AudioFileOpen(void)
{
    DecodingError = 0;
    AudioFileSeekOffset = 0;

    AudioCodec(&AudioFileInfo.Fdt.Name[8],(UINT8 *)AudioFileExtString);

    pRawFileCache = (FILE*)-1;
    pRawFileCache = (FILE*)FileOpen(AudioFileInfo.Path, AudioFileInfo.Fdt.Name, FileOpenStringR);
    if((int)pRawFileCache >= MAX_OPEN_FILES)
    {
        return RETURN_FAIL;
    }
    AudioPlayFileNum ++;

    //to open the flac file two time for improving efficiency of file seek.
    #ifdef FLAC_DEC_INCLUDE
	if(CODEC_FLAC_DEC == CurrentCodec)
	{
	    pFlacFileHandleBake = (FILE*)FileOpen(AudioFileInfo.Path, AudioFileInfo.Fdt.Name, FileOpenStringR);
	    if((int)pFlacFileHandleBake >= MAX_OPEN_FILES)
	    {
	        return RETURN_FAIL;
	    }
	}
    #endif

    #ifdef MEDIA_MODULE
    if(AudioFileInfo.ucSelPlayType == SORT_TYPE_SEL_BROWSER)
    #endif
    {
        GetLongFileName(AudioFileInfo.FindData.Clus,AudioFileInfo.FindData.Index, MusicLongFileName);
    }

    #ifdef _RK_ID3_
    AudioGetID3Info();
    #endif

    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name :uint32 AudioFileClose(void)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 AudioFileClose(void)
{
    RKFIO_FClose((FILE*)pRawFileCache);
    pRawFileCache = (FILE*)-1;

    #ifdef FLAC_DEC_INCLUDE
	if(CurrentCodec == CODEC_FLAC_DEC)
	{
	    RKFIO_FClose((FILE*)pFlacFileHandleBake);
	    pFlacFileHandleBake = (FILE*)-1;
	}
    #endif

    #if 0//def _SDCARD_ //by zs 05.18 slove the machine crash as pull out sd card.
    if ((gSysConfig.Memory == CARD) && (CheckCard() == 0))
    {
        if(FALSE == GetMusicUIState())
        {
            SendMsg(MSG_AUDIO_CARD_CHECK);

            gSysConfig.Memory = FLASH0;

            FileSysSetup(gSysConfig.Memory);

	    }
        return FALSE;
    }
    #endif

}

/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN AudioIsPlaying(void)
  Author        : zs
  Description   : judge whether is in playing status.
  Input         : null
  Return        : 1:playing;
                  0:no playing.
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
BOOLEAN AudioGetNextMusic(int16 msg)
{
    BOOLEAN ret = TRUE;

    if ((AUDIO_STATE_STOP != AudioPlayState)||(AudioStopMode ==Audio_Stop_Force))
    {
        FadeInit(0,pAudioRegKey->samplerate/2,FADE_OUT);
        //FadeFinishedWait(pAudioRegKey->samplerate/2);
        while(AudioPlayInfo.VolumeCnt > 1)
        {
            AudioPlayInfo.VolumeCnt--;
            Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
            DelayMs(5);
        }

        AudioStop(Audio_Stop_Force);
    }

    switch (pAudioRegKey->RepeatMode)
    {
        case AUDIO_REPEAT://single repeat
            if (Audio_Stop_Force == AudioStopMode)
            {
                SysFindFile(&AudioFileInfo,(INT16)msg);
            }
            break;

       case AUDIO_TRY: //ok lyrics
       case AUDIO_ALLONCE://only once all song play cycle.
            if((AudioPlayFileNum >= AudioFileInfo.TotalFiles)&&(AudioStopMode != Audio_Stop_Force))
            {
                AudioPlayFileNum = 0;
                SysFindFile(&AudioFileInfo,(INT16)msg);
				AudioStart();
				AudioPause();
                return TRUE;
            }
            else
            {
                SysFindFile(&AudioFileInfo,(INT16)msg);
            }
            break;

        case AUDIO_ALLREPEAT://all cycle play
            AudioFileInfo.Range = (INT16)FIND_FILE_RANGE_ALL;
            {
                SysFindFile(&AudioFileInfo,(INT16) msg);
            }
            break;

        case AUDIO_ONCE://single once.
            if (Audio_Stop_Force == AudioStopMode)
            {
                SysFindFile(&AudioFileInfo,(INT16) msg);
                break;
            }
            else
            {
                AudioStart();

                AudioPause();

                if(AudioPlayFileNum >= AudioFileInfo.TotalFiles)
                {
                     AudioPlayFileNum = 0;
                }
                return TRUE;
            }
            break;

         case AUDIO_FOLDER_ONCE://directory once.
             AudioFileInfo.Range = (INT16)FIND_FILE_RANGE_DIR;

            if((AudioPlayFileNum >= AudioFileInfo.TotalFiles)&&(AudioStopMode != Audio_Stop_Force))
            {
                 AudioPlayFileNum = 0;
                 SysFindFile(&AudioFileInfo,(INT16)msg);

                 AudioStart();

                 AudioPause();

                 return TRUE;
            }
            else
            {
                SysFindFile(&AudioFileInfo,(INT16)msg);
            }
            break;

        case AUIDO_FOLDER_REPEAT://directory cycle.
            AudioFileInfo.Range = (INT16)FIND_FILE_RANGE_DIR;

            if((AudioPlayFileNum >= AudioFileInfo.TotalFiles)&&(AudioStopMode != Audio_Stop_Force))
            {
                AudioPlayFileNum = 0;
                SysFindFile(&AudioFileInfo,(INT16)msg);
                AudioStart();
                return TRUE;
            }
            else
            {
                SysFindFile(&AudioFileInfo,(INT16)msg);
            }
            break;

        default:
            ret = FALSE;
            break;
    }

    if(AudioPlayFileNum >= AudioFileInfo.TotalFiles)
    {
        AudioPlayFileNum = 0;
    }

    AudioStart();

    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name :  BOOLEAN AudioFFD(unsigned long StepLen)
  Author        :  zs
  Description   :  audio FFD
  Input         :  StepLen:The step of FFD.
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
BOOLEAN AudioFFW(unsigned long StepLen)
{
    uint32 timeout = 200;

    if (AUDIO_STATE_STOP == AudioPlayState)
    {
        return TRUE;
    }

    if (AUDIO_STATE_FFW != AudioPlayState)  //wait for mute
    {
        if (AUDIO_STATE_PLAY == AudioPlayState)
        {
            FadeInit(0,pAudioRegKey->samplerate/2,FADE_OUT);
            FadeFinishedWait(pAudioRegKey->samplerate/2);
        }

        AudioPlayState = AUDIO_STATE_FFW;

        while(DmaGetState(AUDIO_DMACHANNEL_IIS) == DMA_BUSY)
        {
            DelayMs(1);
            if (--timeout == 0)
            {
                break;
            }
        }

        AudioPlayInfo.VolumeCnt = 0;
        Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
    }
    else //if (AUDIO_STATE_FFW == AudioPlayState)   //FFW
    {
        pAudioRegKey->CurrentTime = (pAudioRegKey->CurrentTime > StepLen) ? (pAudioRegKey->CurrentTime - StepLen) : 0;
        CodecSeek(pAudioRegKey->CurrentTime, 0);

        if(CurrentCodec == CODEC_MP3)
        {
            if(AudioFileSeekOffset)
            {
                FileSeek(AudioFileSeekOffset, 0, (HANDLE)pRawFileCache);
            }
        }
    }

    return TRUE;



    #if 0
    if (AUDIO_STATE_STOP == AudioPlayState)
    {
        return TRUE;
    }
    else if ((AUDIO_STATE_PLAY == AudioPlayState) && (AudioPlayInfo.VolumeCnt == AudioPlayInfo.playVolume))
    {
        SendMsg(MSG_MUSIC_FADE_OUT);
        return TRUE;
    }
    else if((!CheckMsg(MSG_MUSIC_FADE_OUT)) && (AudioPlayState != AUDIO_STATE_FFW))
    {
        AudioPlayState = AUDIO_STATE_FFW;
        return TRUE;
    }
    else if(!CheckMsg(MSG_MUSIC_FADE_OUT))
    {
        pAudioRegKey->CurrentTime = (pAudioRegKey->CurrentTime > StepLen) ? (pAudioRegKey->CurrentTime - StepLen) : 0;
        CodecSeek(pAudioRegKey->CurrentTime, 0);

        if(CurrentCodec == CODEC_MP3)
        {
            if(AudioFileSeekOffset)
                FileSeek(AudioFileSeekOffset, 0, (HANDLE)pRawFileCache);
        }

        #ifdef  _RK_EQ_
        EffectAdjust();
    	#endif

        return TRUE;
    }
    #endif
}
/*
--------------------------------------------------------------------------------
  Function name :  BOOLEAN AudioFFW(unsigned long StepLen)
  Author        :  zs
  Description   :  audio FFW
  Input         :  StepLen:The step of FFW
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
BOOLEAN AudioFFD(unsigned long StepLen)
{
    uint32 timeout = 200;

    if (AUDIO_STATE_STOP == AudioPlayState)
    {
        return TRUE;
    }

    if (AUDIO_STATE_FFD != AudioPlayState)  //wait for mute
    {
        if (AUDIO_STATE_PLAY == AudioPlayState)
        {
            FadeInit(0,pAudioRegKey->samplerate/2,FADE_OUT);
            FadeFinishedWait(pAudioRegKey->samplerate/2);
        }

        AudioPlayState = AUDIO_STATE_FFD;
        while(DmaGetState(AUDIO_DMACHANNEL_IIS) == DMA_BUSY)
        {
            DelayMs(1);
            if (--timeout == 0)
            {
                break;
            }
        }

        AudioPlayInfo.VolumeCnt = 0;
        Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
    }
    else //if (AUDIO_STATE_FFD == AudioPlayState)   //FFD
    {
        pAudioRegKey->CurrentTime = (pAudioRegKey->CurrentTime < pAudioRegKey->TotalTime - StepLen) ? (pAudioRegKey->CurrentTime + StepLen) : pAudioRegKey->TotalTime;
        CodecSeek(pAudioRegKey->CurrentTime, 0);

        if (pAudioRegKey->CurrentTime >= pAudioRegKey->TotalTime)
        {
            pAudioRegKey->CurrentTime = pAudioRegKey->TotalTime;
            FileInfo[(uint32)pRawFileCache].Offset = FileInfo[(uint32)pRawFileCache].FileSize;
        }
    }

    return TRUE;

    #if 0
    if (AUDIO_STATE_STOP == AudioPlayState)
    {
        return TRUE;
    }
    else if ((AUDIO_STATE_PLAY == AudioPlayState) && (AudioPlayInfo.VolumeCnt == AudioPlayInfo.playVolume))
    {
        SendMsg(MSG_MUSIC_FADE_OUT);
        return TRUE;
    }
    else if((!CheckMsg(MSG_MUSIC_FADE_OUT)) && (AudioPlayState != AUDIO_STATE_FFD))
    {
        AudioPlayState = AUDIO_STATE_FFD;
        return TRUE;
    }
    else if(!CheckMsg(MSG_MUSIC_FADE_OUT))
    {
        pAudioRegKey->CurrentTime = (pAudioRegKey->CurrentTime < pAudioRegKey->TotalTime - StepLen) ? (pAudioRegKey->CurrentTime + StepLen) : pAudioRegKey->TotalTime;
        CodecSeek(pAudioRegKey->CurrentTime, 0);

        if (pAudioRegKey->CurrentTime >= pAudioRegKey->TotalTime)
        {
            pAudioRegKey->CurrentTime = pAudioRegKey->TotalTime;
            FileInfo[(uint32)pRawFileCache].Offset = FileInfo[(uint32)pRawFileCache].FileSize;
        }

        #ifdef  _RK_EQ_
        EffectAdjust();
    	#endif

        return TRUE;
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name :  void AudioSetAB_A(void)
  Author        :  zs
  Description   :  set the start point A for AB repeat read.
  Input         :  null
  Return        :  null
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void AudioSetAB_A(void)
{
    CodecGetTime(&pAudioRegKey->CurrentTime);
    AudioPlayInfo.ABRequire = AUDIO_AB_A;
    AudioPlayInfo.AudioABStart = pAudioRegKey->CurrentTime;
}

/*
--------------------------------------------------------------------------------
  Function name :  void AudioSetAB(void)
  Author        :  zs
  Description   :  set the end point of AB repeat, and start repeat.
  Input         :  null
  Return        :  null
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void AudioSetAB(void)
{
    uint32 timeout = 100;

    if (AudioPlayInfo.ABRequire == AUDIO_AB_A)    //set the end point of repeat.
    {
        CodecGetTime(&pAudioRegKey->CurrentTime);
        AudioPlayInfo.AudioABEnd = pAudioRegKey->CurrentTime;
        AudioPlayInfo.ABRequire = AUDIO_AB_PLAY;

        //wait for fade out finished
        if (AUDIO_STATE_PLAY == AudioPlayState)
        {
            FadeInit(0,pAudioRegKey->samplerate / 2,FADE_OUT);
            FadeFinishedWait(pAudioRegKey->samplerate/2);
        }

        //wait DMA finished
        AudioPlayState = AUDIO_STATE_FFW;
        while(DmaGetState(AUDIO_DMACHANNEL_IIS) == DMA_BUSY)
        {
            DelayMs(1);
            if (--timeout == 0)
            {
                break;
            }
        }

        AudioPlayInfo.VolumeCnt = 0;
        Codec_SetVolumet(AudioPlayInfo.VolumeCnt);

        // Set to A Point
        AudioFFW((pAudioRegKey->CurrentTime - AudioPlayInfo.AudioABStart));

        //Start Fadein and Play
        AudioPlayState = AUDIO_STATE_PLAY;

        FadeInit(0,pAudioRegKey->samplerate/2,FADE_IN);

        AudioErrorFrameNum = 0;
        SendMsg(MSG_MUSIC_DISPFLAG_STATUS);//cw 2009-5-11

        CodecGetCaptureBuffer((short*)&outptr,&AudioIOBuf.lOutLength);
        memset((uint8*)outptr, 0, AudioIOBuf.lOutLength * sizeof(int16) * 2);
        DmaStart(AUDIO_DMACHANNEL_IIS, (UINT32)outptr, (uint32)(&(I2s_Reg->I2S_TXDR)),  AudioIOBuf.lOutLength, &AudioControlDmaCfg, AudioDmaIsrHandler);

    }

    #if 0
    uint32 timeout = 100;

    if (AudioPlayInfo.ABRequire == AUDIO_AB_A)    //set the end point of repeat.
    {
        CodecGetTime(&pAudioRegKey->CurrentTime);

        AudioPlayInfo.AudioABEnd = pAudioRegKey->CurrentTime;
        AudioPlayInfo.ABRequire = AUDIO_AB_PLAY;

        AudioPlayState = AUDIO_STATE_FFW;

        SendMsg(MSG_MUSIC_FADE_OUT);
        while(CheckMsg(MSG_MUSIC_FADE_OUT))
        {
            ClearMsg(MSG_MUSIC_FADE_IN);
            if(AudioPlayInfo.VolumeCnt > 0)
            {
                AudioPlayInfo.VolumeCnt--;
                Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
            }
            else
            {
                ClearMsg(MSG_MUSIC_FADE_OUT);
            }
        }

        while(DmaGetState(AUDIO_DMACHANNEL_IIS) == DMA_BUSY)
        {
            DelayMs(1);
            if (--timeout == 0)
            {
                break;
            }
        }
        AudioDecodeProc(MSG_AUDIO_FFW, (void*)(pAudioRegKey->CurrentTime - AudioPlayInfo.AudioABStart));
        AudioResume();
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name :  void AudioABStop(void)
  Author        :  zs
  Description   :  stop repeat.
  Input         :  null
  Return        :  null
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void AudioABStop(void)
{
    AudioPlayInfo.ABRequire = AUDIO_AB_NULL;
}

/*
--------------------------------------------------------------------------------
  Function name :  void AudioABProcess(void)
  Author        :
  Description   :  stop repeat.
  Input         :
  Return        :
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void AudioABProcess(void)
{
    uint32 cur_time = 0;

    cur_time = pAudioRegKey->CurrentTime;
    if ((AUDIO_AB_PLAY == AudioPlayInfo.ABRequire) && (AudioPlayInfo.AudioABEnd <= cur_time))
    {
        AudioPlayInfo.ABRequire = AUDIO_AB_A;
        AudioSetAB();
    }
}

/*
--------------------------------------------------------------------------------
  Function name : int32 AudioFileDecodingErr(uint32 error)
  Author        :
  Description  :  Decodeing Error process
  Input          :
  Return        :
  History        : <author>         <time>         <version>
                                2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 AudioFileDecodingErr(uint32 error)
{
    uint32 ret = 0;

    #if 0
    if (0 == error)
    {
		MusicNextFile = 1;

        if (FileInfo[(uint32)pRawFileCache].Offset <= FileInfo[(uint32)pRawFileCache].FileSize / 2)
        {
            if (DecodingError != 1)
            {
                DecodingError = 1;

                SendMsg(MSG_AUDIO_FILE_ERROR);
                AudioStop(Audio_Stop_Normal);

                ret = RETURN_FAIL;
            }
        }
        else
        {
            AudioErrorFileCount = 0;

            AudioStop(Audio_Stop_Normal);

            ret = RETURN_FAIL;
        }
    }
    else if ((pAudioRegKey->CurrentTime >= (pAudioRegKey->TotalTime+1000)) && (0 < pAudioRegKey->CurrentTime))
    {
        MusicNextFile = 1;
        AudioErrorFileCount = 0;
        AudioStop(Audio_Stop_Normal);

        ret = RETURN_FAIL;
    }
    #endif

    if (0 == error)   // Decoding end or error
    {
        if (FileInfo[(uint32)pRawFileCache].Offset <= FileInfo[(uint32)pRawFileCache].FileSize / 2)
        {
            AudioPlayState = AUDIO_STATE_STOP;
            memset((uint8*)outptr, 0x00, AudioIOBuf.lOutLength * 4);
            SendMsg(MSG_AUDIO_DECODE_ERROR);
            ret = RETURN_FAIL;
        }
        else
        {
            AudioPlayState = AUDIO_STATE_STOP;
            memset((uint8*)outptr, 0x00, AudioIOBuf.lOutLength * 4);
            SendMsg(MSG_AUDIO_DECODE_END);
            ret = RETURN_FAIL;
        }
    }

    #if 0
    // last 500ms for Fadeout
    if ((pAudioRegKey->TotalTime - pAudioRegKey->CurrentTime) <= 400)
    {
        if (FadeIsFinished())
        {
            FadeInit(0,pAudioRegKey->samplerate/2,FADE_OUT);
        }
    }
    #endif

    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name : int32 AudioFileServiceHook(void)
  Author         :
  Description   : Msg process
  Input         :
  Return        :
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 AudioFileServiceHook(void)
{
    int32 ret = RETURN_OK;

    #if 0
    if(TRUE == GetMsg(MSG_AUDIO_FILE_ERROR))
    {
        if (Task.TaskID != TASK_ID_MUSIC)
        {
            AudioErrorFileCount++;
            if ((AudioFileInfo.TotalFiles <= AudioErrorFileCount) ||
                (AUDIO_ONCE == pAudioRegKey->RepeatMode) ||
                (AUDIO_REPEAT == pAudioRegKey->RepeatMode))

            {
                AudioDecodeProc(MSG_AUDIO_STOP,(void*)Audio_Stop_Force);
            }
            else
            {
                AudioDecodeProc(MSG_AUDIO_STOP,(void*)Audio_Stop_Normal);
            }
        }
    }
    #endif

    if (GetMsg(MSG_AUDIO_DECODE_ERROR))
    {
        MusicNextFile = 1;
        //AudioStop(Audio_Stop_Normal);

        if (Task.TaskID != TASK_ID_MUSIC)
        {
            if(TRUE == GetMsg(MSG_AUDIO_FILE_ERROR))
            {
                AudioErrorFileCount++;
                if ((AudioFileInfo.TotalFiles <= AudioErrorFileCount) ||
                    (AUDIO_ONCE == pAudioRegKey->RepeatMode) ||
                    (AUDIO_REPEAT == pAudioRegKey->RepeatMode))

                {
                    AudioDecodeProc(MSG_AUDIO_STOP,(void*)Audio_Stop_Force);
                }
                else
                {
                    AudioDecodeProc(MSG_AUDIO_STOP,(void*)Audio_Stop_Normal);
                }
            }
        }
        else
        {
            SendMsg(MSG_AUDIO_FILE_ERROR);
        }
        return ret;

    }

    if (GetMsg(MSG_AUDIO_DECODE_END))
    {
        MusicNextFile = 1;
        AudioErrorFileCount = 0;
        AudioStop(Audio_Stop_Normal);
        return ret;
    }

    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name :int32 AudioFileRepeatModeUpdate(void)
  Author         : Update repeat mode from setmenu
  Description   :
  Input         :
  Return        :
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 AudioFileRepeatModeUpdate(void)
{
    UINT16 tempFileNum;
    INT16  FindFileResult;
    UINT16 i,j;

    if ((AUDIO_TRY == pAudioRegKey->RepeatMode) && ((AUDIOTRYTIME + 1)*1000 <= pAudioRegKey->CurrentTime))
    {
        MusicNextFile = 1;
        AudioStop(Audio_Stop_Normal);
        return RETURN_OK;
    }

    if(GetMsg(MSG_SERVICE_MUSIC_MODE_UPDATE))
    {
        pAudioRegKey->RepeatMode =  gSysConfig.MusicConfig.RepeatMode ;

        AudioFileInfo.Range = (INT16)FIND_FILE_RANGE_ALL;

        pAudioRegKey->AudioFileDirOrAll = AudioFileInfo.Range;

        #ifdef MEDIA_MODULE
        if(AudioFileInfo.ucSelPlayType ==SORT_TYPE_SEL_BROWSER)
        #endif
        {
    	    if((AUDIO_FOLDER_ONCE == pAudioRegKey->RepeatMode)||(AUIDO_FOLDER_REPEAT == pAudioRegKey->RepeatMode))
            {
                AudioPlayFileNum = 0;
                AudioFileInfo.Range = (INT16)FIND_FILE_RANGE_DIR;
                pAudioRegKey->AudioFileDirOrAll = AudioFileInfo.Range;
                FileNumGlobalOrFolder = 0;
                tempFileNum = GetCurFileNum(AudioFileInfo.CurrentFileNum,(UINT8*)AudioFileExtString);
            }
            else
            {
                if(FileNumGlobalOrFolder == 0)
                {
                    tempFileNum = GetGlobeFileNum(AudioFileInfo.CurrentFileNum,AudioFileInfo.Path,(UINT8*)AudioFileExtString);
                    FileNumGlobalOrFolder = 1;
                }
                else
                {
                    tempFileNum = AudioFileInfo.CurrentFileNum;
                }
            }
            AudioFileInfo.CurrentFileNum = tempFileNum;
            AudioFileInfo.PlayedFileNum = AudioFileInfo.CurrentFileNum;
            AudioFileInfo.TotalFiles = GetTotalFiles(AudioFileInfo.Path,(UINT8*)AudioFileExtString);

            AudioFileInfo.TotalFiles = BuildDirInfo((UINT8*)AudioFileExtString);
        }

        if (0 == AudioFileInfo.TotalFiles)
        {
            return RETURN_FAIL;
        }

        if (AudioFileInfo.CurrentFileNum > AudioFileInfo.TotalFiles)
        {
            AudioFileInfo.CurrentFileNum = 1;
        }

        if((AUDIO_TRY == pAudioRegKey->RepeatMode))
        {
           AudioPlayFileNum = 0;
           return RETURN_OK;
        }

        #ifdef MEDIA_MODULE
        if(AudioFileInfo.ucSelPlayType ==SORT_TYPE_SEL_BROWSER)
        #endif
        {
            if((AUDIO_REPEAT == pAudioRegKey->RepeatMode)||(AUDIO_ONCE == pAudioRegKey->RepeatMode))
            {
                tempFileNum = GetCurFileNum(AudioFileInfo.CurrentFileNum,(UINT8*)AudioFileExtString);
                GetDirPath(AudioFileInfo.Path);
            }

             FindFileResult = FindFirst(&AudioFileInfo.Fdt, &AudioFileInfo.FindData,AudioFileInfo.Path, (UINT8*)AudioFileExtString);

             for(i = 1; i < tempFileNum; i++)
             {
                 FindFileResult = FindNext(&AudioFileInfo.Fdt, &AudioFileInfo.FindData, (UINT8*)AudioFileExtString);
             }

             if (AudioFileInfo.Range == FIND_FILE_RANGE_DIR)
             {
                 AudioFileInfo.CurrentFileNum = tempFileNum;
                AudioFileInfo.TotalFiles = GetTotalFiles(AudioFileInfo.Path,(UINT8*)AudioFileExtString);
             }

             AudioFileInfo.PlayedFileNum = AudioFileInfo.CurrentFileNum;

        }

        if((AUDIO_REPEAT == pAudioRegKey->RepeatMode)||(AUDIO_ONCE == pAudioRegKey->RepeatMode))
        {
           AudioPlayFileNum = 0;
           return RETURN_OK;
        }
	}
    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name :int32 AudioFilePlayOderUpdate(void)
  Author        :
  Description   : Update Play Order & Random from setmenu
  Input         :
  Return        :
  History       : <author>         <time>         <version>
                                    2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
int32 AudioFilePlayOderUpdate(void)
{
    if(GetMsg(MSG_SERVICE_MUSIC_ORDER_UPDATE))
    {
       pAudioRegKey->PlayOrder = gSysConfig.MusicConfig.PlayOrder;
       AudioFileInfo.PlayOrder = pAudioRegKey->PlayOrder;

        if (AudioFileInfo.PlayOrder == AUDIO_RAND)
        {
            MaxShuffleAllCount++;
            //CreateRandomList(AudioFileInfo.TotalFiles,AudioFileInfo.CurrentFileNum - 1, MaxShuffleAllCount);
        }
    }

    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioFileOpsInit(void)
  Author        :
  Description  : Init Audiocontrol function  OPS
  Input          :
  Return        :
  History       : <author>         <time>         <version>
                                 2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioFileOpsInit(void)
{
    ModuleOverlay(MODULE_ID_AUDIO_FILE, MODULE_OVERLAY_ALL);

    FileFuncInit();

    AudioControlOps.pAudioTrackInit   = AudioFileInit;
    AudioControlOps.pAudioTrackDeInit = AudioFileDeInit;

    AudioControlOps.pAudioTrackOpen   = AudioFileOpen;
    AudioControlOps.pAudioTrackClose  = AudioFileClose;

    AudioControlOps.pAudioTrackFFD    = AudioFFD;
    AudioControlOps.pAudioTrackFFW    = AudioFFW;
    AudioControlOps.pAudioTrackNext   = AudioGetNextMusic;

    AudioControlOps.pAudioTrackPause  = AudioPause;
    AudioControlOps.pAudioTrackResume = AudioResume;

    AudioControlOps.pHoldonPlayInit     = AudioFileHoldOnInit;
    AudioControlOps.pHoldonPlayProcess  = AudioFileHoldOnProcess;
    AudioControlOps.pHoldonPlayDeInit   = AudioFileHoldOnDeInit;

    AudioControlOps.pAudioTrackSetAB_A    = AudioSetAB_A;
    AudioControlOps.pAudioTrackSetAB      = AudioSetAB;
    AudioControlOps.pAudioTrackABStop     = AudioABStop;
    AudioControlOps.pAudioTrackABProcess  = AudioABProcess;

    AudioControlOps.pAudioTrackEQUpdate   = AudioEQUpdate;
    AudioControlOps.pAudioTrackEQProcess  = AudioEQProcess;

    AudioControlOps.pAudioTrackRepeatModeUpdate = AudioFileRepeatModeUpdate;
    AudioControlOps.pAudioTrackPlayOrderUpdate  = AudioFilePlayOderUpdate;

    AudioControlOps.pAudioTrackDecodingErr = AudioFileDecodingErr;
    AudioControlOps.pAudioTrackMsgProcess = AudioFileServiceHook;
    AudioControlOps.fileMode = FILE_MODE_LOCAL_MUSIC;
}

#endif

/*
********************************************************************************
*
*                         End of AudioControl.c
*
********************************************************************************
*/

