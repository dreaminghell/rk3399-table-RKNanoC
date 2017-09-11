/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º  MusicWin.c
*
* Description:
*
* History:      <author>          <time>        <version>
*               zhangshuai      2009-02-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_MUSICWIN_

#include "SysInclude.h"

#ifdef _MUSIC_

#include "MusicWinInterface.h"
#include "MusicLrc.h"
#include "MusicWin.h"
#include "MainMenu.h"
#include "MessageBox.h"
#include "Hold.h"

#ifdef MEDIA_MODULE
#include "MediaBroWin.h"
#endif

extern INT32 MusicNextFile ;
extern BOOLEAN FileNumGlobalOrFolder;

#ifdef _RK_ID3_
extern FILE_SAVE_STRUCT AudioID3Info;
extern ID3_AudioAlbumArt gID3AudioAlbumArt;

#endif
/*
--------------------------------------------------------------------------------
  Function name : void MusicWinIntInit(void)
  Author        : ZHengYongzhi
  Description   : main menu interrupt initial,register the interrupt functions that main menu will use,

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_INIT_CODE_
void MusicWinIntInit(void)
{

}

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinIntDeInit(void)
  Author        : ZHengYongzhi
  Description   : main menu interrupt auti-initial,unload the interrupt functions

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_DEINIT_CODE_
void MusicWinIntDeInit(void)
{

}

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinMsgInit(void)
  Author        : ZHengYongzhi
  Description   : main menu message initial£¬initial the message parameters that main menu will use

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_INIT_CODE_
void MusicWinMsgInit(void *pArg)
{
    //MusicWin MsgInit
    MediaTitleAdd = ((MUSIC_WIN_ARG*)pArg)->MediaTitleAdd;
    SendMsg(MSG_MUSIC_DISPLAY_ALL);
    ClearMsg(MSG_AUDIO_CARD_CHECK);
    ClearMsg(MSG_AUDIO_FILE_ERROR);
}

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinMsgDeInit(void)
  Author        : ZHengYongzhi
  Description   : main menu message auti-initial

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_DEINIT_CODE_
void MusicWinMsgDeInit(void)
{
    ClearMsg(MSG_MEDIA_BREAKPOINT_PLAY);
}

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinInit(void)
  Author        : ZHengYongzhi
  Description   : main menu message initial function

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_CODE_
void MusicWinInit(void *pArg)
{
    //DEBUG("MusicWin Enter");

    MusicDispType = MUSIC_DISPLAY_NOR;

    KeyReset();
    MusicWinMsgInit(pArg);
    MusicWinIntInit();

    MusicWinSvcStart(pArg);

    if((AUDIO_FOLDER_ONCE == pAudioRegKey->RepeatMode)||(AUIDO_FOLDER_REPEAT == pAudioRegKey->RepeatMode))
        FileNumGlobalOrFolder = 0;

    SetMusicUIState(0) 	;//2009-5-20 zs
}

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinDeInit(void)
  Author        : ZHengYongzhi
  Description   :  main menu message auti-initial function

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_CODE_
void MusicWinDeInit(void)
{
    #ifdef _MUSIC_LRC_
    if (MusicDispType == MUSIC_DISPLAY_LRC)
    {
        MusicLrcDeInit();
    }
    #endif

    MusicWinSvcStop();
    MusicWinIntDeInit();
    MusicWinMsgDeInit();

    //DEBUG("MusicWin Exit");
}

//#ifdef DISPLAY_ENABLE
#if 0

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinScrollFileNameInit(void)
  Author        : ZHengYongzhi
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_SERVICE_CODE_
void MusicWinScrollInit(LCD_RECT *pRect, uint16 ImageID, uint16 *pstr, uint16 Speed)
{
    PicturePartInfo     PicPartInfo;

    PicPartInfo.x       = pRect->x0;
    PicPartInfo.y       = pRect->y0;
    PicPartInfo.yoffset = 0;
    PicPartInfo.ysize   = CH_CHAR_XSIZE_12;

    PicPartInfo.pictureIDNump = ImageID;
    SetScrollStringInfo(pRect, PicPartInfo, pstr, Speed);
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : UINT32 MusicWinService(void)
  Author        : ZHengYongzhi
  Description   :  main menu message service function

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_SERVICE_CODE_
UINT32 MusicWinService(void)
{
    UINT32 Retval = 0;
    TASK_ARG TaskArg;

//-----------------------------------------------------------------------------
//message of backgroud dialog
    if(TRUE == GetMsg(MSG_AUDIO_NOFILE))
    {
        #if 0
        ThreadDelete(&pMainThread, &MusicThread);

        MusicWinDialogType = MUSICWIN_DIALOG_NOFILE;
        TaskArg.Message.TitleID   = SID_WARNING;
        TaskArg.Message.ContentID = SID_NO_MUSIC_FILE;
        TaskArg.Message.HoldTime  = 3;
        WinCreat(&MusicWin, &MessageBoxWin, &TaskArg);
        #endif
    }

    if(TRUE == GetMsg(MSG_AUDIO_CARD_CHECK))
    {
        ThreadDelete(&pMainThread, &MusicThread);

        TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        Retval = 1;
    }

    if(TRUE == GetMsg(MSG_AUDIO_FILE_ERROR))// by zs 06.09
    {
        #ifndef FILE_ERROR_DIALOG
        MusicWinDialogType = MUSICWIN_DIALOG_FORMAT_ERROR;
        SendMsg(MSG_MESSAGEBOX_DESTROY);
        #else
        MusicWinDialogType = MUSICWIN_DIALOG_FORMAT_ERROR;
        TaskArg.Message.TitleID   = SID_WARNING;
        TaskArg.Message.ContentID = SID_FILE_FORMAT_ERROR;
        TaskArg.Message.HoldTime  = 3;
        WinCreat(&MusicWin, &MessageBoxWin, &TaskArg);
        #endif
    }

    if(TRUE == GetMsg(MSG_MESSAGEBOX_DESTROY))
    {
        if(MUSICWIN_DIALOG_FORMAT_ERROR == MusicWinDialogType)
        {
            AudioErrorFileCount++;
            if ((AudioFileInfo.TotalFiles <= AudioErrorFileCount)
                || (AUDIO_ONCE == pAudioRegKey->RepeatMode)
                ||(AUDIO_REPEAT == pAudioRegKey->RepeatMode)
                ||(((AUDIO_ALLONCE == pAudioRegKey->RepeatMode)||(AUDIO_TRY == pAudioRegKey->RepeatMode))&&(AudioPlayFileNum >= AudioFileInfo.TotalFiles)))
            {
                ThreadDelete(&pMainThread, &MusicThread);

                #ifdef MEDIA_MODULE
                if (AudioFileInfo.ucSelPlayType!= SORT_TYPE_SEL_BROWSER)
                {
                    gMusicTypeSelID= AudioFileInfo.ucSelPlayType;
                    TaskArg.MediaBro.CurId= 1;
					TaskArg.MediaBro.TitleAdd = MediaTitleAdd;
                    TaskSwitch(TASK_ID_MEDIABRO, &TaskArg);
                }
                else//the playing musi is from brower,so return to brower
                {

                }
                #else
                {

                }
                #endif

                #ifdef AUDIOHOLDONPLAY
                HoldOnPlayInfo.HoldMusicGetSign = 0 ;
                gSysConfig.MusicConfig.HoldOnPlaySaveFlag = 0;
                #endif

                Retval = 1;
            }
            else
            {
                AudioDecodeProc(MSG_AUDIO_STOP,(void*)Audio_Stop_Normal);
                SendMsg(MSG_MUSIC_DISPLAY_ALL);
            }
        }
        else
        {
            ThreadDelete(&pMainThread, &MusicThread);
            TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
            TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
            Retval = 1;
        }
    }

//-----------------------------------------------------------------------------
//backgroud handle message
    if(TRUE == GetMsg(MSG_AUDIO_NEXTFILE))
    {
        #ifdef _MUSIC_LRC_
        if (MusicDispType == MUSIC_DISPLAY_LRC)
        {
            MusicLrcDeInit();
        }
        #endif
    }

    if(TRUE == GetMsg(MSG_AUDIO_INIT_SUCESS))
    {
       SendMsg(MSG_MUSIC_DISPLAY_UPDATE);
    }

    if (TRUE == GetMsg(MSG_AUDIO_EXIT))
    {
        ThreadDelete(&pMainThread, &MusicThread);

        TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        Retval = 1;
    }

//-----------------------------------------------------------------------------
//UI display message
    if(MusicPlayState == AUDIO_STATE_PLAY )
    {
        SendMsg(MSG_MUSIC_DISPFLAG_SCHED);
        SendMsg(MSG_MUSIC_DISPFLAG_CURRENT_TIME);
    }

    return (Retval);
}

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinPaint(void)
  Author        : Chenwei
  Description   : main menu display program

  Input         :
  Return        :

  History:     <author>         <time>         <version>
               Chenwei        2009/03/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_SERVICE_CODE_
void MusicWinPaint(void)
{
//#ifdef DISPLAY_ENABLE
#if 0

    UINT8                   j, k,l;
    UINT16                  i, TempColor, TempBkColor, TempCharSize,TempTxtMode;
    UINT16 		            y, y1;
    UINT16                  *p2Unicode;
    UINT32                  CurrentTimeSec, TotalTimeSec;
    LCD_RECT                r;
    PICTURE_INFO_STRUCT     PicInfo;
    PicturePartInfo         PicPartInfo;
    char 					SpectrumOutTemp[SPECTRUM_LINE_M];//the size of Buffer is defined by real situation

    TempColor    = LCD_GetColor();
    TempBkColor  = LCD_GetBkColor();
    TempTxtMode  = LCD_SetTextMode(LCD_DRAWMODE_REV);
    TempCharSize = LCD_SetCharSize(FONT_12x12);
    LCD_SetColor(COLOR_BLACK);
    LCD_SetBkColor(COLOR_WHITE);

    if (CheckMsg(MSG_NEED_PAINT_ALL) || (GetMsg(MSG_MUSIC_DISPLAY_ALL)))
    {
        //display backgroud picture,battery and hold icon
        DispPictureWithIDNum(IMG_ID_MUSIC_BACKGROUND);
        DispPictureWithIDNum(IMG_ID_MUSIC_HOLDOFF + MusicHoldState);
        DispPictureWithIDNum(IMG_ID_MUSIC_BATTERYLEVEL0 + MusicBatteryLevel);

        //the other diaplay element message display in here,
        SendMsg(MSG_MUSIC_DISPFLAG_REPEATMODE);
        SendMsg(MSG_MUSIC_DISPFLAG_ORDER);
        SendMsg(MSG_MUSIC_DISPFLAG_EQ);
        SendMsg(MSG_MUSIC_DISPFLAG_VOL);
        SendMsg(MSG_MUSIC_DISPLAY_UPDATE);
    }

    if (GetMsg(MSG_MUSIC_DISPLAY_UPDATE))           //refresh icon when switch song
    {
        SendMsg(MSG_MUSIC_DISPFLAG_STATUS);
        SendMsg(MSG_MUSIC_DISPFLAG_AUDIOTYPE);
        SendMsg(MSG_MUSIC_DISPFLAG_BITRATE);
        SendMsg(MSG_MUSIC_DISPFLAG_SCHED);
        SendMsg(MSG_MUSIC_DISPFLAG_TOTAL_TIME);
        //SendMsg(MSG_MUSIC_DISPFLAG_SPECTRUM);
        SendMsg(MUSIC_UPDATESPECTRUM);
        SendMsg(MSG_MUSIC_DISPFLAG_FILENAME);
        SendMsg(MSG_MUSIC_DISPFLAG_LY);
        ClearMsg(MSG_MUSIC_DISPFLAG_LRC_SCROLL);
        ClearMsg(MSG_MUSIC_DISPFLAG_SCROLL_FILENAME);

        #ifdef _RK_ID3_JPEG_
        if(gID3AudioAlbumArt.AlbumArtFlag == 1)
        {
          //  LCD_DrawBmp(0,0, gID3AudioAlbumArt.AlbumArtWidth-1,gID3AudioAlbumArt.AlbumArtHigh-1,16,(UINT16 *)gID3AudioAlbumArt.AlbumArtBuff);
        }
        #endif

    }

    if(TRUE == GetMsg(MSG_MUSIC_DISPFLAG_LY))           //LY icon,if interface display type is changing,it need to initial different message,
    {
        if(RETURN_OK == MusicLrcCheck(MusicLongFileName))
        {

            MusicLrcInit();

            MusicBgFlag = 1;
            MusicDispType = MUSIC_DISPLAY_LRC;
            SendMsg(MSG_MUSIC_DISPFLAG_LRC);
        }
        else
        {
            MusicDispType = MUSIC_DISPLAY_NOR;
            if(MusicBgFlag == 1)
            {
                DispPictureWithIDNum(IMG_ID_MUSIC_BACKGROUND);
                DispPictureWithIDNum(IMG_ID_MUSIC_HOLDOFF + MusicHoldState);

                SendMsg(MSG_MUSIC_DISPFLAG_REPEATMODE);
                SendMsg(MSG_MUSIC_DISPFLAG_ORDER);
                SendMsg(MSG_MUSIC_DISPFLAG_EQ);
                SendMsg(MSG_MUSIC_DISPFLAG_VOL);
                SendMsg(MSG_MUSIC_DISPFLAG_STATUS);
                SendMsg(MSG_MUSIC_DISPFLAG_AUDIOTYPE);
                MusicBgFlag = 0;
            }
            SendMsg(MSG_MUSIC_DISPFLAG_AB);
            SendMsg(MSG_MUSIC_DISPFLAG_FILENUM);
            SendMsg(MSG_MUSIC_DISPFLAG_CURRENT_TIME);
        }
        DispPictureWithIDNum(IMG_ID_MUSIC_LY0 + MusicDispType);
    }

    if (MusicDispType != MUSIC_DISPLAY_LRC)
    {
        if(TRUE == GetMsg(MSG_BATTERY_UPDATE))
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_BATTERYLEVEL0 + MusicBatteryLevel);
        }

        if ( TRUE == GetMsg(MSG_MUSIC_DISPFLAG_AB))
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_AB_OFF + MusicABState);
        }

        if ( TRUE == GetMsg(MSG_MUSIC_DISPFLAG_REPEATMODE))
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_LOOPMODE_ONCE + MusicRepeatMode);
        }

        if (TRUE == GetMsg(MSG_MUSIC_DISPFLAG_ORDER))
        {

        }

        if (TRUE==GetMsg(MSG_MUSIC_DISPFLAG_CURRENT_TIME))// current play time
        {
            CodecGetTime(&CurrentTimeSec);
            CurrentTimeSec = CurrentTimeSec / 1000;// turn ms to s

            if (CurrentTimeSec != CurrentTimeSecBk)
            {
                CurrentTimeSecBk = CurrentTimeSec;
                GetTimeHMS(CurrentTimeSec,&i,&j,&k);

                GetPictureInfoWithIDNum(IMG_ID_MUSIC_CURTIMENUM0, &PicInfo);
                DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_CURTIMENUM0 + i % 10), PicInfo.xSize * 0, 0);    //hour
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_CURTIMENUMCOLON,        PicInfo.xSize * 1, 0);    //sparator
                DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_CURTIMENUM0 + j / 10), PicInfo.xSize * 2, 0);    //minute
                DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_CURTIMENUM0 + j % 10), PicInfo.xSize * 3, 0);
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_CURTIMENUMCOLON,        PicInfo.xSize * 4, 0);    //sparator
                DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_CURTIMENUM0 + k / 10), PicInfo.xSize * 5, 0);    //second
                DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_CURTIMENUM0 + k % 10), PicInfo.xSize * 6, 0);
            }
        }

        if (TRUE==GetMsg(MSG_MUSIC_DISPFLAG_FILENUM))
        {
            GetPictureInfoWithIDNum(IMG_ID_MUSIC_FILENUM0, &PicInfo);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_FILENUM0 + MusicCurrFileNum / 1000),       PicInfo.xSize * 0, 0);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_FILENUM0 + MusicCurrFileNum % 1000 / 100), PicInfo.xSize * 1, 0);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_FILENUM0 + MusicCurrFileNum % 100 / 10),   PicInfo.xSize * 2, 0);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_FILENUM0 + MusicCurrFileNum % 100 % 10),   PicInfo.xSize * 3, 0);
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_FILENUMPLOT,                                PicInfo.xSize * 4, 0);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_FILENUM0 + MusicTotalFileNum / 1000),      PicInfo.xSize * 5, 0);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_FILENUM0 + MusicTotalFileNum % 1000 / 100),PicInfo.xSize * 6, 0);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_FILENUM0 + MusicTotalFileNum % 100 / 10),  PicInfo.xSize * 7, 0);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_FILENUM0 + MusicTotalFileNum % 100 % 10),  PicInfo.xSize * 8, 0);
        }
    }
    else
    {
        MusicLrcPlay(0);
        if (TRUE == GetMsg(MSG_MUSIC_DISPFLAG_LRC))
        {
            r.x0 = 4;
            r.y0 = 6;
            r.x1 = LCD_WIDTH - 4;
            r.y1 = 60;
            DispPictureWithIDNum(IMG_ID_MUSIC_LRCBACKGROUND);
    		LCD_DispStringInRect(&r, &r, UnicodeLrcBuffer, LCD_TEXTALIGN_CENTER);
        }
    }

    if (TRUE == GetMsg(MSG_MUSIC_DISPFLAG_AUDIOTYPE))
    {
        DispPictureWithIDNum(IMG_ID_MUSIC_FILETYPE_MP3 + CurrentCodecType);
    }

    if (TRUE == GetMsg(MSG_MUSIC_DISPFLAG_STATUS))
    {
        if(AUDIO_STATE_PLAY == MusicPlayState)
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_STATUS_PLAY);
        }
        else
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_STATUS_PAUSE);
        }
    }

    if (TRUE == GetMsg(MSG_MUSIC_DISPFLAG_EQ))
    {
        DispPictureWithIDNum(IMG_ID_MUSIC_EQ_NOR + MusicEQType);
    }

    if(TRUE == GetMsg(MSG_MUSIC_DISPFLAG_BITRATE))
    {
        DispPictureWithIDNum(IMG_ID_MUSIC_BPSICON);

        i = (MusicBitrate /1000) /1000;
        j = (MusicBitrate /1000) % 1000 / 100;
        k = (MusicBitrate /1000) % 100 / 10;
        l = (MusicBitrate /1000) % 10;

        GetPictureInfoWithIDNum(IMG_ID_MUSIC_BPSNUM0, &PicInfo);
        if(0 == i)
        {
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSBACKGROND, PicInfo.xSize * 0, 0);
        }
        else
        {
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSNUM0 + i, PicInfo.xSize * 0, 0);
        }
        if((0 == i) && (0 == j))
        {
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSBACKGROND, PicInfo.xSize * 1, 0);
        }
        else
        {
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSNUM0 + j, PicInfo.xSize * 1, 0);
        }
        if((0 == i) && (0 == j) && (0 == k))
        {
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSBACKGROND, PicInfo.xSize * 2, 0);
        }
        else
        {
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSNUM0 + k, PicInfo.xSize * 2, 0);
        }
        if((0 == i) && (0 == j) && (0 == k) && (0 == l))
        {
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSBACKGROND, PicInfo.xSize * 3, 0);
        }
        else
        {
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSNUM0 + l, PicInfo.xSize * 3, 0);
        }
    }

    if ((TRUE==GetMsg(MSG_MUSIC_DISPFLAG_VOL)) || (GetMsg(MSG_KEY_VOL_STATE_UPDATE)))
    {

        if (CheckMsg(MSG_KEY_VOL_SHOWSTATE))
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_VOLBACKGROUND_SEL);
        }
        else
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_VOLBACKGROUND);
        }

        GetPictureInfoWithIDNum(IMG_ID_MUSIC_VOLNUM0, &PicInfo);
        DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_VOLNUM0 + MusicWinVol / 10, (PicInfo.xSize * 0), 0);
        DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_VOLNUM0 + MusicWinVol % 10, (PicInfo.xSize * 1), 0);
    }

    if(TRUE==GetMsg(MSG_MUSIC_DISPFLAG_SCHED))//progress
    {
        CurrentTimeSec = MusicCurrentTime * (MUSIC_TIME_GUAGE_CONST - 1) / MusicTotalTime;
        if (MusicTimeGuageVal != CurrentTimeSec)
        {
            MusicTimeGuageVal = CurrentTimeSec;
            if(MusicTimeGuageVal <= MUSIC_TIME_GUAGE_CONST-1)  //2009.9.5 Modify By WangBo
            {
                DispPictureWithIDNum(IMG_ID_MUSIC_SCHEDU0 + MusicTimeGuageVal);//20090518 cw
            }
            else
            {
                DispPictureWithIDNum(IMG_ID_MUSIC_SCHEDU46);//20090518 cw
            }
        }
    }

    if(TRUE==GetMsg(MSG_MUSIC_DISPFLAG_TOTAL_TIME))
    {
        CurrentTimeSec = MusicTotalTime;
        CurrentTimeSec = CurrentTimeSec / 1000;//turn ms to s,
        GetTimeHMS(CurrentTimeSec,&i,&j,&k);

        GetPictureInfoWithIDNum(IMG_ID_MUSIC_TOTLETIMENUM0, &PicInfo);
        DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + i % 10), PicInfo.xSize * 0, 0);    //hour
        DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_TOTLETIMENUMCOLON,        PicInfo.xSize * 1, 0);    //sperator
        DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + j / 10), PicInfo.xSize * 2, 0);    //minute
        DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + j % 10), PicInfo.xSize * 3, 0);
        DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_TOTLETIMENUMCOLON,        PicInfo.xSize * 4, 0);    //sperator
        DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + k / 10), PicInfo.xSize * 5, 0);    //second
        DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + k % 10), PicInfo.xSize * 6, 0);
    }

    if(TRUE == GetMsg(MUSIC_UPDATESPECTRUM))
    {
        #ifdef _RK_SPECTRUM_
        CodecGetTime(&CurrentTimeSec);
        CurrentTimeSec = CurrentTimeSec / 200;    //refresh spectrum every 200ms

        if (CurrentTimeSec != CurrentTime200MsBk)
        {
            CurrentTime200MsBk = CurrentTimeSec;

            DispPictureWithIDNum(IMG_ID_MUSIC_SPECBACKGROUND);//draw sepectrum basic.

            GetPictureInfoWithIDNum(IMG_ID_MUSIC_SPEC, &PicInfo);
            //do insert value in here
            for (i = 0; i < SPECTRUM_LINE_M ; i++)
            {
                SpectrumOutTemp[i] = SpectrumOut[i];
                #if 0
                SpectrumOutTemp[2 * i + 0] = SpectrumOut[i];
                if (i < SPECTRUM_LINE_M)
                {
                    SpectrumOutTemp[2 * i + 1] = (SpectrumOut[i] + SpectrumOut[i + 1]) / 2;
                }
                else
                {
                    SpectrumOutTemp[2 * i + 1] = (SpectrumOut[i] + 0) / 2;
                }
                #endif
            }

            for (i = 0;i < 9;i++)
            {
                y1 = (SpectrumOutTemp[i] >> 1);//real pectrum height.
    			if (y1 > 20)
    			{
    				y1 = 20;
    			}
    			y = 20 - y1;

     		    DisplayPicture_part(IMG_ID_MUSIC_SPEC, PicInfo.x + i * (PicInfo.xSize + 2)+3, PicInfo.y, y, y1);
            }
        }
        #endif
    }

    if (TRUE == GetMsg(MSG_MUSIC_DISPFLAG_FILENAME))
    {
        GetPictureInfoWithIDNum(IMG_ID_MUSIC_FILENAMEBK, &PicInfo);
        r.x0 = PicInfo.x;
        r.y0 = PicInfo.y + (PicInfo.ySize - CH_CHAR_XSIZE_12) / 2;
        r.x1 = r.x0 + PicInfo.xSize;
        r.y1 = r.y0 + CH_CHAR_XSIZE_12 - 1 ;

        if(LCD_GetStringSize(MusicLongFileName) > MUSIC_SCROLL_WIDTH)
        {
            MusicWinScrollInit(&r, IMG_ID_MUSIC_FILENAMEBK, MusicLongFileName, 30);
            SendMsg(MSG_MUSIC_DISPFLAG_SCROLL_FILENAME);
        }
        else
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_FILENAMEBK);
            ClearMsg(MSG_MUSIC_DISPFLAG_SCROLL_FILENAME);
            LCD_DispStringInRect(&r, &r, MusicLongFileName, LCD_TEXTALIGN_LEFT);
        }
    }

    if (TRUE == CheckMsg(MSG_MUSIC_DISPFLAG_SCROLL_FILENAME))
    {
        ScrollStringForMusic(MusicLongFileName);
    }

    LCD_SetTextMode(TempTxtMode);
    LCD_SetBkColor(TempBkColor);
    LCD_SetColor(TempColor);
    LCD_SetCharSize(TempCharSize);
#else

#endif
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 MusicWinKeyProc(void)
  Description   : audio key handle program.

  Input         : NULL
  Return        : TRUE\FALSE

  History:     <author>         <time>         <version>
             zhangshuai     2009/02/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MUSIC_SERVICE_CODE_
UINT32 MusicWinKeyProc(void)
{
    UINT32 MusicKeyVal;
    UINT32 RetVal = RETURN_OK;
    TASK_ARG TaskArg;

    MusicKeyVal =  GetKeyVal();

    //5 key function modification
    if (gSysConfig.KeyNum == KEY_NUM_5)
    {
        switch (MusicKeyVal)
        {
        case KEY_VAL_ESC_PRESS_START:           //ong press switch VOL function
            if (CheckMsg(MSG_KEY_VOL_STATE))
            {
                ClearVolMsg();
                SendMsg(MSG_MUSIC_DISPFLAG_VOL);
            }
            else
            {
                KeyVolAdjStart();
                SendMsg(MSG_KEY_VOL_SHOWSTATE);
                SendMsg(MSG_MUSIC_DISPFLAG_VOL);
            }
            break;

        case KEY_VAL_FFD_DOWN:
        case KEY_VAL_FFD_PRESS:
            if (CheckMsg(MSG_KEY_VOL_STATE))
            {
                MusicKeyVal = KEY_VAL_UP_DOWN;
            }
            break;

        case KEY_VAL_FFW_DOWN:
        case KEY_VAL_FFW_PRESS:
            if (CheckMsg(MSG_KEY_VOL_STATE))
            {
                MusicKeyVal = KEY_VAL_DOWN_DOWN;
            }
            break;

        case KEY_VAL_FFD_SHORT_UP:
        case KEY_VAL_FFD_LONG_UP:
        case KEY_VAL_FFW_SHORT_UP:
        case KEY_VAL_FFW_LONG_UP:
            if (CheckMsg(MSG_KEY_VOL_STATE))
            {
                MusicKeyVal = KEY_VAL_NONE;
            }
            break;

        default:
            break;
        }
    }

    //6 key function modification
    if (gSysConfig.KeyNum == KEY_NUM_6)
    {
        switch (MusicKeyVal)
        {
        case KEY_VAL_UP_PRESS_START:            //long press A-B function
            MusicKeyVal = KEY_VAL_ESC_SHORT_UP;
            break;

        case KEY_VAL_UP_PRESS:                  //long press function cancel
        case KEY_VAL_DOWN_PRESS:
            MusicKeyVal = KEY_VAL_NONE;
            break;

        default:
            break;
        }
    }

    //7 key function
    switch (MusicKeyVal)
    {
        case KEY_VAL_ESC_SHORT_UP:                                 // set AB repeat.
            if(AUDIO_STATE_PLAY == MusicPlayState)
            {
                if (AUDIO_AB_NULL == AudioPlayInfo.ABRequire)
                {
                    AudioDecodeProc(MSG_AUDIO_ABSETA,NULL);
                    if(MUSIC_DISPLAY_NOR == MusicDispType)
                    {
                        SendMsg(MSG_MUSIC_DISPFLAG_AB);
                    }
                }
                else if (AUDIO_AB_A == AudioPlayInfo.ABRequire)
                {
                    AudioDecodeProc(MSG_AUDIO_ABPLAY,NULL);
                    if(MUSIC_DISPLAY_NOR == MusicDispType)
                    {
                        SendMsg(MSG_MUSIC_DISPFLAG_AB);
                    }
                }
                else
                {
                    AudioDecodeProc(MSG_AUDIO_ABSTOP,NULL);
                    if(MUSIC_DISPLAY_NOR == MusicDispType)
                    {
                        SendMsg(MSG_MUSIC_DISPFLAG_AB);
                    }
                }
            }
            break;

        case KEY_VAL_MENU_SHORT_UP:                             //enter brower.
            if(CheckMsg(MSG_MUSIC_NEXTFILE))
            {
                if(GetMsg(MSG_MUSIC_NEXTFILE_OK))
                {
                    ClearMsg(MSG_MUSIC_NEXTFILE)  ;
                    ClearMsg(MSG_MUSIC_NEXTFILE_OK);
                }
                else
                {
                    break;
                }
            }

            break;

        case KEY_VAL_MENU_PRESS_START:                          //exit music interface
            BroswerFlag = FALSE;
            TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
            TaskSwitch(TASK_ID_MAINMENU,&TaskArg);
            RetVal = 1;
            break;

        case KEY_VAL_PLAY_SHORT_UP:                             // play pause
            if (AUDIO_STATE_PLAY == MusicPlayState)
            {
                MusicDecodeProc(MSG_AUDIO_PAUSE, NULL);
            }
            else if (AUDIO_STATE_PAUSE == MusicPlayState)
            {
                MusicDecodeProc(MSG_AUDIO_RESUME, NULL);
            }
            SendMsg(MSG_MUSIC_DISPFLAG_STATUS);
            break;

        case KEY_VAL_UP_DOWN:                                   //volume increse
        case KEY_VAL_UP_PRESS:
            if (MusicWinVol < MAX_VOLUME)
            {
                MusicWinVol++;
                MusicDecodeProc(MSG_AUDIO_VOLUMESET,NULL);
            }
            SendMsg(MSG_MUSIC_DISPFLAG_VOL);
            break;

        case KEY_VAL_DOWN_DOWN:                                 //volume reduce
        case KEY_VAL_DOWN_PRESS:
            if(MusicWinVol)
            {
               MusicWinVol--;
               MusicDecodeProc(MSG_AUDIO_VOLUMESET,NULL);
            }
            SendMsg(MSG_MUSIC_DISPFLAG_VOL);
            break;

        case KEY_VAL_FFD_PRESS:                                 // FFW
            MusicDecodeProc(MSG_AUDIO_FFD, (void*)AudioStepLen);

            if(MUSIC_DISPLAY_NOR == MusicDispType)
            {
                SendMsg(MSG_MUSIC_DISPFLAG_CURRENT_TIME);
                if (AudioPlayInfo.ABRequire != AUDIO_AB_NULL)
                {
                    AudioPlayInfo.ABRequire = AUDIO_AB_NULL;
                    AudioDecodeProc(MSG_AUDIO_ABSTOP, NULL);
                    SendMsg(MSG_MUSIC_DISPFLAG_AB);
                }
            }
            SendMsg(MSG_MUSIC_DISPFLAG_SCHED);
            break;

        case KEY_VAL_FFW_PRESS:                                  //FFD
            MusicDecodeProc(MSG_AUDIO_FFW, (void*)AudioStepLen);

            if(MUSIC_DISPLAY_NOR == MusicDispType)
            {
                SendMsg(MSG_MUSIC_DISPFLAG_CURRENT_TIME);
                if (AudioPlayInfo.ABRequire != AUDIO_AB_NULL)
                {
                    AudioPlayInfo.ABRequire = AUDIO_AB_NULL;
                    AudioDecodeProc(MSG_AUDIO_ABSTOP, NULL);
                    SendMsg(MSG_MUSIC_DISPFLAG_AB);
                }
            }
            SendMsg(MSG_MUSIC_DISPFLAG_SCHED);
           break;

        case KEY_VAL_FFD_LONG_UP:                              //Resume
        case KEY_VAL_FFW_LONG_UP:
            MusicDecodeProc(MSG_AUDIO_RESUME, NULL);
            break;

        case KEY_VAL_FFD_SHORT_UP://next file
            AudioErrorFileCount = 0;//sen #20090803#1 all audio file is not support
            AudioStopMode = Audio_Stop_Force;
            MusicNextFile = 1;
            MusicDecodeProc(MSG_AUDIO_FADE, (void*)MusicNextFile);
            break;

        case KEY_VAL_FFW_SHORT_UP://preious file
            AudioErrorFileCount = 0;//sen #20090803#1 all audio file is not support
            AudioStopMode = Audio_Stop_Force;
            MusicNextFile = -1;
            MusicDecodeProc(MSG_AUDIO_FADE, (void*)MusicNextFile);
            break;

        case KEY_VAL_HOLD_ON:
            TaskArg.Hold.HoldAction = HOLD_STATE_ON;
            WinCreat(&MusicWin, &HoldWin, &TaskArg);
            break;

        case KEY_VAL_HOLD_OFF:
            TaskArg.Hold.HoldAction = HOLD_STATE_OFF;
            WinCreat(&MusicWin, &HoldWin, &TaskArg);
            break;

        default:
             RetVal = FALSE;
            break;
    }

    return RetVal;
}


/*
********************************************************************************
*
*                         End of MusicWin.c
*
********************************************************************************
*/
#endif

