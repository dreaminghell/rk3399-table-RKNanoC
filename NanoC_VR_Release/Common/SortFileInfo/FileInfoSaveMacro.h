/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   FileInfoSaveMacro.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo      2009-06-03          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef  _FILESAVEMACRO_H
#define  _FILESAVEMACRO_H
 


#define   _FILE_INFO_SAVE_CODE_    __attribute__((section("FileInfoSaveCode")))
                                 
#define   _FILE_INFO_SAVE_DATA_    __attribute__((section("FileInfoSaveData")))
#define   _FILE_INFO_SAVE_BSS_     __attribute__((section("FileInfoSaveBss"),zero_init))


#define 	SORT_FILENUM_DEFINE 	2048 
//limit the max number of files,file information occupy 4 blocks,and one block have the number of the page 
//is SecPerBlock[0] divide SecPerPage[0], and one page can save 4 file information.



#define 	FILE_NAME_SAVE_CHAR_NUM 	MAX_FILENAME_LEN	/*charactor number to save file name. */
#define 	MEDIA_ID3_SAVE_CHAR_NUM 	FILE_NAME_SAVE_CHAR_NUM //30	/* charactor number to save ID3_TITLE*/



#endif
/*
********************************************************************************
*
*                         End of FileInfoSaveMacro.h
*
********************************************************************************
*/

