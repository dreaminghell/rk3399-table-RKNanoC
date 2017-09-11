/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: SpiNor.h
Desc:

Author:
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

#ifndef _SPINOR_H
#define _SPINOR_H


#define     CHIP_SIGN           0x4F4E414E                  //NANO

/*ID BLOCK SECTOR 0 INFO*/
typedef __packed struct tagIDSEC0
{
    uint32  magic;              //0x0ff0aa55, MASKROM限定不能更改
    uint8   reserved[56-4];
    uint32  nandboot2offset;
    uint32  nandboot2len;
    uint32  nandboot1offset1;
    uint32  nandboot1len1;
    uint32  nandboot1offset2;
    uint32  nandboot1len2;
    uint8   CRC[16];
    uint8   reserved1[512-96];
} IDSEC0, *pIDSEC0;

/*ID BLOCK SECTOR 1 INFO*/
typedef __packed struct tagIDSEC1
{
    uint16  SysAreaBlockRaw;        //系统保留块, 以原始Block为单位
    uint16  SysProgDiskCapacity;    //系统固件盘容量, 以MB为单位
	uint16  SysDataDiskCapacity;
	uint16  Disk2Size;
	uint16  Disk3Size;
	uint32	ChipTag;
	uint32	MachineId;

	uint16	LoaderYear;
	uint16	LoaderDate;
	uint16	LoaderVer;
	uint16	FirmMasterVer;		// (24-25)	BCD编码 固件主版本
    uint16	FirmSlaveVer;			// (26-27)	BCD编码 固件从版本
    uint16  FirmSmallVer;			// (28-29)	BCD编码 固件子版本
	uint16  LastLoaderVer;
	uint16  UpgradeTimes;
	uint16  MachineInfoLen;
	uint8	MachineInfo[30];
	uint16	ManufactoryInfoLen;
	uint8	ManufactoryInfo[30];
	uint16	FlashInfoOffset;
	uint16	FlashInfoLen;
    uint8	Reserved2[382];				// (102-483)保留
	uint32	FlashSize;				    //以sector为单位
	uint16  BlockSize;                  //以SEC表示的BLOCK SIZE
	uint8   PageSize;                  //以SEC表示的PAGE SIZE
	uint8   ECCBits;
	uint8   AccessTime;                //读写cycle时间, ns
    uint8   reserved2[5];
	uint16  FirstReservedBlock;		// (498-499)Idblock之后第一个保留块的的位置		(单位:block)
    uint16  LastReservedBlock;		// (500-501)Idblock之后最后一个保留块的的位置	(单位:block)
	uint16  IdBlock0;
	uint16  IdBlock1;
	uint16  IdBlock2;
	uint16  IdBlock3;
	uint16  IdBlock4;
}IDSEC1, *pIDSEC1;

/*-------------------------------- Includes ----------------------------------*/


/*------------------------------ Global Defines ------------------------------*/


/*------------------------------ Global Typedefs -----------------------------*/


/*----------------------------- External Variables ---------------------------*/


/*------------------------- Global Function Prototypes -----------------------*/
extern void   SPINor1Test(void);
extern uint32 SPINorFlash1_test(void);
extern uint32 SPINorInit(void);
extern uint32 SPINorDeInit(void);

extern uint32 SPINorRead(uint32 Addr, uint8 *pData, uint32 size);
extern uint32 SPINorWrite(uint32 Addr, uint8 *pData, uint32 size);

extern uint32 MDSPINorRead(uint8 lun, uint32 sec, uint32 nSec, void *pData);
extern uint32 MDSPINorWrite(uint8 lun, uint32 sec, uint32 nSec, void *pData);
extern uint32 MDSPIGetInfo(uint8 lun, pMEMDEV_INFO pDevInfo);
extern bool   MDSPIChkEvent(uint32 event);

#endif
