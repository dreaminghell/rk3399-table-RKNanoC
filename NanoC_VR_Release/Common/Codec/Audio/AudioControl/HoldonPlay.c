/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  HoldonPlay.C
*
* Description:
*
* History:      <author>          <time>        <version>
*               Chenwei          2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#define _IN_HOLDON_PLAY_

#include "SysInclude.h"
#include "FsInclude.h"
#include "Audio_globals.h"
#include "AudioControl.h"
#include "HoldonPlay.h"

#ifdef MEDIA_MODULE
#include "MediaBroWin.h"
#include "MediaBroWin.h"
#include "FileInfoSaveMacro.h"
#include "AddrSaveMacro.h"
#endif

#ifdef _MUSIC_

#ifdef AUDIOHOLDONPLAY

extern track_info*          pAudioRegKey;

/*
--------------------------------------------------------------------------------
  Function name : void HoldMusicStart(void)
  Author        : zs
  Description   : start breakpoint,get time and global file number.
  Input         : null
  Return        : null
  History       :  <author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void HoldMusicStart(void)
{
    HoldOnPlayInfo.CurrentFileNum  = gSysConfig.MusicConfig.FileNum ; //solve the bug that it can not save the breakpoint when shutdown.
    HoldOnPlayInfo.HoldCurrentTime = gSysConfig.MusicConfig.CurTime ;

    AudioFileInfo.CurrentFileNum   = HoldOnPlayInfo.CurrentFileNum;
    pAudioRegKey->CurrentTime      = HoldOnPlayInfo.HoldCurrentTime;
    PrevLrcPlayTimeSec             = gSysConfig.MusicConfig.LrcPreTime;

    #ifdef MEDIA_MODULE
    // AudioFileInfo.ucSelPlayType=HoldOnPlayInfo.HoldMusicPlayType           ;
    if (AudioFileInfo.ucSelPlayType != SORT_TYPE_SEL_BROWSER)//enter the audio play from media.
    {
        UINT16 temp1;
        UINT8 ucBufTemp[2];
        UINT8 ucPathBuffer[110];
        {
            AudioFileInfo.ulFullInfoSectorAddr=HoldOnPlayInfo.HoldMusicFullInfoSectorAddr ;
            AudioFileInfo.ulSortInfoSectorAddr=HoldOnPlayInfo.HoldMusicSortInfoSectorAddr ;
           
            AudioFileInfo.uiBaseSortId=HoldOnPlayInfo.HoldMusicuiBaseSortId       ;
            AudioFileInfo.ucCurDeep=HoldOnPlayInfo.HoldMusicucCurDeep          ;
	        AudioFileInfo.TotalFiles=HoldOnPlayInfo.HoldMusicTotalFiles  	;
	        for(temp1=0;temp1<=HoldOnPlayInfo.HoldMusicucCurDeep;temp1++)
            {
                AudioFileInfo.uiCurId[temp1] =  HoldOnPlayInfo.HoldMusicuiCurId[temp1];
            }
        }
        if (AudioFileInfo.ucSelPlayType != MUSIC_TYPE_SEL_MYFAVORITE)
        {
            MDReadData(DataDiskID, (AudioFileInfo.ulSortInfoSectorAddr<<9)+(AudioFileInfo.uiCurId[AudioFileInfo.ucCurDeep]+AudioFileInfo.uiBaseSortId)*2, 2, ucBufTemp);
            temp1 = (UINT16)ucBufTemp[0]+(((UINT16)ucBufTemp[1])<<8);

            MDReadData(DataDiskID, (AudioFileInfo.ulFullInfoSectorAddr<<9)+(UINT32)(temp1)*512+(UINT32)PATH_SAVE_ADDR_OFFSET, (PATH_SIZE+11), ucPathBuffer);
        }
        else 
        {
            AudioFileInfo.TotalFiles = gSysConfig.MedialibPara.gMyFavoriteFileNum;
            MDReadData(DataDiskID, (((UINT32)gFavoBlockSectorAddr)<<9)+MEDIAINFO_PAGE_SIZE+(UINT32)(AudioFileInfo.uiCurId[AudioFileInfo.ucCurDeep])*FAVORITE_MUSIC_SAVE_SIZE, (PATH_SIZE+11), ucPathBuffer);
		}
		
        //get breakpoint file path.
        #ifdef _RECORD_
		if(AudioFileInfo.ucSelPlayType ==  MUSIC_TYPE_SEL_RECORDFILE)
        {
            // FIND_DATA FindDataPlay;
            //GetPathInfo(RecordPathString); 
            //note: RecordPathString is a variable define in mediabrowin,
            //for reducing the number of global variable.here replace with string.
            //it need to change for keeping consisten with RecordPathString definiation when has modify.
            GetPathInfo("U:\\VOICE");
            GetDirPath(AudioFileInfo.Path);
            AudioFileInfo.FindData.Clus=CurDirClus;
            AudioFileInfo.FindData.Index=0;
            for(temp1=0; temp1<AudioFileInfo.CurrentFileNum; temp1++)
            {
               FindNext(&AudioFileInfo.Fdt, &(AudioFileInfo.FindData), AudioFileExtString);
            }            
        }
        else
		#endif
		{
    	    FindFileByShortPath(&AudioFileInfo.Fdt,  &ucPathBuffer[0], &ucPathBuffer[PATH_SIZE]);
    	    memcpy(AudioFileInfo.Path , ucPathBuffer ,PATH_SIZE);
        }
        memcpy(MusicLongFileName,&LongFileName,MAX_FILENAME_LEN*2);
    }
    #endif
    
    if((pAudioRegKey->RepeatMode == AUDIO_FOLDER_ONCE)||(pAudioRegKey->RepeatMode == AUIDO_FOLDER_REPEAT))
    {
         pAudioRegKey->AudioFileDirOrAll =  FIND_FILE_RANGE_DIR;//pAudioRegKey->RepeatMode;//by zs 06.01 解决目录一次的问题
    }
    
    #ifdef MEDIA_MODULE
    if(AudioFileInfo.ucSelPlayType !=  SORT_TYPE_SEL_BROWSER)
    {   
        SendMsg(MSG_MEDIA_BREAKPOINT_PLAY);
    }
    else
    #endif
    {
        SysFindFileInit(&AudioFileInfo, AudioFileInfo.CurrentFileNum,AudioFileInfo.Range,pAudioRegKey->PlayOrder,(UINT8*)AudioFileExtString);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void MusicHoldOnInforSave(UINT CurrentTime, SYS_FILE_INFO *SysFileInfo)
  Author        : zs
  Description   : save breakpoint information.
  Input         : CurrentTime               :breakpoint time.
                  SYS_FILE_INFO *SysFileInfo:audio structure.
  Return        : null
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void MusicHoldOnPlay(void)
{
    #ifdef MEDIA_MODULE     //add azg 7.12 
    if(((HoldOnPlayInfo.HoldMusicGetSign == 1)||(gSysConfig.MusicConfig.HoldOnPlaySaveFlag == 1))&&(HoldOnPlayInfo.CurrentFileNum ==gbAudioFileNum )&&(AudioFileInfo.ucSelPlayType==HoldOnPlayInfo.HoldMusicPlayType))
    #else
    if(((HoldOnPlayInfo.HoldMusicGetSign == 1)||(gSysConfig.MusicConfig.HoldOnPlaySaveFlag == 1))&&(HoldOnPlayInfo.CurrentFileNum ==gbAudioFileNum ))
    #endif
    {
        if (0 != HoldOnPlayInfo.HoldCurrentTime)
        {
            CodecSeek(HoldOnPlayInfo.HoldCurrentTime, 0);
        }
        HoldOnPlayInfo.HoldMusicGetSign = 0;
        gSysConfig.MusicConfig.HoldOnPlaySaveFlag = 0;
    }
}


/*
--------------------------------------------------------------------------------
  Function name : void MusicHoldOnInforSave(UINT CurrentTime, SYS_FILE_INFO *SysFileInfo)
  Author        : zs
  Description   : save breakpoint information.
  Input         : CurrentTime               :breakpoint time.
                  SYS_FILE_INFO *SysFileInfo:audio structure.
  Return        : null
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
_ATTR_AUDIOFILE_TEXT_
void MusicHoldOnInforSave(UINT32 CurrentTime, SYS_FILE_INFO *SysFileInfo)
{
    HoldOnPlayInfo.HoldMusicGetSign = 1;

    #ifdef MEDIA_MODULE
    gSysConfig.MusicConfig.HoldMusicPlayType           = AudioFileInfo.ucSelPlayType;//debug cannot save audio breakpoint when play music from explorer

    HoldOnPlayInfo.HoldMusicPlayType           = AudioFileInfo.ucSelPlayType; 
    if(AudioFileInfo.ucSelPlayType !=  SORT_TYPE_SEL_BROWSER)
    {
        UINT16 i;
        HoldOnPlayInfo.HoldMusicFullInfoSectorAddr = AudioFileInfo.ulFullInfoSectorAddr;
        HoldOnPlayInfo.HoldMusicSortInfoSectorAddr = AudioFileInfo.ulSortInfoSectorAddr;
        
        HoldOnPlayInfo.HoldMusicuiBaseSortId       = AudioFileInfo.uiBaseSortId;
        HoldOnPlayInfo.HoldMusicucCurDeep          = AudioFileInfo.ucCurDeep;
        for(i=0;i<=HoldOnPlayInfo.HoldMusicucCurDeep;i++)
        {
            HoldOnPlayInfo.HoldMusicuiCurId[i] = AudioFileInfo.uiCurId[i];
        }
        HoldOnPlayInfo.HoldMusicTotalFiles  	= AudioFileInfo.TotalFiles;

        gSysConfig.MusicConfig.HoldMusicFullInfoSectorAddr = AudioFileInfo.ulFullInfoSectorAddr;
        gSysConfig.MusicConfig.HoldMusicSortInfoSectorAddr = AudioFileInfo.ulSortInfoSectorAddr;
        
        gSysConfig.MusicConfig.HoldMusicuiBaseSortId       = AudioFileInfo.uiBaseSortId;
        gSysConfig.MusicConfig.HoldMusicucCurDeep          = AudioFileInfo.ucCurDeep;
        for(i=0;i<=HoldOnPlayInfo.HoldMusicucCurDeep;i++)
            gSysConfig.MusicConfig.HoldMusicuiCurId[i]     = AudioFileInfo.uiCurId[i];
	    gSysConfig.MusicConfig.HoldMusicuiTitle		=AudioFileInfo.TotalFiles;
    }
    else
    #endif
    {
        //SysFileInfo->CurrentFileNum = GetCurFileNum(SysFileInfo->CurrentFileNum,SysFileInfo->pExtStr);
        if(SysFileInfo->Range == FIND_FILE_RANGE_DIR)//cycle among directory.
            SysFileInfo->CurrentFileNum = GetGlobeFileNum(SysFileInfo->CurrentFileNum ,SysFileInfo->Path, SysFileInfo->pExtStr);
    }
    HoldOnPlayInfo.CurrentFileNum = SysFileInfo->CurrentFileNum;
    HoldOnPlayInfo.HoldCurrentTime = CurrentTime;
    gSysConfig.MusicConfig.LrcPreTime = PrevLrcPlayTimeSec;
    gSysConfig.MusicConfig.FileNum = HoldOnPlayInfo.CurrentFileNum;//solve the bug:it cannot save the breakpoint when shutdown.
    gSysConfig.MusicConfig.CurTime = HoldOnPlayInfo.HoldCurrentTime;
	
   	#if (2 == FW_IN_DEV)//dgl add
	if (!CheckCard())//no hold if card reject!!!!
	{
	    gSysConfig.MusicConfig.FileNum = 0;
	    gSysConfig.MusicConfig.CurTime = 0;
	    gSysConfig.MusicConfig.LrcPreTime = 0;
	}
	#endif
}

#endif //AUDIOHOLDONPLAY

#endif //_MUSIC_
/*
********************************************************************************
*
*                         End of HoldonPlay.c
*
********************************************************************************
*/

