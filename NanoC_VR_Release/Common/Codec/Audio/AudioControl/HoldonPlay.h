/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��  HoldonPlay.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               Chenwei          2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#ifndef _HOLDON_PLAY_H_
#define _HOLDON_PLAY_H_

#undef  EXT
#ifdef  _IN_HOLDON_PLAY_
#define EXT
#else
#define EXT extern
#endif
#include "SysConfig.h"
#include "SysFindFile.h"


/*
********************************************************************************
*
*                        Struct Define 
*
********************************************************************************
*/
typedef struct _HOLDON_PLAY_INFO
{
    UINT32      HoldCurrentTime;                                // the time when save breakpoint.
    UINT16      CurrentFileNum;                                 // current file serial number.
    UINT8       HoldMusicGetSign;                               // saved breakpoint flag.1:have 0:no

#ifdef MEDIA_MODULE
    UINT32      HoldMusicFullInfoSectorAddr;
    UINT32      HoldMusicSortInfoSectorAddr;   
	UINT32      HoldMusicuiBaseSortId;
	UINT16      HoldMusicPlayType;
	UINT16      HoldMusicuiCurId[MAX_DIR_DEPTH]; 
	UINT16      HoldMusicucCurDeep; 
	UINT16      HoldMusicTotalFiles; 
#endif	
	
}HOLDON_PLAY_INFO;

  void MusicHoldOnInforSave(UINT32 CurrentTime, SYS_FILE_INFO *SysFileInfo);
 void HoldMusicStart(void);

/*
********************************************************************************
*
*                         End of HoldonPlay.h
*
********************************************************************************
*/
#endif

