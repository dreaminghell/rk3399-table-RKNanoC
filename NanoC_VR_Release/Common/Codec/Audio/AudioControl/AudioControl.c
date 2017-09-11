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
#define _IN_AUDIO_CONTROL_

#include "SysInclude.h"

#ifdef _MUSIC_

#include "Effect.h"
#include "pmu.h"
#include "MainMenu.h"
#include "AudioControl.h"
#include "fade.h"
#include "Dma.h"
#include "I2S.h"

extern volatile int is_synthing;
extern  short UseEqTable[CUSTOMEQ_LEVELNUM];

_ATTR_AUDIO_DATA_ DMA_CFGX AudioControlDmaCfg  = {DMA_CTLL_I2S_TX, DMA_CFGL_I2S_TX, DMA_CFGH_I2S_TX,0};

#ifdef BT_VOICENOTIFY
_ATTR_AUDIO_DATA_ uint8 playVolBak = 0;
#endif

#if 1
/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN GetMusicUIState(void)
  Author        : zs
  Description   : get the audio window status.
  Input         : null
  Return        : 1: no presence
                  0: presence
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
BOOLEAN GetMusicUIState(void)
{
   return MusicUIState;
}

/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN SetMusicUIState(void)
  Author        : zs
  Description   : set the audio window status.
  Input         : null
  Return        : 1: no presence
                  0: presence
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void SetMusicUIState(bool State)
{
    MusicUIState = State;
}

/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN AudioPlay(void)
  Author        : zs
  Description   : audio decode,decode next frame.
  Input         : null
  Return        : null
  History       : <author>         <time>         <version>
                   zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
#ifdef _RK_SPECTRUM_
_ATTR_SPECTRUM_BSS_  char  SpectrumOut[SPECTRUM_LINE_M];//存放转换过后的M根谱线的能量
_ATTR_SPECTRUM_DATA_ int   SpectrumLoop = SpectrumLoopTime;
_ATTR_SPECTRUM_BSS_  int   SpectrumCnt;

_ATTR_AUDIO_TEXT_
void AudioSpectrum(short *outptr)
{
    if (FALSE == CheckMsg(MSG_BL_OFF))
    {
        if(FALSE == GetMusicUIState())//backgroud need not to compute the spectrum.
        {
            if (SpectrumLoop == 0)
            {
                if (SpectrumCnt == 0)
                    memset(SpectrumOut, 0, SPECTRUM_LINE_M*sizeof(char));

                if ((AUDIO_STATE_PLAY == AudioPlayState) && (SpectrumCnt < 3))
                {
                    DoSpectrum(outptr,&SpectrumOut[0]);

                    SpectrumCnt++;
                    if (SpectrumCnt >= 3)
                    {
                        SpectrumLoop = SpectrumLoopTime;
                        SendMsg(MUSIC_UPDATESPECTRUM);
                        SpectrumCnt = 0;
                    }
                }
            }
            if (SpectrumLoop > 0)
                SpectrumLoop-- ;
          }
    }
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : void AudioCodecOverLay(void)
  Author        : zs
  Description   :
  Input         : Codec_Type
  Return        :
  History       : <author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioCodecOverLayStep1(void)
{
   switch(CurrentCodec)
   {
    #ifdef MP3_INCLUDE
    case CODEC_MP3:     //mp3
        ModuleOverlay(MODULE_ID_MP3_DECODE, MODULE_OVERLAY_ALL);
        AcceleratorHWInit();
    #endif
        break;

    #ifdef  WMA_INCLUDE
    case CODEC_WMA:     //wma
		ModuleOverlay(MODULE_ID_WMA_COMMON, MODULE_OVERLAY_ALL);
		ModuleOverlay(MODULE_ID_WMA_OPEN_CODEC, MODULE_OVERLAY_ALL);
        break;
    #endif

    #ifdef  AAC_INCLUDE
    case CODEC_AAC:     //aac
		ModuleOverlay(MODULE_ID_AAC_DECODE, MODULE_OVERLAY_ALL);
        break;
    #endif

    #ifdef WAV_INCLUDE
    case CODEC_WAV:     //wav
        ModuleOverlay(MODULE_ID_WAV_DECODE, MODULE_OVERLAY_ALL);
        break;
    #endif

    #ifdef APE_DEC_INCLUDE
    case CODEC_APE_DEC:     //ape
        ModuleOverlay(MODULE_ID_APE_DECODE, MODULE_OVERLAY_ALL);
        break;
    #endif

    #ifdef FLAC_DEC_INCLUDE
    case CODEC_FLAC_DEC:     //flac
        ModuleOverlay(MODULE_ID_FLAC_DECODE, MODULE_OVERLAY_ALL);
        break;
    #endif

    #ifdef SBC_INCLUDE
    case CODEC_SBC:
        ModuleOverlay(MODULE_ID_SBC_DECODE, MODULE_OVERLAY_ALL);
        break;

    #endif

    default:
        break;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void AudioCodecOverLay(void)
  Author        : zs
  Description   :
  Input         : Codec_Type
  Return        :
  History       : <author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
#ifdef WMA_INCLUDE
extern unsigned short g_flagHighRate;
extern int WmaTableOverLay;
#endif
_ATTR_AUDIO_TEXT_
void AudioCodecOverLayStep2(void)
{
    #ifdef  WMA_INCLUDE
    if(CODEC_WMA == CurrentCodec)
    {
        ModuleOverlay(MODULE_ID_WMA_HIGH_LOW_COMMON, MODULE_OVERLAY_ALL);
        if (g_flagHighRate)
        {
            ModuleOverlay(MODULE_ID_WMA_HIGH_RATE, MODULE_OVERLAY_ALL);
        }
        else
        {
            ModuleOverlay(MODULE_ID_WMA_LOW_RATE, MODULE_OVERLAY_ALL);
        }

        if (WmaTableOverLay == 3)
        {
            ModuleOverlay(MODULE_ID_WMA_DATA_16Ob, MODULE_OVERLAY_ALL);
		}
        else if (WmaTableOverLay == 2)
        {
            ModuleOverlay(MODULE_ID_WMA_DATA_44Ob, MODULE_OVERLAY_ALL);
        }
        else if(WmaTableOverLay == 1)
        {
            ModuleOverlay(MODULE_ID_WMA_DATA_44Qb, MODULE_OVERLAY_ALL);
        }
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioCodecOpen(void)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
int32 AudioCodecOpen(void)
{
    AudioCodecOverLayStep1();

    if(0 == CodecOpen(0, CODEC_OPEN_DECODE))
    {
        return ERROR;
	}

    CodecGetLength(&pAudioRegKey->TotalTime);
	CodecGetSampleRate(&pAudioRegKey->samplerate);
    //8000 -- 48000
    if ((pAudioRegKey->samplerate < 8000) || (pAudioRegKey->samplerate > 48000))
    {
        return ERROR;
    }

	CodecGetBitrate(&pAudioRegKey->bitrate);
	CodecGetChannels(&pAudioRegKey->channels);

	if((pAudioRegKey->bitrate <= 0)||(pAudioRegKey->samplerate <= 0))
	{
        return ERROR;
    }

    AudioCodecOverLayStep2();

    return OK;
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioCodecOpen(void)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
int32 AudioCodecClose(void)
{
    CodecClose();

    #ifdef MP3_INCLUDE
    if (CurrentCodec == CODEC_MP3)
    {
    	AcceleratorHWExit();
    }
    #endif

}

/*
--------------------------------------------------------------------------------
  Function name : void AudioSetVolume(void)
  Author        : zs
  Description   : audio output volume set,because the audio volume is control by audio effect indenpently,
                  so package this function in here.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>
                   zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioSetVolume(void)
{
    AudioInOut_Type  *pAudio  = &AudioIOBuf;
    RKEffect     	 *pEffect = &pAudio->EffectCtl;

    if (MusicOutputVol)
    {
        //switch (pEffect->Mode)
        switch (pAudioRegKey->UserEQ.Mode)
        {
            #ifdef _RK_EQ_
            case EQ_ROCK:
            #ifdef _BBE_EQ_
			case EQ_BBE:
                OutputVolOffset = 7;
				break;
            #endif

            case EQ_POP:
            #ifdef _MS_EQ_
            //case EQ_PLAYFX:
                OutputVolOffset = 4;
                break;
            #endif

            case EQ_BASS:
            #ifdef _MS_EQ_
            case EQ_PLAYFX:
                OutputVolOffset = 8;
                break;
            #endif

            case EQ_CLASSIC:
            case EQ_JAZZ:
                OutputVolOffset = 6;
                break;

            case EQ_USER:
                OutputVolOffset = 6;
                if (TRUE==IsEQNOR)
                {
                    OutputVolOffset = 0;
                }
                break;
            #endif

            case EQ_NOR:
            default:
                OutputVolOffset = 0;
                break;
        }
    }

    AudioPlayInfo.playVolume = MusicOutputVol;
}

/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN GetMusicUIState(void)
  Author        : zs
  Description   : get the audio window status.
  Input         : null
  Return        : 1: no presence
                  0: presence
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioVolumeFade(void)
{
    #if 1
    if (CheckMsg(MSG_MUSIC_FADE_IN))
    {
//        ClearMsg(MSG_MUSIC_FADE_OUT);
		ClearMsg(MSG_MUSIC_FADE_IN);

		 while(AudioPlayInfo.VolumeCnt < AudioPlayInfo.playVolume)
        {
            AudioPlayInfo.VolumeCnt++;
            Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
            DelayMs(5);
        }
    }
    else if(CheckMsg(MSG_MUSIC_FADE_OUT))
    {
        ClearMsg(MSG_MUSIC_FADE_IN);

//		 while(AudioPlayInfo.VolumeCnt > 0)
//            {
//                AudioPlayInfo.VolumeCnt--;
//                Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
//                DelayMs(5);
//            }
    }
    #endif

#if 0
    if (GetMsg(MSG_MUSIC_FADE_IN))
    {
        //if(CurrentCodec != CODEC_BT_VOCIE_NOTIFY)
        {
            while(AudioPlayInfo.VolumeCnt < AudioPlayInfo.playVolume)
            {
                AudioPlayInfo.VolumeCnt++;
                Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
                DelayMs(5);
            }
        }
    }
#endif
}


/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN GetMusicUIState(void)
  Author        : zs
  Description   : get the audio window status.
  Input         : null
  Return        : 1: no presence
                  0: presence
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioEffectInit(void)
{
    int i;

    pAudioRegKey->UserEQ.Mode   = gSysConfig.MusicConfig.Eq.Mode;

    #ifdef _RK_EQ_
    for(i = 0;i < 5;i++)
    {
        pAudioRegKey->UserEQ.RKCoef.dbGain[i] = gSysConfig.MusicConfig.Eq.RKCoef.dbGain[i]*2+6;
        if((pAudioRegKey->UserEQ.RKCoef.dbGain[i] > 18) ||( pAudioRegKey->UserEQ.RKCoef.dbGain[i] < 6))// zs 06.01 防止数据超出
        {
            pAudioRegKey->UserEQ.RKCoef.dbGain[i] = 12;
        }
    }
	#endif

    memcpy(&AudioIOBuf.EffectCtl, &pAudioRegKey->UserEQ, sizeof(RKEffect));
}

/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN GetMusicUIState(void)
  Author        : zs
  Description   : get the audio window status.
  Input         : null
  Return        : 1: no presence
                  0: presence
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioEffectDeInit(void)
{
    int i;

    memcpy(&pAudioRegKey->UserEQ ,&AudioIOBuf.EffectCtl, sizeof(RKEffect));//by zs 05.20
    gSysConfig.MusicConfig.Eq.Mode    = pAudioRegKey->UserEQ.Mode;

    #ifdef _RK_EQ_
    for(i = 0;i < 5;i++)
    {
         gSysConfig.MusicConfig.Eq.RKCoef.dbGain[i] = (pAudioRegKey->UserEQ.RKCoef.dbGain[i]-6)>>1;
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name : void AudioSetEQ(void)
  Author        : zs
  Description   : stop repeat read.to call it after audio initialization and every EQ changed.
  Input         : null
  Return        : null
  History       : <author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioSetEQ(void)
{
    #ifdef  _RK_EQ_
    EffectAdjust();
    AudioSetVolume();
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name : AudioEQProcess
  Author        : zs
  Description   : get the audio window status.
  Input         : null
  Return        : 1: no presence
                  0: presence
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioEQProcess(short *outptr, int32 len)
{
    if(EQ_NOR != pAudioRegKey->UserEQ.Mode)
    {
        #ifdef MP3_INCLUDE
        if(CurrentCodec == CODEC_MP3)
        {
            mp3_wait_synth();
        }
        #endif

    	/*audio effect process.*/
        #ifdef _RK_EQ_
        EffectProcess((short*)outptr, len);
        #endif
    }
}

/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN AudioPlay(void)
  Author        : zs
  Description   :
  Input         : null
  Return        : null
  History       : <author>         <time>         <version>
                   zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
 _ATTR_AUDIO_TEXT_
int32 FadeFinishedWait(uint32 timeout)
{
    while(!FadeIsFinished())
    {
        if (AUDIO_STATE_PLAY != AudioPlayState)
            break;

        DelayMs(1);
        if (--timeout == 0)
            break;
    }
    return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name : AudioEQProcess
  Author        : zs
  Description   : get the audio window status.
  Input         : null
  Return        : 1: no presence
                  0: presence
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioEQUpdate(void)
{
    UINT16 i,j;
    uint32 FadeFlag = 0;

    #ifdef _RK_EQ_
    if ((GetMsg(MSG_AUDIO_EQSET_UPDATA)) || (GetMsg(MSG_AUDIO_EQSET_UPDATE_USER_EQ)))//update EQ
    {
        //fade out
        #if 0
        if ((((gSysConfig.MusicConfig.Eq.Mode == EQ_ROCK)||(gSysConfig.MusicConfig.Eq.Mode == EQ_USER))&&( pAudioRegKey->UserEQ.Mode == EQ_NOR ))
                ||(((pAudioRegKey->UserEQ.Mode == EQ_ROCK)||(pAudioRegKey->UserEQ.Mode == EQ_USER))&&(gSysConfig.MusicConfig.Eq.Mode== EQ_NOR ))
                ||((gSysConfig.MusicConfig.Eq.Mode == EQ_JAZZ)&&((pAudioRegKey->UserEQ.Mode == EQ_USER)||(pAudioRegKey->UserEQ.Mode == EQ_NOR)))
                ||((pAudioRegKey->UserEQ.Mode == EQ_JAZZ)&&(gSysConfig.MusicConfig.Eq.Mode == EQ_USER)))
        #endif
        {
            if ((AUDIO_STATE_PLAY == AudioPlayState) && (FadeIsFinished()))
            {
                SendMsg(MSG_AUDIO_EQ_FADE_OUT);
				//FadeInit(0,(pAudioRegKey->samplerate>>1),1); //0.5 sencond fade out
				return;
            }
        }
    }

	if(CheckMsg(MSG_AUDIO_EQ_FADE_OUT))
	{
			while(AudioPlayInfo.VolumeCnt > 1)
            {
                AudioPlayInfo.VolumeCnt--;
                Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
                DelayMs(5);
            }

			pAudioRegKey->UserEQ.Mode =  gSysConfig.MusicConfig.Eq.Mode ;
            AudioIOBuf.EffectCtl.Mode =  gSysConfig.MusicConfig.Eq.Mode ;
            AudioDecodeProc(MSG_AUDIO_EQSET,NULL);
			//FadeInit(0,(pAudioRegKey->samplerate>>1),0);
			ClearMsg(MSG_AUDIO_EQ_FADE_OUT);
			SendMsg(MSG_MUSIC_FADE_IN);
	}
    #endif

}

/*
--------------------------------------------------------------------------------
  Function name : AudioEQProcess
  Author        : zs
  Description   : get the audio window status.
  Input         : null
  Return        : 1: no presence
                  0: presence
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioFadeProcess(short *outptr, int32 len)
{
    short *pBuffer;
    int  eff,i;

	if (!FadeIsFinished())
	{
        #ifdef MP3_INCLUDE
		if(CurrentCodec == CODEC_MP3)
        {
			mp3_wait_synth();
        }
        #endif

		pBuffer=(short*)outptr;
		for(i=0;i<len;i++)
		{
			eff=FadeDoOnce();
			pBuffer[2*i]= ((pBuffer[2*i] * eff) >> 16) ;  //fade in fade out
			pBuffer[2*i+1]= ((pBuffer[2*i+1] * eff)>>16) ;
		}
	}
    #if 0
	else
	{
		if (FadeGetType()==1)	//fade out
		{
//            if (GetMsg(MSG_AUDIO_EQ_FADE_OUT))
//            {
//				//Codec_DACMute();
//				FadeInit(0,(pAudioRegKey->samplerate>>1),0);
//
//                pBuffer=(short*)outptr;
//                for (i=0; i<len; i++)
//                {
//                    eff=FadeDoOnce();
//                    pBuffer[2*i]= 0;//((pBuffer[2*i] * eff) >> 16) ;  //fade in fade out
//                    pBuffer[2*i+1]= 0;//((pBuffer[2*i+1] * eff)>>16) ;
//                }
//
//
//                {
//                    pAudioRegKey->UserEQ.Mode =  gSysConfig.MusicConfig.Eq.Mode ;
//                    AudioIOBuf.EffectCtl.Mode =  gSysConfig.MusicConfig.Eq.Mode ;
//                    AudioDecodeProc(MSG_AUDIO_EQSET,NULL);
//					AudioErrorFrameNum = 0;
//                }
//
//            }
////            else
//            {
//                MusicDecodeProc(MSG_AUDIO_NEXTFILE, (void*)MusicNextFile);
//                if (CheckMsg(MSG_MUSIC_NEXTFILE))
//                {
//                    SendMsg(MSG_MUSIC_NEXTFILE_OK);
//                }
//            }
        }
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name :  BOOLEAN AudioPause(void)
  Author        : zs
  Description   : audio decode pause,to changed paly status to comply temporarily.
  Input         : null
  Return        : TRUE/FALSE
  History       : <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :    ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
BOOLEAN AudioPause(void)
{
    uint32 timeout = 200;

    if (AUDIO_STATE_PLAY == AudioPlayState)
    {
        if(AudioControlOps.fileMode == FILE_MODE_STREAM_MEDIA)
        {

            while(1)
            {
                AudioPlayInfo.VolumeCnt--;
                Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
                DelayMs(2);

                if(AudioPlayInfo.VolumeCnt == 0)
                    break;
            }

            AudioPlayState = AUDIO_STATE_PAUSE;

            while(DmaGetState(AUDIO_DMACHANNEL_IIS) == DMA_BUSY)
            {
                DelayMs(1);
                if (--timeout == 0)
                {
                    break;
                }
            }
        }
        else
        {
            FadeInit(0,pAudioRegKey->samplerate>> 1,FADE_OUT);
            FadeFinishedWait(pAudioRegKey->samplerate >> 1);

            AudioPlayState = AUDIO_STATE_PAUSE;
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

        AutoPowerOffEnable();
    }
    return TRUE;
}

/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN AudioResume(void)
  Author        :  zs
  Description   :  audio decode resume.
                   changed play status.
                   send decode message.
                   start DMA
  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
BOOLEAN AudioResume(void)
{
    if ((AUDIO_STATE_PAUSE != AudioPlayState) && (AUDIO_STATE_FFD != AudioPlayState) && (AUDIO_STATE_FFW != AudioPlayState))
    {
        SendMsg(MSG_MUSIC_FADE_IN);
        return FALSE;
    }

    if (AudioPlayState == AUDIO_STATE_PAUSE)
    {
        AutoPowerOffDisable();
    }

    AudioPlayState = AUDIO_STATE_PLAY;

    FadeInit(0,pAudioRegKey->samplerate >> 1,FADE_IN);

    AudioErrorFrameNum = 0;
    SendMsg(MSG_MUSIC_DISPFLAG_STATUS);//cw 2009-5-11
    AudioSetVolume();

    CodecGetCaptureBuffer((short*)&outptr,&AudioIOBuf.lOutLength);
    memset((uint8*)outptr, 0, AudioIOBuf.lOutLength * sizeof(int16) * 2);
    DmaStart(AUDIO_DMACHANNEL_IIS, (UINT32)outptr, (uint32)(&(I2s_Reg->I2S_TXDR)),  AudioIOBuf.lOutLength, &AudioControlDmaCfg, AudioDmaIsrHandler);

    return TRUE;
}
/*
********************************************************************************
*
*                         Above used for common
*
********************************************************************************
*/
#endif

#if 1
/*
********************************************************************************
*                         Flowed code is Audiocontrol
* Description:
*
* History:      <author>          <time>        <version>
*                 ZS      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/
/*
--------------------------------------------------------------------------------
  Function name :void AudioVariableInit(void)
  Author        : zyz
  Description   : initialization of variable.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioVariableInit(void)
{
    AudioIOBuf.lOutLength = 1152;
    AudioPlayInfo.PlayDirect = 1;
    AudioStopMode = Audio_Stop_Normal;
    pAudioRegKey->CurrentTime = 0;
    pAudioRegKey->TotalTime = 1;
    pAudioRegKey->IsEQUpdate = 0;

    AudioErrorFrameNum = 0;
    OutputVolOffset = 0;
    AudioPlayInfo.VolumeCnt = 0;

    SendMsg(MSG_AUDIO_INIT_FAIL);
}

/*
--------------------------------------------------------------------------------
  Function name :  BOOLEAN AudioVariableDeInit(UINT16 ReqType);
  Author        :  zs
  Description   :
  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioVariableDeInit(void)
{
    AudioPlayInfo.ABRequire = AUDIO_AB_NULL;

    ClearMsg(MSG_MUSIC_DISPFLAG_SCHED);
    ClearMsg(MSG_MUSIC_DISPFLAG_CURRENT_TIME);
    ClearMsg(MUSIC_UPDATESPECTRUM);
    ClearMsg(MSG_MUSIC_DISPFLAG_SCROLL_FILENAME);
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioSCUInit(void)
  Author        : zyz
  Description   :initialization.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioSCUInit(void)
{
    switch(CurrentCodec)
    {
        #ifdef FLAC_DEC_INCLUDE
        case CODEC_FLAC_DEC:
            //SCU_EnterMusic(SCU_FLAC);
            break;
        #endif

        #ifdef APE_DEC_INCLUDE
        case CODEC_APE_DEC:
            //SCU_EnterMusic(SCU_APE);
            break;
        #endif

        #ifdef WMA_INCLUDE
        case CODEC_WMA:
            //SCU_EnterMusic(SCU_WMA);
            ScuClockGateCtr(CLOCK_GATE_TROM1, 1);
            ScuClockGateCtr(CLOCK_GATE_TROM0, 1);
            break;
        #endif

        #ifdef AAC_INCLUDE
        case CODEC_AAC:
            //SCU_EnterMusic(SCU_AAC);
            ScuClockGateCtr(CLOCK_GATE_TROM1, 1);
            ScuClockGateCtr(CLOCK_GATE_TROM0, 1);
            break;
        #endif

        #ifdef WAV_INCLUDE
        case CODEC_WAV:
            //SCU_EnterMusic(SCU_WAV);
            break;
        #endif

        #ifdef MP3_INCLUDE
        case CODEC_MP3:
            //SCU_EnterMusic(SCU_MP3);
            ScuClockGateCtr(CLOCK_GATE_SYNTH, 1);
            ScuClockGateCtr(CLOCK_GATE_IMDCT, 1);
            break;
        #endif

        default:
            break;
    }
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioSCUDeInit(void)
  Author        : zyz
  Description   :auti-initialization for variable.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioSCUDeInit(void)
{
    switch(CurrentCodec)
    {
        #ifdef FLAC_DEC_INCLUDE
        case CODEC_FLAC_DEC:
            //SCU_ExitMusic(SCU_FLAC);
            break;
        #endif

        #ifdef APE_DEC_INCLUDE
        case CODEC_APE_DEC:
            //SCU_EnterMusic(SCU_APE);
            break;
        #endif

        #ifdef WMA_INCLUDE
        case CODEC_WMA:
            //SCU_ExitMusic(SCU_WMA);
            ScuClockGateCtr(CLOCK_GATE_TROM1, 0);
            ScuClockGateCtr(CLOCK_GATE_TROM0, 0);
            break;
        #endif

        #ifdef AAC_INCLUDE
        case CODEC_AAC:
            //SCU_EnterMusic(SCU_AAC);
            ScuClockGateCtr(CLOCK_GATE_TROM1, 0);
            ScuClockGateCtr(CLOCK_GATE_TROM0, 0);
            break;
        #endif

        #ifdef  WAV_INCLUDE
        case CODEC_WAV:
            //SCU_ExitMusic(SCU_WAV);
            break;
        #endif

        #ifdef MP3_INCLUDE
        case CODEC_MP3:
            //SCU_ExitMusic(SCU_MP3);
            ScuClockGateCtr(CLOCK_GATE_SYNTH, 0);
            ScuClockGateCtr(CLOCK_GATE_IMDCT, 0);
            break;
        #endif

        default:
            break;
    }
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioPMUInit(void)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioPMUInit(void)
{
    switch (CurrentCodec)
	{
        #ifdef MP3_INCLUDE
	    case (CODEC_MP3):
		{
			if ((pAudioRegKey->bitrate <= 128000)&&(pAudioRegKey->samplerate <= 44100))
			{
				PMU_EnterModule(PMU_MP3);
			}
			else
			{
				PMU_EnterModule(PMU_MP3H);
			}
			break;
		}
        #endif

        #ifdef WMA_INCLUDE
	 	case (CODEC_WMA):
		{
            if (pAudioRegKey->bitrate <= 128000)
			{
			    PMU_EnterModule(PMU_WMA);
			}
			else
			{
				PMU_EnterModule(PMU_WMAH);
			}
			break;
		}
        #endif

        #ifdef AAC_INCLUDE
	 	case (CODEC_AAC):
		{
            PMU_EnterModule(PMU_AAC);
			break;
		}
        #endif

        #ifdef WAV_INCLUDE
	 	case (CODEC_WAV):
		{
			PMU_EnterModule(PMU_WAV);
			break;
		}
        #endif

        #ifdef APE_DEC_INCLUDE
	 	case (CODEC_APE_DEC):
		{
			PMU_EnterModule(PMU_APE);
			break;
		}
        #endif

        #ifdef FLAC_DEC_INCLUDE
		case (CODEC_FLAC_DEC):
		{
			PMU_EnterModule(PMU_FLAC);
			break;
	    }
        #endif


        #ifdef SBC_INCLUDE
		case (CODEC_SBC):
		{
			PMU_EnterModule(PMU_SBC);
			break;
	    }
        #endif
	}
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioPMUDeInit(void)
  Author        : zyz
  Description   : pmu auti-initialization.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioPMUDeInit(void)
{
	 switch (CurrentCodec)
	 {
        #ifdef MP3_INCLUDE
	 	case (CODEC_MP3):
		{
            if ((pAudioRegKey->bitrate <= 128000)&&(pAudioRegKey->samplerate <= 44100))
            {
				PMU_ExitModule(PMU_MP3);
            }
			else
			{
				PMU_ExitModule(PMU_MP3H);
			}
			break;
		}
        #endif

        #ifdef WMA_INCLUDE
	 	case (CODEC_WMA):
		{
            if (pAudioRegKey->bitrate <= 128000)
            {
				PMU_ExitModule(PMU_WMA);
            }
			else
			{
				PMU_ExitModule(PMU_WMAH);
			}
			break;
		}
        #endif

        #ifdef AAC_INCLUDE
	 	case (CODEC_AAC):
		{
            PMU_ExitModule(PMU_AAC);
			break;
		}
        #endif

        #ifdef WAV_INCLUDE
	 	case (CODEC_WAV):
		{
			PMU_ExitModule(PMU_WAV);
			break;
		}
        #endif

        #ifdef APE_DEC_INCLUDE
	 	case (CODEC_APE_DEC):
		{
			PMU_ExitModule(PMU_APE);
			break;
		}
        #endif

        #ifdef FLAC_DEC_INCLUDE
		case (CODEC_FLAC_DEC):
		{
			PMU_ExitModule(PMU_FLAC);
			break;
		}
        #endif

        #ifdef SBC_INCLUDE
		case (CODEC_SBC):
		{
			PMU_ExitModule(PMU_SBC);
			break;
		}
        #endif

	 }
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioIntAndDmaInit(void)
  Author        : zyz
  Description
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioIntAndDmaInit(void)
{
    /*---------clear interrupt flag.------------*/
    IntPendingClear(FAULT_ID14_PENDSV);
    #ifdef MP3_INCLUDE
    if (CurrentCodec == CODEC_MP3)
    {
        IntPendingClear(INT_ID21_IMDCT36);
        IntPendingClear(INT_ID22_SYNTHESIZE);
    }
    #endif

    /*-------set the priority-----------*/
    IntPrioritySet(FAULT_ID14_PENDSV,0xE0);

    /*----------register interupt callback--------------*/
    IntRegister(FAULT_ID14_PENDSV, AudioDecoding);
    #ifdef MP3_INCLUDE
    if (CurrentCodec == CODEC_MP3)
    {
        IntRegister(INT_ID21_IMDCT36, imdct36_handler);
        IntRegister(INT_ID22_SYNTHESIZE, synth_handler);
    }
    #endif

    /*----------interruption enable--------------*/
    IntEnable(FAULT_ID14_PENDSV);
    #ifdef MP3_INCLUDE
    if (CurrentCodec == CODEC_MP3)
    {
        IntEnable(INT_ID21_IMDCT36);
        IntEnable(INT_ID22_SYNTHESIZE);
    }
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name :void AudioIntAndDmaDeInit(void)
  Author        : zyz
  Description   :
  Input         :
  Return        :
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioIntAndDmaDeInit(void)
{
    /*---------clear interrupt enable------------*/
    #ifdef MP3_INCLUDE
    if (CurrentCodec == CODEC_MP3)
    {
        #if(MP3_EQ_WAIT_SYNTH)
		mp3_wait_synth();
        #endif
        IntDisable(INT_ID21_IMDCT36);
        IntDisable(INT_ID22_SYNTHESIZE);
    }
    #endif

    /*---------clear interrupt Pending---------*/
    #ifdef MP3_INCLUDE
    if (CurrentCodec == CODEC_MP3)
    {
        IntPendingClear(INT_ID21_IMDCT36);
        IntPendingClear(INT_ID22_SYNTHESIZE);
    }
    #endif
    IntPendingClear(FAULT_ID14_PENDSV);

    /*-----------interrupt callback auti-register--------------*/
    #ifdef MP3_INCLUDE
    if (CurrentCodec == CODEC_MP3)
    {
        IntUnregister(INT_ID21_IMDCT36);
        IntUnregister(INT_ID22_SYNTHESIZE);
    }
    #endif
    IntUnregister(FAULT_ID14_PENDSV);

}

/*
--------------------------------------------------------------------------------
  Function name : void AudioDmaIsrHandler(void)
  Author        : zs
  Description   : the callback of DMA0 interrupt.
  Input         : null
  Return        : null
  History       : <author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioDmaIsrHandler(void)
{
    if (AUDIO_STATE_PLAY == AudioPlayState)
    {
        DmaReStart(AUDIO_DMACHANNEL_IIS, (UINT32)outptr, (uint32)(&(I2s_Reg->I2S_TXDR)),  AudioIOBuf.lOutLength, &AudioControlDmaCfg, AudioDmaIsrHandler);
        UserIsrRequest();
    }
}

/*
--------------------------------------------------------------------------------
  Function name : BOOLEAN AudioPlay(void)
  Author        : zs
  Description   : audio decode,decode next frame.
  Input         : null
  Return        : null
  History       : <author>         <time>         <version>
                   zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
extern void filter_2_int_l(short *pwBuffer, unsigned short frameLen);
extern void filter_2_int_r(short *pwBuffer, unsigned short frameLen);

_ATTR_AUDIO_DATA_ short x_l[2] = {0,0};
_ATTR_AUDIO_DATA_ long long y_l[2] = {0,0};
_ATTR_AUDIO_DATA_ short x_r[2] = {0,0};
_ATTR_AUDIO_DATA_ long long y_r[2] = {0,0};

_ATTR_AUDIO_TEXT_
void DC_filter(short *pwBuffer, unsigned short frameLen)
{
    filter_2_int_l(pwBuffer, frameLen);
    filter_2_int_r(pwBuffer + 1, frameLen);
}

_ATTR_AUDIO_TEXT_
void AudioDecoding(void)
{
    uint32 RetVal = 0;
    short *outtmp;

    if (AUDIO_STATE_PLAY != AudioPlayState)
        return ;

    //#if (2 == FW_IN_DEV)
	//if (CheckCard() == 0)
    //    return;
    //#endif

    RetVal = CodecDecode();
    CodecGetCaptureBuffer((short*)&outptr,&AudioIOBuf.lOutLength);
    if (AudioIOBuf.lOutLength == 0)
    {
        AudioPlayState = AUDIO_STATE_STOP;
        SendMsg(MSG_AUDIO_DECODE_END);
    }

    if (AudioControlOps.pAudioTrackDecodingErr)
    {
        RetVal = AudioControlOps.pAudioTrackDecodingErr(RetVal);
        if (RetVal == RETURN_FAIL)
            return;
    }

    CodecGetTime(&pAudioRegKey->CurrentTime);

//    if( (CurrentCodec != CODEC_BT_VOCIE_NOTIFY) && (CurrentCodec != CODEC_BT_PHONE))
//    {
        if (AudioErrorFrameNum < 4)
        {
            if (++AudioErrorFrameNum >= 4)
            {
                SendMsg(MSG_MUSIC_FADE_IN);
                FadeInit(0,pAudioRegKey->samplerate >> 1,FADE_IN);
            }
        }

//    }
//    else
//    {
//        SendMsg(MSG_MUSIC_FADE_IN);
//    }


    if (AudioControlOps.pAudioTrackEQProcess)
    {
        AudioControlOps.pAudioTrackEQProcess((short*)outptr, AudioIOBuf.lOutLength);
    }

    AudioFadeProcess((short*)outptr, AudioIOBuf.lOutLength);

    if (pAudioRegKey->samplerate != FS_44100Hz)
    {
        DC_filter((short*)outptr,AudioIOBuf.lOutLength);
    }

    #ifdef _RK_SPECTRUM_
    AudioSpectrum((short*)outptr);
    #endif

}

/*
--------------------------------------------------------------------------------
  Function name :void AudioStart(void)
  Author        : zs
  Description   :call related initialization,then start audio.
  Input         : null
  Return        : null
  History       :<author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :  ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void AudioStart(void)
{
    int ret = 0;

    PMU_EnterModule(PMU_AUDIO_INIT);
    //init code start
	AudioVariableInit();
    if (AudioControlOps.pAudioTrackOpen)
    {
        if (RETURN_FAIL == AudioControlOps.pAudioTrackOpen())
        {
            AudioPlayState = AUDIO_STATE_STOP;
            //SendMsg(MSG_AUDIO_OPENERROR);
            PMU_ExitModule(PMU_AUDIO_INIT);
            return;
        }
        SendMsg(MSG_AUDIO_INIT_SUCESS);
    }
    else {DEBUG("error!!!");}

    #ifdef BT_VOICE_NOTIFY_INCLUDE
    if(CurrentCodec == CODEC_BT_VOCIE_NOTIFY)
    {
        playVolBak = MusicOutputVol;
        MusicOutputVol = VOICE_NOTIFY_VOL;
    }
    #endif
    AudioSetVolume();

	AudioSCUInit();

    AudioIntAndDmaInit();

    if (ERROR == AudioCodecOpen())
    {
        SendMsg(MSG_AUDIO_FILE_ERROR);
        PMU_ExitModule(PMU_AUDIO_INIT);
        return;
    }

    if (AudioControlOps.pHoldonPlayProcess)
    {
        AudioControlOps.pHoldonPlayProcess();
    }

    AudioPMUInit();
    PMU_ExitModule(PMU_AUDIO_INIT);

    #ifdef _RK_EQ_
    EffectInit();
    AudioSetEQ();
    #endif

    Codec_SetMode(Codec_DACout);
    Codec_SetSampleRate(pAudioRegKey->samplerate);

    ScuSoftResetCtr(RST_I2S, 0);
    I2SInit(I2S_SLAVE_MODE);

    while(DmaGetState(AUDIO_DMACHANNEL_IIS) == DMA_BUSY)
    {
        DEBUG("DMA Channel 2 Busy!");
        DelayMs(100);
    }

    DmaChannel2Disable(1);
    #ifdef BT_VOICE_NOTIFY_INCLUDE
	if(CurrentCodec == CODEC_BT_VOCIE_NOTIFY)
	{
		int i;
		AudioErrorFrameNum = 4;
		AudioDecoding();
		for(i = 0; i < 10; i++)
		{
			Codec_SetVolumet(VOICE_NOTIFY_VOL);
			DelayMs(5);
		}
	}

    #endif
    #ifdef BT_PHONE_INCLUDE
	if(CurrentCodec == CODEC_BT_PHONE)
	{
		int i;
		AudioErrorFrameNum = 4;
		AudioDecoding();
		for(i = 0; i < 10; i++)
		{
			Codec_SetVolumet(MusicOutputVol);
			DelayMs(5);
		}
	}

    #endif
	CodecGetCaptureBuffer((short*)&outptr,&AudioIOBuf.lOutLength);
    DmaStart(AUDIO_DMACHANNEL_IIS, (UINT32)outptr, (uint32)(&(I2s_Reg->I2S_TXDR)),
		     AudioIOBuf.lOutLength, &AudioControlDmaCfg, AudioDmaIsrHandler);
    I2SStart(I2S_START_TX);
    DEBUG("Audio DMA & I2S start...");

    //wrm-add
    //gSysConfig.UsbSensor = 1;

//    if(CurrentCodec == CODEC_BT_VOCIE_NOTIFY  || CurrentCodec == CODEC_BT_PHONE)
//    {
//        while(AudioPlayInfo.VolumeCnt < AudioPlayInfo.playVolume)
//        {
//            AudioPlayInfo.VolumeCnt++;
//            Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
//            DelayUs(5);
//        }
//        Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
//        DelayUs(5);
//        Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
//        DelayUs(5);
//        Codec_SetVolumet(AudioPlayInfo.VolumeCnt);
//        DelayUs(5);
//    }

    SendMsg(MSG_AUDIO_ALL_INIT_SUCESS);
    ClearMsg(MSG_AUDIO_INIT_FAIL);
}

/*
--------------------------------------------------------------------------------
  Function name :  BOOLEAN AudioStop(UINT16 ReqType);
  Author        :  zs
  Description   :  audio decode over,change paly status,and close file.
  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
BOOLEAN AudioStop(UINT16 ReqType)
{
    TASK_ARG TaskArg;
    uint32 timeout = 200;
    uint16 AudioPlayStatebak;
    BOOLEAN ret = TRUE;

    DEBUG("Audio Stop");

    AudioPlayStatebak = AudioPlayState;
    AudioPlayState = AUDIO_STATE_STOP;

    //wait DAM finished
    {
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

    // codec deinitial
    {
        DmaChannel2Disable(0);

        I2SStop(I2S_START_TX);
        ScuSoftResetCtr(RST_I2S, 1);

        if ((pAudioRegKey->samplerate == FS_8000Hz) ||
            (pAudioRegKey->samplerate == FS_12KHz) ||
            (pAudioRegKey->samplerate == FS_16KHz) ||
            (pAudioRegKey->samplerate == FS_24KHz) ||
            (pAudioRegKey->samplerate == FS_32KHz) ||
            (pAudioRegKey->samplerate == FS_48KHz))
        {
            Codec_ExitMode();
        }
    }

    #ifdef _RK_EQ_
    if(!CheckMsg(MSG_AUDIO_INIT_FAIL))
    {
        EffectEnd(); //audio effect finish.
    }
    #endif

    AudioCodecClose();

    if (AudioControlOps.pAudioTrackClose)
    {
        AudioControlOps.pAudioTrackClose();
    }

    AudioVariableDeInit();

    AudioIntAndDmaDeInit();

    AudioPMUDeInit();

    AudioSCUDeInit();

    /*************************************************************/
    //Flash Refresh zyz
    #if (NAND_DRIVER == 1)
    PMU_EnterModule(PMU_MAX);
    FtlRefreshHook();
    PMU_ExitModule(PMU_MAX);
    #endif
    /*************************************************************/

    if ((Audio_Stop_Normal == ReqType) && (AudioPlayFileNum  <= AudioFileInfo.TotalFiles))
    {
        AudioStopMode = Audio_Stop_Normal;//by zs 0512
        AudioGetNextMusic(MusicNextFile);
        SendMsg(MSG_AUDIO_NEXTFILE);
        ret = FALSE;
    }

    //return playstate, for next song state
    AudioPlayState = AudioPlayStatebak;
    if (AudioPlayState != AUDIO_STATE_PAUSE)
    {
        AudioPlayState = AUDIO_STATE_PLAY;
    }

    #ifdef BT_VOICENOTIFY
    if(CurrentCodec == CODEC_BT_VOCIE_NOTIFY)
    {
        MusicOutputVol = playVolBak;
    }
    #endif
    return ret;

}

/*
--------------------------------------------------------------------------------
  Function name :  BOOLEAN AudioDecodeProc(MSG_ID id,void *msg)
  Author        :  zs
  Description   :  audio handle function, remember to set vaule for variable AudioProcId and AudioProcMsg when call
  				   this function.
                   AudioProcId  -- id of msg、AudioProcMsg -- the content of message.
  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
BOOLEAN AudioDecodeProc(MSG_ID id,void * msg)
{
    BOOLEAN ret = TRUE;
    unsigned long  HoldOnTimeTemp;

    switch (id)
    {
        case MSG_AUDIO_DECSTART:
            AudioStart();
            break;

        case MSG_AUDIO_PAUSE:
            if (AudioControlOps.pAudioTrackPause)
            {
                AudioControlOps.pAudioTrackPause();
            }
            break;

        case MSG_AUDIO_RESUME:
            if (AudioControlOps.pAudioTrackResume)
            {
                AudioControlOps.pAudioTrackResume();
            }
            break;

        case MSG_AUDIO_STOP:
        	AudioStop((int)msg);
            break;

        case MSG_AUDIO_FFD:     //FFW
            if (AudioControlOps.pAudioTrackFFD)
            {
                AudioControlOps.pAudioTrackFFD((uint32)msg);
            }
            break;

        case MSG_AUDIO_FFW:     //FFD
            if (AudioControlOps.pAudioTrackFFW)
            {
                AudioControlOps.pAudioTrackFFW((uint32)msg);
            }
            break;

        case MSG_AUDIO_GETPOSI:
            CodecGetTime(&pAudioRegKey->CurrentTime);
            break;

        case MSG_AUDIO_ABPLAY:
            if (AudioControlOps.pAudioTrackSetAB)
            {
                AudioControlOps.pAudioTrackSetAB();
            }
            break;

        case MSG_AUDIO_ABSETA:
            if (AudioControlOps.pAudioTrackSetAB_A)
            {
                AudioControlOps.pAudioTrackSetAB_A();
            }
            break;

        case MSG_AUDIO_ABSTOP:
            if (AudioControlOps.pAudioTrackABStop)
            {
                AudioControlOps.pAudioTrackABStop();
            }
            break;

        case MSG_AUDIO_VOLUMESET:
            AudioSetVolume();
            Codec_SetVolumet(AudioPlayInfo.playVolume);
            break;

        case MSG_AUDIO_EQSET:
            if (msg)
            {
                memcpy(&AudioIOBuf.EffectCtl, &pAudioRegKey->UserEQ, sizeof(RKEffect));
            }
            AudioSetEQ();
            break;

        case MSG_AUDIO_UNMUTE:
            Codec_DACUnMute();
            break;

        case MSG_AUDIO_NEXTFILE:
        {
            if (AudioControlOps.pAudioTrackNext)
            {
                AudioControlOps.pAudioTrackNext((uint32)msg);
            }
            SendMsg(MSG_AUDIO_NEXTFILE);
        }
           break;

		case MSG_AUDIO_FADE:
			//#ifdef _RK_EQ_
			#if 0
			 if((AUDIO_STATE_PLAY == AudioPlayState)&&(FadeIsFinished()))
			 {
			    FadeInit(0,(pAudioRegKey->samplerate>>1),1); //0.5秒  //淡出
			    SendMsg(MSG_MUSIC_NEXTFILE);
			    ClearMsg(MSG_MUSIC_NEXTFILE_OK);
			 }
			 else
             #endif
			 {
			    MusicDecodeProc(MSG_AUDIO_NEXTFILE, (void*)MusicNextFile);
			 }
			break;

        default:
                ret = FALSE;
                break;

    }
    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name :  void MusicInit(void)
  Author        :  zs
  Description   :  audion control initialization.

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void MusicInit(void *pArg)
{
    int32 ret = RETURN_OK;
    AUDIO_THREAD_ARG *pAudioArg = pArg;

    AudioErrorFileCount = 0;
    AudioPlayState = AUDIO_STATE_STOP;

    memset((uint8*)&AudioControlOps, 0, sizeof(AUDIO_CONTROL_OPS));
    pAudioRegKey   = &gRegAudioConfig;
    if (pAudioArg->pAudioOpsInit)
    {
        pAudioArg->pAudioOpsInit();
    }
    else {DEBUG("error!!!!");}

    if (AudioControlOps.pAudioTrackInit)
    {
        if (RETURN_OK != AudioControlOps.pAudioTrackInit(pArg))
        {
            SendMsg(MSG_AUDIO_OPENERROR);
            DEBUG("Audio Track Init Error!");
            return;
        }
    } else {DEBUG("error!!!!");}

    if (AudioControlOps.pHoldonPlayInit)
    {
        AudioControlOps.pHoldonPlayInit();
    }

    #ifdef _RK_EQ_
    AudioEffectInit();
    #endif

    ScuClockGateCtr(CLOCK_GATE_I2S, 1);
    Codec_SetMode(Codec_DACout);

    AutoPowerOffDisable();

    AudioPlayState = AUDIO_STATE_PLAY;

    AudioDecodeProc(MSG_AUDIO_DECSTART,NULL);
}

/*
--------------------------------------------------------------------------------
  Function name :  UINT32 MusicService(void)
  Author        :  zs
  Description   :
  Input         :
  Return        :
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
UINT32 MusicService(void)
{
    int ret = 0;

    //AudioServiceHook();
//    AudioVolumeFade();

    if (AudioControlOps.pAudioTrackABProcess)
    {
        AudioControlOps.pAudioTrackABProcess();
    }

    //Msg Process
    if (AudioControlOps.pAudioTrackMsgProcess)
    {
        ret = AudioControlOps.pAudioTrackMsgProcess();
    }

    //Eq Update
    if (AudioControlOps.pAudioTrackEQUpdate)
    {
        AudioControlOps.pAudioTrackEQUpdate();
    }

	AudioVolumeFade();

    //Repeat Mode update
    if (AudioControlOps.pAudioTrackRepeatModeUpdate)
    {
        ret = AudioControlOps.pAudioTrackRepeatModeUpdate();
    }

    //Play Order update
    if (AudioControlOps.pAudioTrackPlayOrderUpdate)
    {
        ret = AudioControlOps.pAudioTrackPlayOrderUpdate();
    }

    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name :  void MusicDeInit(void)
  Author        :
  Description   :
  Input         :
  Return        :
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIO_TEXT_
void MusicDeInit(void)
{
    //if init fail
    OutputVolOffset = 0;

    if(TRUE != GetMsg(MSG_AUDIO_OPENERROR))
    {
        AudioDecodeProc(MSG_AUDIO_STOP,(void*)Audio_Stop_Force);

        AudioEffectDeInit();

        if (AudioControlOps.pHoldonPlayDeInit)
        {
            AudioControlOps.pHoldonPlayDeInit();
        }

        if (AudioControlOps.pAudioTrackDeInit)
        {
            AudioControlOps.pAudioTrackDeInit();
        }
        else { DEBUG("error!!!");}
    }

    Codec_ExitMode();

    AutoPowerOffEnable();

    ScuClockGateCtr(CLOCK_GATE_I2S, 0);
}
#endif

#endif

/*
********************************************************************************
*
*                         End of AudioControl.c
*
********************************************************************************
*/

