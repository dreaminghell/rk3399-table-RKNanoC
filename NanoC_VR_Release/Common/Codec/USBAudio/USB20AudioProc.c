/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º  AudioControl.C
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                 ZS      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#include "SysInclude.h"

#include "USBConfig.h"

#ifdef _USBAUDIO_DECODE_

#include "audio_file_access.h"
#include "USBAudioBuffer.h"
#include "AudioControl.h"

_ATTR_USB_AUDIO_BSS_    uint8 UsbAudioPlayStatus;

typedef enum
{
    USBAUDIO_PALY_STATE_STOP,
    USBAUDIO_PALY_STATE_PLAY,
    USBAUDIO_PALY_STATE_PAUSE
    
}USBAUDIOPALYSTATE;    

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
void UsbAudioFuncInit(void)
{
    RKFIO_FLength = RKUSB_FLength;
    RKFIO_FRead   = RKUSB_FRead;
    RKFIO_FSeek   = RKUSB_FSeek;
    RKFIO_FTell   = RKUSB_FTell;
    RKFIO_FClose  = RKUSB_FClose;

    RKFIO_FEof  = RKUSB_FEOF;
}

/*
--------------------------------------------------------------------------------
  Function name :int32 UsbAudioTrackInit(void *parg)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int32 UsbAudioTrackInit(void *parg)
{
    UsbAudioPlayStatus = USBAUDIO_PALY_STATE_PLAY;
    
    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int32 UsbAudioTrackDeInit(void)
{
    
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int32 UsbAudioTrackOpen(void)
{   
    CurrentCodec = CODEC_USB_AUDIO; 

    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int32 UsbAudioTrackClose(void)
{
    
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int32 UsbAudioTrackCodecOpen(void)
{
    
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int32 UsbAudioTrackCodecClose(void)
{
    
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int32 UsbAudioTrackNext(int16 direct)
{
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
BOOLEAN UsbAudioTrackPause(void)
{
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
    _ATTR_USB_AUDIO_CODE_

BOOLEAN UsbAudioTrackPlay(void)
{

}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
    _ATTR_USB_AUDIO_CODE_

int32 UsbAudioMsgDecodingErr(uint32 errcode)
{
    if(errcode == 0)
    {
        AudioPlayState = AUDIO_STATE_PAUSE;//Í£Ö¹dma´«Êä
        return RETURN_FAIL;
    }
    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name :void BtFileFuncInit(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>       
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int32 UsbAudioMsgProcess(void)
{
    if(UsbAudioPlayStatus == USBAUDIO_PALY_STATE_PLAY)
    {        
        if(usbAudio_buf_len() < (USBAUDIO_BUFSIZE/3))
        {
            UsbAudioPlayStatus = USBAUDIO_PALY_STATE_PAUSE;
            DEBUG("usb audio pause");
            AudioPause();
            usbAudio_buf_reset();

            SendMsg(MSG_USB_DISPFLAG_STATUS);
        }        
    }
    else if(UsbAudioPlayStatus == USBAUDIO_PALY_STATE_PAUSE)
    {        
        if(usbAudio_buf_len() > (USBAUDIO_BUFSIZE*3/4))
        {
            UsbAudioPlayStatus = USBAUDIO_PALY_STATE_PLAY;
            DEBUG("usb audio resume");
            AudioResume();
            SendMsg(MSG_USB_DISPFLAG_STATUS);
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
extern void AudioEQUpdate(void);
extern void AudioEQProcess(short *outptr, int32 len);
_ATTR_USB_AUDIO_CODE_
void UsbAudioAudioInit(void)
{    
    UsbAudioFuncInit();

    AudioControlOps.pAudioTrackInit   = UsbAudioTrackInit;
    AudioControlOps.pAudioTrackDeInit = NULL;

    AudioControlOps.pAudioTrackOpen   = UsbAudioTrackOpen;    
    AudioControlOps.pAudioTrackClose  = NULL;

    AudioControlOps.pAudioTrackNext   = UsbAudioTrackNext;    
    
    AudioControlOps.pAudioTrackPause  = UsbAudioTrackPause;
    AudioControlOps.pAudioTrackResume = UsbAudioTrackPlay;
    AudioControlOps.pAudioTrackEQUpdate   = AudioEQUpdate;
    AudioControlOps.pAudioTrackEQProcess  = AudioEQProcess;
    AudioControlOps.pAudioTrackDecodingErr = UsbAudioMsgDecodingErr;
    AudioControlOps.pAudioTrackMsgProcess  = UsbAudioMsgProcess;  
    AudioControlOps.fileMode = FILE_MODE_STREAM_MEDIA;
}

#endif 

/*
********************************************************************************
*
*                         End of AudioControl.c
*
********************************************************************************
*/

