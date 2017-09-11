
#ifndef _SYS_FINDFILE_H
#define _SYS_FINDFILE_H

#include "FsInclude.h"

#undef  EXT
#ifdef  IN_SYS_FINDFILE
#define EXT 
#else
#define EXT extern
#endif

/******************************************************************************/
/*                                                                            */
/*                          Macro Define                                      */
/*                                                                            */
/******************************************************************************/
//Service Section define
#define     _ATTR_SYS_FINDFILE_TEXT_        __attribute__((section("FindFileCode")))
#define     _ATTR_SYS_FINDFILE_DATA_        __attribute__((section("FindFileData")))
#define     _ATTR_SYS_FINDFILE_BSS_         __attribute__((section("FindFileBss"),zero_init))     

//------------------------------------------------------------------------------


/******************************************************************************/
/*                                                                            */
/*                          Struct Define                                     */
/*                                                                            */
/******************************************************************************/
//����˳����
typedef enum
{
    AUDIO_INTURN,
    AUDIO_RAND
    
}AUDIOPLAYMODE;

//Play Range define
#define     FIND_FILE_RANGE_DIR            2// once direction
#define     FIND_FILE_RANGE_ALL            3// cycle in direction

typedef struct 
{
    UINT16      TotalFiles;     //the total number of current direction/disk
	UINT16      CurrentFileNum; 
	UINT16      PlayedFileNum;  
	
    UINT16      DiskTotalFiles; //he total number of current disk
    
	UINT16      Range;//cycle direction or once direction
	UINT16      PlayOrder; // random or order
	
	uint8      *pExtStr;       //file type
	
	FDT         Fdt;
	FIND_DATA   FindData; 
	
	UINT16      RandomBuffer[16];
	UINT8       Path[3 + (MAX_DIR_DEPTH - 1) * 12 + 1];

#ifdef MEDIA_MODULE	
	UINT16  ucSelPlayType;  // add by phc
	UINT16  ucCurDeep; // add by phc
	UINT32  ulFullInfoSectorAddr;  // add by phc
	UINT32  ulSortInfoSectorAddr;  // add by phc
	UINT16  uiCurId[MAX_DIR_DEPTH]; // add by phc
	
	UINT32  uiBaseSortId; // add by phc
#endif	

} SYS_FILE_INFO;

/******************************************************************************/
/*                                                                            */
/*                          Variable Define                                   */
/*                                                                            */
/******************************************************************************/
_ATTR_SYS_BSS_  EXT SYS_FILE_INFO   AudioFileInfo ;
_ATTR_SYS_BSS_  EXT SYS_FILE_INFO   VideoFileInfo ;

/******************************************************************************/
/*                                                                            */
/*                         Function Declare                                   */
/*                                                                            */
/******************************************************************************/
//dgl audio UINT16 SysCheckTotalFileNum(uint8 *pExtStr);

INT16 SysFindFileInit(SYS_FILE_INFO *pSysFileInfo,UINT16 GlobalFileNum,UINT16 FindFileRange,UINT16 PlayMode, uint8 *pExtStr);


INT16 SysFindFileExt(SYS_FILE_INFO *pSysFileInfo, INT16 Offset);
                 
INT16 SysFindFile(SYS_FILE_INFO *pSysFileInfo,    INT16 Offset);

void SysFindFileModify(SYS_FILE_INFO *pSysFileInfo, UINT16       Range,UINT16 Mode);

void GetPlayInfo(UINT16 PlayMode, UINT16 *pRange, UINT16 *pRepMode);
void GetDirPath(UINT8 *pPath);
void CreatRandomFileList(UINT16 CurrentFileNum,UINT16 TotalFileNum,UINT16 *pBuffer);

//------------------------------------------------------------------------------

#endif
//******************************************************************************

