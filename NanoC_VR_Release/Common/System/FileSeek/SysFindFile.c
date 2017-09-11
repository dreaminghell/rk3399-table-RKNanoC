


#define  IN_SYS_FINDFILE

#include "SysInclude.h"
#include "FsInclude.h"
#include "SysFindFile.h"
#include "AudioControl.h"

#ifdef MEDIA_MODULE
#include "MediaBroWin.h"
#endif

/*
--------------------------------------------------------------------------------
  Function name :  UINT16 SysCheckTotalFileNum(uint8 *pExtStr)
  Author        :  zs
  Description   :  check there is the type file in media
  Input         :  uint8 *pExtStr: file name
  Return        :  temp: file total.
  History       :  <author>         <time>         <version>
                     zs            2009/02/27         Ver1.0
  desc          : org
--------------------------------------------------------------------------------
_ATTR_SYS_FINDFILE_TEXT_
UINT16 SysCheckTotalFileNum(uint8 *pExtStr)
{
    UINT16 temp = 0;

    temp = dglBuildDirInfo(pExtStr);

    return(temp);
}

*/

/*
-----------------------------------------------------------------------------------------------
  Function name :  INT16 SysFindFileInit(SYS_FILE_INFO *pSysFileInfo, UINT16
                       GlobalFileNum,UINT16 FindFileRange,UINT16 FindFileMode,uint8 *pExtStr)
  Author        :  zs
  Description   :  initial the search variables when enter module.
  Input         :  SYS_FILE_INFO *pSysFileInfo : return file information.
                   UINT16        GlobalFileNum : Input: global file number.
                   UINT16        FindFileRange : Input: the range to find file
                   UINT16        FindFileMode  : Input: random mode
                   uint8         *pExtStr      : Input: file type

  Return        :  return value:0
  History       :  <author>         <time>         <version>
                     zs            2009/02/27         Ver1.0
  desc          : org
-------------------------------------------------------------------------------------------------
*/
_ATTR_SYS_FINDFILE_TEXT_
INT16 SysFindFileInit(SYS_FILE_INFO *pSysFileInfo,UINT16 GlobalFileNum,UINT16 FindFileRange,UINT16 PlayMode, uint8 *pExtStr)
{
    UINT16 i,j;
    UINT16 tempFileNum;
    INT16  FindFileResult;
    INT16  RetVal = 0;

    pSysFileInfo->TotalFiles = BuildDirInfo(pExtStr);

    if (0 == pSysFileInfo->TotalFiles)
    {
        return -1;
    }

    pSysFileInfo->DiskTotalFiles = pSysFileInfo->TotalFiles;
    pSysFileInfo->CurrentFileNum = GlobalFileNum;
    pSysFileInfo->pExtStr        = pExtStr;
    pSysFileInfo->Range          = FindFileRange;
    pSysFileInfo->PlayOrder       = PlayMode;


    if (pSysFileInfo->CurrentFileNum > pSysFileInfo->TotalFiles)
    {
        pSysFileInfo->CurrentFileNum = 1;
    }

    tempFileNum = GetCurFileNum(pSysFileInfo->CurrentFileNum,pExtStr);
    GetDirPath(pSysFileInfo->Path);

    FindFileResult = FindFirst(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->Path, pExtStr);

    for(i = 1; i < tempFileNum; i++)
    {
        FindFileResult = FindNext(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pExtStr);
    }

    if (pSysFileInfo->Range == FIND_FILE_RANGE_DIR)
    {
        pSysFileInfo->CurrentFileNum = tempFileNum;
        pSysFileInfo->TotalFiles = GetTotalFiles(pSysFileInfo->Path,pExtStr);
    }

    pSysFileInfo->PlayedFileNum = pSysFileInfo->CurrentFileNum;

    return 0;
}


/*
--------------------------------------------------------------------------------
  Function name : INT16 SysFindFileExt(SYS_FILE_INFO *pSysFileInfo,INT16 Offset)
  Author        :  zs
  Description   :  search file.
  Input         :  SYS_FILE_INFO *pSysFileInfo:structure to find file.
                   Offset == 0   find current file.
                   Offset > 0    find backward file,the offset is the offset file number relatived to the current file.
                   Offset < 0    find forward file,the offset is the offset file number relatived to the current file.

  Return        :
  History       :  <author>         <time>         <version>
                     zs            2009/02/27         Ver1.0
  desc          : org
--------------------------------------------------------------------------------
*/

_ATTR_SYS_FINDFILE_TEXT_
INT16 SysFindFileExt(SYS_FILE_INFO *pSysFileInfo,INT16 Offset)
{
    UINT16  i, tempFileNum;

    UINT16  uiNeedFindNext = 1;
    INT16   FindFileResult = -1;

    if(Offset == 0)
    {
        return (RETURN_OK);
    }

    //���ݲ���˳��Ͳ���ģʽ(Ŀ¼����ȫ��)������ǰ�����ļ����ļ���
    if(pSysFileInfo->PlayOrder == AUDIO_RAND) //�������   azg 8.18
    {
//        if(GetMsg(MSG_MEDIA_BREAKPOINT_PLAY)==FALSE)
        {
            //pSysFileInfo->CurrentFileNum = randomGenerator(Offset, pSysFileInfo->TotalFiles)+1;
            pSysFileInfo->PlayedFileNum ++ ;
        }
        if(pSysFileInfo->CurrentFileNum > pSysFileInfo->TotalFiles)
        {
            pSysFileInfo->CurrentFileNum = 1;
        }
    }
    else //˳�򲥷�
    {
        if(Offset > 0) //��һ��
        {
            pSysFileInfo->CurrentFileNum ++ ;

            if (pSysFileInfo->CurrentFileNum > pSysFileInfo->TotalFiles)
            {
                pSysFileInfo->CurrentFileNum = 1;
                #ifdef MEDIA_MODULE
                if(pSysFileInfo->ucSelPlayType == SORT_TYPE_SEL_BROWSER)//��Դ��������ʽ�£���Ҫ���ļ�·��ָ�ص�һ���ļ���·��
                #endif
                {
                    if(pSysFileInfo->Range == FIND_FILE_RANGE_DIR)//Ŀ¼��ѭ��  //�ļ����ҷ�Χ��һ���ļ���
                    {
                        //�������һ�������ش�ͷ��ʼ��Ŀ¼�еĵ�һ�׸�
                        FindFileResult = FindFirst(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->Path, pSysFileInfo->pExtStr);
                        uiNeedFindNext = 0;
                    }
                    else
                    {
                        tempFileNum = GetCurFileNum(pSysFileInfo->CurrentFileNum,pSysFileInfo->pExtStr);
                        GetDirPath(pSysFileInfo->Path);
                        FindFileResult = FindFirst(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->Path, pSysFileInfo->pExtStr);
                        uiNeedFindNext = FindFileResult;
                    }
                }

                #ifdef MEDIA_MODULE
                #ifdef _RECORD_
                if(pSysFileInfo->ucSelPlayType == MUSIC_TYPE_SEL_RECORDFILE)//ý����е�¼��Ҳ��Ҫָ�ص�һ���ļ�
                {
                    FindFileResult = FindFirst(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->Path, pSysFileInfo->pExtStr);
                    uiNeedFindNext = 0;
                }
                #endif
                #endif
            }

        }
        else if (Offset < 0) //��һ��
        {

            pSysFileInfo->CurrentFileNum -- ;

            if (pSysFileInfo->CurrentFileNum == 0)
            {
                pSysFileInfo->CurrentFileNum = pSysFileInfo->TotalFiles;
            }
        }
        pSysFileInfo->PlayedFileNum = pSysFileInfo->CurrentFileNum;
    }

    #ifdef MEDIA_MODULE
    pSysFileInfo->uiCurId[pSysFileInfo->ucCurDeep]= pSysFileInfo->CurrentFileNum-1;//7.3 azg add

    #ifdef _RECORD_
    // ----���¼���ļ������ļ�������ǰһ�׸������
    if((pSysFileInfo->ucSelPlayType == MUSIC_TYPE_SEL_RECORDFILE)&&(pSysFileInfo->PlayOrder == AUDIO_RAND || Offset < 0))
    {
        tempFileNum = pSysFileInfo->CurrentFileNum;

        FindFileResult = FindFirst(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->Path,  pSysFileInfo->pExtStr);
        for (i=1; i<tempFileNum; i++)
        {
            FindFileResult = FindNext(&pSysFileInfo->Fdt, &pSysFileInfo->FindData,  pSysFileInfo->pExtStr);
        }
        uiNeedFindNext = 0;
    }
    #endif

    #endif

    //�����ļ��ſ�ʼ���ļ�����ȡ·���Ͷ��ļ�����Ϣ��Ϊ����Ĵ��ļ���׼��
    #ifdef MEDIA_MODULE
    if(pSysFileInfo->ucSelPlayType != SORT_TYPE_SEL_BROWSER)
    {
        switch(pSysFileInfo->ucSelPlayType)
        {
           case SORT_TYPE_SEL_NOW_PLAY:
           case SORT_TYPE_SEL_FILENAME:
           case SORT_TYPE_SEL_ID3TITLE:
           case SORT_TYPE_SEL_ID3SINGER:
           case SORT_TYPE_SEL_ID3ALBUM:
           case SORT_TYPE_SEL_GENRE:
                i = 0;
                do
                {
                    UINT8 ucBufTemp[2];
                    UINT16 temp1;
                    UINT8 FileInfoBuf[110];

                    MDReadData(DataDiskID, (pSysFileInfo->ulSortInfoSectorAddr<<9)+(UINT32)((pSysFileInfo->CurrentFileNum + pSysFileInfo->uiBaseSortId-1)*2), 2, ucBufTemp);
                    temp1 = (ucBufTemp[0]&0xff)+((ucBufTemp[1]&0xff)<<8); // ��ȡ�ļ��� (����ϸ�ļ���Ϣ���е�λ��)
                    MDReadData(DataDiskID, (pSysFileInfo->ulFullInfoSectorAddr<<9)+(UINT32)(temp1)*512, 80, FileInfoBuf);
                    memcpy(MusicLongFileName,FileInfoBuf,80);

                    MDReadData(DataDiskID, (pSysFileInfo->ulFullInfoSectorAddr<<9)+(UINT32)(temp1)*512+(UINT32)PATH_ADDR_OFFSET, (PATH_SIZE+11), FileInfoBuf);
                    FindFileResult =  FindFileByShortPath(&(pSysFileInfo->Fdt), &FileInfoBuf[0], &FileInfoBuf[PATH_SIZE]);
                    memcpy(pSysFileInfo->Path,FileInfoBuf,PATH_SIZE);


                   if(FindFileResult==RETURN_OK)
                        break; // �ҵ��ɲ��Ÿ���

                   if(i==pSysFileInfo->TotalFiles)
                        break; // ��ѭ�����и�������û�пɲ�������ʱ�˳��ó���

                   i++;
                }while(1);

                break;

            case MUSIC_TYPE_SEL_MYFAVORITE:
                pSysFileInfo->TotalFiles = gSysConfig.MedialibPara.gMyFavoriteFileNum; // ��ֹ�ڲ�������ʱɾ�����ղؼи���

                if(pSysFileInfo->TotalFiles ==0)
                {
                    break;
                }

                i = 0;
                do
                {
                   FindFileResult = GetFavoInfo(&(pSysFileInfo->Fdt), (pSysFileInfo->CurrentFileNum+ pSysFileInfo->uiBaseSortId - 1),  pSysFileInfo->Path , MusicLongFileName);

                   if(FindFileResult==RETURN_OK) break; // �ҵ��ɲ��Ÿ���

                   if(pSysFileInfo->TotalFiles ==0)  break; // �����ղؼ�����ʱ����������ղؼ�
                   i++;
                }while(1);
                break;

            #ifdef _RECORD_
            case MUSIC_TYPE_SEL_RECORDFILE:
                if ((uiNeedFindNext) && (Offset > 0))
                {
                   FindFileResult = FindNext(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->pExtStr);
                   if (FindFileResult == NOT_FIND_FILE)
                   {
                      tempFileNum = GetCurFileNum(pSysFileInfo->CurrentFileNum,pSysFileInfo->pExtStr);
                      GetDirPath(pSysFileInfo->Path);
                      FindFileResult = FindFirst(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->Path, pSysFileInfo->pExtStr);
                   }
                }
                memcpy((UINT8*)MusicLongFileName,&LongFileName,MAX_FILENAME_LEN*2);
                break;
            #endif

            default:
                break;
        }

    }
    else
    #endif
    {
        if((Offset < 0) || (pSysFileInfo->PlayOrder == AUDIO_RAND))
        {

            tempFileNum = pSysFileInfo->CurrentFileNum;
            if (pSysFileInfo->Range != FIND_FILE_RANGE_DIR)//Ŀ¼��ѭ��
            {
                tempFileNum = GetCurFileNum(pSysFileInfo->CurrentFileNum,pSysFileInfo->pExtStr);
                GetDirPath(pSysFileInfo->Path);
            }

            FindFileResult = FindFirst(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->Path, pSysFileInfo->pExtStr);
            for (i = 1; i < tempFileNum; i++)
            {
                FindFileResult = FindNext(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->pExtStr);
            }
            uiNeedFindNext = 0;
        }

        if ((uiNeedFindNext) && (Offset > 0))
        {

            FindFileResult = FindNext(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->pExtStr);

            if (FindFileResult == NOT_FIND_FILE) /* û�з���ָ���ļ�*/
            {

                tempFileNum = GetCurFileNum(pSysFileInfo->CurrentFileNum,pSysFileInfo->pExtStr);
                GetDirPath(pSysFileInfo->Path);
                FindFileResult = FindFirst(&pSysFileInfo->Fdt, &pSysFileInfo->FindData, pSysFileInfo->Path, pSysFileInfo->pExtStr);
            }
        }
    }
    return(FindFileResult);
}

/*
--------------------------------------------------------------------------------
  Function name : INT16 SysFindFile(SYS_FILE_INFO *pSysFileInfo,    INT16 Offset)
  Author        :  zs
  Description   :  �����ļ�
  Description   :  search file.
  Input         :  SYS_FILE_INFO *pSysFileInfo:structure to find file.
                   Offset == 0   find current file.
                   Offset > 0    find backward file,the offset is the offset file number relatived to the current file.
                   Offset < 0    find forward file,the offset is the offset file number relatived to the current file.

  Return        :
  History       :  <author>         <time>         <version>
                     zs            2009/02/27         Ver1.0
  desc          :   org
--------------------------------------------------------------------------------
*/
_ATTR_SYS_FINDFILE_TEXT_
INT16 SysFindFile(SYS_FILE_INFO *pSysFileInfo,    INT16 Offset)
{

    UINT16  i, tempFileNum;
    INT16   FindFileResult = -1;

    if(Offset == 0)
        return (RETURN_OK);

    if(Offset<0)
    {
        while(Offset++)
        {
            FindFileResult =  SysFindFileExt(pSysFileInfo,-1);

            if(FindFileResult != RETURN_OK)
                return FindFileResult;
        }
    }
    else
    {
        while(Offset--)
        {
            FindFileResult =  SysFindFileExt(pSysFileInfo,1);

            if(FindFileResult != RETURN_OK)
                return FindFileResult;
        }
    }
}

/*
********************************************************************************
*  Copyright (C),2004-2007, Fuzhou Rockchip Electronics Co.,Ltd.
*  Function name :  GetDirPath()
*  Author:          ZHengYongzhi
*  Description:     get file full path
*
*  Input:   pPath           ���� store the start address of file path.
*  Output:  pPath           ���� file path
*  Return:
*  Calls:
*
*  History:     <author>         <time>         <version>
*             ZhengYongzhi     2006/01/01         Ver1.0
*     desc: ORG
********************************************************************************
*/
//_ATTR_SYS_FINDFILE_TEXT_
void GetDirPath(UINT8 *pPath)
{
    UINT16 i,j;

    *pPath++ = 0x55;    //'U';
    *pPath++ = 0x3a;    //':';
    *pPath++ = 0x5c;    //'\\';
    for (i = 1; i <= CurDirDeep; i++)
    {
        for (j = 0; j < 11 ; j++ )
        {
            *pPath++ = SubDirInfo[i].DirName[j];
        }
        *pPath++ = 0x5c;//'\\';
    }
    if (CurDirDeep != 0)
    {
        pPath--;
    }
    *pPath= 0;
}

/*
--------------------------------------------------------------------------------
  Function name : UINT8 GetFileType(UINT8 *pBuffer, UINT8 *pStr)
  Author        : zs
  Description   : get file type.
  Input         : null
  Return        : null
  History       : <author>         <time>         <version>
                    zs            2009/02/20         Ver1.0
  desc          :         ORG
--------------------------------------------------------------------------------
*/
UINT8 GetFileType(UINT8 *pBuffer, UINT8 *pStr)
{
    UINT8 Len;
    UINT8 Retval = 0xff;
    UINT8 i;

    i = 0;
    Len = strlen((char*)pStr);

    while (i <= Len)
    {
        i += 3;
        if ((*(pBuffer + 0) == *(pStr + 0)) && (*(pBuffer + 1) == *(pStr + 1)) &&
            (*(pBuffer + 2) == *(pStr + 2)))
        {
            break;
        }
        pStr += 3;
    }

    if (i <= Len)
   {
        Retval = i / 3;
    }
    return (Retval);
}

/*
********************************************************************************
*
*                         End of SysFindFile.c
*
********************************************************************************
*/

