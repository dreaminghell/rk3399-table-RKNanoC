

/******************************************************************************
*
*  Copyright (C),2007, Fuzhou Rockchip Co.,Ltd.
*
*  File name :     effect.c
*  Description:    audio effect processing
*  Remark:
*
*  History:
*           <author>      <time>     <version>       <desc>
*           Huweiguo     07/09/27      1.0
*
*******************************************************************************/

#include "audio_globals.h"
#include "SysInclude.h"
#ifdef _RK_EQ_

#include "AudioControl.h"
#include "Effect.h"

#ifdef _MS_EQ_
#include "msaudspapi.h"
#endif

extern AudioInOut_Type     AudioIOBuf;
extern int CurrentCodec;
extern  int preshift;
extern  int ISampleRate;
extern eqfilter filters[6];

#ifdef _BBE_EQ_
extern int BBEProcessInit(short p_coef, short c_coef, int sample_rate);
extern int BBEProProcess(short *in, int len, short *out);
#endif

/**************************************************************************
* function description: set parameters of RockEQ
* input:  [pEft]   --
* output:
* return:    0:success; other:failure.
* note:
***************************************************************************/
_ATTR_AUDIO_EQ_DATA_ short PresetGain[5][CUSTOMEQ_BANDNUM] =
{
    {16,  8, 12, 16, 16},  // ROCK
    {12, 16, 16, 12, 12},  // POP
    {12, 12, 12, 10, 10},  // CLASSIC
    {18, 16, 14, 10, 10},  // BASS
    {16, 12, 12, 16, 18}   // JAZZ
};

_ATTR_AUDIO_EQ_DATA_ short UseEqTable[CUSTOMEQ_LEVELNUM] = {6, 8, 10, 12, 14, 16, 18};
//_ATTR_AUDIO_EQ_BSS_  short UseEQ[CUSTOMEQ_BANDNUM];

#ifdef _BBE_EQ_
_ATTR_AUDIO_EQ_DATA_ short BBECoefTbl[3][2] =
{
    {7,  8},    //  low 
    {7,  8},    //  mid
    {7,  8}     //  high
};
#endif

/**************************************************************************
* function description: audio effect is end.
* input   :  null
* output  : null
* return  :    0:success; other:failure.
* note:
***************************************************************************/
_ATTR_AUDIO_EQ_TEXT_
ReadEqData(char *p , int off ,int size)
{
	ReadModuleData(MODULE_ID_AUDIO_EQ , p , off , size );
}

/**************************************************************************
* function description: audio effect is end.
* input   :  null
* output  : null
* return  :    0:success; other:failure.
* note:
***************************************************************************/
_ATTR_AUDIO_EQ_TEXT_
long RKEQAdjust(RKEffect *pEft, int SamplingRate)
{
    int i;

    switch (pEft->Mode)
    {
        case EQ_ROCK:
        case EQ_POP:
        case EQ_CLASSIC:
        case EQ_BASS:
        case EQ_JAZZ:
            RockEQAdjust(SamplingRate, PresetGain[pEft->Mode-EQ_ROCK], 1);
            break;

        case EQ_USER:
            #if 0
            for (i = 0;i < 5;i++)
            {                
                UseEQ[i] = pEft->RKCoef.dbGain[i];
            }
            RockEQAdjust(SamplingRate, UseEQ, 1);
            #endif
            RockEQAdjust(SamplingRate, pEft->RKCoef.dbGain, 1);
            break;

        default:
            break;
    }
    return 0;
}

#ifdef _BBE_EQ_

/**************************************************************************
* function description: audio effect is end.
* input   :  null
* output  : null
* return  :    0:success; other:failure.
* note:
***************************************************************************/
_ATTR_AUDIO_EQ_TEXT_
long BBEEQAdjust(int SamplingRate)
{
    short p_coef, c_coef;
    
	p_coef = BBECoefTbl[gSysConfig.MusicConfig.Eq.BBEMode][0];
    c_coef = BBECoefTbl[gSysConfig.MusicConfig.Eq.BBEMode][1];
	BBEProcessInit(p_coef, c_coef, SamplingRate);
}
#endif


#ifdef _MS_EQ_
/**************************************************************************
* 函数描述:  PlayFx EQ参数设置, 当音效模式为NOR 时
* 入口参数:
* 出口参数:
* 返回值:    0:处理成功; other:失败。
* 注释:
***************************************************************************/
_ATTR_AUDIO_EQ_TEXT_
long MsEQAdjust(long MsMode, unsigned long SamplingRate)
{
    if (MsAudspInit(SamplingRate) != 0)
    {
        return 1;
    }
    
    switch (MsMode/*AudioIOBuf.EffectCtl.MSMode*/)
    {
        #if 0
        case AUDIO_LEQ:
            ms_set_hv_enabled(0);
            ms_set_bb_enabled(0);
            ms_set_leq_enabled(1);
            ms_set_bb_level(3);
            break;
        #endif

        case AUDIO_BB:
            ms_set_leq_enabled(0);
            ms_set_hv_enabled(0);
            ms_set_bb_enabled(1);
            ms_set_bb_level(4);
            break;

        case AUDIO_3D:
            ms_set_leq_enabled(0);
            ms_set_bb_enabled(0);
            ms_set_hv_enabled(1);
            ms_set_bb_level(3);
            ms_set_hv_spatialize(8);
            ms_set_hv_width(8);
            break;

        default: // MS EQ Off
            ms_set_leq_enabled(0);
            ms_set_bb_enabled(0);
            ms_set_hv_enabled(0);
            ms_set_bb_level(1);
            break;
    }
    
    singal_effect_change_and_refresh_argtbl(pAuDSP, (void *)g_argTable, (void *)g_argTableNew);

    return 0;
    
}

#endif

/**************************************************************************
* description	: audio effect adjust.
* input   		: null
* output  		: null
* return  		: 0:success; other:failure.
* note    		: 
***************************************************************************/
uint32 EqTypeBack = 0;
_ATTR_AUDIO_EQ_TEXT_
long EffectAdjust(void)
{
    AudioInOut_Type  *pAudio = &AudioIOBuf;
    RKEffect         *pEffect = &pAudio->EffectCtl;
    uint32           IsrTemp = UserIsrDisable();
    unsigned long    SamplingRate = 44100;
    int              i,j=0;
    
    pAudioRegKey->UserEQ.Mode =  gSysConfig.MusicConfig.Eq.Mode ;
    AudioIOBuf.EffectCtl.Mode =  gSysConfig.MusicConfig.Eq.Mode ;

    CodecGetSampleRate(&SamplingRate);
    
    //User EQ Check: is Normal????
    if (AudioIOBuf.EffectCtl.Mode == EQ_USER)
    {
        for (i = 0; i < CUSTOMEQ_BANDNUM; i++)
        {
            AudioIOBuf.EffectCtl.RKCoef.dbGain[i] = 6 + 2*(gSysConfig.MusicConfig.Eq.RKCoef.dbGain[i]);

            if((AudioIOBuf.EffectCtl.RKCoef.dbGain[i] > 18) ||( AudioIOBuf.EffectCtl.RKCoef.dbGain[i] < 6))//zs 06.01 防止数据超出
            {
                pAudioRegKey->UserEQ.RKCoef.dbGain[i] = 12;
                AudioIOBuf.EffectCtl.RKCoef.dbGain[i] = 12;
            }
        }
        
        IsEQNOR = TRUE;
        for (i = 0; i < CUSTOMEQ_BANDNUM; i++)
        {
            if (gSysConfig.MusicConfig.Eq.RKCoef.dbGain[i] != 3)
            {
                IsEQNOR = FALSE;
                break;
            }
        }  

        if (TRUE == IsEQNOR)
        {
            pEffect->Mode = EQ_NOR;
        }
    }

    #if 0 //def _MS_EQ_
    if ((EqTypeBack == EQ_PLAYFX) && (pEffect->Mode != EQ_PLAYFX))
    {
        //MsEQAdjust(MsMode, SamplingRate);
        MsAudspEnd();
    }
    #endif    

    
    EqTypeBack = pEffect->Mode;

    switch (pEffect->Mode)
    {
        case EQ_NOR:            
            PMU_ExitModule(PMU_EQ);
            break;
            
        case EQ_ROCK:
        #ifdef _BBE_EQ_
		case EQ_BBE:
            PMU_EnterModule(PMU_EQ);
            ModuleOverlay(MODULE_ID_AUDIO_RKEQ, MODULE_OVERLAY_ALL);
            
            BBEEQAdjust(SamplingRate);
            break;
        #endif
        
        case EQ_POP:
        #ifdef _MS_EQ_
        //case EQ_PLAYFX:			
            PMU_EnterModule(PMU_EQ);
            ModuleOverlay(MODULE_ID_AUDIO_MSEQ, MODULE_OVERLAY_ALL);

            EqTypeBack = EQ_PLAYFX;
            AudioIOBuf.EffectCtl.MSMode = AUDIO_3D;
            MsEQAdjust(AudioIOBuf.EffectCtl.MSMode, SamplingRate);
            break;            
        #endif

        case EQ_BASS:
        #ifdef _MS_EQ_
        case EQ_PLAYFX:			
            PMU_EnterModule(PMU_EQ);
            ModuleOverlay(MODULE_ID_AUDIO_MSEQ, MODULE_OVERLAY_ALL);

            EqTypeBack = EQ_PLAYFX;
            AudioIOBuf.EffectCtl.MSMode = AUDIO_BB;
            MsEQAdjust(AudioIOBuf.EffectCtl.MSMode, SamplingRate);
            break;            
        #endif
            
        case EQ_CLASSIC:
        case EQ_JAZZ:
        case EQ_USER:
            PMU_EnterModule(PMU_EQ);
            ModuleOverlay(MODULE_ID_AUDIO_RKEQ, MODULE_OVERLAY_ALL);
            
            RKEQAdjust(pEffect, SamplingRate);
            
            break;

        default:
            break;
    }

    UserIsrEnable(IsrTemp);  
    
    return 0; //success.
}

/**************************************************************************
* function description: audio effect is processing.
* input   :[pBuffer] -- the buffer put pcm data.
		   [PcmLen] --  the length of pcm data.
* output  : null
* return  : 0:success; other:failure.
* note    : the data that audio effect would take care need use data format LRLRLR.
***************************************************************************/
_ATTR_AUDIO_EQ_TEXT_
long EffectProcess(short *pBuffer, long PcmLen)
{
    AudioInOut_Type  *pAudio = &AudioIOBuf;
    RKEffect   *pEffect = &pAudio->EffectCtl;

    if (pBuffer == 0)
        return 1;

    if (PcmLen <= 0)
        return 0;
	
    switch (pEffect->Mode)
    {
        case EQ_ROCK:
        #ifdef _BBE_EQ_
		case EQ_BBE:
			BBEProProcess(pBuffer, PcmLen, pBuffer);
            break;
        #endif
        
        case EQ_POP:
        case EQ_BASS:		
        #ifdef _MS_EQ_
        case EQ_PLAYFX:
            MsAudspProssing(pBuffer, PcmLen);
            break;
		#endif

        case EQ_CLASSIC:
        case EQ_JAZZ:
        case EQ_USER:
			if (!(PcmLen & (64-1))) //by Vincent
			{
            	RockEQProcess(pBuffer, PcmLen);
			}
            break;


        case EQ_NOR:
        default:
            break;
    }
    
    return 0; //success
}

/**************************************************************************
* function description: audio effect is initialization.
* input   :  null
* output  : null
* return  :    0:success; other:failure.
* note:
***************************************************************************/
_ATTR_AUDIO_TEXT_
long EffectInit(void)
{
    return 0;
}


/**************************************************************************
* function description: audio effect is end.
* input   :  null
* output  : null
* return  :    0:success; other:failure.
* note:
***************************************************************************/
 _ATTR_AUDIO_EQ_TEXT_
long EffectEnd(void)
{
	AudioInOut_Type  *pAudio = &AudioIOBuf;
    RKEffect   *pEffect = &pAudio->EffectCtl;
    
    #ifdef _MS_EQ_
    if (pEffect->Mode == EQ_PLAYFX)
        MsAudspEnd();
    #endif

    return 0;
}

#endif// _RK_EQ_

