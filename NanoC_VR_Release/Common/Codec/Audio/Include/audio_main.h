/*
********************************************************************************
*                   Copyright (c) 2008, Rockchip
*                         All rights reserved.
*
* File Name£º   audio_main.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             Vincent Hsiung     2009-1-8          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _AUDIO_MAIN_H_
#define _AUDIO_MAIN_H_

#include "SysConfig.h"

/************************************************************
1. Select any Audio CODEC plug-in
*************************************************************/
#ifdef _MP3_DECODE_
	#define MP3_INCLUDE
#endif

#ifdef _WMA_DECODE_
	#define WMA_INCLUDE
#endif

#ifdef _WAV_DECODE_
	#define WAV_INCLUDE
#endif

#ifdef _APE_DECODE_
	#define APE_DEC_INCLUDE
#endif

#ifdef _FLAC_DECODE_
	#define FLAC_DEC_INCLUDE
#endif

#ifdef _AAC_DECODE_
	#define AAC_INCLUDE
#endif

#ifdef _SBC_DECODE
    #define SBC_INCLUDE
#endif

#ifdef BT_PHONE
    #define BT_PHONE_INCLUDE
#endif

#ifdef BT_VOICENOTIFY
    #define BT_VOICE_NOTIFY_INCLUDE
#endif   

#ifdef _USBAUDIO_DECODE_
    #define USB_AUDIO_INCLUDE
#endif

#ifdef _RK_EQ_
    #define RK_MP3_EQ_WAIT_SYNTH     
#endif 

#ifdef RK_MP3_EQ_WAIT_SYNTH
    #define MP3_EQ_WAIT_SYNTH  1
#else
    #define MP3_EQ_WAIT_SYNTH  0
#endif

#ifdef _RK_ID3_
#endif

#ifdef _RK_SPECTRUM_
#endif

#ifdef VIDEO_MP2_DECODE
    #define MP2_INCLUDE
#endif
/************************************************************
2. Include WMA Encoding  or MP3 Encoding or ADPCM Encoding
                   Use Only One Encoding
*************************************************************/
/*------------------------------------------------------------
- Select One of Encoding
------------------------------------------------------------*/
#ifdef _WAV_ENCODE_
	#define ADPCM_ENC_INCLUDE
#endif

#ifdef _RK_ID3_JPEG_
    #define RK_JPEG_ID3
    #define RK_JPEG_ASF
#endif 
/* sections define */
// for wma overlay add by helun
#define _ATTR_WMA_DATA_16Ob_     __attribute__((section("wma_16Ob")))
#define ATTR_WMA_DATA_44Qb_    __attribute__((section("wma_44Qb")))
#define ATTR_WMA_DATA_44Ob_      __attribute__((section("wma_44Ob")))

//-------------------------------------------MP3----------------------------------------------------------
#define _ATTR_MP3INIT_TEXT_     __attribute__((section("Mp3InitCode")))
#define _ATTR_MP3INIT_DATA_     __attribute__((section("Mp3InitData")))
#define _ATTR_MP3INIT_BSS_      __attribute__((section("Mp3InitBss"),zero_init))

#define _ATTR_MP3DEC_TEXT_     __attribute__((section("Mp3Code")))
#define _ATTR_MP3DEC_DATA_     __attribute__((section("Mp3Data")))
#define _ATTR_MP3DEC_BSS_      __attribute__((section("Mp3Bss"),zero_init))

//-------------------------------------------WAV----------------------------------------------------------
#define _ATTR_WAVDEC_INIT_TEXT_     __attribute__((section("WavDecInitCode")))
#define _ATTR_WAVDEC_INIT_DATA_     __attribute__((section("WavDecInitData")))
#define _ATTR_WAVDEC_INIT_BSS_      __attribute__((section("WavDecInitBss"),zero_init))

#define _ATTR_WAVDEC_TEXT_     __attribute__((section("WavDecCode")))
#define _ATTR_WAVDEC_DATA_     __attribute__((section("WavDecData")))
#define _ATTR_WAVDEC_BSS_      __attribute__((section("WavDecBss"),zero_init))

//-----------------------------------------FLAC----------------------------------------------------------
#define     _ATTR_FLACDEC_TEXT_          __attribute__((section("FlacDecCode")))
#define     _ATTR_FLACDEC_DATA_          __attribute__((section("FlacDecData")))
#define     _ATTR_FLACDEC_BSS_           __attribute__((section("FlacDecBss"),zero_init))
//-----------------------------------------AAC----------------------------------------------------------
#define     _ATTR_AACDEC_TEXT_          __attribute__((section("AacDecCode")))
#define     _ATTR_AACDEC_DATA_          __attribute__((section("AacDecData")))
#define     _ATTR_AACDEC_BSS_           __attribute__((section("AacDecBss"),zero_init))

//#define     _ATTR_AACDEC_DATA_ROADD          __attribute__((section("AacRODataADD")))
#define     _ATTR_AACDEC_DATA_RO         __attribute__((section("AacROData")))

//-------------------------------------------ID3----------------------------------------------------------
#define _ATTR_ID3_TEXT_     __attribute__((section("Id3Code")))
#define _ATTR_ID3_DATA_     __attribute__((section("Id3Data")))
#define _ATTR_ID3_BSS_      __attribute__((section("Id3Bss"),zero_init))

//-------------------------------------------ID3 JPG------------------------------------------------------
//#define _ATTR_ID3JPG_TEXT_     __attribute__((section("Id3JpgCode")))
//#define _ATTR_ID3JPG_DATA_     __attribute__((section("Id3JpgData")))
//#define _ATTR_ID3JPG_BSS_      __attribute__((section("Id3JpgBss"),zero_init))

//-------------------------------------------WAV Encode----------------------------------------------------------
#define _ATTR_MSADPCM_TEXT_     __attribute__((section("EncodeMsadpcmCode")))
#define _ATTR_MSADPCM_DATA_     __attribute__((section("EncodeMsadpcmData")))
#define _ATTR_MSADPCM_BSS_      __attribute__((section("EncodeMsadpcmBss"),zero_init))

//-------------------------------------------MP3 Encode----------------------------------------------------------

#define _ATTR_AUDIO_TEXT_     __attribute__((section("AudioCode")))
#define _ATTR_AUDIO_DATA_     __attribute__((section("AudioData")))
#define _ATTR_AUDIO_BSS_      __attribute__((section("AudioBss"),zero_init))

#define _ATTR_MSEQ_TEXT_     __attribute__((section("MsEqCode")))
#define _ATTR_MSEQ_DATA_     __attribute__((section("MsEqData")))
#define _ATTR_MSEQ_BSS_      __attribute__((section("MsEqBss"),zero_init))

//-------------------------------------------MP2 Encode----------------------------------------------------------
#define _ATTR_MP2DEC_TEXT_     __attribute__((section("Mp2Code"/*"MP3DEC_CODE_SEG"*/)))
#define _ATTR_MP2DEC_DATA_     __attribute__((section("Mp2Data"/*"MP3DEC_DATA_SEG"*/)))
#define _ATTR_MP2DEC_BSS_      __attribute__((section("Mp2Bss"/*"MP3DEC_BSS_SEG"*/),zero_init))

//-----------------------------------------SBC----------------------------------------------------------
#define     _ATTR_SBCDEC_TEXT_          __attribute__((section("SbcDecCode")))
#define     _ATTR_SBCDEC_DATA_          __attribute__((section("SbcDecData")))
#define     _ATTR_SBCDEC_BSS_           __attribute__((section("SbcDecBss"),zero_init))

//-------------------------------------Aec algorithm----------------------------------------------------------
#define     _ATTR_AECALG_TEXT_          __attribute__((section("AecAlgCode")))
#define     _ATTR_AECALG_DATA_          __attribute__((section("AecAlgData")))
#define     _ATTR_AECALG_BSS_           __attribute__((section("AecAlgBss"),zero_init))

#endif		// _AUDIO_MAIN_H_

