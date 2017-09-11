/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: MemDev.h
Desc: 

Author: 
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

#ifndef _MEMDEV_H
#define _MEMDEV_H

/*-------------------------------- Includes ----------------------------------*/
#include    <stdio.h>
#include    <string.h>
#include    "SysConfig.h"

/*------------------------------ Global Defines ------------------------------*/
#define MEM_DEV_UNKNOW        0
#define MEM_DEV_NAND          1
#define MEM_DEV_SPINOR        2
#define MEM_DEV_EMMC          3
#define MEM_DEV_SD            4
#define MEM_DEV_HOST          5

#define MEM_DEV_SCAN_NAND     (1<<MEM_DEV_NAND)
#define MEM_DEV_SCAN_SPINOR   (1<<MEM_DEV_SPINOR)
#define MEM_DEV_SCAN_EMMC     (1<<MEM_DEV_EMMC)
#define MEM_DEV_SCAN_SD       (1<<MEM_DEV_SD)
#define MEM_DEV_SCAN_HOST     (1<<MEM_DEV_HOST)

#define MEM_DEV_MASK_NAND     (1<<MEM_DEV_NAND)
#define MEM_DEV_MASK_SPINOR   (1<<MEM_DEV_SPINOR)
#define MEM_DEV_MASK_EMMC     (1<<MEM_DEV_EMMC)
#define MEM_DEV_MASK_SD       (1<<MEM_DEV_SD)
#define MEM_DEV_MASK_HOST     (1<<MEM_DEV_HOST)

#define MEM_DEV_SCAN_ALL_FLASH (MEM_DEV_SCAN_NAND|MEM_DEV_SCAN_SPINOR|MEM_DEV_SCAN_EMMC)

#define MEM_DEV_MASK_ALL_FLASH (MEM_DEV_MASK_NAND|MEM_DEV_MASK_SPINOR|MEM_DEV_MASK_EMMC)

typedef     unsigned long                   MEMDEV_ID;

#define MD_MAJOR(DevID)           ((uint16)(DevID>>16))
#define MD_MINOR(DevID)           ((uint16)(DevID))
#define MD_MKDEV(major, minor)    ((MEMDEV_ID)((major<<16)|(minor))) 
#define MD_MKLUN(DevID)           ((uint8)(DevID))


#define MEM_DEV_NAND0          MD_MKDEV(MEM_DEV_NAND, 0) 
#define MEM_DEV_NAND1          MD_MKDEV(MEM_DEV_NAND, 1)
#define MEM_DEV_NAND2          MD_MKDEV(MEM_DEV_NAND, 2)
#define MEM_DEV_NAND3          MD_MKDEV(MEM_DEV_NAND, 3)

#define MEM_DEV_SPINOR0        MD_MKDEV(MEM_DEV_SPINOR, 0) 
#define MEM_DEV_SPINOR1        MD_MKDEV(MEM_DEV_SPINOR, 1)
#define MEM_DEV_SPINOR2        MD_MKDEV(MEM_DEV_SPINOR, 2)


#define MEM_DEV_EMMC0          MD_MKDEV(MEM_DEV_EMMC, 0) 
#define MEM_DEV_EMMC1          MD_MKDEV(MEM_DEV_EMMC, 1)
#define MEM_DEV_EMMC2          MD_MKDEV(MEM_DEV_EMMC, 2)
#define MEM_DEV_EMMC3          MD_MKDEV(MEM_DEV_EMMC, 3)


#define MEM_DEV_SD0          MD_MKDEV(MEM_DEV_SD, 0) 
#define MEM_DEV_SD1          MD_MKDEV(MEM_DEV_SD, 1)
#define MEM_DEV_SD2          MD_MKDEV(MEM_DEV_SD, 2)
#define MEM_DEV_SD3          MD_MKDEV(MEM_DEV_SD, 3)

#define MEM_DEV_UDISK0          MD_MKDEV(MEM_DEV_HOST, 0) 
#define MEM_DEV_UDISK1          MD_MKDEV(MEM_DEV_HOST, 1)
#define MEM_DEV_UDISK2          MD_MKDEV(MEM_DEV_HOST, 2)


#ifdef _USBHOST_
#define UserDisk3ID          MEM_DEV_UDISK0 
#endif

#if (1==FW_IN_DEV)

#define SysDiskID            MEM_DEV_NAND0            
#define DataDiskID           MEM_DEV_NAND1

#define UserDisk0ID          MEM_DEV_NAND2

#ifdef  _MULT_DISK_
#define UserDisk1ID          MEM_DEV_NAND3

#ifdef _SDCARD_
#define UserDisk2ID          MEM_DEV_SD0
#else
#define UserDisk2ID          0
#endif

#else   //no define _MULT_DISK_
#define UserDisk1ID          0 

#ifdef _SDCARD_
#define UserDisk2ID          MEM_DEV_SD0
#else
#define UserDisk2ID          0
#endif

#endif

#elif(2==FW_IN_DEV)

#define SysDiskID            MEM_DEV_SPINOR0            
#define DataDiskID           MEM_DEV_SPINOR1

#define UserDisk0ID          MEM_DEV_SD0
#define UserDisk1ID          0
#define UserDisk2ID          0


#elif(3==FW_IN_DEV)

#define SysDiskID            MEM_DEV_EMMC0            
#define DataDiskID           MEM_DEV_EMMC1

#define UserDisk0ID          MEM_DEV_EMMC2
#define UserDisk1ID          0
#define UserDisk2ID          0

#elif(4==FW_IN_DEV)

#define SysDiskID            MEM_DEV_SD0            
#define DataDiskID           MEM_DEV_SD1

#define UserDisk0ID          MEM_DEV_SD2
#define UserDisk1ID          0
#define UserDisk2ID          0

#else

#endif


#define MD_MAX_UDISK         3

#define MD_MAX_NOTIFY        3

#define                     MD_EVENT_PRESENT    1
#define                     MD_EVENT_CHANGE     2
#define                     MD_EVENT_VAILID     3
//#define                     MD_EVENT_REMOVE     4
//#define                     MD_EVENT_HOTPLUG    5


#define RETURN_DEV_OK          (0)
#define RETURN_DEV_REMOVE      (-1)
#define RETURN_DEV_INSERT      (-2)


#define                     SYS_PROTECT                     //定义系统区写保护使能

/*------------------------------ Global Typedefs -----------------------------*/



typedef __packed struct tagMEMDEV_INFO
{
    uint32  capacity;           //(Sector为单位)  4Byte
    uint16  BlockSize;          //(Sector为单位)  2Byte
    uint16  PageSize;           //(Sector为单位)  1Byte
} MEMDEV_INFO, *pMEMDEV_INFO;


typedef void (*MEN_NOTIFIER)(uint16 major, uint32 event, uint32 param);

/*----------------------------- External Variables ---------------------------*/

extern  uint32 	SysProgDiskCapacity;			//系统程序盘容量
extern  uint32 	SysProgRawDiskCapacity;			//系统程序盘容量
extern  uint32 	SysDataDiskCapacity;			//系统程序盘容量
extern  uint32 	SysUserDisk2Capacity;			//系统程序盘容量

extern  uint16  UpgradeTimes;
extern  uint16  LoaderVer;
extern  uint8   ValidSysDisk;

/*------------------------- Global Function Prototypes -----------------------*/

extern uint32 MemDevInit(uint32 mask);   
extern uint32 MDDeInit(uint32 mask);

extern uint32 MDProbe(MEMDEV_ID DevID);

extern uint32 MDRead(MEMDEV_ID DevID, uint32 SecAdrr, uint32 nSec, void* pBuf);
extern uint32 MDWrite(MEMDEV_ID DevID, uint32 SecAdrr, uint32 nSec, void* pBuf);

extern uint32 MDGetInfo(MEMDEV_ID DevID, pMEMDEV_INFO pDevInfo);
extern uint32 MDGetCapacity(MEMDEV_ID DevID);

extern  bool MDChkValid(MEMDEV_ID DevID);
extern  bool MDChkPresent(MEMDEV_ID DevID);

extern  uint32 MDGetUDiskTab(MEMDEV_ID *pUDiskID, uint32 mask);

extern uint32 MDScanDev(uint8 AutoInit, uint32 mask);

extern  uint32 MDRegNotify(MEN_NOTIFIER notify);
extern  uint32 MDUnRegNotify(MEN_NOTIFIER notify);

extern  uint32 MDAddDisk(MEMDEV_ID DiskID);
extern  uint32 MDRemoveDisk(MEMDEV_ID DiskID);

extern  uint32 MDSyncIO(void);

#endif
