/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  SearchAndSaveFileInfo.C
*
* Description:
*
* History:      <author>          <time>        <version>
*               anzhiguo      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_FILEINFOSAVE_

#include "SysInclude.h"

#ifdef  MEDIA_MODULE


#include "FsInclude.h"

#include "FileInfoSaveMacro.h"
#include "AddrSaveMacro.h"
#include "FileInfoSaveStruct.h"
#include "FileInfoSaveGloable.h"
#include "FileInfoSortStruct.h"

#include "FileInfoSortMacro.h"


#include "FileInfo.h"

#include "SysFindFile.h"
#include "AudioControl.h"
#include "wma_parse.h"

extern void MedialibUpdataUi(UINT16 PicId);

extern UINT32 GetCmpResult(UINT16 wch);
extern int PinyinCharCmp(UINT16 wch1, UINT16 wch2);
extern UINT32 PinyinStrnCmp(UINT16 *str1, UINT16 *str2, UINT32 length);
extern UINT8 BrowserListInsertBySort(FILE_INFO_INDEX_STRUCT *head, FILE_INFO_INDEX_STRUCT *pNode);
extern void SortPageBufferInit(UINT8 *pBuffer);
extern void ChildChainInit(void);
extern void GetSortName(UINT32 SectorOffset,UINT16 FileNum);
extern void SortFunction(UINT32 uiSaveAddrOffset,UINT16 FileNum);
extern void SortUpdateFun(MEDIALIB_CONFIG * Sysfilenum ,UINT32 MediaInfoAddress);

extern	UINT16 gChildChainDivValue[CHILD_CHAIN_NUM];
extern	FILE_INFO_INDEX_STRUCT  *pChildChainHead[CHILD_CHAIN_NUM];
extern	FILE_INFO_INDEX_STRUCT 	gChildChainHead[CHILD_CHAIN_NUM];  

extern	FILE_INFO_INDEX_STRUCT	gSortNameBuffer0[SORT_FILE_NAME_BUF_SIZE];

extern	UINT8 	gPageTempBuffer[MEDIAINFO_PAGE_SIZE]; 
extern	UINT8 	gSubPageBuffer[MEDIAINFO_PAGE_SIZE];  


/*
--------------------------------------------------------------------------------
  Function name : void MediaGetLongFileName(UINT16 * flname)
  Author        : anzhiguo
  Description   : get long file name from media

  Input         :
  Return        :

  History:     <author>         <time>         <version>
                anzhiguo     2009/06/02         Ver1.0
  desc:
--------------------------------------------------------------------------------
*/
_FILE_INFO_SAVE_CODE_
void MediaGetLongFileName(UINT16 * flname)
{
    memcpy(flname,&LongFileName,MAX_FILENAME_LEN*2);
}

/*
--------------------------------------------------------------------------------
  Function name : void FileSaveStructInit(FILE_SAVE_STRUCT  *pFileSaveTemp)
  Author        : anzhiguo
  Description   : initial the structure variable that save file infomation. 

  Input         : pFileSaveTemp :structure variable to keeping file 
  Return        :

  History:     <author>         <time>         <version>
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_FILE_INFO_SAVE_CODE_
void FileSaveStructInit(MEDIA_FILE_SAVE_STRUCT  *pFileSaveTemp)
{
    UINT32  i;

    for (i=0;i<ID3_TAG_SIZE;i++)
    {
        pFileSaveTemp->id3_title[i] = 0;
        pFileSaveTemp->id3_singer[i] = 0;
        pFileSaveTemp->id3_album[i] = 0;
        pFileSaveTemp->Genre[i] = 0;
    }
    pFileSaveTemp->DirClus = 0;
    pFileSaveTemp->Index = 0;
}
/*
--------------------------------------------------------------------------------
  Function name : void PageWriteBufferInit(void)
  Author        : anzhiguo
  Description   : buffer initialization that be used to write flash page.

  Input         :
  Return        :

  History:     <author>         <time>         <version>
                anzhiguo     2009/06/02         Ver1.0
  desc:        
--------------------------------------------------------------------------------
*/
_FILE_INFO_SAVE_CODE_
void PageWriteBufferInit(void)
{
    UINT32  i;

    for (i=0;i<MEDIAINFO_PAGE_SIZE;i++)
    {
        gPageWriteBuffer[i]=0;
    }
}


/*
--------------------------------------------------------------------------------
  Function name : unsigned char SaveFileInfo(unsigned char *Buffer, FILE_SAVE_STRUCT  *pFileSaveTemp)
  Author        : anzhiguo
  Description   : wirte the file information that need to save to cache buffer,the information include 
                  long file name,ID3Title,ID3singer,ID3Album,file path,short file name.

  Input         : Buffer：Buffer to save file information.
                  pFileSaveTemp：the information of current file.
  Return        : return 1

  History:     <author>         <time>         <version>
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_FILE_INFO_SAVE_CODE_
UINT8 SaveFileInfo(UINT8 *Buffer, MEDIA_FILE_SAVE_STRUCT  *pFileSaveTemp)
{
    UINT32 i;
    UINT8 *pBuffer,*pPath;

    pBuffer = Buffer;

    for (i=0;i<MEDIA_CHAR_NUM_PER_FILE_NAME;i++)//保存长文件名信息
    {
        if (pFileSaveTemp->LongFileName[i] == 0)
        {
            break;
        }
        
        *pBuffer++ = pFileSaveTemp->LongFileName[i]&0xff;//低字节
        *pBuffer++ = ((pFileSaveTemp->LongFileName[i])>>8)&0xff;//高字节
    }
    //pBuffer = Buffer+ID3_TITLE_SAVE_ADDR_OFFSET这样做是防止LongFileName没有用掉其最大空间，引起存储信息位置不一
    pBuffer = Buffer+ID3_TITLE_SAVE_ADDR_OFFSET;//长文件名保存占用的空间是CHAR_NUM_PER_FILE_NAME的2倍

    for (i=0;i<ID3_TAG_SIZE;i++)//保存id3的title信息
    {
        if (pFileSaveTemp->id3_title[i] == 0)
        {
            break;
        }

        *pBuffer++ = (pFileSaveTemp->id3_title[i])&0xff;
        *pBuffer++ = (pFileSaveTemp->id3_title[i])>>8;
    }

    pBuffer = Buffer + ID3_SINGLE_SAVE_ADDR_OFFSET;

    for (i=0;i < ID3_TAG_SIZE ; i++)//保存id3的singer信息
    {
        if (pFileSaveTemp->id3_singer[i] == 0)
        {
            break;
        }

        *pBuffer++ = (pFileSaveTemp->id3_singer[i])&0xff;
        *pBuffer++ = (pFileSaveTemp->id3_singer[i])>>8;
    }

    pBuffer = Buffer+ID3_ALBUM_SAVE_ADDR_OFFSET;
    for (i=0;i<ID3_TAG_SIZE;i++)//保存id3的album信息
    {
        if (pFileSaveTemp->id3_album[i] == 0)
        {
            break;
        }

        *pBuffer++ = (pFileSaveTemp->id3_album[i])&0xff;
        *pBuffer++ = (pFileSaveTemp->id3_album[i])>>8;
    }

    pBuffer = Buffer+ID3_GENRE_SAVE_ADDR_OFFSET;
    for (i=0;i<ID3_TAG_SIZE;i++)//保存id3的album信息
    {
        if (pFileSaveTemp->Genre[i] == 0)
        {
            break;
        }

        *pBuffer++ = (pFileSaveTemp->Genre[i])&0xff;
        *pBuffer++ = (pFileSaveTemp->Genre[i])>>8;
    }


    pBuffer = Buffer+PATH_SAVE_ADDR_OFFSET;//偏移了260个字节
    pPath = AudioFileInfo.Path;

    while (*pPath!=0)//保存文件的路径信息
    {
        *pBuffer++ = *pPath++;
    }
    *pBuffer = 0;

    pBuffer = Buffer+SHORT_NAME_SAVE_ADDR_OFFSET;
    for (i=0;i<11;i++)//保存文件名
    {
        *pBuffer++ = AudioFileInfo.Fdt.Name[i];
    }

    pBuffer = Buffer+FILENUM_SAVE_ADDR_OFFSET;

    return 1;
}

/*
--------------------------------------------------------------------------------
  Function name : HANDLE FileOpenByFileFDT(FDT FileFDT, uint8 *Type)
  Author        : anzhiguo
  Description   : open the file by specified mode.

  Input         : FileFDT:FDF item of current file.
                  Type:open mode
  Return        : Not_Open_FILE:can not open

  History:     <author>         <time>         <version>
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
//it is the best to save this to file system,as it is still need in finding next file when playing. 
_FILE_INFO_SAVE_CODE_
HANDLE FileOpenByFileFDT(FDT FileFDT, uint8 *Type)
{
    uint8 i;
    MY_FILE *fp;
    HANDLE Rt, OsRt;

    OsRt=NOT_OPEN_FILE;

    // 查找空闲文件登记项
    for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
    {
        if (FileInfo[Rt].Flags == 0)
        {
            break;
        }
    }

    if (Rt < MAX_OPEN_FILES)
    {
        fp = FileInfo + Rt;

        for (i=0; i<11; i++)
        {
            fp->Name[i]=FileFDT.Name[i];
        }
        
        fp->Flags = FILE_FLAGS_READ;
        if (Type[0] == 'W' || Type[1] == 'W')
        fp->Flags |= FILE_FLAGS_WRITE;

        fp->FileSize = FileFDT.FileSize;
        fp->FstClus = FileFDT.FstClusLO | (uint32)FileFDT.FstClusHI << 16;
        fp->Clus = fp->FstClus;
        fp->Offset = 0;
        fp->RefClus = fp->Clus;
        fp->RefOffset = 0;
        OsRt=Rt;

    }

    return (OsRt);
}

/*
--------------------------------------------------------------------------------
  Function name : void SearchAndSaveFileInfo(void)
  Author        : anzhiguo
  Description   : check all file information,and save it to specified flash,at the same time to remember 
                  the information sort by file name to memory.
                  
  Input         :
  Return        :

  History:     <author>         <time>         <version>
                anzhiguo     2009/06/02         Ver1.0
  desc:
--------------------------------------------------------------------------------
*/
_FILE_INFO_SAVE_CODE_
void SearchAndSaveFileInfo(void)
{
    UINT32  i,j;
    UINT32 DirClus, index;
    HANDLE hMusicFile;
    MEDIA_FILE_SAVE_STRUCT  *pFileSaveTemp = &gFileSaveInfo; // 用于保存文件信息

    UINT16 uiMusicFileCount = 0;  // 音乐文件个数计数
    UINT16 uiFindFileResult = 0 ; // 文件检索结果

    UINT16 uiFileIndex = 0;       // Flash Page控制计数     该值等于4时，已经将一个page大小的baffer写满，可以写flash了
    UINT32 ulFileFullInfoSectorAddr = 0; // 保存文件详细信息的起始sector地址

    UINT16 uiHundredFlag=0;
    UINT16 uiTenFlag=0;
    UINT16 uiCountTemp=0;

    FILE_SAVE_STRUCT ID3FileInfo;

    ulFileFullInfoSectorAddr = MediaInfoAddr + FILE_SAVE_INFO_SECTOR_START; //保存详细的文件信息起始地址(sec值)

    PageWriteBufferInit();

    GotoRootDir(AudioFileExtString); // 获取根目录下音乐文件的信息
    GetDirPath(AudioFileInfo.Path);	// 获取当前路径  /????????这里会不会有问题

    FindDataInfo.Clus=CurDirClus; // 当前目录首簇号
    FindDataInfo.Index=0;	// 从第一个FDT索引项开始

    uiFindFileResult = FindNext(&AudioFileInfo.Fdt, &FindDataInfo, AudioFileExtString);

    while (uiMusicFileCount<SORT_FILENUM_DEFINE) //  for(i = 0 ;i < TotalFiles ; )
    {
        if (uiFindFileResult==RETURN_OK)
        {
            hMusicFile = FileOpenByFileFDT(AudioFileInfo.Fdt, "R"); // 改写的FileOpen函数搜索速度大为提高

            if (hMusicFile != NOT_OPEN_FILE)//成功打开文件//打开文件并解析ID3信息
            {
                #ifdef _RK_ID3_
                ModuleOverlay(MODULE_ID_AUDIO_ID3, MODULE_OVERLAY_ALL); //调用ID3 解析代码
                memset((UINT8*)&ID3FileInfo, 0, sizeof(FILE_SAVE_STRUCT));
                GetAudioId3Info(hMusicFile , &ID3FileInfo, &AudioFileInfo.Fdt.Name[8]);
                #endif             
            }//打开文件并解析ID3信息

            FileClose(hMusicFile);
            hMusicFile = -1;
            FileSaveStructInit(pFileSaveTemp); // 将用于记录文件保存信息的结构体清0
            MediaGetLongFileName(pFileSaveTemp->LongFileName);// 获取保存的长文件名信息
            

            for (i=0;i<ID3_TAG_SIZE;i++)
            {
                pFileSaveTemp->id3_title[i] = ID3FileInfo.ID3Title[i]; // 保存歌手信息
                pFileSaveTemp->id3_singer[i] = ID3FileInfo.ID3Artist[i]; // 保存歌手信息
                pFileSaveTemp->id3_album[i] = ID3FileInfo.ID3Album[i];  // 保存专辑信息
                pFileSaveTemp->Genre[i] =ID3FileInfo.Genre[i];// 保存专辑信息
            }

            if (pFileSaveTemp->id3_title[0]==0)
            {
                memcpy(pFileSaveTemp->id3_title,pFileSaveTemp->LongFileName,ID3_TAG_SIZE*2)	;
            }
            //读到的歌手，专辑，流派为other时，清零方便后面排序和ui上的处理
            if (((pFileSaveTemp->id3_singer[0]==0x006f)||(pFileSaveTemp->id3_singer[0]==0x004f))
                &&((pFileSaveTemp->id3_singer[1]==0x0074)||(pFileSaveTemp->id3_singer[1]==0x0054))
                &&((pFileSaveTemp->id3_singer[2]==0x0068)||(pFileSaveTemp->id3_singer[2]==0x0048))
                &&((pFileSaveTemp->id3_singer[3]==0x0065)||(pFileSaveTemp->id3_singer[3]==0x0045))
                &&((pFileSaveTemp->id3_singer[4]==0x0072)||(pFileSaveTemp->id3_singer[4]==0x0052))
                &&(pFileSaveTemp->id3_singer[5]==0x0000))
            {
                memset(pFileSaveTemp->id3_singer,0,ID3_TAG_SIZE*2)	;
            }

            if (((pFileSaveTemp->id3_album[0]==0x006f)||(pFileSaveTemp->id3_album[0]==0x004f))
                &&((pFileSaveTemp->id3_album[1]==0x0074)||(pFileSaveTemp->id3_album[1]==0x0054))
                &&((pFileSaveTemp->id3_album[2]==0x0068)||(pFileSaveTemp->id3_album[2]==0x0048))
                &&((pFileSaveTemp->id3_album[3]==0x0065)||(pFileSaveTemp->id3_album[3]==0x0045))
                &&((pFileSaveTemp->id3_album[4]==0x0072)||(pFileSaveTemp->id3_album[4]==0x0052))
                &&(pFileSaveTemp->id3_album[5]==0x0000))
            {
                memset(pFileSaveTemp->id3_album,0,ID3_TAG_SIZE*2)  ;
            }

            if (((pFileSaveTemp->Genre[0]==0x006f)||(pFileSaveTemp->Genre[0]==0x004f))
                &&((pFileSaveTemp->Genre[1]==0x0074)||(pFileSaveTemp->Genre[1]==0x0054))
                &&((pFileSaveTemp->Genre[2]==0x0068)||(pFileSaveTemp->Genre[2]==0x0048))
                &&((pFileSaveTemp->Genre[3]==0x0065)||(pFileSaveTemp->Genre[3]==0x0045))
                &&((pFileSaveTemp->Genre[4]==0x0072)||(pFileSaveTemp->Genre[4]==0x0052))
                &&(pFileSaveTemp->Genre[5]==0x0000))
            {
                memset(pFileSaveTemp->Genre,0,ID3_TAG_SIZE*2)  ;
            }
            
            SaveFileInfo(&gPageWriteBuffer[uiFileIndex<<BYTE_PER_FILE_SHIFT], pFileSaveTemp); // 保存文件信息及排序索引信息 把获取的当前文件的信息存于全局变量gPageWriteBuffer的某个位置
            uiMusicFileCount++; // 文件个数增加一个
            uiFileIndex++;


            if (uiFileIndex == FILE_SAVE_NUM_PER_PAGE)//gPageWriteBuffer 已经存满信息，大小为8k ，就开始写flash
            {
                uiFileIndex = 0;//文件序号回零
                MDWrite(DataDiskID, ulFileFullInfoSectorAddr, (MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE),gPageWriteBuffer);//gPageTempBuffer);// 
                #if 0
                for (j=0;j<MEDIAINFO_PAGE_SIZE;j++)//gPageWriteBuffer 清零
                {
                    gPageWriteBuffer[j]=0;
                }
                #endif
                memset((uint8*)gPageWriteBuffer, 0, MEDIAINFO_PAGE_SIZE);
                ulFileFullInfoSectorAddr  +=  (MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE);//指向下一个page，为下次写做准备
            }

            uiCountTemp = uiMusicFileCount;
            if ( uiHundredFlag == (uiCountTemp/20))//ui显示case ，每找到20个文件刷一张图片
            {
                MedialibUpdataDisplay(uiTenFlag);
                ++uiTenFlag;

                if (uiTenFlag==3)
                {
                    uiTenFlag = 0;
                }
                ++uiHundredFlag;
            }
            uiFindFileResult = FindNext(&AudioFileInfo.Fdt, &FindDataInfo, AudioFileExtString);

        }

        else if (uiFindFileResult == NOT_FIND_FILE)
        {
            GotoNextDir(AudioFileExtString);			// 遍历下一个目录，找完子目录再找同级目录
            if (CurDirDeep == 0) // 所有目录遍历完回到了根目录
            {
                break;
            }
            while (1)
            {
                if ((SubDirInfo[1].DirName[0] != 'V') ||
                (SubDirInfo[1].DirName[1] != 'O') ||
                (SubDirInfo[1].DirName[2] != 'I') ||
                (SubDirInfo[1].DirName[3] != 'C') ||
                (SubDirInfo[1].DirName[4] != 'E') )// 过滤录音目录下的文件
                //(SubDirInfo[1].DirName[5] != 'D'))    // 过滤录音目录下的文件
                {
                    break;
                }
                GotoNextDir(AudioFileExtString);
                if (CurDirDeep == 0)
                {
                    break;
                }
            }
            if (CurDirDeep == 0) // 所有目录遍历完回到了根目录
            {
                break;
            }

            GetDirPath(AudioFileInfo.Path);	// 获取当前路径
            FindDataInfo.Clus=CurDirClus;
            FindDataInfo.Index=0;
            uiFindFileResult = FindNext(&AudioFileInfo.Fdt, &FindDataInfo, AudioFileExtString);

        }

    }

    if (uiFileIndex) // 保存不足2K的文件信息
    {
        MDWrite(DataDiskID, ulFileFullInfoSectorAddr, (MEDIAINFO_PAGE_SIZE/512),gPageWriteBuffer);//gPageTempBuffer);// 
    }
    
    gSysConfig.MedialibPara.gMusicFileNum =uiMusicFileCount; // 得到系统全部文件数目，最大为SORT_FILENUM_DEFINE个

}



/*
--------------------------------------------------------------------------------
  Function name : UINT32 SaveSortInfo(UINT32 uiSaveType, UINT32 ulSortSectorAddr)
  Author        : anzhiguo
  Description   : save sort information

  Input         : uiSaveType:information type : ID3_TITLE_TYPE :FILE_NAME_TYPE:ID3_ARTIST_TYPE:ID3_ALBUM_TYPE four type
                  ulSortSectorAddr save address.
  Return        :

  History:     <author>         <time>         <version>
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_FILE_INFO_SORT_CODE_
UINT32 SaveSortInfo(UINT32 uiSaveType, UINT32 ulSortSectorAddr,UINT32 MediaInfoAddress)//如果uiSaveType是艺术家或专辑(ID3_ARTIST_TYPE或ID3_ALBUM_TYPE)会保存分类信息
{
    UINT32  i,j;

    UINT32 uiFileCount = 0;
    UINT32 uiFileSubCount = 0;  // 文件个数计数
    UINT32 uiSameCount = 0;     // ID3信息归类时，相同类型计数
    UINT32 uiFileIndex = 0;     // Flash Page控制计数
    UINT16 uiFileSortTemp = 0;  // 文件排序号临时变量

    UINT32 uiSubCount = 0;      // 用于每条子链的文件ID3信息分类计数
    UINT32 uiSubCountTotal = 0; // 用于ID3信息分类的所有文件计数

    UINT32 ulID3SubSectorAddr = 0; // ID3信息归类信息保存地址

    UINT16 PreSortFileName[SORT_FILENAME_LEN];

    FILE_INFO_INDEX_STRUCT  *pTemp = NULL; // 排序链表头指针

    SortPageBufferInit(gPageTempBuffer);

    SortPageBufferInit(gSubPageBuffer);

    switch (uiSaveType)
    {
        case FILE_NAME_TYPE:
        case ID3_TITLE_TYPE:
            i = 1;
            break;
        case ID3_ARTIST_TYPE:
            i = 0;
            ulID3SubSectorAddr = MediaInfoAddress+ID3ARTIST_SORT_SUB_SECTOR_START;
            break;
        case ID3_ALBUM_TYPE:
            i = 0;
            ulID3SubSectorAddr = MediaInfoAddress+ID3ALBUM_SORT_SUB_SECTOR_START;
            break;
        case ID3_GENRE_TYPE:
            i = 0;
            ulID3SubSectorAddr = MediaInfoAddress+ID3GENRE_SORT_SUB_SECTOR_START;
            break;
        default:
            break;
    }

    for (;i<CHILD_CHAIN_NUM;i++)
    {
        pTemp = pChildChainHead[i];

        while (pTemp->pNext!=NULL)
        {
            pTemp = pTemp->pNext;
            uiFileSortTemp = (UINT16)(pTemp - &gSortNameBuffer0[0]); //计算当前的文件在所有文件中的排序号

            //计算当前要保存的信息与gSortNameBuffer0[0]的偏移地址，因为pChildChainHead链表的信息来源于gSortNameBuffer0
            gPageTempBuffer[uiFileIndex++] = uiFileSortTemp & 0xFF;     //保存文件排序后的序号
            gPageTempBuffer[uiFileIndex++] = (uiFileSortTemp>>8) & 0xFF;//保存文件排序后的序号

            switch (uiSaveType)
            {
                case FILE_NAME_TYPE:
                case ID3_TITLE_TYPE:
                    uiFileSubCount++;
                    break;
                case ID3_ARTIST_TYPE://同一个艺术家 或专辑有几个文件，保存其第一个文件的偏移地址，并保存该专辑下的文件个数
                case ID3_ALBUM_TYPE:
                case ID3_GENRE_TYPE	:
                    if (0==uiFileSubCount)
                    {
                        gSubPageBuffer[uiSubCount++] = uiFileSortTemp&0xFF; // 记录文件保存号
                        gSubPageBuffer[uiSubCount++] = (uiFileSortTemp>>8)&0xFF;// 记录文件保存号
                        gSubPageBuffer[uiSubCount++] = 0;
                        gSubPageBuffer[uiSubCount++] = 0;
                        uiFileSubCount++;
                    }
                    //PinyinStrnCmp 返回 1 表示 两个字符窜相等，同一个艺术家或同一个专辑
                    else if (1==PinyinStrnCmp(pTemp->SortFileName, PreSortFileName, SORT_FILENAME_LEN)) //(pTemp->SortFileName)等于(pNodePrev->SortFileName)
                    {
                        uiSameCount++;
                    }
                    else // (pTemp->SortFileName)大于(pNodePrev->SortFileName),不会有小于的情况，因为已经排过序的了
                    {
                        gSubPageBuffer[uiSubCount++] = (uiSameCount+1)&0xFF; // 记录上一个Item包含的元素个数
                        gSubPageBuffer[uiSubCount++] = ((uiSameCount+1)>>8)&0xFF;
                        gSubPageBuffer[uiSubCount++] = 0; // 保留空间, 使每次保存大小为2指数倍
                        gSubPageBuffer[uiSubCount++] = 0; // 保留空间, 使每次保存大小为2指数倍

                        if (uiSubCount>=MEDIAINFO_PAGE_SIZE)//gSubPageBuffer 已经写好了，可以写flash了
                        {
                            uiSubCount = 0;

                            //ulID3SubSectorAddr 是sec的地址
                            MDWrite(DataDiskID, ulID3SubSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gSubPageBuffer);

                            SortPageBufferInit(gSubPageBuffer);
                            ulID3SubSectorAddr += (UINT32)(MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE);
                        }

                        gSubPageBuffer[uiSubCount++] = uiFileSortTemp&0xFF; // 记录文件保存号，文件排序后的序号
                        gSubPageBuffer[uiSubCount++] = (uiFileSortTemp>>8)&0xFF; // 记录文件保存号
                        gSubPageBuffer[uiSubCount++] = uiFileCount&0xFF;
                        gSubPageBuffer[uiSubCount++] = (uiFileCount>>8)&0xFF;

                        uiFileSubCount++;

                        uiSameCount = 0;
                    }

                    break;
                default:
                    break;

            }


            for (j=0;j<SORT_FILENAME_LEN;j++)
            {
                PreSortFileName[j] = pTemp->SortFileName[j];
            }
            
            if (uiFileIndex>=MEDIAINFO_PAGE_SIZE)
            {
                uiFileIndex = 0;
                
                MDWrite(DataDiskID, ulSortSectorAddr, MEDIAINFO_PAGE_SIZE/512, gPageTempBuffer);
                SortPageBufferInit(gPageTempBuffer);
                ulSortSectorAddr += (MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE);
            }
            uiFileCount++;
        }

    }

    if (uiFileIndex)	// 保存排序信息不足2K的部分
    {
        MDWrite(DataDiskID, ulSortSectorAddr, MEDIAINFO_PAGE_SIZE/512, gPageTempBuffer);
    }
    if (uiSubCount)//// 保存排序信息不足2K的部分
    {
        gSubPageBuffer[uiSubCount++] = (uiSameCount+1)&0xFF; // 统计最后一个Item的个数
        gSubPageBuffer[uiSubCount++] = ((uiSameCount+1)>>8)&0xFF;
        gSubPageBuffer[uiSubCount++] = 0; // 保留空间, 使每次保存大小为2指数倍
        gSubPageBuffer[uiSubCount++] = 0; // 保留空间, 使每次保存大小为2指数倍
        
        MDWrite(DataDiskID, ulID3SubSectorAddr, MEDIAINFO_PAGE_SIZE/512, gSubPageBuffer);
    }

    return uiFileSubCount;

}

#endif




