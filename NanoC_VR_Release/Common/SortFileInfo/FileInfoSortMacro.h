/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   FileInfoSortMacro.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo      2009-06-03          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _FILEINFOSORT_H
#define _FILEINFOSORT_H



#define   _FILE_INFO_SORT_CODE_      __attribute__((section("FileInfoSortCode")))
#define   _FILE_INFO_SORT_DATA_      __attribute__((section("FileInfoSortData")))
#define   _FILE_INFO_SORT_BSS_       __attribute__((section("FileInfoSortBss"),zero_init))
                                    

#define   SORT_TYPE_ITEM_NUM  5   /*sort type number */
#if 1
#define 	SORT_FILE_NAME_BUF_SIZE 		 ((SORT_FILENUM_DEFINE+1)/2)//the number of one cache buffer to store file info is one half of max sort file number.
//#define 	SORT_FILENAME_LEN  				((1024*8)/SORT_FILE_NAME_BUF_SIZE-1)  /* 从文件名中截取的参与排序字符的个数，目前的16k的排序空间最大可以保存8*1024个文件的排序信息*/
#define 	SORT_FILENAME_LEN  				(4)
//the macro respresent the number that cut out from file name to participate sort.now,memory size is 16k that can save 8k file sort info.

#else
#define 	SORT_FILE_NAME_BUF_SIZE 		 10	/* 一个排序缓冲区可以存放的排序文件的个数，是最大可排序文件个数的一半 */
#define 	SORT_FILENAME_LEN  				4//((1024*8)/SORT_FILE_NAME_BUF_SIZE-1)  /* 从文件名中截取的参与排序字符的个数，目前的16k的排序空间最大可以保存8*1024个文件的排序信息*/
#endif
#define 	CHILD_CHAIN_NUM				72 /*to check the child link table number */

#define   FILE_NAME_TYPE    0       //the order must be consistency with the definition in file AddrSaveMacro.h
#define   ID3_TITLE_TYPE    1
#define   ID3_ARTIST_TYPE   2
#define   ID3_ALBUM_TYPE    3
#define   ID3_GENRE_TYPE    4


typedef struct _FILE_INFO_INDEX_STRUCT{

  struct	_FILE_INFO_INDEX_STRUCT	*pNext;
  UINT16  SortFileName[SORT_FILENAME_LEN];
	
}FILE_INFO_INDEX_STRUCT; /*file sort information*/


extern UINT32 GetCmpResult(UINT16 wch);
extern int PinyinCharCmp(UINT16 wch1, UINT16 wch2);
extern UINT32 PinyinStrnCmp(UINT16 *str1, UINT16 *str2, UINT32 length);
extern UINT8 BrowserListInsertBySort(FILE_INFO_INDEX_STRUCT *head, FILE_INFO_INDEX_STRUCT *pNode);
extern void SortPageBufferInit(UINT8 *pBuffer);
extern void ChildChainInit(void);
extern void GetSortName(UINT32 SectorOffset,UINT16 FileNum);
extern void SortFunction(UINT32 uiSaveAddrOffset,UINT16 FileNum);
extern void SortUpdateFun(MEDIALIB_CONFIG * Sysfilenum ,UINT32 MediaInfoAddress);



#endif
/*
********************************************************************************
*
*                         End of FileInfoSort.c
********************************************************************************
*/


