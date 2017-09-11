/*
********************************************************************************
*          Copyright (C),2004-2005, tonyzyz, Fuzhou Rockchip Co.,Ltd.
*                             All Rights Reserved
*                                    V1.00
* FileName   : FavoReset.c
* Author     : azg
* Description: 
* History    :        
*           <author>        <time>     <version>       <desc>
*            azg       07/08/21       1.0            ORG
*
********************************************************************************
*/
#define _IN_FAVORESET
#include "SysInclude.h"

#ifdef MEDIA_MODULE

#include "FsInclude.h"
#include "AddrSaveMacro.h"

#include "MediaBroWin.h"

#include "FavoOperateGloable.h"
#include "FavoOperateMacro.h"
#ifdef FAVOSUB

_FAVORESET_CODE_
UINT16 FavoReset(void)
{
    UINT16 j;
    UINT16 i=0;
    UINT16 uiSaveIndex=0;
    UINT16 uiPageCount=0;
    UINT32 ulPreBlockSectorAddr;
    FDT Rt;
    UINT16 uiFavoDelCount=0;
    UINT8 *pPathBuffer;

    if(gSysConfig.MedialibPara.gMyFavoriteFileNum==0)//û���ļ�
    {
        return;
    }
    ulPreBlockSectorAddr = gFavoBlockSectorAddr;
    {
    if(gFavoBlockSectorAddr==(MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START2))
        gFavoBlockSectorAddr = MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START1;
    else
       gFavoBlockSectorAddr = MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START2;
    }
    gFavoResetBuffer[0] = 0xFA; // FAVORITE��ı�־λ
    gFavoResetBuffer[1] = 0x00; //         
    gFavoResetBuffer[2] = (gSysConfig.MedialibPara.gMyFavoriteFileNum)&0xff; 
    gFavoResetBuffer[3] = ((gSysConfig.MedialibPara.gMyFavoriteFileNum)>>8)&0xff;

    MDWrite(DataDiskID, gFavoBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);

    for(i=0;i<gSysConfig.MedialibPara.gMyFavoriteFileNum;i++)
    {   
        pPathBuffer = &gFavoResetBuffer[uiSaveIndex*FAVORITE_MUSIC_SAVE_SIZE];
        //�ղؼ�·����Ϣ��ŵ�ַΪ�ղؼ���ʼ��ַ��һ��MEDIAINFO_PAGE_SIZE��С
        MDReadData(DataDiskID, (((UINT32)ulPreBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)<<9)+(UINT32)(i)*FAVORITE_MUSIC_SAVE_SIZE, (PATH_SIZE+SHORT_NAME_SIZE), pPathBuffer);

        if(RETURN_OK == FindFileByShortPath(&Rt, &pPathBuffer[0], &pPathBuffer[PATH_SIZE]))
        {
            uiSaveIndex++;
        }
        else
        {   
            uiFavoDelCount++;
        }
        if(uiSaveIndex==FAVORITE_NUM_PER_PAGE)//д��һ��buf
        {
            uiSaveIndex = 0;
            //uiPageCount��һ����Ϊǰ����һ��MEDIAINFO_PAGE_SIZE��С�Ŀռ�ֻ�����һ���ղؼе��ļ����ͱ��
            MDWrite(DataDiskID, (gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET +(uiPageCount*MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE)), MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);

            uiPageCount++;
        }           

    }

    if(uiSaveIndex)
    {          
        MDWrite(DataDiskID, (gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET +(uiPageCount*MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE)), MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);
    }
    memset(gFavoResetBuffer,0xff,MEDIAINFO_PAGE_SIZE);
    
    MDWrite(DataDiskID, ulPreBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);


    if(uiFavoDelCount)//��ɾ�����ļ����������дһ��
    {
        gSysConfig.MedialibPara.gMyFavoriteFileNum -= uiFavoDelCount;    

        ulPreBlockSectorAddr = gFavoBlockSectorAddr;
        {
            if(gFavoBlockSectorAddr==(MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START2))
                gFavoBlockSectorAddr = MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START1;
            else
                gFavoBlockSectorAddr = MediaInfoAddr+ FAVORITE_BLOCK_SECTOR_START2;
        }

        gFavoResetBuffer[0] = 0xFA; // the flag bit flag the FAVORITE block.
        gFavoResetBuffer[1] = 0x00; //         
        gFavoResetBuffer[2] = (gSysConfig.MedialibPara.gMyFavoriteFileNum)&0xff; 
        gFavoResetBuffer[3] = ((gSysConfig.MedialibPara.gMyFavoriteFileNum)>>8)&0xff;
        
        MDWrite(DataDiskID, gFavoBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);

        uiPageCount = gSysConfig.MedialibPara.gMyFavoriteFileNum/FAVORITE_NUM_PER_PAGE;

        if(gSysConfig.MedialibPara.gMyFavoriteFileNum%FAVORITE_NUM_PER_PAGE>0) 
        {
            ++uiPageCount;
        }
        for(i=0;i<uiPageCount;i++)
        {
            //read the old file path info.
            MDReadData(DataDiskID, (ulPreBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)<<9+i*MEDIAINFO_PAGE_SIZE, MEDIAINFO_PAGE_SIZE, gFavoResetBuffer);
            MDWrite(DataDiskID, (gFavoBlockSectorAddr+FAVO_PATH_SECTOR_OFFSET)+i*(MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE), MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);
        }

        memset(gFavoResetBuffer,0xff,MEDIAINFO_PAGE_SIZE);
        
        MDWrite(DataDiskID, ulPreBlockSectorAddr, MEDIAINFO_PAGE_SIZE/SECTOR_BYTE_SIZE, gFavoResetBuffer);

    }

    return;    
}

#endif

#endif
