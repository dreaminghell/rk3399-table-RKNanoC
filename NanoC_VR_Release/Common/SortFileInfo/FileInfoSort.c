/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   FileInfoSort.C
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo      2009-06-03          1.0
*    desc:      ORG.
********************************************************************************
*/
#define  _IN_FILEINFOSORT_

#include "SysInclude.h"

#ifdef MEDIA_MODULE

#include "FsInclude.h"



#include "AddrSaveMacro.h"


#include "FileInfoSortMacro.h"

#include "FileInfoSaveMacro.h"
#include "FileInfoSortStruct.h"


_FILE_INFO_SORT_BSS_	UINT16 gChildChainDivValue[CHILD_CHAIN_NUM];//72*2�ֽ�  ��Ÿ�����������׸�Ԫ���ĵ�һ����ĸ��Ҫ�Ƚϵ�unicode ͨ���������Ԫ���Ա��Ҹ���Ӧ��������
_FILE_INFO_SORT_BSS_	FILE_INFO_INDEX_STRUCT  *pChildChainHead[CHILD_CHAIN_NUM]; //72*4�ֽ� ��������������ͷ�ڵ��ָ��
_FILE_INFO_SORT_BSS_	FILE_INFO_INDEX_STRUCT 	gChildChainHead[CHILD_CHAIN_NUM];  //72*8�ֽ� ��������������ͷ���

/* ע�⣺�����������������ַ��������������ʵ�ֿɽ��������һ���� */
_FILE_INFO_SORT_BSS_	FILE_INFO_INDEX_STRUCT	gSortNameBuffer0[SORT_FILE_NAME_BUF_SIZE];//1536*8�ֽ� �����ȡ���ļ�����Ϣ����פ���ڴ棬����ʱ����, Buffer0
_FILE_INFO_SORT_BSS_	FILE_INFO_INDEX_STRUCT	gSortNameBuffer1[SORT_FILE_NAME_BUF_SIZE];//1536*8�ֽ� �����ȡ���ļ�����Ϣ����פ���ڴ棬����ʱ����, Buffer1

#if 1
_FILE_INFO_SORT_BSS_	UINT8 	gPageTempBuffer[MEDIAINFO_PAGE_SIZE]; //2048�ֽ� ������ʱ��¼һ��Page���ļ���Ϣ
_FILE_INFO_SORT_BSS_	UINT8 	gSubPageBuffer[MEDIAINFO_PAGE_SIZE];  //2048�ֽ� ������ʱ��¼һ��Page���ļ���Ϣ
#else
_FILE_INFO_SORT_BSS_	UINT8 	gPageTempBuffer[512]; //2048�ֽ� ������ʱ��¼һ��Page���ļ���Ϣ
_FILE_INFO_SORT_BSS_	UINT8 	gSubPageBuffer[512];  //2048�ֽ� ������ʱ��¼һ��Page���ļ���Ϣ

#endif

extern UINT32 SaveSortInfo(UINT32 uiSaveType, UINT32 ulSortSectorAddr,UINT32 MediaInfoAddress);
/*
--------------------------------------------------------------------------------
  Function name : UINT32 GetPYCode(UINT32 wch)   
  Author        : anzhiguo
  Description   : ��ȡ�����ַ��������
                  
  Input         : wch�������ַ���unicodeֵ
  Return        : pinCode:�ַ�ƴ�������

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/                                  
#ifdef  PYSORT_ENABLE
_FILE_INFO_SORT_CODE_ 
UINT32 GetPYCode(UINT16 wch)  
{   
    UINT32 pinCode;
   
    extern UINT32 ReadDataFromIRAM(UINT32 addr);
    
    pinCode = ReadDataFromIRAM(wch- UNICODE_BEGIN+BASE_PYTABLE_ADDR_IN_IRM); // �ӵ���IRAM��ƴ�������õ����ֵ�ƴ������ֵ
    pinCode += UNICODE_BEGIN;
    
    return (pinCode);
}	

#endif 
/*
--------------------------------------------------------------------------------
  Function name : UINT32 GetCmpResult(UINT32 wch)   
  Author        : anzhiguo
  Description   : ��ȡ�����ַ��������
                  
  Input         : wch�������ַ���unicodeֵ
  Return        : pinCode:�ַ�ƴ�������

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/                                  
_FILE_INFO_SORT_CODE_
    
UINT32 GetCmpResult(UINT16 wch)
{
    UINT32 cmpCode; 
    
#ifdef  PYSORT_ENABLE    	
    if ((wch >= UNICODE_BEGIN) && (wch <= UNICODE_END))
    {
        cmpCode = GetPYCode(wch); 
    }
    else 
#endif   

    if(wch>=97&&wch<=122) // 97 = 'a', 122 = 'z'  Сд��ĸȫ��ת���ɴ�д��ĸ
    {
    	cmpCode = wch-32;
    }
    else
    {
    	cmpCode = wch;
    }
    
    return (cmpCode);
}
/*
--------------------------------------------------------------------------------
  Function name : int PinyinCharCmp(UINT32 wch1, UINT32 wch2)   
  Author        : anzhiguo
  Description   : ��ƴ�����򣬱Ƚ������ַ������С
                  
  Input         : wch1���ַ�1��unicodeֵ   wch2���ַ�2��unicodeֵ
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/                                  
                                  
_FILE_INFO_SORT_CODE_
int PinyinCharCmp(UINT16 wch1, UINT16 wch2)
{
    return (GetCmpResult(wch1) - GetCmpResult(wch2));		
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 PinyinStrnCmp(UINT16 *str1, UINT16 *str2, UINT32 length)  
  Author        : anzhiguo
  Description   : ��ƴ�����򣬱Ƚ������ַ�����С
                  
  Input         : str1���ַ���1   str2���ַ���2
  Return        : 0 str1 < str2
                  1 str1 = str2
                  2 str1 > str2
  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/                                     
_FILE_INFO_SORT_CODE_

UINT32 PinyinStrnCmp(UINT16 *str1, UINT16 *str2, UINT32 length)
{
    UINT32  i = 0;
	
    if (str1 == NULL)
    {
        return 0;
    }

    if (str2 == NULL)
    {
			return 2;
    }

    for (i = 0; i < length; i++)
    {
			if ((str1[i] == NULL) || (str2[i] == NULL) || (GetCmpResult(str1[i]) != GetCmpResult(str2[i])))
			{
	    	break;
			}
    }
    
    if(i==length)  return 1;

    if(GetCmpResult(str1[i])>GetCmpResult(str2[i]))
       return 2;
   	else if(GetCmpResult(str1[i]) == GetCmpResult(str2[i]))
       return 1;  //���
    else
       return 0;
    
}
/*
--------------------------------------------------------------------------------
  Function name : UINT8 BrowserListInsertBySort(FILE_INFO_INDEX_STRUCT *head, FILE_INFO_INDEX_STRUCT *pNode) 
  Author        : anzhiguo
  Description   : ��ʼ��˫������
                  
  Input         : *head   ��������ͷָ�� 
                  *pFileSaveTemp   ��������ṹ   
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/                                       
_FILE_INFO_SORT_CODE_

UINT8 BrowserListInsertBySort(FILE_INFO_INDEX_STRUCT *head, FILE_INFO_INDEX_STRUCT *pNode)
{
	FILE_INFO_INDEX_STRUCT *p,*q;
	UINT32 i;

	p = head;	
   
	while(p->pNext!=NULL)
	{ 
		q = p->pNext;
		// 2 ��ʾ q->SortFileName > pNode->SortFileName
		if(PinyinStrnCmp(q->SortFileName, pNode->SortFileName, SORT_FILENAME_LEN)==2) //˳��ɨ������,���½����뵽������Ľ��ǰ
		{				
			pNode->pNext = q;
			p->pNext = pNode;

			return 1;
		}
		p = q;
	}

	if(p->pNext==NULL)  // ��������β,˵������Ľ��Ϊ��ǰ�������ֵ,������뵽��β
	{
		pNode->pNext = NULL;
		p->pNext = pNode;
		return 1;
	}

	return 0;
}
/*
--------------------------------------------------------------------------------
  Function name : void SortPageBufferInit(UINT8 *pBuffer) 
  Author        : anzhiguo
  Description   : 
                  
  Input         : *pBuffer   Ҫ��ʼ����bufָ��
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/      
_FILE_INFO_SORT_CODE_
    
void SortPageBufferInit(UINT8 *pBuffer)
{
	 UINT32  i;
	
	 for(i=0;i<MEDIAINFO_PAGE_SIZE;i++) 
      pBuffer[i]=0;
}

/*
--------------------------------------------------------------------------------
  Function name : void ChildChainInit(void)
  Author        : anzhiguo
  Description   : ��ʼ��˫������
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         gChildChainDivValue[i] �� pChildChainHead[i] һһ��Ӧ
--------------------------------------------------------------------------------
*/                                    
_FILE_INFO_SORT_CODE_
void ChildChainInit(void)
{
	 UINT32 i,j;
	 for(i=0;i<CHILD_CHAIN_NUM;i++)	// ��ʼ��������ͷ�ڵ�
	 {

	   //pChildChainHead ��һ��ָ�����飬����ÿ��Ԫ�ؾ�Ϊһ��FILE_INFO_INDEX_STRUCT���͵�ָ�룬
	   //gChildChainHead ��һ���ṹ�����飬����ÿ��Ԫ�ؾ�Ϊһ��FILE_INFO_INDEX_STRUCT���͵Ľṹ�������
	 	pChildChainHead[i] = &gChildChainHead[i]; 
	 	pChildChainHead[i]->pNext = NULL; 
	 }
	 
	 gChildChainDivValue[0]=  0x0000; // �����ַ�Ϊ�յķ����ڴ�
	 gChildChainDivValue[1]=   0x41; // С��Ӣ����ĸ���ַ����������ּ���������  0x41 A��ĸ��ascall ��
	 
	 j = 2;
	 for(i='A';i<'Z'+1;i++)
	 {
	    gChildChainDivValue[j] = i+1; // Ӣ���ַ�,A~Z
	    j++;
	 }
	 
	 gChildChainDivValue[j++] = 0x4DFF; // Ӣ���ַ�������֮���unicode�ַ�
	 
	 j = 29;
	 for(i=j;i<CHILD_CHAIN_NUM-1;i++)
	 {
	    gChildChainDivValue[i] = gChildChainDivValue[i-1]+(0x51a5)/(CHILD_CHAIN_NUM-30); // �Ժ��ֲ����ַ�����42�ȷ�
	 }
	 
	 gChildChainDivValue[CHILD_CHAIN_NUM-1] = 0xffff; // ���ں��ֵ������ַ�ȫ������������
	 
	 /*for(i=0;i<CHILD_CHAIN_NUM;i++)
	 {
	    DisplayTestDecNum((i%6)*50,(i/6)*15,gChildChainDivValue[i]);
	 }
	 while(1);*/
	 /* ����������Ϊ����ƴ������ʱ����ƴ����ĸ�ָ��������� */
	 /*gChildChainDivValue[0]=   0x0000;  
	 gChildChainDivValue[1]=   0x4DFF; //0x0000~0x4DFF; //  0~9,��ĸ,��������					
	 gChildChainDivValue[2] =  0x4EC3; //0x4E00~0x4EC3; //  A, ߹		            
	 gChildChainDivValue[3] =  0x5235; //0x4EC4~0x5235; //  B����                
	 gChildChainDivValue[4] =  0x576a; //0x5236~0x576a; //  C, ��                
	 gChildChainDivValue[5] =  0x5b40; //0x576b~0x5b40; //  D, ��                
	 gChildChainDivValue[6] =  0x5bf0; //0x5b41~0x5bf0; //  E����                
	 gChildChainDivValue[7] =  0x5e55; //0x5bf1~0x5e55; //  F, ��                
	 gChildChainDivValue[8] =  0x6198; //0x5e56~0x6198; //  G, �                
	 gChildChainDivValue[9] =  0x65b3; //0x6199~0x65b3; //  H, �o,ha             
	 gChildChainDivValue[10] = 0x6c00; //0x65b4~0x6c00; //  J, آ                
	 gChildChainDivValue[11] = 0x6e30; //0x6c01~0x6e30; //  K, ��                
	 gChildChainDivValue[12] = 0x7448; //0x6e31~0x7448; //  L, ��                
	 gChildChainDivValue[13] = 0x7810; //0x7449~0x7810; //  M, �`                
	 gChildChainDivValue[14] = 0x7953; //0x7811~0x7953; //  N, ǂ                
	 gChildChainDivValue[15] = 0x7978; //0x7954~0x7978; //  O, ��                
	 gChildChainDivValue[16] = 0x7be2; //0x7979~0x7be2; //  P, �r                
	 gChildChainDivValue[17] = 0x7fe9; //0x7be3~0x7fe9; //  q, ��,qi             
	 gChildChainDivValue[18] = 0x8131; //0x7fea~0x8131; //  r, ��,ra             
	 gChildChainDivValue[19] = 0x8650; //0x8132~0x8650; //  s, ��,sa             
	 gChildChainDivValue[20] = 0x89d0; //0x8651~0x89d0; //  t, ��                
	 gChildChainDivValue[21] = 0x8c43; //0x89d1~0x8c43; //  w, ��                
	 gChildChainDivValue[22] = 0x9169; //0x8c44~0x9169; //  x, Ϧ                
	 gChildChainDivValue[23] = 0x9904; //0x916a~0x9904; //  y, Ѿ                
	 gChildChainDivValue[24] = 0x9fa4; //0x9905~0x9fa4; //  z, ��*/                
}                           

/*
--------------------------------------------------------------------------------
  Function name : void GetSortName(UINT32 SectorOffset)
  Author        : anzhiguo
  Description   : ��flash�ж�ȡ�����ļ��ĳ��ļ���������ŵ� gSortNameBuffer0�� gSortNameBuffer1 ��
                  ������ռ��������ģ�ÿ�����Դ�ŵ��ļ�����ϵͳ����������ļ�����һ��
                  
  Input         : SectorOffset���������ַ����ļ���Ϣ�е�ƫ�Ƶ�ַ
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         �ӱ�����Flash�е��ļ���Ϣ��ȡ��Ҫ������ַ�(�ļ�����ID3��Ϣ)�����Ҵ���gSortNameBuffer0[].SortFileName��
                �������ļ�����Ϣ(��Ҫ������ַ���Ϣ)��ȡ
--------------------------------------------------------------------------------
*/                                    
_FILE_INFO_SORT_CODE_
void GetSortName(UINT32 SectorOffset,UINT16 FileNum)
{
	UINT16  i,j;
	UINT16  FileCount;	
	UINT8  TempBuffer[SORT_FILENAME_LEN*2];
	FILE_INFO_INDEX_STRUCT *pTemp;
	
	pTemp = gSortNameBuffer0;
	FileCount = FileNum;
	for(i=0;i<FileCount;i++)
	{
	    //��flash�ж�ȡ��ȡ�ĳ��ļ�����Ϣ  //�����ַ�ļ�����4k page��flash�л������
        MDReadData(DataDiskID, ((UINT32)(MediaInfoAddr+FILE_SAVE_INFO_SECTOR_START)<<9)+BYTE_NUM_SAVE_PER_FILE*(UINT32)(i)+SectorOffset, SORT_FILENAME_LEN*2, TempBuffer)	;
		for(j=0;j<SORT_FILENAME_LEN;j++)
		{		
  		    pTemp->SortFileName[j] = (TempBuffer[j*2]&0xff)+((TempBuffer[j*2+1]&0xff)<<8);
		}
    pTemp++;
	}
}

/*
--------------------------------------------------------------------------------
  Function name : void GetSortName(UINT32 SectorOffset)
  Author        : anzhiguo
  Description   : �ļ������ܺ���
                  
  Input         : uiSaveAddrOffset���������ַ�(�ļ�����id3title��id3singer��id3ablum)���ļ���Ϣ�е�ƫ�Ƶ�ַ
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         ��gSortNameBuffer0��gSortNameBuffer1 �е���Ϣ���з������У������ɵ���Ӧ������pChildChainHead���ʵ���λ����
--------------------------------------------------------------------------------
*/                                        
_FILE_INFO_SORT_CODE_ 
void SortFunction(UINT32 uiSaveAddrOffset,UINT16 FileNum)
{
	UINT32  i,j;
	UINT16  filenum;  
    UINT32 uiFirsCharSortVal=0; // ��ǰ�������������ֵ����ַ���ƴ������ֵ 
    
    FILE_INFO_INDEX_STRUCT *pTemp;
	
	 pTemp = gSortNameBuffer0;   
     filenum = FileNum;
     memset(&gSortNameBuffer0[0],0,sizeof(FILE_INFO_INDEX_STRUCT)*SORT_FILE_NAME_BUF_SIZE);
     memset(&gSortNameBuffer1[0],0,sizeof(FILE_INFO_INDEX_STRUCT)*SORT_FILE_NAME_BUF_SIZE);
         
     SortPageBufferInit(gPageTempBuffer);
     ChildChainInit();                  // �Ը��������ͷ�ڵ���г�ʼ������������ÿ������ķֶιؼ���
   	 GetSortName(uiSaveAddrOffset,filenum);     // ��ȡҪ������ļ���Ϣ
   	 
   	 /* �������ļ��ĳ��ļ����������� */
   	 for(i=0;i<filenum;i++)
     {
      	 uiFirsCharSortVal = GetCmpResult(pTemp->SortFileName[0]);
      	 j = 0;
      	 while(uiFirsCharSortVal>gChildChainDivValue[j]) // ͨ�����ļ��ĵ�һ���ַ����ж���Ҫ���ĸ�������������
   	 		     j++; 
   	 		 BrowserListInsertBySort(pChildChainHead[j], pTemp); // ��gSortNameBuffer0�е��ļ���Ϣ�����������pChildChainHead��
   	 		 pTemp++;
      	
     }
}


/*
--------------------------------------------------------------------------------
  Function name : void SortUpdateFun(void)
  Author        : anzhiguo
  Description   : ���ļ�����Ϣ�������򣬲���ָ��Flash�д洢������Ϣ
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/                                     
_FILE_INFO_SORT_CODE_
void SortUpdateFun(MEDIALIB_CONFIG * Sysfilenum ,UINT32 MediaInfoAddress)
{  
   UINT32   uiSortTypeCount = 0;            // ��������ѡ��
   UINT32   uiSortInfoAddrOffset = 0;       // ͬһ���ļ�������Ϣ�еĲ�ͬ���ݵ�ƫ�Ƶ�ַ,��λByte ��Ҫ���ļ���Ϣ�Ļ�ȡ��ַ
   UINT32   ulFileSortInfoSectorAddr = 0;   // �����ļ�������Ϣ����ʼsector��ַ  ��Ҫ������ļ������Ϣ����ʼ��ַ
    
   UINT32   uiCountTemp[SORT_TYPE_ITEM_NUM];   
                
   for(uiSortTypeCount=0;uiSortTypeCount<SORT_TYPE_ITEM_NUM;uiSortTypeCount++)
   {   	 
   	 switch(uiSortTypeCount)
   	 {
   	    case FILE_NAME_TYPE:
   	 	 	     uiSortInfoAddrOffset = FILE_NAME_SAVE_ADDR_OFFSET;
   	 	 	     ulFileSortInfoSectorAddr = MediaInfoAddress + FILENAME_SORT_INFO_SECTOR_START;   	 	 	      	 	 	 
   	 	 	     
   	 	 	     break; 
   	 	case ID3_TITLE_TYPE:
   	 	 	     uiSortInfoAddrOffset = ID3_TITLE_SAVE_ADDR_OFFSET;//������ļ���Ϣ��ƫ�Ƶ�ַ(����ڱ����ļ���Ϣ��ʼ��ַ)
   	 	 	     ulFileSortInfoSectorAddr = MediaInfoAddress + ID3TITLE_SORT_INFO_SECTOR_START;   	 	 	     
   	 	 	     
   	 	 	     break;
   	 	 case ID3_ARTIST_TYPE:
   	 	 	     uiSortInfoAddrOffset = ID3_SINGLE_SAVE_ADDR_OFFSET;
   	 	 	     ulFileSortInfoSectorAddr = MediaInfoAddress + ID3ARTIST_SORT_INFO_SECTOR_START;   	 	 	     

   	 	 	     break;
   	 	 case ID3_ALBUM_TYPE:
   	 	 	     uiSortInfoAddrOffset = ID3_ALBUM_SAVE_ADDR_OFFSET;
   	 	 	     ulFileSortInfoSectorAddr = MediaInfoAddress + ID3ALBUM_SORT_INFO_SECTOR_START;   	 	 	     

   	 	 	     break;
   	 	 case ID3_GENRE_TYPE:
   	 	 	     uiSortInfoAddrOffset = ID3_GENRE_SAVE_ADDR_OFFSET;
   	 	 	     ulFileSortInfoSectorAddr = MediaInfoAddress + ID3GENRE_SORT_INFO_SECTOR_START;  
   	 	 	  break;
   	 }	
   	 
   	 SortFunction(uiSortInfoAddrOffset,Sysfilenum->gMusicFileNum);//��ȡ��Ӧ���ļ���Ϣ��������һ��ֻ����һ����Ϣ
     
     uiCountTemp[uiSortTypeCount] = SaveSortInfo(uiSortTypeCount, ulFileSortInfoSectorAddr,MediaInfoAddress); //�����ļ�������ţ��Լ�������Ϣ�������ظ����ļ�������
   } 
   
   Sysfilenum->gID3TitleFileNum =  uiCountTemp[1]; // �õ�����ID3 Title��Ϣ���ļ�����   uiCountTemp[0] �����е������ļ�����
   Sysfilenum->gID3ArtistFileNum = uiCountTemp[2]; // �õ�����ID3 Artist��Ϣ���ļ�����
   Sysfilenum->gID3AlbumFileNum =  uiCountTemp[3]; // �õ�����ID3 Album��Ϣ���ļ����� 
   Sysfilenum->gID3GenreFileNum =  uiCountTemp[4]; // �õ�����ID3 Genre��Ϣ���ļ����� 
}

#endif

/*
********************************************************************************
*
*                         End of FileInfoSort.c
********************************************************************************
*/


