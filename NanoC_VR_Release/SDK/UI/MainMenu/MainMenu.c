/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  MainMenu.c
*
* Description:
*
* History:      <author>          <time>        <version>
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_MAINMENU_

#include "SysInclude.h"
#include "FsInclude.h"
#include "MainMenu.h"
#include "MainMenuInterface.h"
#include "Hold.h"


_ATTR_SYS_DATA_ UINT8 MainIdFlag = 0;
/*
--------------------------------------------------------------------------------
  Function name : void MainMenuIntInit(void)
  Author        : ZHengYongzhi
  Description   : main menu interrupt initialization

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_INIT_CODE_
void MainMenuIntInit(void)
{
    //MainMenu  interrupt initialization
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuIntDeInit(void)
  Author        : ZHengYongzhi
  Description   : main menu interrupt auti-initial

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_DEINIT_CODE_
void MainMenuIntDeInit(void)
{
    //MainMenu
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuMsgInit(void)
  Author        : ZHengYongzhi
  Description   : main menu message initial function

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_INIT_CODE_
void MainMenuMsgInit(void)
{
    SendMsg(MSG_MENU_DISPLAY_ALL);

}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuMsgDeInit(void)
  Author        : ZHengYongzhi
  Description   : main menu message auti-initialization

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_DEINIT_CODE_
void MainMenuMsgDeInit(void)
{
 	// ClearMsg(MSG_MENU_DISPLAY_BACK);
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuInit(void)
  Author        : ZHengYongzhi
  Description   : main menu initialization

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuInit(void *pArg)
{
    //DEBUG("MainMenu Enter");
    //system content switch initial code
    MenuId = ((MAINMENU_WIN_ARG*)pArg)->MenuID;

    #ifdef _BROWSER_
    if (MainIdFlag == MAINMENU_ID_BROWSER)
    {
        MenuId     = MAINMENU_ID_BROWSER;
        MainIdFlag = 0;
    }
    #endif

    //service initial
    MainMenuMsgInit();      //message initial
    MainMenuIntInit();      //interrupt initial
    KeyReset();             //key initial
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuDeInit(void)
  Author        : ZHengYongzhi
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuDeInit(void)
{
    MainMenuIntDeInit();
    //DEBUG("MainMenu Exit");
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 MainMenuService(void)
  Author        : ZHengYongzhi
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_SERVICE_CODE_
UINT32 MainMenuService(void)
{
    return(RETURN_OK);
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 MainMenuKey(void)
  Author        : ZHengYongzhi
  Description   : main menu key handle function.

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_SERVICE_CODE_
UINT32 MainMenuKey(void)
{
    uint32 RetVal;
    UINT32 MainMenuKeyVal;
    WIN    *pWin;
	TASK_ARG TaskArg;

    RetVal = RETURN_OK;

    MainMenuKeyVal =  GetKeyVal();

    //printf("key = 0x%x  \n", MainMenuKeyVal);

    //5 key function modification
    if (gSysConfig.KeyNum == KEY_NUM_5)
    {
        switch (MainMenuKeyVal)
        {
        case KEY_VAL_FFD_SHORT_UP:
            MainMenuKeyVal = KEY_VAL_DOWN_SHORT_UP;
            break;

        case KEY_VAL_FFD_PRESS:
            MainMenuKeyVal = KEY_VAL_DOWN_PRESS;
            break;

        case KEY_VAL_FFW_SHORT_UP:
            MainMenuKeyVal = KEY_VAL_UP_SHORT_UP;
            break;

        case KEY_VAL_FFW_PRESS:
            MainMenuKeyVal = KEY_VAL_UP_PRESS;
            break;

        default:
            break;
        }
    }

    //6 key function modification
    if (gSysConfig.KeyNum == KEY_NUM_6)
    {

    }

    //7 key function
    switch (MainMenuKeyVal)
    {
    case KEY_VAL_MENU_SHORT_UP:
    case KEY_VAL_MENU_PRESS:
        RetVal = MainMenuModeKey();
        break;

    case KEY_VAL_DOWN_SHORT_UP:
    case KEY_VAL_DOWN_PRESS:
        if (++MenuId > (MAINMENU_ID_MAXNUM - 1))
        {
            MenuId = 0;
        }

        #ifdef _RADIO_
        if ((!gSysConfig.FMEnable) && (MenuId == MAINMENU_ID_RADIO))
        {
            MenuId++;
        }
        #endif

        SendMsg(MSG_MENU_DISPLAY_ITEM);
        break;

    case KEY_VAL_UP_SHORT_UP:
    case KEY_VAL_UP_PRESS:
        if (MenuId == 0)
        {
            MenuId = (MAINMENU_ID_MAXNUM - 1);
        }
        else
        {
            MenuId -- ;
        }

        #ifdef _RADIO_
        if ((!gSysConfig.FMEnable) && (MenuId == MAINMENU_ID_RADIO))
        {
            MenuId--;
        }
        #endif

        SendMsg(MSG_MENU_DISPLAY_ITEM);
        break;

    case KEY_VAL_HOLD_ON:
        TaskArg.Hold.HoldAction = HOLD_STATE_ON;
        WinCreat(&MainMenuWin, &HoldWin, &TaskArg);
        break;

    case KEY_VAL_HOLD_OFF:
        TaskArg.Hold.HoldAction = HOLD_STATE_OFF;
        WinCreat(&MainMenuWin, &HoldWin, &TaskArg);
        break;

    default:
        break;
    }
    return (RetVal);
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuDisplay(void)
  Author        : ZHengYongzhi
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_SERVICE_CODE_
void MainMenuDisplay(void)
{
//#ifdef DISPLAY_ENABLE
#if 0

    if (CheckMsg(MSG_NEED_PAINT_ALL) || (GetMsg(MSG_MENU_DISPLAY_ALL)))
    {
        //display backgroud picture,battery and hold icon.
        //send all initial message
        SendMsg(MSG_MENU_DISPLAY_ITEM);
    }

    if (TRUE == GetMsg(MSG_MENU_DISPLAY_ITEM))
    {
        DispPictureWithIDNum(IMG_ID_MAINMENU_ITEMMUSIC + MenuId);
        //DispPictureWithIDNum(IMG_ID_MAINMENU_HOLDOFF + MainmenuHoldState);
        SendMsg(MSG_BATTERY_UPDATE);
    }

    if (TRUE == GetMsg(MSG_BATTERY_UPDATE))
    {
        //DispPictureWithIDNum(IMG_ID_MAINMENU_BATTERYLEVEL0 + MainMenuBatteryLevel);
    }
#else
//
#endif
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuModeKey(void)
  Author        : ZHengYongzhi
  Description   : main menu modules switch

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
extern THREAD MusicThread;
extern THREAD RecordThread;
extern THREAD FMThread;
extern void AudioFileOpsInit(void);
extern void BTPhoneInit(void);
_ATTR_MAIN_MENU_SERVICE_CODE_
INT16 MainMenuModeKey(void)
{
    INT16  RetVal = 0;
    TASK_ARG TaskArg;

    switch(MenuId)
    {
    #ifdef MEDIA_MODULE
    #ifdef _MUSIC_
    case MAINMENU_ID_MUSIC:
        TaskArg.Medialib.CurId= 0;
        TaskSwitch(TASK_ID_MEDIALIB, &TaskArg);
        RetVal = 1;
        break;
    #endif

    #else

    #ifdef _MUSIC_
    case MAINMENU_ID_MUSIC:
        //因为蓝牙线程也有使用
        //如果有蓝牙线程工作，先关闭蓝牙线程
		#ifdef _BLUETOOTH_
        if (TRUE == ThreadCheck(pMainThread, &BlueToothThread))
        {
            ThreadDeleteAll(&pMainThread);
        }
		#endif
        if (TRUE != ThreadCheck(pMainThread, &MusicThread))
        {
            ThreadDeleteAll(&pMainThread);
        }

        TaskArg.Music.FileNum = gSysConfig.MusicConfig.FileNum;
        TaskArg.Music.pAudioOpsInit = AudioFileOpsInit;
        TaskSwitch(TASK_ID_MUSIC, &TaskArg);
        RetVal = 1;
        break;
    #endif

    #endif

    #ifdef _EBOOK_
    case MAINMENU_ID_EBOOK:
        TaskArg.Browser.FileType = FileTypeText;
        TaskArg.Browser.FileNum  = 0;
        BroswerFlag = TRUE;
        TaskSwitch(TASK_ID_BROWSER, &TaskArg);
        RetVal = 1;
        break;
    #endif

    #ifdef _VIDEO_
    case MAINMENU_ID_VIDEO: //video module
        TaskArg.Browser.FileType = FileTypeVideo;
        TaskArg.Browser.FileNum  = 0;
        BroswerFlag = TRUE;
        TaskSwitch(TASK_ID_BROWSER, &TaskArg);
        RetVal = 1;
         break;
    #endif

    #ifdef _PICTURE_
    case MAINMENU_ID_PICTURE: //picture module
        TaskArg.Browser.FileType = FileTypePicture;
        TaskArg.Browser.FileNum  = 0;
        BroswerFlag = TRUE;
        TaskSwitch(TASK_ID_BROWSER, &TaskArg);
        RetVal = 1;
        break;
    #endif

    #ifdef _RECORD_
    case MAINMENU_ID_RECORD:
        if (TRUE != ThreadCheck(pMainThread, &RecordThread))
        {
            ThreadDeleteAll(&pMainThread);
        }

        TaskArg.Record.RecordType = RECORD_TYPE_MIC;
        TaskArg.Record.RecordFirst = 0;
        TaskSwitch(TASK_ID_RECORD,&TaskArg);
        RetVal = 1;
        break;
    #endif

    #ifdef _RADIO_
    case MAINMENU_ID_RADIO:
        #if 1
        if (gSysConfig.FMEnable)
        {
            if (TRUE != ThreadCheck(pMainThread, &FMThread))
            {
                ThreadDeleteAll(&pMainThread);
            }

            TaskSwitch(TASK_ID_RADIO, NULL);
    		RetVal = 1;
        }

        #else
        {
            //if (TRUE != ThreadCheck(pMainThread, &MusicThread))
            {
                ThreadDeleteAll(&pMainThread);
            }

            TaskArg.Music.FileNum = gSysConfig.MusicConfig.FileNum;
            TaskArg.Music.pAudioOpsInit = BTPhoneInit;
            ModuleOverlay(MODULE_ID_BT, MODULE_OVERLAY_ALL);
            TaskSwitch(TASK_ID_MUSIC, &TaskArg);

            RetVal = 1;
        }

        #endif

        break;
    #endif

    #ifdef _BROWSER_
    case MAINMENU_ID_BROWSER:
        MainIdFlag = MAINMENU_ID_BROWSER;
        TaskArg.Browser.FileType = FileTypeALL;
        TaskArg.Browser.FileNum  = 0;
        BroswerFlag = TRUE;
        TaskSwitch(TASK_ID_BROWSER, &TaskArg);
        RetVal = 1;
        break;
    #endif

    #ifdef  _SYSSET_
    case MAINMENU_ID_SETMENU:
        TaskSwitch(TASK_ID_SYSSET, NULL);
        RetVal = 1;
        break;
    #endif

	#ifdef _BLUETOOTH_
    case MAINMENU_ID_BLUETOOTH:
		if (TRUE != ThreadCheck(pMainThread, &BlueToothThread))
        {
            ThreadDeleteAll(&pMainThread);
        }

        TaskSwitch(TASK_ID_BT, (TASK_ARG*)NULL);
        RetVal = 1;
        break;
    #endif

    default:
        break;
    }

    return (RetVal);
}
/*
********************************************************************************
*
*                         End of MainMenu.c
*
********************************************************************************
*/
