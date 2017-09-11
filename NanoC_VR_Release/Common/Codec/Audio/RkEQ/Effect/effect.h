/******************************************************************************
*
*  Copyright (C),2007, Fuzhou Rockchip Co.,Ltd.
*
*  File name :     effect.h
*  Description:   audio effect process.
*  Remark:
*
*  History:
*           <author>      <time>     <version>       <desc>
*           Huweiguo     07/09/27      1.0
*
*******************************************************************************/

#ifndef __EFFECT_H__
#define __EFFECT_H__


#define CUSTOMEQ_LEVELNUM        7
#define CUSTOMEQ_BANDNUM         5

/* EQ mode */
typedef enum
{
    //no audio effect.
    EQ_NOR,
        
    //RK audio effect.
    #ifdef _RK_EQ_
    EQ_ROCK,
    EQ_POP,
    EQ_CLASSIC,
    EQ_BASS,
    EQ_JAZZ,
    EQ_USER,
    #endif

    #ifdef _MS_EQ_
    EQ_PLAYFX,
    #endif
    
    #ifdef _BBE_EQ_
	EQ_BBE,
    #endif

    //if add new audio effect mode,add in here,do not change the order of upside.

} eEQMode;


/*parameter structure of RK audio effect.*/
typedef __packed struct
{
    short    dbGain[CUSTOMEQ_BANDNUM]; //5 band EQµÄÔöÒæ
} tRKEQCoef;


typedef __packed struct
{
    eEQMode Mode;      // EQ mode
    
    #ifdef _MS_EQ_
    long    MSMode;    // PlayFx mode
    #endif
    
    #ifdef _BBE_EQ_
    long      BBEMode;	//for BBE
    #endif

    #ifdef _RK_EQ_
    tRKEQCoef  RKCoef;
    #endif

    //tPFEQCoef  PFCoef;
} RKEffect;


/* API interface function. */

long EffectInit(void);       // initialization.
long EffectEnd(void);      //handle over.
long EffectProcess(short *pBuffer, long PcmLen);   //audio effect process function,call it every frame.
long EffectAdjust(void);      //this function is for adjust audio effect.
long RKEQAdjust(RKEffect *pEft, int SamplingRate);

#endif

