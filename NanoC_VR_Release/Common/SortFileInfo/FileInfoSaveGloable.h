/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   FileInfoSaveGloable.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo      2009-06-03          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _FILESAVEGLOBAL_H
#define _FILESAVEGLOBAL_H



#undef  EXT
#ifdef _IN_FILEINFOSAVE_
#define EXT
#else
#define EXT     extern
#endif


_FILE_INFO_SAVE_BSS_ EXT   FIND_DATA FindDataInfo;
_FILE_INFO_SAVE_BSS_ EXT   MEDIA_FILE_SAVE_STRUCT gFileSaveInfo; //it is a structure variable to save file information
_FILE_INFO_SAVE_BSS_ EXT   ID3_INFO_STRUCT gID3Info; //it is use to save id3 information 
_FILE_INFO_SAVE_BSS_ EXT   UINT8 	gPageWriteBuffer[MEDIAINFO_PAGE_SIZE]; //it use to remember a page file information temporarily 
_FILE_INFO_SAVE_BSS_ EXT   UINT16 gPathDisplayBuffer[MAX_FILENAME_LEN]; //to remember direction level information,just need MAX_DIR_DEPTH-1 levels.
//_FILE_INFO_SAVE_BSS_ EXT   UINT16 SubDirNum[MAX_DIR_DEPTH-1]; // 记录文件的目录层次信息,只需MAX_DIR_DEPTH-1层


#endif
/*
********************************************************************************
*
*                         End of FileInfoSaveGloable.h
*
********************************************************************************
*/



