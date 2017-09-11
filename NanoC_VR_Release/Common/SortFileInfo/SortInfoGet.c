/*
********************************************************************************
*          Copyright (C),2004-2005, tonyzyz, Fuzhou Rockchip Co.,Ltd.
*                             All Rights Reserved
*                                    V1.00
* FileName   : SortInfoGet.c
* Author     : 
* Description: media libary UI call
* History    :        
*           <author>        <time>     <version>       <desc>
*            azg            06/08/09       1.0            ORG
*
********************************************************************************
*/


#include "SysInclude.h"

#ifdef  MEDIA_MODULE
#include "FsInclude.h"

#include "AddrSaveMacro.h"
#include "FileInfoSaveMacro.h"
#include "MediaBroWin.h"

#include "FileInfoSaveStruct.h"
#include "FileInfoSaveGloable.h"

#include "SortInfoGetMacro.h"
#include "AudioControl.h"

/*
--------------------------------------------------------------------------------
  Function name : void GetSavedMusicPath(UINT8 *pPathBuffer, UINT32 ulFullInfoSectorAddr, UINT32 ulSortSectorAddr, UINT16 uiSortId)
  
  Author        : anzhiguo
  Description   : get path information,file number and long file name.
                  
  Input         : pPathBuffer -- to store path information
                  ulFullInfoSectorAddr --- the start address in flash to store the detailed file information.
                  ulSortSectorAddr --- the start addressin flash to save sort information
                  uiSortId -- the got file index in sort information,
                  Filenum  -- the current file index in whole files.(index 1 is frist)
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         
--------------------------------------------------------------------------------
*/   
_ATTR_MEDIABROWIN_CODE_
void GetSavedMusicPath(UINT8 *pPathBuffer, UINT32 ulFullInfoSectorAddr, UINT32 ulSortSectorAddr, UINT16 uiSortId )
{
    UINT32 temp1;
    UINT8 ucBufTemp[2];
    MDReadData(DataDiskID, (ulSortSectorAddr<<9)+((UINT32)uiSortId)*2, 2, ucBufTemp);
    temp1 = (UINT16)ucBufTemp[0]+(((UINT16)ucBufTemp[1])<<8);
    MDReadData(DataDiskID, (ulFullInfoSectorAddr<<9)+(UINT32)(temp1)*BYTE_NUM_SAVE_PER_FILE+(UINT32)PATH_SAVE_ADDR_OFFSET, (PATH_SIZE+SHORT_NAME_SIZE) ,pPathBuffer);

}

/*
--------------------------------------------------------------------------------
  Function name : 
  void GetSavedMusicFileName(unsigned char *pFileName, SORT_INFO_ADDR_STRUCT AddrInfo, unsigned int uiSortId, unsigned int uiCharNum, unsigned int uiCurDeep)  
  Author        : anzhiguo
  Description   : get long file name of saved file.
                  
  Input         : AddrInfo  --- sturcture variable to save file information.

                  uiCharNum --- the byte number will read.
                  
  Return        : pFileName the file long file name pointer that want to get

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         
--------------------------------------------------------------------------------
*/   
_ATTR_MEDIABROWIN_CODE_
void GetSavedMusicFileName(UINT16 *pFileName, SORT_INFO_ADDR_STRUCT AddrInfo, UINT16 uiSortId, UINT16 uiCharNum, UINT16 uiCurDeep)
{
    UINT16 i;
    UINT16 temp1;
    UINT8   ucBufTemp[2];   
    UINT32  AddrOffset;
    UINT8 	FileInfoBuf[FILE_NAME_SAVE_CHAR_NUM*2];

    MDReadData(DataDiskID, (AddrInfo.ulFileSortInfoSectorAddr<<9)+(UINT32)(uiSortId*2), 2,ucBufTemp);

    temp1 = (ucBufTemp[0]&0xff)+((ucBufTemp[1]&0xff)<<8); // 获得对应的文件保存号

    if(uiCurDeep==0)//读取长文件名和idtitle时会有地址上的区别
    {
        AddrOffset = (UINT32)(temp1)*BYTE_NUM_SAVE_PER_FILE + AddrInfo.uiSortInfoAddrOffset; // 计算文件保存偏移地址
    }
    else //专辑，歌手，流派分类时，直接读取长文件名
    {
        AddrOffset = (UINT32)(temp1)*BYTE_NUM_SAVE_PER_FILE; // 计算文件保存偏移地址
    }
    MDReadData(DataDiskID, (AddrInfo.ulFileFullInfoSectorAddr<<9)+AddrOffset, FILE_NAME_SAVE_CHAR_NUM*2, FileInfoBuf);

    for(i=0;i<uiCharNum;i++)
    {
        *pFileName++ = (UINT16)FileInfoBuf[2*i]+((UINT16)FileInfoBuf[2*i+1]<<8); 
    } 
}
/*
--------------------------------------------------------------------------------
  Function name : void GetSummaryFileName(UINT16 *pFileName, SORT_INFO_ADDR_STRUCT AddrInfo, UINT16 uiSortId)  
  Author        : anzhiguo
  
  Description   : get media sort information:album name, artist or genre.
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         
--------------------------------------------------------------------------------
*/   
_ATTR_MEDIABROWIN_CODE_
void GetSummaryFileName(UINT16 *pFileName, SORT_INFO_ADDR_STRUCT AddrInfo, UINT16 uiSortId)
{
    UINT16 i;
    UINT16 uiSaveNO;
    UINT8  ucBufTemp[2];   
    UINT32  AddrOffset; 
    UINT8 	FileInfoBuf[FILE_NAME_SAVE_CHAR_NUM*2];

    MDReadData(DataDiskID, (AddrInfo.ulSortSubInfoSectorAddr<<9)+(unsigned long)(uiSortId*SUB_PER_ITEM_SIZE), 2, ucBufTemp);
    uiSaveNO = (ucBufTemp[0]&0xff)+((ucBufTemp[1]&0xff)<<8);

    AddrOffset = (unsigned long)(uiSaveNO)*BYTE_NUM_SAVE_PER_FILE + AddrInfo.uiSortInfoAddrOffset;//compute the offset address of saved file. 
    MDReadData(DataDiskID, (AddrInfo.ulFileFullInfoSectorAddr<<9)+AddrOffset, MEDIA_ID3_SAVE_CHAR_NUM*2, FileInfoBuf);
    for(i=0;i<MEDIA_ID3_SAVE_CHAR_NUM;i++)
    {
        *pFileName++ = (UINT16)FileInfoBuf[2*i]+((UINT16)FileInfoBuf[2*i+1]<<8); 
    }
}
/*
--------------------------------------------------------------------------------
  Function name : unsigned int GetSummaryInfo(unsigned long ulSumSectorAddr, unsigned int uiSumId, unsigned int uiFindSumType)
{
  Author        : anzhiguo
  Description   : get id3 sort information item number,such as file number of album or the same artist.
                  
  Input         : ulSumSectorAddr ID3信息归类存储地址
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:        point to sec address of correspond information in flash.
--------------------------------------------------------------------------------
*/   
_ATTR_MEDIABROWIN_CODE_
UINT16 GetSummaryInfo(UINT32 ulSumSectorAddr, UINT16 uiSumId, UINT16 uiFindSumType)
{
    UINT16 uiSumInfo;
    UINT8 	ucBufTemp[2];
    UINT32 uiSumTypeOffset;

    if(uiFindSumType==FIND_SUM_FULLINFO)//detailed information
    {
        uiSumTypeOffset = SUB_FULLINFO_START_OFFSET;
    }   
    else if(uiFindSumType==FIND_SUM_SORTSTART)//sort information
    {
        uiSumTypeOffset = SUB_SORT_START_ID_OFFSET;//这几个宏定义需要重新定义，读明白id3分类信息的存放顺序和结构
    }
    else if(uiFindSumType==FIND_SUM_ITEMNUM)//item information
    {
        uiSumTypeOffset = SUB_INCLUDE_ITEMNUM_OFFSET;
    }

    MDReadData(DataDiskID, (ulSumSectorAddr<<9)+(UINT32)uiSumId*8+uiSumTypeOffset, 2, ucBufTemp);
    uiSumInfo = (ucBufTemp[0]&0xff)+((ucBufTemp[1]&0xff)<<8);

    return uiSumInfo;
}


#endif


