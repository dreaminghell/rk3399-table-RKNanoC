/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   FileInfoSort.C
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


_FILE_INFO_SORT_BSS_	UINT16 gChildChainDivValue[CHILD_CHAIN_NUM];//72*2字节  存放各个子链表的首个元属的第一个字母，要比较的unicode 通过与该数组元属对比找个对应的子链表
_FILE_INFO_SORT_BSS_	FILE_INFO_INDEX_STRUCT  *pChildChainHead[CHILD_CHAIN_NUM]; //72*4字节 各排序个子链表的头节点的指针
_FILE_INFO_SORT_BSS_	FILE_INFO_INDEX_STRUCT 	gChildChainHead[CHILD_CHAIN_NUM];  //72*8字节 各排序个子链表的头结点

/* 注意：以下两个数组物理地址必须相连，具体实现可将其紧挨在一起定义 */
_FILE_INFO_SORT_BSS_	FILE_INFO_INDEX_STRUCT	gSortNameBuffer0[SORT_FILE_NAME_BUF_SIZE];//1536*8字节 保存截取的文件名信息，常驻于内存，排序时调用, Buffer0
_FILE_INFO_SORT_BSS_	FILE_INFO_INDEX_STRUCT	gSortNameBuffer1[SORT_FILE_NAME_BUF_SIZE];//1536*8字节 保存截取的文件名信息，常驻于内存，排序时调用, Buffer1

#if 1
_FILE_INFO_SORT_BSS_	UINT8 	gPageTempBuffer[MEDIAINFO_PAGE_SIZE]; //2048字节 用于临时记录一个Page的文件信息
_FILE_INFO_SORT_BSS_	UINT8 	gSubPageBuffer[MEDIAINFO_PAGE_SIZE];  //2048字节 用于临时记录一个Page的文件信息
#else
_FILE_INFO_SORT_BSS_	UINT8 	gPageTempBuffer[512]; //2048字节 用于临时记录一个Page的文件信息
_FILE_INFO_SORT_BSS_	UINT8 	gSubPageBuffer[512];  //2048字节 用于临时记录一个Page的文件信息

#endif

extern UINT32 SaveSortInfo(UINT32 uiSaveType, UINT32 ulSortSectorAddr,UINT32 MediaInfoAddress);
/*
--------------------------------------------------------------------------------
  Function name : UINT32 GetPYCode(UINT32 wch)   
  Author        : anzhiguo
  Description   : 获取汉字字符的排序号
                  
  Input         : wch：汉字字符的unicode值
  Return        : pinCode:字符拼音排序号

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
    
    pinCode = ReadDataFromIRAM(wch- UNICODE_BEGIN+BASE_PYTABLE_ADDR_IN_IRM); // 从调入IRAM的拼音排序表得到汉字的拼音排序值
    pinCode += UNICODE_BEGIN;
    
    return (pinCode);
}	

#endif 
/*
--------------------------------------------------------------------------------
  Function name : UINT32 GetCmpResult(UINT32 wch)   
  Author        : anzhiguo
  Description   : 获取汉字字符的排序号
                  
  Input         : wch：汉字字符的unicode值
  Return        : pinCode:字符拼音排序号

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

    if(wch>=97&&wch<=122) // 97 = 'a', 122 = 'z'  小写字母全部转换成大写字母
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
  Description   : 按拼音排序，比较两个字符的码大小
                  
  Input         : wch1：字符1的unicode值   wch2：字符2的unicode值
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
  Description   : 按拼音排序，比较两个字符串大小
                  
  Input         : str1：字符串1   str2：字符串2
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
       return 1;  //相等
    else
       return 0;
    
}
/*
--------------------------------------------------------------------------------
  Function name : UINT8 BrowserListInsertBySort(FILE_INFO_INDEX_STRUCT *head, FILE_INFO_INDEX_STRUCT *pNode) 
  Author        : anzhiguo
  Description   : 初始化双向链表
                  
  Input         : *head   插入链表头指针 
                  *pFileSaveTemp   待插入结点结构   
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
		// 2 表示 q->SortFileName > pNode->SortFileName
		if(PinyinStrnCmp(q->SortFileName, pNode->SortFileName, SORT_FILENAME_LEN)==2) //顺序扫描链表,将新结点插入到比它大的结点前
		{				
			pNode->pNext = q;
			p->pNext = pNode;

			return 1;
		}
		p = q;
	}

	if(p->pNext==NULL)  // 若到了链尾,说明插入的结点为当前链表最大值,将其插入到链尾
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
                  
  Input         : *pBuffer   要初始化的buf指针
                  
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
  Description   : 初始化双向链表
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         gChildChainDivValue[i] 与 pChildChainHead[i] 一一对应
--------------------------------------------------------------------------------
*/                                    
_FILE_INFO_SORT_CODE_
void ChildChainInit(void)
{
	 UINT32 i,j;
	 for(i=0;i<CHILD_CHAIN_NUM;i++)	// 初始化各链表头节点
	 {

	   //pChildChainHead 是一个指针数组，它的每个元素均为一个FILE_INFO_INDEX_STRUCT类型的指针，
	   //gChildChainHead 是一个结构体数组，它的每个元素均为一个FILE_INFO_INDEX_STRUCT类型的结构体变量，
	 	pChildChainHead[i] = &gChildChainHead[i]; 
	 	pChildChainHead[i]->pNext = NULL; 
	 }
	 
	 gChildChainDivValue[0]=  0x0000; // 排序字符为空的放置于此
	 gChildChainDivValue[1]=   0x41; // 小于英文字母的字符，包括数字及其它符号  0x41 A字母的ascall 码
	 
	 j = 2;
	 for(i='A';i<'Z'+1;i++)
	 {
	    gChildChainDivValue[j] = i+1; // 英文字符,A~Z
	    j++;
	 }
	 
	 gChildChainDivValue[j++] = 0x4DFF; // 英文字符到汉字之间的unicode字符
	 
	 j = 29;
	 for(i=j;i<CHILD_CHAIN_NUM-1;i++)
	 {
	    gChildChainDivValue[i] = gChildChainDivValue[i-1]+(0x51a5)/(CHILD_CHAIN_NUM-30); // 对汉字部分字符进行42等分
	 }
	 
	 gChildChainDivValue[CHILD_CHAIN_NUM-1] = 0xffff; // 大于汉字的其它字符全部放至此链表
	 
	 /*for(i=0;i<CHILD_CHAIN_NUM;i++)
	 {
	    DisplayTestDecNum((i%6)*50,(i/6)*15,gChildChainDivValue[i]);
	 }
	 while(1);*/
	 /* 以下设置是为了在拼音排序时，按拼音字母分隔排序链表 */
	 /*gChildChainDivValue[0]=   0x0000;  
	 gChildChainDivValue[1]=   0x4DFF; //0x0000~0x4DFF; //  0~9,字母,其它符号					
	 gChildChainDivValue[2] =  0x4EC3; //0x4E00~0x4EC3; //  A, 吖		            
	 gChildChainDivValue[3] =  0x5235; //0x4EC4~0x5235; //  B，八                
	 gChildChainDivValue[4] =  0x576a; //0x5236~0x576a; //  C, 嚓                
	 gChildChainDivValue[5] =  0x5b40; //0x576b~0x5b40; //  D, 咑                
	 gChildChainDivValue[6] =  0x5bf0; //0x5b41~0x5bf0; //  E，妸                
	 gChildChainDivValue[7] =  0x5e55; //0x5bf1~0x5e55; //  F, 发                
	 gChildChainDivValue[8] =  0x6198; //0x5e56~0x6198; //  G, 旮                
	 gChildChainDivValue[9] =  0x65b3; //0x6199~0x65b3; //  H, 妎,ha             
	 gChildChainDivValue[10] = 0x6c00; //0x65b4~0x6c00; //  J, 丌                
	 gChildChainDivValue[11] = 0x6e30; //0x6c01~0x6e30; //  K, 咔                
	 gChildChainDivValue[12] = 0x7448; //0x6e31~0x7448; //  L, 垃                
	 gChildChainDivValue[13] = 0x7810; //0x7449~0x7810; //  M, 嘸                
	 gChildChainDivValue[14] = 0x7953; //0x7811~0x7953; //  N, 莻                
	 gChildChainDivValue[15] = 0x7978; //0x7954~0x7978; //  O, 噢                
	 gChildChainDivValue[16] = 0x7be2; //0x7979~0x7be2; //  P, 妑                
	 gChildChainDivValue[17] = 0x7fe9; //0x7be3~0x7fe9; //  q, 七,qi             
	 gChildChainDivValue[18] = 0x8131; //0x7fea~0x8131; //  r, 亽,ra             
	 gChildChainDivValue[19] = 0x8650; //0x8132~0x8650; //  s, 仨,sa             
	 gChildChainDivValue[20] = 0x89d0; //0x8651~0x89d0; //  t, 他                
	 gChildChainDivValue[21] = 0x8c43; //0x89d1~0x8c43; //  w, 屲                
	 gChildChainDivValue[22] = 0x9169; //0x8c44~0x9169; //  x, 夕                
	 gChildChainDivValue[23] = 0x9904; //0x916a~0x9904; //  y, 丫                
	 gChildChainDivValue[24] = 0x9fa4; //0x9905~0x9fa4; //  z, 帀*/                
}                           

/*
--------------------------------------------------------------------------------
  Function name : void GetSortName(UINT32 SectorOffset)
  Author        : anzhiguo
  Description   : 从flash中读取所有文件的长文件名，并存放到 gSortNameBuffer0和 gSortNameBuffer1 中
                  这两块空间是连续的，每个可以存放的文件数是系统允许的做大文件数的一半
                  
  Input         : SectorOffset：待排序字符在文件信息中的偏移地址
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         从保存在Flash中的文件信息读取需要排序的字符(文件名、ID3信息)，并且存与gSortNameBuffer0[].SortFileName中
                将所有文件的信息(需要排序的字符信息)读取
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
	    //从flash中读取截取的长文件名信息  //这里地址的计算在4k page的flash中会出问题
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
  Description   : 文件排序功能函数
                  
  Input         : uiSaveAddrOffset：待排序字符(文件名，id3title，id3singer，id3ablum)在文件信息中的偏移地址
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:         将gSortNameBuffer0和gSortNameBuffer1 中的信息进行分类排列，即分派到对应子链表pChildChainHead的适当的位子上
--------------------------------------------------------------------------------
*/                                        
_FILE_INFO_SORT_CODE_ 
void SortFunction(UINT32 uiSaveAddrOffset,UINT16 FileNum)
{
	UINT32  i,j;
	UINT16  filenum;  
    UINT32 uiFirsCharSortVal=0; // 当前插入结点排序名字的首字符的拼音排序值 
    
    FILE_INFO_INDEX_STRUCT *pTemp;
	
	 pTemp = gSortNameBuffer0;   
     filenum = FileNum;
     memset(&gSortNameBuffer0[0],0,sizeof(FILE_INFO_INDEX_STRUCT)*SORT_FILE_NAME_BUF_SIZE);
     memset(&gSortNameBuffer1[0],0,sizeof(FILE_INFO_INDEX_STRUCT)*SORT_FILE_NAME_BUF_SIZE);
         
     SortPageBufferInit(gPageTempBuffer);
     ChildChainInit();                  // 对各子链表的头节点进行初始化，并给定义每个链表的分段关键字
   	 GetSortName(uiSaveAddrOffset,filenum);     // 获取要排序的文件信息
   	 
   	 /* 对所有文件的长文件名进行排序 */
   	 for(i=0;i<filenum;i++)
     {
      	 uiFirsCharSortVal = GetCmpResult(pTemp->SortFileName[0]);
      	 j = 0;
      	 while(uiFirsCharSortVal>gChildChainDivValue[j]) // 通过长文件的第一个字符来判断需要在哪个子链表中排序
   	 		     j++; 
   	 		 BrowserListInsertBySort(pChildChainHead[j], pTemp); // 将gSortNameBuffer0中的文件信息逐个插入链表pChildChainHead中
   	 		 pTemp++;
      	
     }
}


/*
--------------------------------------------------------------------------------
  Function name : void SortUpdateFun(void)
  Author        : anzhiguo
  Description   : 对文件名信息进行排序，并在指定Flash中存储排序信息
                  
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
   UINT32   uiSortTypeCount = 0;            // 排序类型选择
   UINT32   uiSortInfoAddrOffset = 0;       // 同一个文件保存信息中的不同内容的偏移地址,单位Byte 需要的文件信息的获取地址
   UINT32   ulFileSortInfoSectorAddr = 0;   // 保存文件排序信息的起始sector地址  需要保存的文件序号信息的起始地址
    
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
   	 	 	     uiSortInfoAddrOffset = ID3_TITLE_SAVE_ADDR_OFFSET;//保存的文件信息的偏移地址(相对于保存文件信息起始地址)
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
   	 
   	 SortFunction(uiSortInfoAddrOffset,Sysfilenum->gMusicFileNum);//获取对应的文件信息，并排序，一次只能是一种信息
     
     uiCountTemp[uiSortTypeCount] = SaveSortInfo(uiSortTypeCount, ulFileSortInfoSectorAddr,MediaInfoAddress); //保存文件的排序号，以及分类信息，并返回该类文件的数量
   } 
   
   Sysfilenum->gID3TitleFileNum =  uiCountTemp[1]; // 得到具有ID3 Title信息的文件个数   uiCountTemp[0] 是所有的音乐文件个数
   Sysfilenum->gID3ArtistFileNum = uiCountTemp[2]; // 得到具有ID3 Artist信息的文件个数
   Sysfilenum->gID3AlbumFileNum =  uiCountTemp[3]; // 得到具有ID3 Album信息的文件个数 
   Sysfilenum->gID3GenreFileNum =  uiCountTemp[4]; // 得到具有ID3 Genre信息的文件个数 
}

#endif

/*
********************************************************************************
*
*                         End of FileInfoSort.c
********************************************************************************
*/


