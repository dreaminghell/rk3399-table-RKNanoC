/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   AudioControl.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               ZS      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _AUDIO_CONTROL_H_
#define _AUDIO_CONTROL_H_

#include "FsInclude.h"
#include "audio_globals.h"
#include "HoldonPlay.h"

#undef  EXT
#ifdef _IN_AUDIO_CONTROL_
#define EXT
#else
#define EXT extern
#endif

//#define SINATAB   //output sine wave.
//#define NOISR

/*
*-------------------------------------------------------------------------------
*  
*                           Macro define
*  
*-------------------------------------------------------------------------------
*/

//section define
#define _ATTR_AUDIO_TEXT_     __attribute__((section("AudioCode")))
#define _ATTR_AUDIO_DATA_     __attribute__((section("AudioData")))
#define _ATTR_AUDIO_BSS_      __attribute__((section("AudioBss"),zero_init))
#define _ATTR_AUDIO_INIT_TEXT_  __attribute__((section("AudioInitCode")))

#define _ATTR_AUDIO_EQ_TEXT_     __attribute__((section("AudioEQCode")))
#define _ATTR_AUDIO_EQ_DATA_     __attribute__((section("AudioEQData")))
#define _ATTR_AUDIO_EQ_BSS_      __attribute__((section("AudioEQBss"),zero_init))

#define _ATTR_AUDIOFILE_TEXT_     __attribute__((section("AudioFileCode")))
#define _ATTR_AUDIOFILE_DATA_     __attribute__((section("AudioFileData")))
#define _ATTR_AUDIOFILE_BSS_      __attribute__((section("AudioFileBss"),zero_init))

//
#define AUDIO_DMACHANNEL_IIS    DMA_CHN2
#define AUDIO_DMACHANNEL_HW     DMA_CHN1

#define AUDIOCONSOLE            1
#define AUDIOTRYTIME            10

#define AudioStepLen            (10000)//ms the step of FFW.

#define Audio_Stop_Normal       0
#define Audio_Stop_Force        1

#define Audio_Direct_BackWord   (0<<8)
#define Audio_Direct_Forword    (1<<8)


#define AUDIONEXTFILE           1
#define AUDIOCURRENTFILE        0
#define AUDIOPREFILE            -1
#define CHAR_NUM_PER_FILE_NAME      (UINT32)40    /*the total charactor of save file name */
#define ID3_SAVE_CHAR_NUM          (UINT32)80//40    /*must bigger than ID3_TAG_SIZE,if not, may be overflow when operating id3 information*/
#define MAX_STRING_SIZE_WCHAR ID3_SAVE_CHAR_NUM
#define MAX_STRING_SIZE_CHAR (MAX_STRING_SIZE_WCHAR*2)
/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/
typedef struct
{
    uint32 ABRequire;
    uint32 AudioABStart;
    uint32 AudioABEnd;
    uint32 PlayDirect;
    uint8 playVolume;
    uint8 VolumeCnt;
    
}AUDIOCONTROL;




//define the play status.
typedef enum
{
    AUDIO_STATE_PLAY,
    AUDIO_STATE_FFD,
    AUDIO_STATE_FFW,
    AUDIO_STATE_PAUSE,
    AUDIO_STATE_STOP
    
}AUDIOSTATE;



//reapeat mode
typedef enum
{
    AUDIO_ONCE,           //repeat once one song.
    AUDIO_REPEAT,         //repeat one song
    AUDIO_FOLDER_ONCE,    //once directory.
    AUIDO_FOLDER_REPEAT,  //directory cycle
    AUDIO_ALLONCE,        //repeat once all song.
    AUDIO_ALLREPEAT,      //cycle play all song.
    AUDIO_TRY             //lyrics
    
}AUDIOREPEATMODE;



//define A-B status.
typedef enum
{
    AUDIO_AB_NULL,
    AUDIO_AB_A,
    AUDIO_AB_PLAY
    
}AUDIOABSTATE;


//EQ mode define.
//need be keep consisten with modes define in file effect.h. 
typedef enum
{
    AUDIO_NORMAL,
    AUDIO_MSEQ,
    AUDIO_ROCK,
    AUDIO_POP,
    AUDIO_CLASSIC,
    AUDIO_BASS,
    AUDIO_JAZZ,
    AUDIO_USER
    
}AUDIOEQTYPE;

//audio effect define of user.
typedef enum
{
    EQ_N12DB,
    EQ_N8DB,
    EQ_N4DB,
    EQ_0DB,
    EQ_4DB,
    EQ_8DB,
    EQ_12DB
    
}USER_EQ;

//display mode define.
typedef enum
{
    AUDIO_ID3,
    AUDIO_LRC,
    AUDIO_FREQCHART,
    AUDIO_CLOSE
    
}AUDIODISPMODE;

typedef enum
{
    AUDIO_3D,
    AUDIO_BB,
    AUDIO_LEQ
    
}AUDIOMSEQMODE;

typedef  struct _eqfilter {
    long a[5];		 
    long b[2][4];
}eqfilter;

typedef struct _AUDIO_CONTROL_OPS_ {
    
    int32 (*pAudioTrackInit)(void*); 
    int32 (*pAudioTrackDeInit)(void);

    int32 (*pAudioTrackOpen)(void);
    int32 (*pAudioTrackClose)(void);
    
    BOOLEAN (*pAudioTrackPause)(void);
    BOOLEAN (*pAudioTrackResume)(void);

    BOOLEAN (*pAudioTrackFFD)(uint32 );
    BOOLEAN (*pAudioTrackFFW)(uint32 );

    BOOLEAN (*pAudioTrackNext)(int16 );

    void (*pHoldonPlayInit)(void);
    void (*pHoldonPlayProcess)(void);
    void (*pHoldonPlayDeInit)(void);

    void (*pAudioTrackSetAB_A)(void);
    void (*pAudioTrackSetAB)(void);
    void (*pAudioTrackABStop)(void);
    void (*pAudioTrackABProcess)(void);

    void (*pAudioTrackEQUpdate)(void);
    void (*pAudioTrackEQProcess)(short *, int32 );

    int32 (*pAudioTrackRepeatModeUpdate)(void);
    int32 (*pAudioTrackPlayOrderUpdate)(void);
    
    int32 (*pAudioTrackDecodingErr)(uint32 );
    int32 (*pAudioTrackMsgProcess)(void);
    int32 fileMode;
    
}AUDIO_CONTROL_OPS;

#define FILE_MODE_LOCAL_MUSIC   (0)
#define FILE_MODE_STREAM_MEDIA  (1) 

/*sturcture for detailed information saved.*/
typedef struct _FILE_SAVE_STRUCT
{
   // UINT16  LongFileName[CHAR_NUM_PER_FILE_NAME+1];
    UINT16  ID3Title[ID3_SAVE_CHAR_NUM+1];
    UINT16  ID3Artist[ID3_SAVE_CHAR_NUM+1];
    UINT16  ID3Album[ID3_SAVE_CHAR_NUM+1];
    UINT16  Genre[ID3_SAVE_CHAR_NUM+1];
    UINT16  Time[9];
    char    Name[12];
	char    Path[3 + (MAX_DIR_DEPTH - 1) * 12 + 1];

    UINT16 RecordFileOrNot; //whether is record file.1:yes, 0:no
}FILE_SAVE_STRUCT;

/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/

//外部变量接口
#define     MusicOutputVol                  gSysConfig.OutputVolume
_ATTR_AUDIO_BSS_    extern int              CurrentCodec;

_ATTR_AUDIO_BSS_    EXT int   				AudioStopMode;   //normal stop or Force//Audio_Stop_Normal or Audio_Stop_Force 
_ATTR_AUDIO_BSS_    EXT uint32              outptr;
_ATTR_AUDIO_BSS_    EXT UINT16              AudioPlayState;
_ATTR_AUDIO_BSS_    EXT UINT16              MusicLongFileName[MAX_FILENAME_LEN];
//_ATTR_AUDIO_BSS_    EXT UINT16              MusicLongDirPath[DISP_PATHNAME_LEN];
//_ATTR_AUDIO_BSS_    EXT RKEffect            NaNoEQ;
_ATTR_AUDIO_BSS_    EXT AudioInOut_Type     AudioIOBuf;
_ATTR_AUDIO_BSS_    EXT track_info*         pAudioRegKey;
_ATTR_AUDIO_BSS_    EXT AUDIOCONTROL        AudioPlayInfo;
_ATTR_AUDIO_BSS_    EXT uint32              AudioPlayFileNum;
_ATTR_SYS_BSS_      EXT HOLDON_PLAY_INFO    HoldOnPlayInfo;
_ATTR_AUDIO_BSS_    EXT BOOLEAN             MusicUIState; //audio interface UI status.0 mean display audio interface.1 mean not.
_ATTR_AUDIO_BSS_    EXT UINT32              PrevLrcPlayTimeSec;
_ATTR_AUDIO_BSS_    EXT UINT16              AudioErrorFileCount;//sen #20090803#1 all audio file is not support
_ATTR_AUDIO_BSS_    EXT UINT32              AudioErrorFrameNum;
//the offset value of audio file seek.
_ATTR_AUDIO_BSS_    EXT UINT32              AudioFileSeekOffset;
_ATTR_AUDIO_BSS_    EXT BOOL                IsEQNOR;

_ATTR_AUDIO_BSS_    EXT AUDIO_CONTROL_OPS   AudioControlOps;



#ifdef _IN_AUDIO_CONTROL_
_ATTR_AUDIO_DATA_ EXT INT32      MusicNextFile = 1;
_ATTR_AUDIO_DATA_ EXT track_info gRegAudioConfig =
{
    1,                          //unsigned long TotalTime;    total play time of song.
    0,                          //unsigned long CurrentTime;  current time of song play.
    1,                          //unsigned long Bitrate;      the bit rate of song.
    1,                          //unsigned long Samplerate;   sample rate of song.
    1,                          //channels
    1,                          //total_length
    0,                          //cur_pos
    AUDIO_ALLONCE,            //unsigned long RepeatMode;  cycle mode
    AUDIO_INTURN,               //unsigned long PlayOrder;   play mode
    FIND_FILE_RANGE_ALL,        //directory file or all file.
    //eq
    {
        EQ_NOR,
            
        #ifdef _MS_EQ_
        0, 
        #endif    
        
		#ifdef _BBE_EQ_    
        0,
        #endif
        
		#ifdef _RK_EQ_
        {EQ_0DB, EQ_0DB, EQ_0DB, EQ_0DB, EQ_0DB}
		#endif
    },//tEffect       UserEQ; 用户EQ
    
    0,                          //artist
    0,                          //title
    0,                          //album
};
#else
_ATTR_AUDIO_DATA_ EXT INT32      MusicNextFile;
_ATTR_AUDIO_DATA_ EXT track_info gRegAudioConfig;
#endif

/*
--------------------------------------------------------------------------------
  
   Functon Declaration 
  
--------------------------------------------------------------------------------
*/
void Audio_I2SInit(void);
void AudioHWInit(void);

BOOLEAN AudioIsPlaying(void);
BOOLEAN AudioIsPause(void);

void AudioSetVolume(void);
void AudioSetEQ(void);

UINT8 GetFileType(UINT8 *pBuffer, UINT8 *pStr);
void AudioCodecOverLay(void);

void AudioGetNextNum(UINT16 Direct);

BOOLEAN AudioGetNextMusic(int16 msg);

void AudioDmaIsrHandler(void);
void AudioStart(void);
void AudioDecoding(void);
void AudioPlay(void);
BOOLEAN AudioPause(void);
BOOLEAN AudioFFW(unsigned long StepLen);
BOOLEAN AudioFFD(unsigned long StepLen);
BOOLEAN AudioStop(UINT16 ReqType);
BOOLEAN AudioResume(void);
BOOLEAN AudioSetPosition(unsigned long Desti_Posi);

void AudioSetAB_A(void);
void AudioSetAB(void);
void AudioABStop(void);
BOOLEAN AudioDecodeProc(MSG_ID id,void * msg);
void MusicInit(void *pArg);
UINT32 MusicService(void);
void MusicDeInit(void);
void SetMusicUIState(bool State);
void AudioSysFindFile(UINT32 FindDirection,UINT16 FindRange,UINT16 FindOrder);
void AudioFileOpsInit(void);

#ifdef MP3_INCLUDE
extern void AcceleratorHWInit(void);
extern void dma1_handler();
extern void imdct36_handler();
extern void synth_handler(); 
extern volatile int synth_hw_busy;

#endif

/*
--------------------------------------------------------------------------------
  
  Description:  audio thread definition.
  
--------------------------------------------------------------------------------
*/
#ifdef _IN_AUDIO_CONTROL_
_ATTR_AUDIO_DATA_ EXT THREAD MusicThread = {

    NULL,
    NULL,
    
    MusicInit,
    MusicService,
    MusicDeInit,
    
    NULL                           
};
#else
_ATTR_AUDIO_DATA_ EXT THREAD MusicThread;
#endif

/*
********************************************************************************
*
*                         End of AudioControl.h
*
********************************************************************************
*/

#endif
