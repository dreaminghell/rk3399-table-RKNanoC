/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   FileInfoSaveStruct.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo      2009-06-03          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _FILESAVESTRUCT_H
#define _FILESAVESTRUCT_H

//#include "FileInfoSaveMacro.h"

#undef  EXT
#ifdef  _IN_FILEINFOSAVE_
#define EXT
#else
#define EXT extern
#endif


typedef struct _MEDIA_FILE_SAVE_STRUCT{	
	 UINT16  LongFileName[FILE_NAME_SAVE_CHAR_NUM];//it is long file name of file system
	 UINT16  id3_title[MEDIA_ID3_SAVE_CHAR_NUM];
	 UINT16  id3_singer[MEDIA_ID3_SAVE_CHAR_NUM];
	 UINT16  id3_album[MEDIA_ID3_SAVE_CHAR_NUM];
	 UINT16  Genre[MEDIA_ID3_SAVE_CHAR_NUM];
	 UINT32  DirClus;
	 UINT32  Index;
	 UINT32  DirDeep;
}MEDIA_FILE_SAVE_STRUCT;



//id3 的这个结构体在合并zs的代码时可以去掉
//#define   ID3_V2_INBUFFER_SIZE   2048
//#define   ID3_V1_INBUFFER_SIZE   128
#define   ID3_TAG_SIZE           FILE_NAME_SAVE_CHAR_NUM //30
//#define   ID3_GENREN_INFO        10

typedef struct _ID3_INFO_STRUCT{
UINT32   id3_size;
UINT16  id3_title_size;
UINT16  id3_singer_size;
UINT16  id3_album_size;
UINT16  id3_title[ID3_TAG_SIZE];
UINT16  id3_singer[ID3_TAG_SIZE];
UINT16  id3_album[ID3_TAG_SIZE];
//short  id3_genren;  //liupai
//short  id3_genren_info[ID3_GENREN_INFO];
UINT16  id3_ver;     //v1,v2
UINT16  id3_check;   //null or full

}ID3_INFO_STRUCT;

#endif

/*
********************************************************************************
*
*                         End of FileInfoSaveStruct.h
*
********************************************************************************
*/


