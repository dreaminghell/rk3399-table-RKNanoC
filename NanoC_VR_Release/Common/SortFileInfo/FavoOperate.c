/*
********************************************************************************
*          Copyright (C),2004-2005, tonyzyz, Fuzhou Rockchip Co.,Ltd.
*                             All Rights Reserved
*                                    V1.00
* FileName   : FavoOperate.c
* Author     : phc
* Description: 
* History    :        
*           <author>        <time>     <version>       <desc>
*            phc       07/08/07       1.0            ORG
*
********************************************************************************
*/

#define _IN_FAVORITE_OPERATE_

#include "SysInclude.h"
//#ifdef FAVOSUB

#ifdef MEDIA_MODULE
#include "FsInclude.h"



#include "medialibwin.h"
#include "MediaBroWin.h"

#include "AddrSaveMacro.h"
#include "FavoOperateMacro.h"



#include "FavoOperateGloable.h"

#include "SysFindFile.h"

/*
--------------------------------------------------------------------------------
  Function name : void SysCpuInit(void)
  Author        : ZHengYongzhi
  Description   : pll setting.disable interrupt,pwm Etc.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void FavoBlockInit(void)
{
    UINT8 ucBuffer[4];
    //read flag area and floder of favourite block
    MDReadData(DataDiskID, ((MediaInfoAddr+FAVORITE_BLOCK_SECTOR_START2)<<9), 4, ucBuffer);
    //judgu the flag of favourite
    if(ucBuffer[0]==0xFA&&ucBuffer[1]==0x00)
    {
        gFavoBlockSectorAddr = MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START2;
        gSysConfig.MedialibPara.gMyFavoriteFileNum = (UINT16)ucBuffer[2] + (((UINT16)ucBuffer[3])<<8);
    }
    else
    {        
        MDReadData(DataDiskID, ((MediaInfoAddr+FAVORITE_BLOCK_SECTOR_START1)<<9), 4, ucBuffer);
        if(ucBuffer[0]==0xFA&&ucBuffer[1]==0x00)
        {
            gFavoBlockSectorAddr = MediaInfoAddr+  FAVORITE_BLOCK_SECTOR_START1;
            gSysConfig.MedialibPara.gMyFavoriteFileNum = (UINT16)ucBuffer[2] + (((UINT16)ucBuffer[3])<<8);
        }
        else
        {         
            gFavoBlockSectorAddr = MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START1;
            gSysConfig.MedialibPara.gMyFavoriteFileNum = 0;
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : UINT16 GetPathInfo(UINT8 *Path)  
  Author        : anzhiguo
  Description   : get the path info
                  
  Input         : 
                  
  Return        : Path pointer to info.

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:       this function will be called when backgroud muisc.
--------------------------------------------------------------------------------
*/   
//_ATTR_MEDIABROWIN_CODE_
_ATTR_FAT_FIND_CODE_
UINT16 GetPathInfo(UINT8 *Path)
{
    UINT16 i;
    UINT16 uiCurDirNum;
    UINT8 *pPath;
    UINT8 PathName[12];
    FDT Fdt;
    FIND_DATA FindData;

    pPath = Path;    

    if (pPath[1] == ':')
    {
        CurDirDeep=0;						//direction deepth is 0
        CurDirClus=BootSector.BPB_RootClus;	//direction cluster boot direction  
        pPath += 2;
    }		

    if (Path[0] == '.')             // '.' show the start point is current path.
    {
        if (Path[1] == '\0' || Path[1] == '\\')		//case "." or ".\"
        {
            Path++;
        }
    }

    if(pPath[0] == '\\')
    {
        pPath++;
    }

    while (*pPath != '\0')
    {
        for (i=0; i<11; i++)		//fill space to direction item
        {
            PathName[i]=' ';
        }

        for (i=0; i<12; i++)
        {
            if (*pPath == '\\')		//to separator
            {
                pPath++;
                break;
            }
            if (*pPath == '\0')		//to end of path
            {
                break;				  	
            }
            PathName[i]=*pPath++;
        }

        uiCurDirNum=1;
        FindData.Clus=CurDirClus;
        FindData.Index=0;
        while(1)
        {    
            if (RETURN_OK != FindDirNext(&Fdt,&FindData))
            {
                return NOT_FIND_DIR;
            }
            for(i=0;i<11;i++)
            {
                if(PathName[i]!=Fdt.Name[i]) break;
            }

            if(i==11)
            {			        
                ++CurDirDeep; 
                CurDirClus = ChangeDir(uiCurDirNum);	
                FindData.Clus=CurDirClus;
                FindData.Index=0; 
                break;      
            }			     
            uiCurDirNum++;
        }
    }

    return RETURN_OK;    
}

/*
--------------------------------------------------------------------------------
  Function name : void MusicBroKeyFun(void)
  Author        : anzhiguo
  Description   : get file FDT information by the path infomation and short file name.
                  
  Input         : 
                  
  Return        : RETURN_OK     
                  NOT_FIND_FILE   

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:        
--------------------------------------------------------------------------------
*/   
_ATTR_SYS_CODE_
UINT16 FindFileByShortPath(FDT *Rt, UINT8 *PathBuffer, UINT8 *ShortFileName)
{
    FIND_DATA FindData;
    UINT32 i;
    UINT16 uiCurDirNum;

    if(RETURN_OK != GetPathInfo(PathBuffer))
    {
        return NOT_FIND_FILE;
    }
    FindData.Clus=CurDirClus;
    FindData.Index=0;
    while(1) // find file
    {
        //uiFindFileResult = FindNext(&AudioFileInfo.Fdt, &FindDataInfo, AudioFileExtString);	
        if (RETURN_OK != FindNext(Rt, &FindData, "*"))
        {
            return NOT_FIND_FILE;
        }
        
        for(i=0;i<11;i++)
        {
            if(ShortFileName[i]!=Rt->Name[i]) 
            {
                break;
            }
        }

        if(i==11)  
        {
            return RETURN_OK;
        }
    }

    return NOT_FIND_FILE;
}

/*
--------------------------------------------------------------------------------
  Function name : UINT16 GetFavoInfo(FDT *Rt, UINT16 uiFavoID) 
  Author        : anzhiguo
  Description   : get fath info
                  
  Input         : uiFavoID -- the serial number in favourite folder.
                  
  Return        : Rt--find the pointer of file fdt info.

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:        
--------------------------------------------------------------------------------
*/   
//sytem code is called in FileModule.c
_ATTR_SYS_CODE_
UINT16 GetFavoInfo(FDT *Rt, UINT16 uiFavoID ,UINT8* path, UINT8* longname)
{   
    UINT8 ucPathBuffer[PATH_SIZE+SHORT_NAME_SIZE],filenum[2];
    UINT16 temp;

    MDReadData(DataDiskID, (UINT32)((gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)<<9)+(UINT32)(uiFavoID)*FAVORITE_MUSIC_SAVE_SIZE, (PATH_SIZE+11), ucPathBuffer);
    memcpy(path,ucPathBuffer,PATH_SIZE);
    temp = FindFileByShortPath(Rt, &ucPathBuffer[0], &ucPathBuffer[PATH_SIZE]);  
    memcpy(longname,&LongFileName,MAX_FILENAME_LEN*2);

    return(temp);
}

#ifdef FAVOSUB
/*
--------------------------------------------------------------------------------
  Function name : void ChangeFavoSaveBlock(void)
  Author        : anzhiguo
  Description   : exchange block address that keeping favourite floder.prevent multiple write,and make bad block.
                  
  Input         : 
                  
  Return        :
  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:        
--------------------------------------------------------------------------------
*/   
_ATTR_MEDIABROWIN_CODE_
void ChangeFavoSaveBlock(void)
{
    if(gFavoBlockSectorAddr==(MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START2))
        gFavoBlockSectorAddr = MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START1;
    else
       gFavoBlockSectorAddr = MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START2;
}

/*
--------------------------------------------------------------------------------
  Function name : UINT16 SaveGroupFavoSong(UINT32 ulFullInfoSectorAddr, UINT32 ulSortSectorAddr, UINT16 uiBaseSortId, UINT16 uiSaveItemNum)  
  Author        : anzhiguo
  Description   : save a group songs to favourite
                  
  Input         : ulFullInfoSectorAddr --sec address to save thedetailed file info
                  ulSortSectorAddr --sec address that to save the file sort info
                  uiBaseSortId--
                  uiSaveItemNum -- the number that add to favourite.
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:        
--------------------------------------------------------------------------------
*/   
_ATTR_MEDIABROSUBWIN_CODE_
UINT16 SaveGroupFavoSong(UINT32 ulFullInfoSectorAddr, UINT32 ulSortSectorAddr, UINT16 uiBaseSortId, UINT16 uiSaveItemNum)
{
    UINT16 i,j,k;
    UINT8 *pBuff;
    UINT8 *pBuff1;
    FDT Fdt;
    UINT32 ulPreBlockSectorAddr;
    UINT16 uiPageCount = 0;  //
    UINT16 uiFavoCountIndex = 0;//the file number when page is not been fill full?

    UINT16 basesortid;
    UINT16 filenum = uiSaveItemNum;
    basesortid = uiBaseSortId;
    ModuleOverlay(MODULE_ID_FAVO_RESET, MODULE_OVERLAY_ALL);//just for utilize the favoreset data memory(quote other global varible group.)

    PMU_EnterModule(PMU_MEDIAUPDATA);
    if(gSysConfig.MedialibPara.gMyFavoriteFileNum==0)//no file in favourite
    { 
        ulPreBlockSectorAddr = MediaInfoAddr+FAVORITE_BLOCK_SECTOR_START2;
        gFavoBlockSectorAddr = MediaInfoAddr+FAVORITE_BLOCK_SECTOR_START1;
        memset(gFavoResetBuffer,0xff,MEDIAINFO_PAGE_SIZE);
       
        MDWrite(DataDiskID, ulPreBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);//
    }
    else
    {
        ulPreBlockSectorAddr = gFavoBlockSectorAddr;//backup old favourite address(read out old information in here,then add new file information write to new favourite address.)
        ChangeFavoSaveBlock();//exchang the favourite block address,to ready to write to flash
    }

    memset(gFavoResetBuffer,0,MEDIAINFO_PAGE_SIZE);
    memset(gFavoResetBuffer1,0,MEDIAINFO_PAGE_SIZE);

    gFavoResetBuffer[0] = 0xFA; // FAVORITE block flag bit
    gFavoResetBuffer[1] = 0x00;         
    gFavoResetBuffer[2] = (gSysConfig.MedialibPara.gMyFavoriteFileNum+filenum)&0xff; 
    gFavoResetBuffer[3] = ((gSysConfig.MedialibPara.gMyFavoriteFileNum+filenum)>>8)&0xff; 


    MDWrite(DataDiskID, gFavoBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);

    uiPageCount = gSysConfig.MedialibPara.gMyFavoriteFileNum/FAVORITE_NUM_PER_PAGE;

    for(i=0;i<uiPageCount+1;i++)//旧的收藏夹信息写到新收藏夹的地址
    {
        //从原来的收藏夹地址读取旧收藏夹信息
        MDReadData(DataDiskID, ((ulPreBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET*(i+1)))<<9, MEDIAINFO_PAGE_SIZE, gFavoResetBuffer);

        if(i == uiPageCount)
        {
            break;
        }

        //写入新的收藏夹的地址
        MDWrite(DataDiskID, (gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET+(UINT32)i*MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE), MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);
    } 

    //求取收藏夹信息最后一个page存放的文件数(0表示文件信息总大小是一个page大小的整数倍)
    uiFavoCountIndex = gSysConfig.MedialibPara.gMyFavoriteFileNum%FAVORITE_NUM_PER_PAGE;

    //get file path information and save it.
    for(i=0;i<filenum;i++)
    {
        gSysConfig.MedialibPara.gMyFavoriteFileNum ++;
        //get a file number,and save it to favourite,it can be used to judge that whether this file had been in favourite.
        GetSavedMusicPath(gPathBuffer, SortInfoAddr.ulFileFullInfoSectorAddr, SortInfoAddr.ulFileSortInfoSectorAddr, i+uiBaseSortId);

        if(RETURN_OK != FindFileByShortPath(&Fdt, &gPathBuffer[0], &gPathBuffer[PATH_SIZE]))//if the file which path is pointed by path information open fail,then it will not add to favourite. 
        {
            gSysConfig.MedialibPara.gMyFavoriteFileNum--;
            continue;
        }    
        else //else judge whethe the file had been in favourite
        {
            UINT32 count;
                        
            for(k=0;k<uiPageCount;k++)//wirte the old favourite information write to new favourite address
            { 
                //读取收藏夹信息判断文件是否已经添加
                MDReadData(DataDiskID, ((ulPreBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)<<9+((UINT32)i*MEDIAINFO_PAGE_SIZE)), MEDIAINFO_PAGE_SIZE, gFavoResetBuffer1);
                for(count=0;count < FAVORITE_NUM_PER_PAGE;count++)
                {
                    pBuff = &gFavoResetBuffer1[count*FAVORITE_MUSIC_SAVE_SIZE];
                    if(memcmp(gPathBuffer,pBuff,(PATH_SIZE+11))==0)// 路径信息相同，收藏夹中已经保存，不在保存
                    {
                        gSysConfig.MedialibPara.gMyFavoriteFileNum--;
                        SendMsg(MSG_FILE_IN_FAVO);
                        break;
                    }
                }
                if(CheckMsg(MSG_FILE_IN_FAVO))
                {
                    break;
                }
            } 
            if(GetMsg(MSG_FILE_IN_FAVO))//跳出继续添加下一个文件
            {
                continue;
            }
            MDReadData(DataDiskID, ((ulPreBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)<<9+((UINT32)uiPageCount*MEDIAINFO_PAGE_SIZE)), MEDIAINFO_PAGE_SIZE, gFavoResetBuffer1)	;

            //get the number of favourite in the last page,(0 means the total size of file information is multiple page size)

            for(count=0;count < uiFavoCountIndex;count++)
            {
                pBuff = &gFavoResetBuffer1[count*FAVORITE_MUSIC_SAVE_SIZE];
                if(memcmp(gPathBuffer,pBuff,(PATH_SIZE+11))==0)// path information is the same,favourite had saved,do not need to save again.
                {
                    gSysConfig.MedialibPara.gMyFavoriteFileNum--;
                    SendMsg(MSG_FILE_IN_FAVO);
                    break;
                }  
            }

            if(GetMsg(MSG_FILE_IN_FAVO))
            {
                continue;
            }
        }

        pBuff1 = &gFavoResetBuffer[uiFavoCountIndex*FAVORITE_MUSIC_SAVE_SIZE];

        for(j=0;j<FAVORITE_MUSIC_PATH_SIZE+FAVORITE_MUSIC_NAME_SIZE;j++)
        {
            pBuff1[j] = gPathBuffer[j]; // save path information
        }
        uiFavoCountIndex++;

        if(uiFavoCountIndex==FAVORITE_NUM_PER_PAGE)   
        {
            MDWrite(DataDiskID, (gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)+(UINT32)(uiPageCount*MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE), MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);

            uiFavoCountIndex=0;
            uiPageCount++;
        }
    }

    if(uiFavoCountIndex)
    {  
        MDWrite(DataDiskID, (gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)+(UINT32)(uiPageCount*MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE), MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);
    }
       
    memset(gFavoResetBuffer,0xff,MEDIAINFO_PAGE_SIZE);
    MDWrite(DataDiskID, ulPreBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);//清除收藏夹存储区

    PMU_ExitModule(PMU_MEDIAUPDATA);  
}


/*
--------------------------------------------------------------------------------
  Function name : void RemoveOneFavoSong(UINT16 uiDelID)  
  Author        : anzhiguo
  Description   : delete a file info in favourite. 
                  
  Input         : uiDelID -- the file serial number that will delete. 
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:        
--------------------------------------------------------------------------------
*/   
_ATTR_MEDIAFAVOSUBWIN_CODE_
void RemoveOneFavoSong(UINT16 uiDelID)
{
    UINT16 j,i=0;
    UINT16 uiSaveIndex=0;
    UINT16 uiPageCount=0;
    UINT32 ulPreBlockSectorAddr;
    
    ModuleOverlay(MODULE_ID_FAVO_RESET, MODULE_OVERLAY_ALL);//just for utilize the memort of favoreset(quote the global data group)

    PMU_EnterModule(PMU_MEDIAUPDATA);
    ulPreBlockSectorAddr = gFavoBlockSectorAddr;
    ChangeFavoSaveBlock();

    memset(gFavoResetBuffer,0,MEDIAINFO_PAGE_SIZE);

    gFavoResetBuffer[0] = 0xFA; // the flag of FAVORITE block
    gFavoResetBuffer[1] = 0x00; //         
    gFavoResetBuffer[2] = (gSysConfig.MedialibPara.gMyFavoriteFileNum-1)&0xff; 
    gFavoResetBuffer[3] = ((gSysConfig.MedialibPara.gMyFavoriteFileNum-1)>>8)&0xff;

    //写收藏夹标记page
    MDWrite(DataDiskID, gFavoBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);
    //读取所有的文件号

    while(1)
    {
        if(i==uiDelID)  //要删除文件的id时，读取其下一个文件的信息，补充到删除信息的位置上
        {
            ++i;
        }

        if(i==gSysConfig.MedialibPara.gMyFavoriteFileNum) 
        {
            break;
        }
        
        MDReadData(DataDiskID, (((ulPreBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)<<9)+((UINT32)i*FAVORITE_MUSIC_SAVE_SIZE)), (PATH_SIZE+11), &gFavoResetBuffer[uiSaveIndex*FAVORITE_MUSIC_SAVE_SIZE]);

        uiSaveIndex++;

        if(uiSaveIndex==FAVORITE_NUM_PER_PAGE)
        {
            uiSaveIndex = 0;

            MDWrite(DataDiskID, (gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)+(UINT32)uiPageCount*MEDIAINFO_PAGE_SIZE, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);

            uiPageCount++;
        }
        ++i;            
    }

    if(uiSaveIndex)
    {    
        MDWrite(DataDiskID, (gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)+(UINT32)uiPageCount*MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);
    }

    --gSysConfig.MedialibPara.gMyFavoriteFileNum;         

    memset(gFavoResetBuffer,0xff,MEDIAINFO_PAGE_SIZE);
    
    MDWrite(DataDiskID, ulPreBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);

    PMU_ExitModule(PMU_MEDIAUPDATA);
    
    return;
}

/*
--------------------------------------------------------------------------------
  Function name : UINT16 GetPathInfo(UINT8 *Path)  
  Author        : anzhiguo
  Description   : delete whole favourite.
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009/06/02         Ver1.0
  desc:        
--------------------------------------------------------------------------------
*/   
_ATTR_MEDIAFAVOSUBWIN_CODE_
void RemoveAllFavoSong(void)
{
    memset(gFavoResetBuffer,0xff,MEDIAINFO_PAGE_SIZE);
    MDWrite(DataDiskID, MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START1, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);
    MDWrite(DataDiskID, MediaInfoAddr + FAVORITE_BLOCK_SECTOR_START2, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);
    
    gSysConfig.MedialibPara.gMyFavoriteFileNum = 0;
}

#endif
#endif


