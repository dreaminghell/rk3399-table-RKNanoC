/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   ChargeWin.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _CHARGE_H_
#define _CHARGE_H_

#undef  EXT
#ifdef _IN_CHARGE_
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
#define _ATTR_CHARGE_WIN_CODE_         __attribute__((section("ChargeWinCode")))
#define _ATTR_CHARGE_WIN_DATA_         __attribute__((section("ChargeWinData")))
#define _ATTR_CHARGE_WIN_BSS_          __attribute__((section("ChargeWinBss"),zero_init))

/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/

/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
  
   Functon Declaration 
  
--------------------------------------------------------------------------------
*/
extern void CharegeWinInit(void *pArg);
extern void ChargeWinDeInit(void);

extern UINT32 ChargeWinService(void);
extern UINT32 ChargeWinKey(void);
extern void CharegeWinDisplay(void);

/*
--------------------------------------------------------------------------------
  
  Description:  window sturcture definition
  
--------------------------------------------------------------------------------
*/
#ifdef _IN_CHARGE_
_ATTR_CHARGE_WIN_DATA_ WIN ChargeWin = {
    
    NULL,
    NULL,
    
    ChargeWinService,               //window service handle function.
    ChargeWinKey,                   //window key service handle function.
    CharegeWinDisplay,              //window display service handle function.
    
    CharegeWinInit,                 //window initial handle function.
    ChargeWinDeInit                 //window auti-initial handle function.
    
};
#else 
_ATTR_CHARGE_WIN_DATA_ EXT WIN ChargeWin;
#endif

/*
********************************************************************************
*
*                         End of ChargeWin.h
*
********************************************************************************
*/
#endif

