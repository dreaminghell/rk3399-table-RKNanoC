/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: MSC.c
Desc: 
Author: chenfen@rock-chips.com
Date: 12-06-20
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/

#include "USBConfig.h"

/*------------------------------------ Defines -------------------------------*/
#ifdef  _USBHOST_

/*----------------------------------- Typedefs -------------------------------*/

#if 0
/* Command Block Wrapper */
typedef __packed struct _HOSTMSC_CBW
{
    uint32      dCBWSignature;
    uint32      dCBWTag;
    uint32      dCBWDataTransferLength;
    uint8       bCBWFlags;
    uint8       bCBWLUN;
    uint8       bCDBLength;
    uint8       CBWCDB[CBWCDBLENGTH];
} HOST_CBW, *pHOST_CBW;


/* Command Status Wrapper */
typedef __packed struct _HOSTMSC_CSW
{
    uint32      dCSWSignature;
    uint32      dCSWTag;
    uint32      dCSWDataResidue;
    uint8       bCSWStatus;
} HOST_CSW;
#endif

typedef struct tagSCSI_CMD
{
    uint8       cmd[16];           /* command           */
    uint32      sense[64/4];         /* for request sense */
    uint8       status;            /* SCSI Status     */
    uint8       lun;               /* Target LUN        */
    uint8       cmdlen;            /* command len       */
    uint8       dirin;             /* Is dir in        */
    uint8       *pdata;             /* pointer to data    */
    uint32      datalen;           /* Total data length  */
    //uint32    actlen;            /* tranfered bytes    */
} SCSI_CMD, pSCSI_CMD;


typedef	struct tagMSC_DEV
{
    uint8       valid;
    uint8       BulkOut;
    uint8       BulkIn;
    uint8       LunNum;
    
    uint32      SendCtrlPipe;  
    uint32      RecvCtrlPipe;
    uint32      SendBulkPipe;  
    uint32      RecvBulkPipe;

    uint8       LunMap[MAX_LUN_NUM];    
    uint32      capacity[MAX_LUN_NUM];   //容量

    HOST_DEV    *pDev;
}MSC_DEV, *pMSC_DEV;

/*-------------------------- Forward Declarations ----------------------------*/


/* ------------------------------- Globals ---------------------------------- */

int32 MstTest(void);
/*--------------------------- Local Function Prototypes ----------------------*/
static int32 MscProbe(HOST_INTERFACE *intf);

static void MscDisconnect(HOST_INTERFACE *intf);


/*-------------------------------- Local Statics: ----------------------------*/

_ATTR_USB_HOST_CODE_
static USB_DEV_ID MscDevID = 
{
    USB_DEVICE_ID_MATCH_INT_INFO,
    0,
    0,
    USB_CLASS_MASS_STORAGE,
    US_SC_SCSI,
    US_PR_BULK
};

_ATTR_USB_HOST_CODE_
HOST_DEV_DRIVER gMscDriver = 
{
	"MSC",
    MscProbe,
    MscDisconnect,
    &MscDevID
};

_ATTR_USB_HOST_BSS_ static MSC_DEV gMscDev;

_ATTR_USB_HOST_BSS_ static uint32 CBWTag;

_ATTR_USB_HOST_BSS_ static SCSI_CMD SCSICmd;

/*------------------------ Function Implement --------------------------------*/

#if 0
/*
Name:       MscReset
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
static int32 MscReset(MSC_DEV *pMsc)
{
    int32 ret;
    
    ret = HostCtrlMsg(pMsc->pDev, pMsc->SendCtrlPipe, USB_TYPE_CLASS|USB_RECIP_INTERFACE, 
                        USB_BBB_RESET, 0, 0, NULL, 0, USB_CNTL_TIMEOUT*5);    
    if (ret<0)
        return ret;

    USBDelayMS(150);
    ret = HostCleanEpStall(pMsc->pDev, pMsc->SendCtrlPipe, (pMsc->BulkIn|0x80));

    USBDelayMS(150);

    ret = HostCleanEpStall(pMsc->pDev, pMsc->SendCtrlPipe, pMsc->BulkOut);

    USBDelayMS(150);

    return HOST_OK;
}
#endif

/*
Name:       MscBulkOnlyXfer
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 MscBulkOnlyXfer(SCSI_CMD *sc)
{
    int32 ret, ret1, retry;
    HOST_CBW cbw;
    HOST_CSW csw;
    MSC_DEV *pMsc = &gMscDev;
    GINTMSK_DATA gintmsk;

    /* COMMAND phase */
    cbw.dCBWSignature = CBWSIGNATURE;
    cbw.dCBWTag = CBWTag++;
    cbw.dCBWDataTransLen = sc->datalen;
    cbw.bCBWFlags = (sc->dirin)? CBWFLAGS_IN : CBWFLAGS_OUT;
    cbw.bCBWLUN = sc->lun;
    cbw.bCDBLength = sc->cmdlen;
    
    /* copy the command data into the CBW command data buffer */
    memset(cbw.CBWCDB, 0, sizeof(cbw.CBWCDB));
    memcpy(cbw.CBWCDB, sc->cmd, sc->cmdlen);

    REPEAT:
    ret = HostBulkMsg(pMsc->pDev, pMsc->SendBulkPipe, &cbw, UMASS_BBB_CBW_SIZE, USB_CNTL_TIMEOUT*5); 
    if (ret < 0)
        {
         gintmsk.d32 = OTGReg->Core.gintmsk;
         gintmsk.b.nptxfemp = (ret != 0);
         OTGReg->Core.gintmsk = gintmsk.d32;
         goto REPEAT;
         return ret;
        }

   
    USBDelayUS(1);
    
    /* DATA phase + error handling */
	/* no data, go immediately to the STATUS phase */
	if (sc->datalen == 0)
		goto STATUS;

    ret = HostBulkMsg(pMsc->pDev, (sc->dirin)? pMsc->RecvBulkPipe : pMsc->SendBulkPipe,
                        sc->pdata, sc->datalen, USB_CNTL_TIMEOUT*5); 
    if (ret < 0)
    {
        if (-HOST_STALL == ret)
        {
            ret1 = HostCleanEpStall(pMsc->pDev, pMsc->SendCtrlPipe, (sc->dirin)? (pMsc->BulkIn|0x80) : pMsc->BulkOut);
            if (ret1 < 0)
                return ret1;
        }
        else
            return ret;
    }
    
    //sc->actlen = ret;
    /* STATUS phase + error handling */
STATUS:
    retry = 0;
AGAIN:    
    ret1 = HostBulkMsg(pMsc->pDev, pMsc->RecvBulkPipe, &csw, UMASS_BBB_CSW_SIZE, USB_CNTL_TIMEOUT*5); 
    if (ret1 < 0)
    {
        if (-HOST_STALL == ret1)
        {
            ret1 = HostCleanEpStall(pMsc->pDev, pMsc->SendCtrlPipe, (pMsc->BulkIn|0x80));
            if (ret1 >= 0 && (retry++ < 1))
                goto AGAIN;
        }
        else
            return ret1;
    }
    else
    {
        // Check Csw
        sc->status = csw.bCSWStatus;
    }
    
    return (ret >= 0)? HOST_OK : ret;
}

/*
Name:       MscRequestSense
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 MscRequestSense(uint8 lun)
{
    int32 ret;
	uint8 *ptr;
    SCSI_CMD *sc = &SCSICmd;

    sc->lun = lun;
    sc->dirin = 1;
	ptr=(uint8 *)sc->pdata;
	memset(&sc->cmd[0], 0, 12);

    sc->cmd[0]=SCSI_REQ_SENSE;
	sc->cmd[1]=sc->lun<<5;
	sc->cmd[4]=18;
	sc->datalen=18;
	sc->pdata= (uint8*)&sc->sense[0];
	sc->cmdlen=0x0a;
    
	ret = MscBulkOnlyXfer(sc);
	sc->pdata = (uint8 *)ptr;

    return ret;
}

/*
Name:       MSCTestUnitReady
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 MscTestUnitReady(uint8 lun)
{
    int32 ret;
    SCSI_CMD *sc = &SCSICmd;
    int retries = 10;

    do 
    {
        sc->lun = lun;
        sc->dirin = 0;
        memset(&sc->cmd[0], 0, 12);
        sc->cmd[0]   = SCSI_TST_U_RDY;
        sc->cmd[1]   = sc->lun<<5;
        sc->cmdlen   = 0x06;
        sc->datalen  = 0;

        ret = MscBulkOnlyXfer(sc);
        if(HOST_OK == ret) 
        {
            return ret;
        }
        else if (-HOST_STALL == ret)
        {
            MscRequestSense(lun);
            USBDelayMS(1); 
        }
        else
        {
            return ret;
        }
    } while(retries--);

    return ret;
}

/*
Name:       MSCReadCapacity
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 MscReadCapacity (uint8 lun, uint32 *pCapacity)
{
    int32 ret;
    SCSI_CMD *sc = &SCSICmd;
    int retries = 3;
    uint32 cap[2];
    
	do 
    {
        sc->lun = lun;
        sc->dirin = 1;
        memset(&sc->cmd[0], 0, 12);
        sc->cmd[0]=SCSI_RD_CAPAC;
        sc->cmd[1]=sc->lun<<5;
        sc->cmdlen=0x0a;
        sc->datalen=8;
        sc->pdata = (uint8*)&cap[0];

		ret = MscBulkOnlyXfer(sc);
        if(HOST_OK == ret) 
        { 
            *pCapacity = HostSwap32(cap[1]);
            return ret;
        }
        else if (-HOST_STALL == ret)
        {
            MscRequestSense(lun);
            USBDelayMS(1); 
        }
        else
        {
            return ret;
        }
	} while(retries--);

	return ret;
}

/*
Name:       MSCReadCapacity
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 MscReadFormatCapacity(uint8 lun, uint32 *pCapacity)
{
    int32 ret;
    SCSI_CMD *sc = &SCSICmd;
    int retries = 3;
    uint32 cap[4];
    
	do 
    {
        sc->lun = lun;
        sc->dirin = 1;
        memset(&sc->cmd[0], 0, 12);
        sc->cmd[0]=SCSI_RD_FORMAT_CAPAC;
        sc->cmd[1]=sc->lun<<5;
        sc->cmd[8]=0xfc;        //参见 mass_storage_ufi.pdf
        sc->cmdlen=0x0a;
        sc->datalen=0x0c;
        sc->pdata = (uint8*)&cap[0];

		ret = MscBulkOnlyXfer(sc);
        if(HOST_OK == ret) 
        { 
            if ((HostSwap32(cap[2]) & 0xffff)==512)
                *pCapacity = HostSwap32(cap[1]);
            return ret;
        }
        else if (-HOST_STALL == ret)
        {
            MscRequestSense(lun);
            USBDelayMS(1); 
        }
        else
        {
            return ret;
        }
	} while(retries--);

    return ret;
}

/*
Name:       MscInquiry
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 MscInquiry(uint8 lun, void *pInquiry)
{
    int32 ret;
    SCSI_CMD *sc = &SCSICmd;
    int retries = 3;

    sc->lun = lun;
    sc->dirin = 1;
    memset(&sc->cmd[0], 0, 12);
    sc->cmd[0]=SCSI_INQUIRY_CMD;
    sc->cmd[1]=sc->lun<<5;
    sc->cmd[4]=36;
    sc->cmdlen=0x06;
    sc->datalen=36;
    sc->pdata = (uint8*)pInquiry;

	do 
    {
		ret = MscBulkOnlyXfer(sc);
        if(HOST_OK == ret) 
        { 
            return ret;
        }
	} while(retries--);

	return ret;
}

/*
Name:       MscGetMaxLun
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 MscGetMaxLun(uint8 *pLun)
{
    MSC_DEV *pMsc = &gMscDev;

    return  HostCtrlMsg(pMsc->pDev, pMsc->RecvCtrlPipe, 0xA1, USB_BBB_GET_MAX_LUN, 
                      0, 0, pLun, 1, USB_CNTL_TIMEOUT*5);    
}

/*
Name:       MscGetPipe
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 MscGetPipe(MSC_DEV *pMsc, HOST_INTERFACE* intf)
{
    int32 i, ret = -HOST_ERR;
    HOST_EP_DESC *ep = &intf->EpDesc[0];
    uint8 epnum = intf->IfDesc.bNumEndpoints;     
        
    for (i=0; i<epnum; i++, ep++)
    {
        if (USBEpType(ep) == USB_ENDPOINT_XFER_BULK)
        {
            if (USBEpDirIn(ep))
            {
                if (!pMsc->BulkIn)
                    pMsc->BulkIn = USBEpNum(ep);
            }
            else
            {
                if (!pMsc->BulkOut)
                    pMsc->BulkOut = USBEpNum(ep);
            }
        }
    }
    
    if (pMsc->BulkIn && pMsc->BulkOut)
    {
        pMsc->SendCtrlPipe = HostCreatePipe(intf->pDev, 0, 0);
        pMsc->RecvCtrlPipe = HostCreatePipe(intf->pDev, 0, 1);
        pMsc->SendBulkPipe = HostCreatePipe(intf->pDev, pMsc->BulkOut, 0);
        pMsc->RecvBulkPipe = HostCreatePipe(intf->pDev, pMsc->BulkIn, 1);
        pMsc->pDev = intf->pDev;
        ret = HOST_OK;
    }

    return ret;
}

/*
Name:       MscGetInfo
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 MscGetInfo(MSC_DEV *pMsc, uint8 lun)
{
    int32 ret;
    uint8 TmpBuf[64];

    ret = MscInquiry(lun, TmpBuf);
    if (ret < 0)
        return ret;

    if((TmpBuf[0]&0x1f) == 5)//cdrom
    {
        return -HOST_ERR;
    }

    ret = MscReadFormatCapacity(lun, &pMsc->capacity[lun]);
    if (ret < 0)
        return ret;
    #if 0
    ret = MscReadCapacity(lun, &pMsc->capacity[lun]);
    if (ret < 0)
        return ret;
    pMsc->capacity[lun]++;
    #endif
    ret = MscTestUnitReady(lun);
    if (ret < 0)
        return ret;

    return HOST_OK;
}

/*
Name:       MSCProbe
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 MscProbe(HOST_INTERFACE* intf)
{
    int32 i, n, ret;
    MSC_DEV *pMsc = &gMscDev;
    
    uint8 LunNum;

    CBWTag = 0;
    memset (pMsc, 0, sizeof(MSC_DEV));

    ret =  MscGetPipe(pMsc, intf);
    if (ret < 0)
        return ret;

    ret = MscGetMaxLun(&LunNum);
    if (ret < 0)
    {
        if (-HOST_STALL == ret)
        {
            HostCleanEpStall(pMsc->pDev, pMsc->SendCtrlPipe, 0);
            LunNum = 1;
        }
        else
            return ret;
    }
    else
    {
        LunNum++;
    }

    if (LunNum > MAX_LUN_NUM)
    {
        LunNum = MAX_LUN_NUM;
    }

    for (i=0, n=0; i<LunNum; i++)
    {
        ret = MscGetInfo(pMsc, i);
        if (ret >= 0)
        {
            pMsc->LunMap[n++] = i;
        }
    }

    if (n > 0)
    {
        ret = HOST_OK;
        pMsc->LunNum = n;
        pMsc->valid = 1;
    }
    else
    {
        ret = -HOST_ERR;
    }

    //if (ret == HOST_OK)
    //    MstTest();

    return ret;
}

/*
Name:       MscRead10
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
uint32 MscRead10(uint8 lun, uint32 start, uint32 blks, void *pBuf)
{
    MSC_DEV *pMsc = &gMscDev;
    SCSI_CMD *sc = &SCSICmd;
    if (!pMsc->valid)
        return -HOST_ERR;

    if(!MscChkEvent(MD_EVENT_PRESENT))
    {

         return -HOST_ERR;
    }

    sc->lun = pMsc->LunMap[lun];
    sc->dirin = 1;
    memset(&sc->cmd[0], 0, 12);
    sc->cmd[0]=SCSI_READ10;
    sc->cmd[1]=sc->lun<<5;
    sc->cmd[2]=((uint8) (start>>24))&0xff;
    sc->cmd[3]=((uint8) (start>>16))&0xff;
    sc->cmd[4]=((uint8) (start>>8))&0xff;
    sc->cmd[5]=((uint8) (start))&0xff;
    sc->cmd[7]=((uint8) (blks>>8))&0xff;
    sc->cmd[8]=(uint8) blks & 0xff;
    sc->cmdlen=0x0a;
    sc->datalen=blks<<9;
    sc->pdata = (uint8*)pBuf;
    return MscBulkOnlyXfer(sc);
    
}

/*
Name:       MscWrite10
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
uint32 MscWrite10(uint8 lun, uint32 start, uint32 blks, void *pBuf)
{
    MSC_DEV *pMsc = &gMscDev;
    SCSI_CMD *sc = &SCSICmd;

    if (!pMsc->valid)
        return -HOST_ERR;

    if(!MscChkEvent(MD_EVENT_PRESENT))
    {   
         return -HOST_ERR;
    }

    sc->lun = pMsc->LunMap[lun];
    sc->dirin = 0;
    memset(&sc->cmd[0], 0, 12);
    sc->cmd[0]=SCSI_WRITE10;
    sc->cmd[1]=sc->lun<<5;
    sc->cmd[2]=((uint8) (start>>24))&0xff;
    sc->cmd[3]=((uint8) (start>>16))&0xff;
    sc->cmd[4]=((uint8) (start>>8))&0xff;
    sc->cmd[5]=((uint8) (start))&0xff;
    sc->cmd[7]=((uint8) (blks>>8))&0xff;
    sc->cmd[8]=(uint8) blks & 0xff;
    sc->cmdlen=0x0a;
    sc->datalen=blks<<9;
    sc->pdata = (uint8*)pBuf;

    return MscBulkOnlyXfer(sc);
}


/*
Name:       MscChkEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
bool MscChkEvent(uint32 event)
{
    uint8 status, change;
    bool ret;

    MSC_DEV *pMsc = &gMscDev;

    UHCPortStatus(&status, &change);

    if (!status && pMsc->valid)
    {
        MscDeInit();
    }
  
    if (event == MD_EVENT_PRESENT)
    {
        ret = (bool)status;
    }
    else if (event == MD_EVENT_CHANGE)
    {
        ret = (bool)change;  
    }
    else if (event == MD_EVENT_VAILID)
    {
        ret = (bool)pMsc->valid;
    }
    else
    {
        ret = FALSE;
    }

    return ret;
}


/*
Name:       MscDisconnect
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void MscDisconnect(HOST_INTERFACE* intf)
{
    MscDeInit();
}

/*
Name:       MscGetDiskInfo
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
uint32 MscGetDiskInfo(uint8 lun, pMEMDEV_INFO pDevInfo)
{
    MSC_DEV *pMsc = &gMscDev;
    uint8 i;

    if (!pMsc->valid)
        return ERROR;

    if(!MscChkEvent(MD_EVENT_PRESENT))
    {
         return -HOST_ERR;
    }

    i = pMsc->LunMap[lun];
    
    //pDevInfo->Manufacturer = 0;
    pDevInfo->BlockSize = 8*256;  //固定成128K
    pDevInfo->PageSize = 8;
    pDevInfo->capacity = pMsc->capacity[i];

    return OK;
}

/*
Name:       MscDevProbe
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
uint32 MscDevProbe(void)
{
    uint32 i;
    int32 ret;
    MSC_DEV *pMsc = &gMscDev;
    
    ret = HostPortchange(1);
    if (ret < 0)
        return ERROR;

    if (!pMsc->valid) 
        return ERROR;

    for (i=0; i<pMsc->LunNum; i++) 
    {
        MDAddDisk(MD_MKDEV(MEM_DEV_HOST, i));
    }
    
    return OK;
}

/*
Name:       MscDeInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
void MscDeInit(void)
{
    uint32 i;
    MSC_DEV *pMsc = &gMscDev;

    if (!pMsc->valid)
        return;

    for (i=0; i<pMsc->LunNum; i++) 
    {
        MDRemoveDisk(MD_MKDEV(MEM_DEV_HOST, i));
    }

    pMsc->valid = 0;;

    Scu_otgphy_suspend(1);
    ScuClockGateCtr(CLOCK_GATE_USBPHY, 0);
    ScuClockGateCtr(CLOCK_GATE_USBBUS, 0);
}

/*
Name:       MscInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
uint32 MscInit(void)
{
    uint32 timeout;
    MSC_DEV *pMsc = &gMscDev;
    
    //ScuLDOSet(SCU_LDO_33);
    //PMU_EnterModule(PMU_USB);  

    ScuClockGateCtr(CLOCK_GATE_USBPHY, 1);
    ScuClockGateCtr(CLOCK_GATE_USBBUS, 1);
    Scu_otgphy_suspend(0);    
    HostInit();
    
    memset (pMsc, 0, sizeof(MSC_DEV));
    HostRegDrv(&gMscDriver);

    timeout = 0;
    if(!MscChkEvent(MD_EVENT_PRESENT))
    {
        MscDeInit();
        return ERROR; 
    }
    while(GetMsg(MSG_USB_HOST_CONNECTED) == FALSE)
    {
        MDScanDev(1,MEM_DEV_SCAN_HOST);  
        if (++timeout > 10*1000)
        {
            MscDeInit();
            return ERROR;
        }
        DelayMs(1);
    }
    
    return 0;
}

_ATTR_USB_HOST_CODE_
void MscDeInitNop(void)
{
}

_ATTR_USB_HOST_CODE_
uint32 MscInitNop(void)
{
    return 0;
}


#if 0
uint32 MscTestBuf[1024*4/4];
/*
Name:       MscInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
int32 MstTest(void)
{
    int32 ret = -1;
    uint32 i;
    MSC_DEV *pMsc = &gMscDev;

    if (pMsc->capacity[0] == 0)
        return ret;

    for (i=0; i<pMsc->capacity[0]-8; i+=8)
    {
        ret = MscRead10(0, i, 8, MscTestBuf);
        if (ret < 0)
            break;
    }

    return ret;
}
#endif

#endif
