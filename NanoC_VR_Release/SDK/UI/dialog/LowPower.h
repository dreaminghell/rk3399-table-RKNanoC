/*
********************************************************************************
*                   Copyright (c) 2008,anzhiguo
*                         All rights reserved.
*
* File Name£º  DIALOGBOX.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo      2009-2-27         1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef  _DIALOG_LOWPOWER_H
#define  _DIALOG_LOWPOWER_H

#undef  EXT
#ifdef  _IN_DIALOG_LOWPOWER_
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
#define _ATTR_LOWERPOWER_CODE_         __attribute__((section("LowPowerBoxCode")))
#define _ATTR_LOWERPOWER_DATA_         __attribute__((section("LowPowerBoxData")))
#define _ATTR_LOWERPOWER_BSS_          __attribute__((section("LowPowerBoxBss"),zero_init))

#define LOWERPOWER_DISPLAY_TIME        3
/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/

_ATTR_LOWERPOWER_BSS_ EXT UINT32 DialogLowPowerSystickCounterBack;



/*
--------------------------------------------------------------------------------
  
   Functon Declaration 
  
--------------------------------------------------------------------------------
*/
extern void DialogLowPowerInit(void *pArg);
extern void DialogLowPowerDeInit(void);
extern uint32 DialogLowPowerService(void);
extern uint32 DialogLowPowerKey(void);
extern void DialogLowPowerDisplay(void);

/*
--------------------------------------------------------------------------------
  
  Description:  window sturcture definition
  
--------------------------------------------------------------------------------
*/
#ifdef _IN_DIALOG_LOWPOWER_
_ATTR_LOWERPOWER_DATA_ WIN LowPowerWin = {
    
    NULL,
    NULL,
    
    DialogLowPowerService,              //window service handle function.
    DialogLowPowerKey,                  //window key service handle function.
    DialogLowPowerDisplay,              //window display service handle function.
    
    DialogLowPowerInit,                 //window initial handle function.
    DialogLowPowerDeInit                //window auti-initial handle function.
    
};
#else 
_ATTR_LOWERPOWER_DATA_ EXT WIN LowPowerWin;
#endif

/*
********************************************************************************
*
*                         End of DIALOGBOX.h
*
********************************************************************************
*/
#endif

