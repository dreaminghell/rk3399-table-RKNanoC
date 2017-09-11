/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   FileInfo.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                               2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _FILEINFO_H
#define _FILEINFO_H

#undef  EXT
#ifdef _IN_FILEINFOSAVE_
#define EXT
#else
#define EXT extern
#endif


/*
*-------------------------------------------------------------------------------
*  
*                           Include File
*  
*-------------------------------------------------------------------------------
*/




//section define
//permanent code of main menu






extern void SearchAndSaveFileInfo(void);

/*
********************************************************************************
*
*                         End of FileInfo.h
*
********************************************************************************
*/
#endif


