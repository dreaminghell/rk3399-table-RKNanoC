/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: SpiNor.c
Desc:

Author: chenfen
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/

#include "MDconfig.h"

#if (SPINOR_DRIVER==1)
#include "Gpio.h"
#include "hw_spi.h"
#include "SpiNor.h"
#include "ModuleOverlay.h"

/*------------------------------------ Defines -------------------------------*/

//#define SPINOR_TEST

#define JEDECID_WINBOND     0xEF
#define JEDECID_ST          0x20
#define JEDECID_SPANSION    0x01
#define JEDECID_CFEON       0x1C


#define CMD_READ_JEDECID        (0x9F)
#define CMD_READ_DATA           (0x03)
#define CMD_READ_STATUS         (0x05)
#define CMD_WRITE_STATUS        (0x01)
#define CMD_PAGE_PROG           (0x02)
#define CMD_SECTOR_ERASE        (0x20)
#define CMD_BLOCK_ERASE         (0xD8)
#define CMD_CHIP_ERASE          (0xC7)
#define CMD_WRITE_EN            (0x06)
#define CMD_WRITE_DIS           (0x04)


#define NOR_PAGE_SIZE           256
#define NOR_BLOCK_SIZE          (64*1024)
#define NOR_SEC_PER_BLK         (NOR_BLOCK_SIZE/512)

#define SPI_CTRL_CONFIG         (SPI_MASTER_MODE|MOTOROLA_SPI|RXD_SAMPLE_NO_DELAY|APB_BYTE_WR|MSB_FBIT|LITTLE_ENDIAN_MODE|CS_2_SCLK_OUT_1_CK|CS_KEEP_LOW|SERIAL_CLOCK_POLARITY_HIGH|SERIAL_CLOCK_PHASE_START|DATA_FRAME_8BIT)

#define SPI_IDB_NUM             1
#define SPI_IDB_SIZE            (SPI_IDB_NUM*NOR_SEC_PER_BLK)


/*----------------------------------- Typedefs -------------------------------*/

typedef  struct tagSPINOR_SPEC
{
    uint8 Manufacturer;
    uint8 MemType;
    uint16 PageSize;
    uint32 BlockSize;
    uint32 capacity;
    uint8 valid;
}SPINOR_SPEC, *pSPINOR_SPEC;


/*-------------------------- Forward Declarations ----------------------------*/
void SPINorTest(void);

/* ------------------------------- Globals ---------------------------------- */
_ATTR_FLASH_BSS_ static uint32 DummySize;
_ATTR_FLASH_BSS_ static uint8  SPICsn;

_ATTR_FLASH_BSS_ SPINOR_SPEC NorSpec;

_ATTR_FLASH_INIT_CODE_
const uint8 NorDeviceCode[] =
{
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19
};

_ATTR_FLASH_INIT_CODE_
const uint32 NorMemCapacity[] =
{
    0x20000,        //128k-byte
    0x40000,        //256k-byte
    0x80000,        //512k-byte
    0x100000,       // 1M-byte
    0x200000,       // 2M-byte
    0x400000,       // 4M-byte
    0x800000,       // 8M-byte
    0x1000000,      //16M-byte
    0x2000000       // 32M-byte
};


/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/

/*------------------------ Function Implement --------------------------------*/


/*
Name:       SPISetClk
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_INIT_CODE_
static void SPISetClk(void)
{
    //SPI CTRL ѡ��24M ���� ����Ƶ����
    CRUReg->CRU_CLKSEL_SPI_CON = 1<<0 | 0<<1 | 1<<16 | 63<<17;
}

/*
Name:       SPIIomux
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_INIT_CODE_
static void SPIIomux(uint32 index)
{
    if (index)
    {
        //spi_rxd_p1, spi_txd_p1, spi_clk_p1, spi_csn1
        GRFReg->GPIO0B_IOMUX = 1<<2 | 1<<4 | 1<<6 | 1<<8 | 3<<18| 3<<20| 3<<22 | 3<<24;

        //spi0 �е�gpio����Ӱ�쵽 spi1 �ӿ�
        GRFReg->GPIO0A_IOMUX = 0<<6 | 0<<8 | 0<<10 | 0<<12 | 3<<22| 3<<24| 3<<26 | 3<<28;

        GRFReg->IOMUX_CON1 = 1<<1 | 1<<17; //spi_xxx_p1 available for spi ������ֻӰ��miso
        SPICsn = 2;
    }
    else
    {
        //spi_rxd_p0, spi_txd_p0, spi_clk_p0, spi_csn0
        GRFReg->GPIO0A_IOMUX = 2<<6 | 2<<8 | 2<<10 | 2<<12 | 3<<22| 3<<24| 3<<26 | 3<<28;

        #if 1
        //GRFReg->GPIO0A_IOMUX = 0<<12|3<<28; //ʹ��GPIOA[6]��SPI Flash0��CS
        //GRFReg->GPIO0A_IOMUX = 0<<13|3<<28; //ʹ��GPIOA[6]��SPI Flash0��CS
        GpioMuxSet(GPIOPortA_Pin6,IOMUX_GPIOA6_IO);
        //����SPI Flash0 CSƬѡ��Ч
        Gpio_SetPinDirection(GPIOPortA_Pin6,GPIO_OUT);
        Gpio_SetPinLevel(GPIOPortA_Pin6, GPIO_LOW);//SPI0 Flash0_CS
        #endif

        //spi1 �е�gpio����Ӱ�쵽 spi0 �ӿ�
        GRFReg->GPIO0B_IOMUX = 0<<2 | 0<<4 | 0<<6 | 0<<8 | 3<<18| 3<<20| 3<<22 | 3<<24;

        GRFReg->IOMUX_CON1 = 0<<1 | 1<<17; //spi_xxx_p0 available for spi ������ֻӰ��miso
        SPICsn = 1;
    }
}

/*
Name:       SPIProgCmd
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static uint32 SPIProgCmd(uint32 *pCmd, uint32 CmdLen, uint8 *pData, uint32 size)
{
    uint32 ret=OK;

    SPICtl->SPI_CTRLR0 = SPI_CTRL_CONFIG | TRANSMIT_ONLY;
    SPICtl->SPI_ENR = 1;
    SPICtl->SPI_SER = SPICsn;
    if (SPICsn == 1)
        Gpio_SetPinLevel(GPIOPortA_Pin6, GPIO_LOW);//SPI0 Flash0_CS

    while(CmdLen)
    {
        SPICtl->SPI_TXDR[0] = *pCmd++;
        CmdLen--;
    }

    while (size)
    {
        if ((SPICtl->SPI_SR & TRANSMIT_FIFO_FULL) != TRANSMIT_FIFO_FULL)
        {
            SPICtl->SPI_TXDR[0] = *pData++;
            size--;
        }
    }

    while ((SPICtl->SPI_SR & TRANSMIT_FIFO_EMPTY) != TRANSMIT_FIFO_EMPTY);

    DelayUs(1); //Ƶ���ܽϸ�ʱ����12MHZ�����������ʱ����̻�ʧ��
    SPICtl->SPI_SER = 0;
    SPICtl->SPI_ENR = 0;
    if (SPICsn == 1)
        Gpio_SetPinLevel(GPIOPortA_Pin6, GPIO_HIGH);//SPI0 Flash0_CS
    DelayUs(1);               //cs# high time > 100ns

    return ret;

}

/*
Name:       SPISendCmd
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
static uint32 SPISendCmd(uint32 *pCmd, uint32 CmdLen, uint8 *pData, uint32 size)
{
    uint32 ret=OK;
    uint32 time;
    uint32 DummyLen, DataLen;

    SPICtl->SPI_CTRLR0 = SPI_CTRL_CONFIG| TRANSMIT_RECEIVE;

    SPICtl->SPI_ENR = 1;
    SPICtl->SPI_SER = SPICsn;
    if (SPICsn == 1)
        Gpio_SetPinLevel(GPIOPortA_Pin6, GPIO_LOW);//SPI0 Flash0_CS


    /*�������շ�ģʽ, ���������ͬʱ��������ݽ���, ����Ҫ���˵�CmdLen���ȵ�����
     DummySize ���������ɺ�, ����û�����ϳ���, ��Ҫ�ȴ�����Ч�����ݳ���,
     һ��SPINOR�ǲ���Ҫ�ȴ���, ����DummySize=0*/
    DummyLen = (size > 0)? (DummySize+CmdLen):CmdLen;
    DataLen = size + DummyLen;
    while (DataLen)
    {
        if (CmdLen)
        {
            SPICtl->SPI_TXDR[0] = *pCmd++;
            CmdLen--;
        }
        else
        {
            SPICtl->SPI_TXDR[0] = 0xFFFFFFFF;     //send clock
        }

        for(time=0; time<100; time++)  //����SPI Flash�ٶ���12Mb����һ�����ݻ���һ�����ݣ������Ҫ����8bit��ʱ�䣬Ҳ����83.34ns*8=666.67ns��timeout��1us�����
        {
            if ((SPICtl->SPI_SR & RECEIVE_FIFO_EMPTY) != RECEIVE_FIFO_EMPTY)
            {
                if (DummyLen > 0)   //�����������, ����Ҫ�ȼ���ʱ��,���ݲų���
                {
                    *pData = (uint8)(SPICtl->SPI_RXDR[0] & 0xFF); //��Ч����
                    DummyLen--;
                }
                else
                {
                    *pData++ = (uint8)(SPICtl->SPI_RXDR[0] & 0xFF);
                }
                break;
            }
            //DelayUs(1);
            //ASMDelay(1);
        }

        if (time>=100)
        {
            ret = ERROR;
            break;
        }

        DataLen--;
    }

    SPICtl->SPI_SER = 0;
    SPICtl->SPI_ENR = 0;
    if (SPICsn == 1)
        Gpio_SetPinLevel(GPIOPortA_Pin6, GPIO_HIGH);//SPI0 Flash0_CS
    DelayUs(1);               //cs# high time > 100ns

    return ret;

}

/*
Name:       SPINorWaitBusy
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_WRITE_CODE_
uint32 SPINorWaitBusy(void)
{
    uint32 cmd[1];
    uint8 status;
    uint32 i;

    cmd[0] = CMD_READ_STATUS;

    for (i=0; i<200000; i++) // 2000ms
    {
        SPISendCmd(cmd, 1, &status, 1);
        if ((status & 0x01) == 0)		//busy pin
            return OK;

        DelayUs(10);
    }

    return ERROR;
}

/*
Name:       SPINorSysCodeChk
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 SPINorSysCodeChk(void)
{
    uint8 src[512];
    uint8 dst[512];
    uint32 i, addr;
    PFIRMWARE_HEADER pFirmHeader;

    addr = SPI_IDB_SIZE<<9;
    SPINorRead(addr, src, 512);

    pFirmHeader = (PFIRMWARE_HEADER)src;

	if(pFirmHeader->FwSign[0] != 'R' ||pFirmHeader->FwSign[1] != 'K' ||pFirmHeader->FwSign[2] != 'n'
        || pFirmHeader->FwSign[3] != 'a' ||pFirmHeader->FwSign[4] != 'n' ||pFirmHeader->FwSign[5] != 'o')
    {
        return ERROR;
    }

    if(OK != SPINorRead(addr+pFirmHeader->FwEndOffset, dst, 512))
    {
        return ERROR;
    }

    for(i=0; i<512; i++)
    {
        if(src[i] != dst[i])
            return ERROR;
    }

    SysProgRawDiskCapacity = (pFirmHeader->FwEndOffset+512+511)/512;
    i = SysProgRawDiskCapacity & (NOR_SEC_PER_BLK-1);
    if (i)
        SysProgRawDiskCapacity = SysProgRawDiskCapacity - i + NOR_SEC_PER_BLK;

    return OK;
}



/*
Name:       SPINorInit
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 SPINorInit(void)
{
    int32 i;
    uint32 cmd[4];
    uint8 data[4];
    uint32 ret = ERROR;
    pSPINOR_SPEC spec = &NorSpec;

    SPISetClk();
    //enable spi controller clk
    //init spi
    SPICtl->SPI_BAUDR = 8;     //sclk_out = 24M/2 = 12MHz
    SPICtl->SPI_CTRLR0 = (SPI_MASTER_MODE | TRANSMIT_RECEIVE | MOTOROLA_SPI | RXD_SAMPLE_NO_DELAY | APB_BYTE_WR
                            | MSB_FBIT | LITTLE_ENDIAN_MODE | CS_2_SCLK_OUT_1_CK | CS_KEEP_LOW | SERIAL_CLOCK_POLARITY_HIGH
                            | SERIAL_CLOCK_PHASE_START | DATA_FRAME_8BIT); // 8bit data frame size, CPOL=1,CPHA=1
    //iomux
    //������spi������iomux������SPI-CLK����һ��������
    //SPIIomux();

    DummySize = 0;

    cmd[0] = CMD_READ_JEDECID;
    //cmd[1] = cmd[2] = cmd[3] = 0;

    for(i=1; i>=0; i--)  //̽��SPI1 �� SPI0 �Ƿ��н�NOR
    {
        SPIIomux(i);
        if (OK != SPISendCmd(cmd, 1, data, 3))
        {
            continue;
        }

        if ((0xFF==data[0] && 0xFF==data[1]) || (0x00==data[0] && 0x00==data[1]))
        {
            continue;
        }
        else
            break;
    }

    if (i < 0)
        return ERROR;

    memset(spec, 0, sizeof(SPINOR_SPEC));
    spec->BlockSize = NOR_BLOCK_SIZE;
    spec->PageSize = NOR_PAGE_SIZE;
    spec->Manufacturer = data[0];
    spec->MemType = data[1];

    for(i=0; i<sizeof(NorDeviceCode); i++)
    {
        if (data[2] == NorDeviceCode[i])
        {
            spec->capacity = NorMemCapacity[i] >>9;
            break;
        }
    }

    if (i>=sizeof(NorDeviceCode))
        return ERROR;

    {
		uint8 DataBuf[1024];
        pIDSEC0 IdSec0;
        pIDSEC1 IdSec1;

		for (i=0; i<SPI_IDB_NUM; i++)	//��2��BLOCK�����ID PAGE
		{
            if (OK != SPINorRead((i*spec->BlockSize)<<9, DataBuf, 1024))
            {
                continue;
            }

            IdSec0 = (pIDSEC0)DataBuf;
            if (IdSec0->magic == 0xFCDC8C3B)   //0x0ff0aa55 + rc4
			{
                IdSec1=(pIDSEC1)(DataBuf+512);      //IdSec1 NOT RC4
                if (IdSec1->ChipTag == CHIP_SIGN)
                {
                    //spec->PageSize=IdSec1->pageSize;
                    //spec->BlockSize=IdSec1->blockSize;

                    LoaderVer    = IdSec1->LoaderVer;
                    UpgradeTimes = IdSec1->UpgradeTimes;

                    SysProgDiskCapacity = (uint32)(IdSec1->SysProgDiskCapacity)<<11;
                    SysProgRawDiskCapacity = SysProgDiskCapacity>>1;
                    SysDataDiskCapacity = SysUserDisk2Capacity=0;

                    //if ((SysProgDiskCapacity) <= spec->capacity)
                        ret = OK;

                    break;
                }
			}
		}
    }


    if (OK == ret)
    {
        if (OK == SPINorSysCodeChk())
        {
            spec->valid = 1;
            printf ("SPI Flash0 OK\n");
        }
    }

    #ifdef SPINOR_TEST
    SPINorTest();
    #endif
    return ret;  //���Ƿ���OK�������Ͳ��ý���̽����
}

/*
Name:       SPINorInit
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
uint32 SPINorDeInit(void)
{
    CRUReg->CRU_SOFTRST_CON0 = 1<<11 | 1<<27;
    DelayUs(10);
    CRUReg->CRU_SOFTRST_CON0 = 0<<11 | 1<<27;
    return OK;
}

_ATTR_FLASH_CODE_
void SPINor1Test(void)
{
    uint32 i,lun=0;
    uint8 FwUpdateBuf[1024];
    uint8 FwVerifyBuf[1024];

    /******************/
    for(i=0; i<1024; i++)
    {
        FwVerifyBuf[i] = (i+1)%255;//0xFFFFFFFF;
    }
    memset(FwUpdateBuf, 0, 1024);
    printf("SPI0Flash: SysProgRawDiskCapacity = %x",SysProgRawDiskCapacity);
    if (OK != MDSPINorRead(1, SysProgRawDiskCapacity, 2, (uint8 *)FwUpdateBuf))
    {
        printf ("Not ok 1\n");
        //while(1);
    }

    memset(FwUpdateBuf, 0, 1024);
    if (OK != MDSPINorWrite(1, SysProgRawDiskCapacity, 2, (uint8 *)FwVerifyBuf))
    {
        //printf ("Not ok 2\n");
        //while(1);
    }
    if (OK != MDSPINorRead(1, SysProgRawDiskCapacity, 2, (uint8 *)FwUpdateBuf))
    {
        printf ("Not ok 3\n");
        //while(1);
    }

    for(i=0; i<1024; i++)
    {
        if (FwUpdateBuf[i] != FwVerifyBuf[i])
        {
            printf ("w[%d]=%d, r[%d]=%d\n",i,FwVerifyBuf[i],i,FwUpdateBuf[i]);
            break;
        }
    }
    //printf ("test end:w[%d]=%d, r[%d]=%d\n",i-1,FwVerifyBuf[i-1],i-1,FwUpdateBuf[i-1]);
/******************/
}



_ATTR_FLASH_CODE_
uint32 SPINorFlash1_test(void)
{
#if 1
    uint32 cmd[4];
    uint8 data[4];
    int i;
    SPINOR_SPEC spec1;
    pSPINOR_SPEC spec = &spec1;
    cmd[0] = CMD_READ_JEDECID;

    if (OK != SPISendCmd(cmd, 1, data, 4))
    {
        printf ("SPI Flash0 not OK\n");
    }
    else
    {
        //printf ("SPI Flash0 OK\n");
    }
    if ((0xFF==data[0] && 0xFF==data[1]) || (0x00==data[0] && 0x00==data[1]))
    {
        printf ("SPI Flash1 not Verify\n");
    }
    else
    {
        printf ("data[0]=0x%x,data[1]=0x%x,data[2]=0x%x,data[3]=0x%x\n",data[0],data[1],data[2],data[3]);
    }
    memset(spec, 0, sizeof(SPINOR_SPEC));
    spec->BlockSize = NOR_BLOCK_SIZE;
    spec->PageSize = NOR_PAGE_SIZE;
    spec->Manufacturer = data[0];
    spec->MemType = data[1];

    for(i=0; i<sizeof(NorDeviceCode); i++)
    {
        if (data[2] == NorDeviceCode[i])
        {
            spec->capacity = NorMemCapacity[i] >>9;
            break;
        }
    }

    if (i>=sizeof(NorDeviceCode))
        return ERROR;

    {
		uint8 DataBuf[1024];
        pIDSEC0 IdSec0;
        pIDSEC1 IdSec1;

		for (i=0; i<SPI_IDB_NUM; i++)	//��2��BLOCK�����ID PAGE
		{
            if (OK != SPINorRead((i*spec->BlockSize)<<9, DataBuf, 1024))
            {
                continue;
            }

            IdSec0 = (pIDSEC0)DataBuf;
            if (IdSec0->magic == 0xFCDC8C3B)   //0x0ff0aa55 + rc4
			{
                IdSec1=(pIDSEC1)(DataBuf+512);      //IdSec1 NOT RC4
                if (IdSec1->ChipTag == CHIP_SIGN)
                {
                    printf ("Flash0-ID PAGE:DataBuf[0]=0x%x,DataBuf[1]=0x%x\n",DataBuf[0],DataBuf[1]);
                    break;
                }
			}
		}
    }
    return OK;
#endif
}


/*
Name:       SPINorRead
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
uint32 SPINorRead(uint32 Addr, uint8 *pData, uint32 size)
{
    uint32 cmd[4];

    cmd[0] = CMD_READ_DATA;
    cmd[1] = (Addr >> 16) & 0xFF;
    cmd[2] = (Addr >> 8) & 0xFF;
    cmd[3] = (Addr & 0xFF);

    return SPISendCmd(cmd, 4, pData, size);

}

/*
Name:       SPINorRead
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_WRITE_CODE_
uint32 SPINorWrite(uint32 Addr, uint8 *pData, uint32 size)
{
    uint32 ret = OK;
    uint32 cmd[4];
    uint32 PageSize, nPage;
    pSPINOR_SPEC spec = &NorSpec;

    PageSize = spec->PageSize;
    if (Addr & (PageSize-1))    //��ַ������PAGE����
        return ERROR;

    nPage = size/PageSize;
    while (nPage)
    {
        cmd[0] = CMD_WRITE_EN;
        SPISendCmd(cmd, 1, NULL, 0);

        cmd[0] = CMD_PAGE_PROG;
        cmd[1] = (Addr >> 16) & 0xFF;
        cmd[2] = (Addr >> 8) & 0xFF;
        cmd[3] = (Addr & 0xFF);

        SPIProgCmd(cmd, 4, pData, PageSize);
        ret = SPINorWaitBusy();
        if (OK != ret)
            return ret;

        Addr += PageSize;
        pData += PageSize;
        nPage--;
    }

    return ret;
}

/*
Name:       SPINorGetCapacity
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
uint32 SPINorGetCapacity(uint8 lun)
{
    if (0 == lun)
    {
        return SysProgDiskCapacity;
    }
    else if (1 == lun)
    {
        return SysDataDiskCapacity;
    }
    else if (0xFF == lun)
    {
        return NorSpec.capacity;
    }
    else
    {
        return 0;
    }
}

/*
Name:       SPINorBlkErase
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_WRITE_CODE_
uint32 SPINorBlkErase(uint32 sec)
{
    uint32 ret = OK;
    uint32 cmd[4];
    uint32 blk;

    blk = sec/NOR_SEC_PER_BLK;
    cmd[0] = CMD_WRITE_EN;
    SPISendCmd(cmd, 1, NULL, 0);

    cmd[0] = CMD_BLOCK_ERASE;
    cmd[1] = (blk & 0xFF);
    cmd[2] = 0;
    cmd[3] = 0;

    SPISendCmd(cmd, 4, NULL, 0);

    ret = SPINorWaitBusy();

    return ret;
}

/*
Name:       SPINorReadIDB
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
uint32 MDSPINorRead(uint8 lun, uint32 sec, uint32 nSec, void *pData)
{

    sec = sec + SPI_IDB_SIZE + ((lun>0)? SysProgRawDiskCapacity:0);

    if (sec+nSec > NorSpec.capacity)
        return ERROR;

    return SPINorRead(sec<<9, pData, nSec<<9);
}

/*
Name:       MDSPINorWrite
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_WRITE_CODE_
uint32 MDSPINorWrite(uint8 lun, uint32 sec, uint32 nSec, void *pData)
{
    uint32 ret;
    uint32 len, BlkOffset;

    #ifdef SYS_PROTECT
    if (lun < 1)
        return ERROR;
    #endif

    sec = sec + SPI_IDB_SIZE + ((lun>0)? SysProgRawDiskCapacity:0);
    if (sec+nSec > NorSpec.capacity)
        return ERROR;

    BlkOffset = sec & (NOR_SEC_PER_BLK-1);
    if (BlkOffset)
    {
        len = NOR_SEC_PER_BLK-BlkOffset;
        len = (nSec>len)? len : nSec;
        ret = SPINorWrite(sec<<9, pData, len<<9);
        if (OK != ret)
            return ret;

        sec += len;
        nSec -= len;
    }

    while (nSec)
    {
        len = (nSec>NOR_SEC_PER_BLK)? NOR_SEC_PER_BLK : nSec;
        ret = SPINorBlkErase(sec);
        if (OK != ret)
            return ret;

        ret = SPINorWrite(sec<<9, pData, len<<9);
        if (OK != ret)
            return ret;

        sec += len;
        nSec -= len;
    }

    return ret;
}

/*
Name:       MDGetInfo
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
uint32 MDSPIGetInfo(uint8 lun, pMEMDEV_INFO pDevInfo)
{
    pSPINOR_SPEC spec = &NorSpec;

    pDevInfo->BlockSize = NOR_SEC_PER_BLK;
    pDevInfo->PageSize = 4;
    pDevInfo->capacity = SPINorGetCapacity(lun);

    return OK;
}

#ifdef SPINOR_TEST

/*extern*/ uint32 ProbeReadBuf[2048/4];   //FLASH̽��ʱ�õ�PAGE BUF
/*extern*/ uint32 ProbeWriteBuf[2048/4];  //FLASH̽��ʱ�õ�PAGE BUF
//uint8 TestSPIRet;

/*
Name:       SPINorWriteIDB
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/

void WriteReservedData(uint32 block , uint8 *Buf, uint16 Length);
void ReadReservedData(uint32 block , uint8 *Buf, int16 Length);

_ATTR_FLASH_CODE_
void SPINorTest(void)
{
    uint32 i;

    //TestSPIRet = 0;
    for(i=0; i<512; i++)
    {
        ProbeWriteBuf[i] = i+1;//0xFFFFFFFF;
    }
    #if 1
    WriteReservedData(0, (uint8 *)ProbeWriteBuf, 2048);

    memset(ProbeReadBuf, 0, 2048);
    ReadReservedData(0, (uint8 *)ProbeReadBuf, 2048);
    #else
    memset(ProbeReadBuf, 0, 2048);
    if (OK != MDSPINorWrite(1, 0, 4, (uint8 *)ProbeWriteBuf))
    {
        while(1);
    }

    if (OK != MDSPINorRead(1, 0, 4, (uint8 *)ProbeReadBuf))
    {
        while(1);
    }
    #endif
    for(i=0; i<512; i++)
    {
        if (ProbeWriteBuf[i] != ProbeReadBuf[i])
        {
            while(1);
        }
    }
    while(1);
}
#endif

#endif

