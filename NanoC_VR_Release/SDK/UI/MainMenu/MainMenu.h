/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   MainMenu.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _MAINMENU_H_
#define _MAINMENU_H_

#undef  EXT
#ifdef _IN_MAINMENU_
#define EXT
#else
#define EXT extern
#endif

/*
*-------------------------------------------------------------------------------
*  
*                           Macro define
*  
*-------------------------------------------------------------------------------
*/
//section define
//main menu permanent code
#define _ATTR_MAIN_MENU_CODE_         __attribute__((section("MainMenuCode")))
#define _ATTR_MAIN_MENU_DATA_         __attribute__((section("MainMenuData")))
#define _ATTR_MAIN_MENU_BSS_          __attribute__((section("MainMenuBss"),zero_init))

//main menu initial code
#define _ATTR_MAIN_MENU_INIT_CODE_    __attribute__((section("MainMenuInitCode")))
#define _ATTR_MAIN_MENU_INIT_DATA_    __attribute__((section("MainMenuInitData")))
#define _ATTR_MAIN_MENU_INIT_BSS_     __attribute__((section("MainMenuInitBss"),zero_init))

//main menu auti-initial code
#define _ATTR_MAIN_MENU_DEINIT_CODE_  __attribute__((section("MainMenuDeInitCode")))
#define _ATTR_MAIN_MENU_DEINIT_DATA_  __attribute__((section("MainMenuDeInitData")))
#define _ATTR_MAIN_MENU_DEINIT_BSS_   __attribute__((section("MainMenuDeInitBss"),zero_init))

//main menu content switch code
#define _ATTR_MAIN_MENU_SERVICE_CODE_ __attribute__((section("MainMenuServiceCode")))
#define _ATTR_MAIN_MENU_SERVICE_DATA_ __attribute__((section("MainMenuServiceData")))
#define _ATTR_MAIN_MENU_SERVICE_BSS_  __attribute__((section("MainMenuServiceBss"),zero_init))

/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/
typedef enum 
{
    #ifdef _MUSIC_
    MAINMENU_ID_MUSIC= (UINT32)0,//audio module
    #endif
    
    #ifdef _VIDEO_
    MAINMENU_ID_VIDEO,            //video module   
    #endif

    #ifdef _RADIO_
    MAINMENU_ID_RADIO,           //FM
    #endif

	#ifdef _BLUETOOTH_
    MAINMENU_ID_BLUETOOTH,           //bluetooth
    #endif
    
    #ifdef _PICTURE_
    MAINMENU_ID_PICTURE,         //picture resource
    #endif

    #ifdef _RECORD_
    MAINMENU_ID_RECORD,          //recording
    #endif

    #ifdef _EBOOK_
    MAINMENU_ID_EBOOK,           //ebook
    #endif

    #ifdef _BROWSER_
    MAINMENU_ID_BROWSER,         //brower
    #endif

    #ifdef _SYSSET_
    MAINMENU_ID_SETMENU,          //setting module
    #endif
    
    MAINMENU_ID_MAXNUM
    
} MAINMENU_ID;

/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/
#define MainMenuBatteryLevel       gBattery.Batt_Level
#define MainmenuHoldState          HoldState

_ATTR_MAIN_MENU_BSS_ EXT UINT16    MenuId; //moduel id of main menu interface

/*
--------------------------------------------------------------------------------
  
   Functon Declaration 
  
--------------------------------------------------------------------------------
*/
extern void MainMenuInit(void *pArg);
extern void MainMenuDeInit(void);

extern UINT32 MainMenuService(void);
extern UINT32 MainMenuKey(void);
extern void MainMenuDisplay(void);

INT16 MainMenuModeKey(void);


/*
--------------------------------------------------------------------------------
  
  Description:  window sturcture definition
  
--------------------------------------------------------------------------------
*/
#ifdef _IN_MAINMENU_
_ATTR_MAIN_MENU_DATA_ WIN MainMenuWin = {
    
    NULL,
    NULL,
    
    MainMenuService,               //window service handle function.
    MainMenuKey,                   //window key service handle function.
    MainMenuDisplay,               //window display service handle function.
    
    MainMenuInit,                  //window initial handle function.
    MainMenuDeInit                 //window auti-initial handle function.
    
};
#else 
_ATTR_MAIN_MENU_DATA_ EXT WIN MainMenuWin;
#endif

/*
********************************************************************************
*
*                         End of Win.h
*
********************************************************************************
*/
#endif

