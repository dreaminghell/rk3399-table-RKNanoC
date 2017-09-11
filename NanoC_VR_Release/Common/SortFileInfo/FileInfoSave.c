/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��  SearchAndSaveFileInfo.C
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

  Input         : Buffer��Buffer to save file information.
                  pFileSaveTemp��the information of current file.
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

    for (i=0;i<MEDIA_CHAR_NUM_PER_FILE_NAME;i++)//���泤�ļ�����Ϣ
    {
        if (pFileSaveTemp->LongFileName[i] == 0)
        {
            break;
        }
        
        *pBuffer++ = pFileSaveTemp->LongFileName[i]&0xff;//���ֽ�
        *pBuffer++ = ((pFileSaveTemp->LongFileName[i])>>8)&0xff;//���ֽ�
    }
    //pBuffer = Buffer+ID3_TITLE_SAVE_ADDR_OFFSET�������Ƿ�ֹLongFileNameû���õ������ռ䣬����洢��Ϣλ�ò�һ
    pBuffer = Buffer+ID3_TITLE_SAVE_ADDR_OFFSET;//���ļ�������ռ�õĿռ���CHAR_NUM_PER_FILE_NAME��2��

    for (i=0;i<ID3_TAG_SIZE;i++)//����id3��title��Ϣ
    {
        if (pFileSaveTemp->id3_title[i] == 0)
        {
            break;
        }

        *pBuffer++ = (pFileSaveTemp->id3_title[i])&0xff;
        *pBuffer++ = (pFileSaveTemp->id3_title[i])>>8;
    }

    pBuffer = Buffer + ID3_SINGLE_SAVE_ADDR_OFFSET;

    for (i=0;i < ID3_TAG_SIZE ; i++)//����id3��singer��Ϣ
    {
        if (pFileSaveTemp->id3_singer[i] == 0)
        {
            break;
        }

        *pBuffer++ = (pFileSaveTemp->id3_singer[i])&0xff;
        *pBuffer++ = (pFileSaveTemp->id3_singer[i])>>8;
    }

    pBuffer = Buffer+ID3_ALBUM_SAVE_ADDR_OFFSET;
    for (i=0;i<ID3_TAG_SIZE;i++)//����id3��album��Ϣ
    {
        if (pFileSaveTemp->id3_album[i] == 0)
        {
            break;
        }

        *pBuffer++ = (pFileSaveTemp->id3_album[i])&0xff;
        *pBuffer++ = (pFileSaveTemp->id3_album[i])>>8;
    }

    pBuffer = Buffer+ID3_GENRE_SAVE_ADDR_OFFSET;
    for (i=0;i<ID3_TAG_SIZE;i++)//����id3��album��Ϣ
    {
        if (pFileSaveTemp->Genre[i] == 0)
        {
            break;
        }

        *pBuffer++ = (pFileSaveTemp->Genre[i])&0xff;
        *pBuffer++ = (pFileSaveTemp->Genre[i])>>8;
    }


    pBuffer = Buffer+PATH_SAVE_ADDR_OFFSET;//ƫ����260���ֽ�
    pPath = AudioFileInfo.Path;

    while (*pPath!=0)//�����ļ���·����Ϣ
    {
        *pBuffer++ = *pPath++;
    }
    *pBuffer = 0;

    pBuffer = Buffer+SHORT_NAME_SAVE_ADDR_OFFSET;
    for (i=0;i<11;i++)//�����ļ���
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

    // ���ҿ����ļ��Ǽ���
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
    MEDIA_FILE_SAVE_STRUCT  *pFileSaveTemp = &gFileSaveInfo; // ���ڱ����ļ���Ϣ

    UINT16 uiMusicFileCount = 0;  // �����ļ���������
    UINT16 uiFindFileResult = 0 ; // �ļ��������

    UINT16 uiFileIndex = 0;       // Flash Page���Ƽ���     ��ֵ����4ʱ���Ѿ���һ��page��С��bafferд��������дflash��
    UINT32 ulFileFullInfoSectorAddr = 0; // �����ļ���ϸ��Ϣ����ʼsector��ַ

    UINT16 uiHundredFlag=0;
    UINT16 uiTenFlag=0;
    UINT16 uiCountTemp=0;

    FILE_SAVE_STRUCT ID3FileInfo;

    ulFileFullInfoSectorAddr = MediaInfoAddr + FILE_SAVE_INFO_SECTOR_START; //������ϸ���ļ���Ϣ��ʼ��ַ(secֵ)

    PageWriteBufferInit();

    GotoRootDir(AudioFileExtString); // ��ȡ��Ŀ¼�������ļ�����Ϣ
    GetDirPath(AudioFileInfo.Path);	// ��ȡ��ǰ·��  /????????����᲻��������

    FindDataInfo.Clus=CurDirClus; // ��ǰĿ¼�״غ�
    FindDataInfo.Index=0;	// �ӵ�һ��FDT�����ʼ

    uiFindFileResult = FindNext(&AudioFileInfo.Fdt, &FindDataInfo, AudioFileExtString);

    while (uiMusicFileCount<SORT_FILENUM_DEFINE) //  for(i = 0 ;i < TotalFiles ; )
    {
        if (uiFindFileResult==RETURN_OK)
        {
            hMusicFile = FileOpenByFileFDT(AudioFileInfo.Fdt, "R"); // ��д��FileOpen���������ٶȴ�Ϊ���

            if (hMusicFile != NOT_OPEN_FILE)//�ɹ����ļ�//���ļ�������ID3��Ϣ
            {
                #ifdef _RK_ID3_
                ModuleOverlay(MODULE_ID_AUDIO_ID3, MODULE_OVERLAY_ALL); //����ID3 ��������
                memset((UINT8*)&ID3FileInfo, 0, sizeof(FILE_SAVE_STRUCT));
                GetAudioId3Info(hMusicFile , &ID3FileInfo, &AudioFileInfo.Fdt.Name[8]);
                #endif             
            }//���ļ�������ID3��Ϣ

            FileClose(hMusicFile);
            hMusicFile = -1;
            FileSaveStructInit(pFileSaveTemp); // �����ڼ�¼�ļ�������Ϣ�Ľṹ����0
            MediaGetLongFileName(pFileSaveTemp->LongFileName);// ��ȡ����ĳ��ļ�����Ϣ
            

            for (i=0;i<ID3_TAG_SIZE;i++)
            {
                pFileSaveTemp->id3_title[i] = ID3FileInfo.ID3Title[i]; // ���������Ϣ
                pFileSaveTemp->id3_singer[i] = ID3FileInfo.ID3Artist[i]; // ���������Ϣ
                pFileSaveTemp->id3_album[i] = ID3FileInfo.ID3Album[i];  // ����ר����Ϣ
                pFileSaveTemp->Genre[i] =ID3FileInfo.Genre[i];// ����ר����Ϣ
            }

            if (pFileSaveTemp->id3_title[0]==0)
            {
                memcpy(pFileSaveTemp->id3_title,pFileSaveTemp->LongFileName,ID3_TAG_SIZE*2)	;
            }
            //�����ĸ��֣�ר��������Ϊotherʱ�����㷽����������ui�ϵĴ���
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
            
            SaveFileInfo(&gPageWriteBuffer[uiFileIndex<<BYTE_PER_FILE_SHIFT], pFileSaveTemp); // �����ļ���Ϣ������������Ϣ �ѻ�ȡ�ĵ�ǰ�ļ�����Ϣ����ȫ�ֱ���gPageWriteBuffer��ĳ��λ��
            uiMusicFileCount++; // �ļ���������һ��
            uiFileIndex++;


            if (uiFileIndex == FILE_SAVE_NUM_PER_PAGE)//gPageWriteBuffer �Ѿ�������Ϣ����СΪ8k ���Ϳ�ʼдflash
            {
                uiFileIndex = 0;//�ļ���Ż���
                MDWrite(DataDiskID, ulFileFullInfoSectorAddr, (MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE),gPageWriteBuffer);//gPageTempBuffer);// 
                #if 0
                for (j=0;j<MEDIAINFO_PAGE_SIZE;j++)//gPageWriteBuffer ����
                {
                    gPageWriteBuffer[j]=0;
                }
                #endif
                memset((uint8*)gPageWriteBuffer, 0, MEDIAINFO_PAGE_SIZE);
                ulFileFullInfoSectorAddr  +=  (MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE);//ָ����һ��page��Ϊ�´�д��׼��
            }

            uiCountTemp = uiMusicFileCount;
            if ( uiHundredFlag == (uiCountTemp/20))//ui��ʾcase ��ÿ�ҵ�20���ļ�ˢһ��ͼƬ
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
            GotoNextDir(AudioFileExtString);			// ������һ��Ŀ¼��������Ŀ¼����ͬ��Ŀ¼
            if (CurDirDeep == 0) // ����Ŀ¼������ص��˸�Ŀ¼
            {
                break;
            }
            while (1)
            {
                if ((SubDirInfo[1].DirName[0] != 'V') ||
                (SubDirInfo[1].DirName[1] != 'O') ||
                (SubDirInfo[1].DirName[2] != 'I') ||
                (SubDirInfo[1].DirName[3] != 'C') ||
                (SubDirInfo[1].DirName[4] != 'E') )// ����¼��Ŀ¼�µ��ļ�
                //(SubDirInfo[1].DirName[5] != 'D'))    // ����¼��Ŀ¼�µ��ļ�
                {
                    break;
                }
                GotoNextDir(AudioFileExtString);
                if (CurDirDeep == 0)
                {
                    break;
                }
            }
            if (CurDirDeep == 0) // ����Ŀ¼������ص��˸�Ŀ¼
            {
                break;
            }

            GetDirPath(AudioFileInfo.Path);	// ��ȡ��ǰ·��
            FindDataInfo.Clus=CurDirClus;
            FindDataInfo.Index=0;
            uiFindFileResult = FindNext(&AudioFileInfo.Fdt, &FindDataInfo, AudioFileExtString);

        }

    }

    if (uiFileIndex) // ���治��2K���ļ���Ϣ
    {
        MDWrite(DataDiskID, ulFileFullInfoSectorAddr, (MEDIAINFO_PAGE_SIZE/512),gPageWriteBuffer);//gPageTempBuffer);// 
    }
    
    gSysConfig.MedialibPara.gMusicFileNum =uiMusicFileCount; // �õ�ϵͳȫ���ļ���Ŀ�����ΪSORT_FILENUM_DEFINE��

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
UINT32 SaveSortInfo(UINT32 uiSaveType, UINT32 ulSortSectorAddr,UINT32 MediaInfoAddress)//���uiSaveType�������һ�ר��(ID3_ARTIST_TYPE��ID3_ALBUM_TYPE)�ᱣ�������Ϣ
{
    UINT32  i,j;

    UINT32 uiFileCount = 0;
    UINT32 uiFileSubCount = 0;  // �ļ���������
    UINT32 uiSameCount = 0;     // ID3��Ϣ����ʱ����ͬ���ͼ���
    UINT32 uiFileIndex = 0;     // Flash Page���Ƽ���
    UINT16 uiFileSortTemp = 0;  // �ļ��������ʱ����

    UINT32 uiSubCount = 0;      // ����ÿ���������ļ�ID3��Ϣ�������
    UINT32 uiSubCountTotal = 0; // ����ID3��Ϣ����������ļ�����

    UINT32 ulID3SubSectorAddr = 0; // ID3��Ϣ������Ϣ�����ַ

    UINT16 PreSortFileName[SORT_FILENAME_LEN];

    FILE_INFO_INDEX_STRUCT  *pTemp = NULL; // ��������ͷָ��

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
            uiFileSortTemp = (UINT16)(pTemp - &gSortNameBuffer0[0]); //���㵱ǰ���ļ��������ļ��е������

            //���㵱ǰҪ�������Ϣ��gSortNameBuffer0[0]��ƫ�Ƶ�ַ����ΪpChildChainHead�������Ϣ��Դ��gSortNameBuffer0
            gPageTempBuffer[uiFileIndex++] = uiFileSortTemp & 0xFF;     //�����ļ����������
            gPageTempBuffer[uiFileIndex++] = (uiFileSortTemp>>8) & 0xFF;//�����ļ����������

            switch (uiSaveType)
            {
                case FILE_NAME_TYPE:
                case ID3_TITLE_TYPE:
                    uiFileSubCount++;
                    break;
                case ID3_ARTIST_TYPE://ͬһ�������� ��ר���м����ļ����������һ���ļ���ƫ�Ƶ�ַ���������ר���µ��ļ�����
                case ID3_ALBUM_TYPE:
                case ID3_GENRE_TYPE	:
                    if (0==uiFileSubCount)
                    {
                        gSubPageBuffer[uiSubCount++] = uiFileSortTemp&0xFF; // ��¼�ļ������
                        gSubPageBuffer[uiSubCount++] = (uiFileSortTemp>>8)&0xFF;// ��¼�ļ������
                        gSubPageBuffer[uiSubCount++] = 0;
                        gSubPageBuffer[uiSubCount++] = 0;
                        uiFileSubCount++;
                    }
                    //PinyinStrnCmp ���� 1 ��ʾ �����ַ�����ȣ�ͬһ�������һ�ͬһ��ר��
                    else if (1==PinyinStrnCmp(pTemp->SortFileName, PreSortFileName, SORT_FILENAME_LEN)) //(pTemp->SortFileName)����(pNodePrev->SortFileName)
                    {
                        uiSameCount++;
                    }
                    else // (pTemp->SortFileName)����(pNodePrev->SortFileName),������С�ڵ��������Ϊ�Ѿ��Ź������
                    {
                        gSubPageBuffer[uiSubCount++] = (uiSameCount+1)&0xFF; // ��¼��һ��Item������Ԫ�ظ���
                        gSubPageBuffer[uiSubCount++] = ((uiSameCount+1)>>8)&0xFF;
                        gSubPageBuffer[uiSubCount++] = 0; // �����ռ�, ʹÿ�α����СΪ2ָ����
                        gSubPageBuffer[uiSubCount++] = 0; // �����ռ�, ʹÿ�α����СΪ2ָ����

                        if (uiSubCount>=MEDIAINFO_PAGE_SIZE)//gSubPageBuffer �Ѿ�д���ˣ�����дflash��
                        {
                            uiSubCount = 0;

                            //ulID3SubSectorAddr ��sec�ĵ�ַ
                            MDWrite(DataDiskID, ulID3SubSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gSubPageBuffer);

                            SortPageBufferInit(gSubPageBuffer);
                            ulID3SubSectorAddr += (UINT32)(MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE);
                        }

                        gSubPageBuffer[uiSubCount++] = uiFileSortTemp&0xFF; // ��¼�ļ�����ţ��ļ����������
                        gSubPageBuffer[uiSubCount++] = (uiFileSortTemp>>8)&0xFF; // ��¼�ļ������
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

    if (uiFileIndex)	// ����������Ϣ����2K�Ĳ���
    {
        MDWrite(DataDiskID, ulSortSectorAddr, MEDIAINFO_PAGE_SIZE/512, gPageTempBuffer);
    }
    if (uiSubCount)//// ����������Ϣ����2K�Ĳ���
    {
        gSubPageBuffer[uiSubCount++] = (uiSameCount+1)&0xFF; // ͳ�����һ��Item�ĸ���
        gSubPageBuffer[uiSubCount++] = ((uiSameCount+1)>>8)&0xFF;
        gSubPageBuffer[uiSubCount++] = 0; // �����ռ�, ʹÿ�α����СΪ2ָ����
        gSubPageBuffer[uiSubCount++] = 0; // �����ռ�, ʹÿ�α����СΪ2ָ����
        
        MDWrite(DataDiskID, ulID3SubSectorAddr, MEDIAINFO_PAGE_SIZE/512, gSubPageBuffer);
    }

    return uiFileSubCount;

}

#endif




