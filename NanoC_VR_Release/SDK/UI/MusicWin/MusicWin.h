/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   AudioWin.h
*
* Description:
*
* History:      <author>          <time>        <version>
*                Chenwei           2009-02-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#ifndef _MUSICWIN_H_
#define _MUSICWIN_H_
#undef  EXT
#ifdef _IN_MUSICWIN_
#define EXT
#else
#define EXT extern
#endif

/*
********************************************************************************
*
*                        Macro Define
*
********************************************************************************
*/
//section define
//music menu permanent code.
#define _ATTR_MUSIC_CODE_         __attribute__((section("MusicCode")))
#define _ATTR_MUSIC_DATA_         __attribute__((section("MusicData")))
#define _ATTR_MUSIC_BSS_          __attribute__((section("MusicBss"),zero_init))

//music menu initial code
#define _ATTR_MUSIC_INIT_CODE_    __attribute__((section("MusicInitCode")))
#define _ATTR_MUSIC_INIT_DATA_    __attribute__((section("MusicInitData")))
#define _ATTR_MUSIC_INIT_BSS_     __attribute__((section("MusicInitBss"),zero_init))

//music  menu auti-initial code
#define _ATTR_MUSIC_DEINIT_CODE_  __attribute__((section("MusicDeInitCode")))
#define _ATTR_MUSIC_DEINIT_DATA_  __attribute__((section("MusicDeInitData")))
#define _ATTR_MUSIC_DEINIT_BSS_   __attribute__((section("MusicDeInitBss"),zero_init))

//music menu dispatch code
#define _ATTR_MUSIC_SERVICE_CODE_ __attribute__((section("MusicServiceCode")))
#define _ATTR_MUSIC_SERVICE_DATA_ __attribute__((section("MusicServiceData")))
#define _ATTR_MUSIC_SERVICE_BSS_  __attribute__((section("MusicServiceBss"),zero_init))

/*----------------------------------------------------------------------------------------*/
//music playing interface display type.
#define MUSIC_DISPLAY_NOR                   0               //normal display interface
#define MUSIC_DISPLAY_LRC                   1               //display LRC interface

//music playing interface dialog,message box type.
#define MUSICWIN_DIALOG_MESSAGE_NULL        0
#define MUSICWIN_DIALOG_NOFILE              1
#define MUSICWIN_DIALOG_FORMAT_ERROR        2

#define MUSIC_TIME_GUAGE_CONST              47            //total item number of progress
#define MUSIC_SCROLL_WIDTH                  (126 - 26)    //character scroll width,if exclude this width,it will scroll

/*
********************************************************************************
*
*                         Variable Define
*
********************************************************************************
*/
_ATTR_MUSIC_BSS_ EXT   UINT8               Spetrum;
_ATTR_MUSIC_BSS_ EXT   UINT32              MusicTimeGuageVal;  //guage progress
_ATTR_MUSIC_BSS_ EXT   UINT8               MusicDispType;
_ATTR_MUSIC_BSS_ EXT   UINT8               MusicWinDialogType;
_ATTR_MUSIC_BSS_ EXT   UINT8               MusicBgFlag;// 1:have LCR,0: NO,do not need refresh screen.
_ATTR_MUSIC_BSS_ EXT   UINT16              MediaTitleAdd;
_ATTR_MUSIC_BSS_ EXT   UINT32              CurrentTimeSecBk;
_ATTR_MUSIC_BSS_ EXT   UINT32              CurrentTime200MsBk;

/*
********************************************************************************
*
*                         Functon Declaration
*
********************************************************************************
*/
extern void MusicWinInit(void *pArg);
extern void MusicWinDeInit(void);

extern void MusicWinIntInit(void);
extern void MusicWinIntDeInit(void);
extern void MusicWinMsgInit(void *pArg);
extern void MusicWinMsgDeInit(void);
extern UINT32 MusicWinService(void);
extern void MusicWinPaint(void);
extern UINT32 MusicWinKeyProc(void);

/*
********************************************************************************
*
*                         Description:  window sturcture definition
*
********************************************************************************
*/
#ifdef _IN_MUSICWIN_
_ATTR_MUSIC_DATA_ WIN MusicWin = {

    NULL,
    NULL,

    MusicWinService,                    //window service handle function.
    MusicWinKeyProc,                    //window key service handle function.
    MusicWinPaint,                      //window display service handle function.

    MusicWinInit,                       //window initial handle function.
    MusicWinDeInit                      //window auti-initial handle function.

};
#else
_ATTR_MUSIC_DATA_ EXT WIN MusicWin;
#endif

/*
********************************************************************************
*
*                         End of AudioWin.h
*
********************************************************************************
*/
#endif


