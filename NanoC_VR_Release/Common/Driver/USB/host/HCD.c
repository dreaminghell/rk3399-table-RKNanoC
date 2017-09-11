/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: HCD.c
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
#include "interrupt.h"

/*------------------------------------ Defines -------------------------------*/
#ifdef  _USBHOST_


/*----------------------------------- Typedefs -------------------------------*/

/*-------------------------- Forward Declarations ----------------------------*/

/* ------------------------------- Globals ---------------------------------- */

_ATTR_USB_HOST_BSS_  USBHOST_CTRL gHostCtrl;

/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/

/*------------------------ Function Implement --------------------------------*/

/*
Name:       UHCAllocChn
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static UHC_CHN_INFO * UHCAllocChn(void)
{
    uint32 i;
    UHC_CHN_INFO* hc = &gHostCtrl.ChnInfo[0];

    for(i=0; i<MAX_UHC_CHN; i++, hc++)
    {
        if (0 == hc->valid)
        {
            memset((void*)hc, 0, sizeof(UHC_CHN_INFO));
            hc->valid = 1;
            hc->chn = i;
            return hc;
        }
    }
    printf("ch alloc error---------------\n");
    return NULL;
}

/*
Name:       UHCFreeChn
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCFreeChn(UHC_CHN_INFO * hc)
{
    if (hc->valid)
    {
        uint32 i;
        if(hc->XferStarted)
        {
            if (hc->HaltPending || hc->HaltOnQueue)
                USBDelayUS(1);
            else
                UHCHaltChn(hc, HC_XFER_TIMEOUT);
        }

        for (i=0; i<1000; i++)
        {
            if (!hc->XferStarted)
                break;

            USBDelayUS(1);
        }

        hc->valid = 0;
    }
}

/*
Name:       UHCTxData
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCTxData(int8 chn, uint32 *pBuf, uint32 len)
{
    uint32 * FIFO = (uint32 *)HC_CHN_FIFO(chn);

    while(len--)
    {
        *FIFO = *pBuf++;
    }
}

/*
Name:       UHCRxData
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCRxData(int8 chn, uint32 *pBuf, uint32 len)
{
    uint32 * FIFO = (uint32 *)HC_CHN_FIFO(chn);

    while(len--)
    {
        *pBuf++ = *FIFO;
    }
}

/*
Name:       UHCRxPkt
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
void UHCRxPkt(UHC_CHN_INFO *hc, uint32 bytes)
{
    uint32 dword = (bytes + 3) >>2;

    printf ("UHCRxPkt\n");
    /* xferbuf must be DWORD aligned. */
    UHCRxData(hc->chn, (uint32 *)hc->XferBuf, dword);

    hc->XferCnt += bytes;
    hc->XferBuf += bytes;
}

/*
Name:       UHCTxPacket
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCTxPkt(UHC_CHN_INFO *hc)
{
    uint32 remain;
    uint32 byte;
    uint32 dword;

    remain = hc->XferLen - hc->XferCnt;
    byte =  (remain > hc->MaxPkt) ? hc->MaxPkt : remain;

    dword = (byte+3)>>2;

    /* xferbuf must be DWORD aligned. */
    UHCTxData(hc->chn, (uint32 *)hc->XferBuf, dword);

    hc->XferCnt += byte;
    hc->XferBuf += byte;
}

/*
Name:       UHCSaveToggle
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
void UHCSaveToggle(UHC_CHN_INFO* hc, USB_REQ *urb)
{
    HCTSIZ_DATA hctsiz;
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];

    hctsiz.d32 = pChnReg->hctsizn;
    if (hctsiz.b.pid == DWC_HCTSIZ_DATA0)
    {
        urb->DataToggle = 0;
    }
    else//(hctsiz.b.pid == DWC_HCTSIZ_DATA1)
    {
        urb->DataToggle = 1;
    }
}

/*
Name:       UHCGetActualLen
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 UHCGetActualLen(UHC_CHN_INFO* hc, uint32 ChnStatus, int32 *ShortRead)
{
	HCTSIZ_DATA hctsiz;
	uint32 	length;
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];

	hctsiz.d32 = pChnReg->hctsizn;

    if (ChnStatus == HC_XFER_COMPL)
    {
		if (hc->EpIsIn) //ep_is_in
        {
			length = hc->XferLen - hctsiz.b.xfersize;
			if (ShortRead != NULL)
            {
				*ShortRead = (hctsiz.b.xfersize != 0);
			}
		}
        else
        {
			length = hc->XferLen;
		}
	}
    else
	{
		/*
		 * Must use the hctsiz.pktcnt field to determine how much data
		 * has been transferred. This field reflects the number of
		 * packets that have been transferred via the USB. This is
		 * always an integral number of packets if the transfer was
		 * halted before its normal completion. (Can't use the
		 * hctsiz.xfersize field because that reflects the number of
		 * bytes transferred via the AHB, not the USB).
		 */
        length = (hc->StartPktCnt - hctsiz.b.pktcnt) * hc->MaxPkt;
	}

	return length;

}

/*
Name:       UHCUpdateXfer
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
int32 UHCUpdateXfer(UHC_CHN_INFO* hc, USB_REQ *urb, uint32 ChnStatus)
{
    //HCHN_REG *pChnReg  = &OTGReg->Host.hchn[pChn->chn];
    int32 XferDone = 0;

    if (HC_XFER_COMPL == ChnStatus)
    {
        int32 ShortRead = 0;

        urb->ActualLen += UHCGetActualLen(hc, ChnStatus, &ShortRead);
        if (ShortRead || (urb->ActualLen >= urb->BufLen))
        {
           XferDone = 1;
        }
    }
    else
    {
        urb->ActualLen += UHCGetActualLen(hc, ChnStatus, NULL);
    }

    return XferDone;
}

/*
Name:       UHCCompleteUrb
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
void UHCCompleteUrb(USB_REQ *urb, int32 status)
{
    urb->status = status;
    urb->completed = 1;
}

/*
Name:       UHCCompleteXfer
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
void UHCCompleteXfer(UHC_CHN_INFO* hc, HCHN_REG *pChnReg, uint32 status)
{
    HCINT_DATA hcint;
    int32 ret;

    hcint.d32 = pChnReg->hcintn;
    if (hcint.b.nyet)
    {
        /*
         * Got a NYET on the last transaction of the transfer. This
         * means that the endpoint should be in the PING state at the
         * beginning of the next transfer.
         */
        hc->urb->PingState = 1;
        CLEAN_HC_INT(pChnReg, nyet);
        //hcint.d32 = 0;
        //hcint.b.nyet = 1;
        //pChnReg->hcintn = hcint.d32;
    }

    //complete non periodic xfer
	if (hc->EpIsIn) //ep_is_in
     {
		/*
		 * IN transfers in Slave mode require an explicit disable to
		 * halt the channel. (In DMA mode, this call simply releases
		 * the channel.)
		 */
		ret = UHCHaltChn(hc, status);
        if (ret == 0)
            UHCReleaseChn(hc);
	}
    else
    {
		/*
		 * The channel is automatically disabled by the core for OUT
		 * transfers in Slave mode.
		 */
        hc->status = status;
        UHCReleaseChn(hc);
	}
}

/*
Name:       UHCDoPing
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCDoPing(UHC_CHN_INFO* hc)
{
    HCTSIZ_DATA hctsiz;
    HCCHAR_DATA hcchar;
	HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];

	hctsiz.d32 = 0;
	hctsiz.b.dopng = 1;
	hctsiz.b.pktcnt = 1;
	pChnReg->hctsizn =  hctsiz.d32;

	hcchar.d32 = pChnReg->hccharn;
	hcchar.b.chen = 1;
	hcchar.b.chdis = 0;

	pChnReg->hccharn = hcchar.d32;
}

/*
Name:       UHCStartXfer
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCStartXfer(UHC_CHN_INFO *hc)
{
    HCCHAR_DATA hcchar;
    HCTSIZ_DATA hctsiz;
    uint16  NumPkt;
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];

    hctsiz.d32 = 0;

    if (hc->DoPing)
    {
        UHCDoPing(hc);
        hc->XferStarted = 1;
        return;
    }

    if (hc->XferLen > 0)
    {

        NumPkt = (hc->XferLen + hc->MaxPkt- 1) / hc->MaxPkt;
    }
    else
    {
        /* Need 1 packet for transfer length of 0. */
        NumPkt = 1;
    }

    if (hc->EpIsIn)
    {
        /* Always program an integral # of max packets for IN transfers. */
        hc->XferLen = NumPkt * hc->MaxPkt;
    }

    hctsiz.b.xfersize = hc->XferLen;

    hc->StartPktCnt = NumPkt;
	hctsiz.b.pktcnt = NumPkt;;
	hctsiz.b.pid = hc->PidStart;

	pChnReg->hctsizn = hctsiz.d32;

	hcchar.d32 = pChnReg->hccharn;
	//hcchar.b.multicnt = hc->multi_count;
	/* Set host channel enable after all other setup is complete. */
	hcchar.b.chen = 1;
	hcchar.b.chdis = 0;
	pChnReg->hccharn = hcchar.d32;

	hc->XferStarted = 1;

	if (!hc->EpIsIn && hc->XferLen > 0)
	{
		/* Load OUT packet into the appropriate Tx FIFO. */
		UHCTxPkt(hc);
	}
}

/*
Name:       UHCContinueXfer
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 UHCContinueXfer(UHC_CHN_INFO *hc)
{
    if (hc->PidStart == DWC_OTG_HC_PID_SETUP)
    {
        /* SETUPs are queued only once since they can't be NAKed. */
        return 0;
    }
    else if (hc->EpIsIn)
    {
        HCCHAR_DATA hcchar;
        HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];

        hcchar.d32 = pChnReg->hccharn;
    	hcchar.b.chen = 1;
    	hcchar.b.chdis = 0;
    	pChnReg->hccharn = hcchar.d32;

        return 1;
    }
    else
    {
        /* OUT transfers. */
		if (hc->XferCnt < hc->XferLen)
	    {
            UHCTxPkt(hc);
            return 1;
	    }
        else
        {
            return 0;
        }
    }
}

/*
Name:       UHCStartWaitXfer
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
int32 UHCStartWaitXfer(USB_REQ *urb, int32 timeout)
{
    int32 i;
    UHC_CHN_INFO *hc = NULL;

    timeout *= 1000;

    hc = UHCAllocChn();
    UHCInitChn(hc, urb);

    UHCProcessXfer();
    for (i=0; i<timeout; i++)
    {
        if (urb->completed)
            break;

        USBDelayUS(1);
    }

    if (i >= timeout)
    {

        UHCCompleteUrb(urb, -HOST_TIMEOUT);
    }

    UHCFreeChn(hc);
    if(urb->status < 0)
       {

       }
    return (urb->status<0)? urb->status : urb->ActualLen;
}

/*
Name:       UHCProcessXfer
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
int32 UHCProcessXfer(void)
{
    int32 i;
    int32 ret = 0;
    GINTMSK_DATA gintmsk;
    GNPTXSTS_DATA txsts;
    UHC_CHN_INFO* hc = &gHostCtrl.ChnInfo[0];

    for(i=0; i<MAX_UHC_CHN; i++, hc++)
    {
        if (hc->valid && hc->urb && !hc->urb->completed)
        {
            break;
        }

    }

    if (i >= MAX_UHC_CHN)
        {
         goto OUT;
        }

    txsts.d32 = OTGReg->Core.gnptxsts;
    if (txsts.b.nptxqspcavail == 0)
    {
        ret = 1;
    }
    else if (hc->HaltPending)
    {
        ret = 0;
    }
    else if (hc->HaltOnQueue)
    {
        UHCHaltChn(hc, hc->status);
        ret = 0;
    }
    else if (hc->DoPing)
    {
        if (!hc->XferStarted)
        {
            UHCStartXfer(hc);
        }
        ret = 0;
    }
    else if (!hc->EpIsIn || hc->PidStart == DWC_OTG_HC_PID_SETUP)
    {
        if (txsts.b.nptxfspcavail*4 >= hc->MaxPkt)
        {
            if (!hc->XferStarted)
            {
                UHCStartXfer(hc);

                ret = 1;
            }
            else
            {
                ret = UHCContinueXfer(hc);
            }
        }
        else
        {
            ret = -1;
        }
    }
    else
    {

        if (!hc->XferStarted)
        {
            UHCStartXfer(hc);
            ret = 1;
        }
        else
        {
            ret = UHCContinueXfer(hc);
        }
    }

OUT:
    /*
    * Ensure NP Tx FIFO empty interrupt is disabled when
    * there are no non-periodic transfers to process.
    */

    gintmsk.d32 = OTGReg->Core.gintmsk;
    gintmsk.b.nptxfemp = (ret != 0);
    OTGReg->Core.gintmsk = gintmsk.d32;

    return ret;
}

/*
Name:       UHCInitChnReg
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCInitChnReg(UHC_CHN_INFO *hc)
{
    HCHN_REG *pChnReg;
    HCCHAR_DATA hcchar;
    HCINTMSK_DATA hcintmaskn;
    HOST_REG *pHostReg = &OTGReg->Host;

    pChnReg = &pHostReg->hchn[hc->chn];

    /* Clear old interrupt conditions for this host channel. */
    hcintmaskn.d32 = 0xFFFFFFFF;
    hcintmaskn.b.reserved = 0;
    pChnReg->hcintn = hcintmaskn.d32;

    /* Enable channel interrupts required for this transfer. */
	hcintmaskn.d32 = 0;
	hcintmaskn.b.chhltd = 1;

    switch (hc->EpType)
    {
        case DWC_OTG_EP_TYPE_CONTROL:
        case DWC_OTG_EP_TYPE_BULK:
			hcintmaskn.b.xfercompl = 1;
			hcintmaskn.b.stall = 1;
			hcintmaskn.b.xacterr = 1;
			hcintmaskn.b.datatglerr = 1;
			if (hc->EpIsIn)
			{
				hcintmaskn.b.bblerr = 1;
			}
			else
			{
				hcintmaskn.b.nak = 1;
				hcintmaskn.b.nyet = 1;
				if (hc->DoPing)
				{
					hcintmaskn.b.ack = 1;
				}
			}
			if (hc->ErrorState)
			{
				hcintmaskn.b.ack = 1;
			}
			break;
        default:
            break;
    }

    pChnReg->hcintmaskn = hcintmaskn.d32;
    pHostReg->haintmsk |= (0x1ul<<hc->chn);
    /* Make sure host channel interrupts are enabled. */
    //gintmsk.b.hchint = 1;

	hcchar.d32 = 0;
	hcchar.b.devaddr = hc->DevAddr;
	hcchar.b.epnum = hc->EpNum;
	hcchar.b.epdir = hc->EpIsIn;
	hcchar.b.lspddev = (hc->speed == DWC_OTG_EP_SPEED_LOW);
	hcchar.b.eptype = hc->EpType;
	hcchar.b.mps = hc->MaxPkt;

    pChnReg->hccharn = hcchar.d32;

}

/*
Name:       UHCInitChn
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
void UHCInitChn(UHC_CHN_INFO *hc, USB_REQ *urb)
{
    HOST_PIPE pipe = urb->pipe;

    hc->DevAddr = pipe.b.devaddr;
    hc->EpNum = pipe.b.epnum;
    hc->MaxPkt = pipe.b.mps;
    hc->EpType = pipe.b.eptype;
    hc->EpIsIn = pipe.b.epdir;

    hc->speed = urb->pDev->speed;

    hc->XferStarted = 0;
    hc->status = HC_XFER_IDLE;
    hc->ErrorState = (urb->ErrCnt > 0);
    hc->HaltPending = 0;
    hc->HaltOnQueue = 0;

    hc->DoPing = urb->PingState;
    hc->PidStart = (urb->DataToggle)? DWC_OTG_HC_PID_DATA1 : DWC_OTG_HC_PID_DATA0;

    hc->XferBuf = (uint8*)((uint32)urb->TransBuf + urb->ActualLen);
    hc->XferLen = urb->BufLen - urb->ActualLen;
    hc->XferCnt = 0;

    switch (hc->EpType)
    {
        case DWC_OTG_EP_TYPE_CONTROL:
            switch (urb->CtrlPhase)
            {
                case DWC_OTG_CONTROL_SETUP:
                    hc->DoPing = 0;
			        hc->EpIsIn = 0;
			        hc->PidStart = DWC_OTG_HC_PID_SETUP;
                    hc->XferBuf = (uint8*)urb->SetupPkt;
                    hc->XferLen = 8;
                    break;

                 case DWC_OTG_CONTROL_DATA:
                    hc->PidStart = (urb->DataToggle)? DWC_OTG_HC_PID_DATA1 : DWC_OTG_HC_PID_DATA0;
                    break;

                 case DWC_OTG_CONTROL_STATUS:
                    /*
                    * Direction is opposite of data direction or IN if no
                    * data.
                    */
                    if (urb->BufLen == 0)
                        hc->EpIsIn = 1;
                    else
                        hc->EpIsIn = (pipe.b.epdir != 1);

                    if (hc->EpIsIn)
                        hc->DoPing = 0;

			        hc->PidStart = DWC_OTG_HC_PID_DATA1;
			        hc->XferLen = 0;
				    hc->XferBuf = (uint8*)urb->StatusBuf;
                    break;
                default:
                    break;
			}
			break;

        case DWC_OTG_EP_TYPE_BULK:
            break;
        default:
            break;
    }

    hc->urb = urb;
    UHCInitChnReg(hc);
}

/*
Name:       UHCGetChn
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
UHC_CHN_INFO* UHCGetChn(int8 chn)
{
    if (chn >= MAX_UHC_CHN)
        return NULL;

     return &gHostCtrl.ChnInfo[chn];
}

/*
Name:       UHCHaltChn
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
int32 UHCHaltChn(UHC_CHN_INFO* hc, uint32      status)
{
    int32 ret = 1;
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];
    HCCHAR_DATA hcchar;
    GNPTXSTS_DATA txsts;

    if (hc->HaltPending)
    {
        return ret;
    }

    hc->status = status;
    hcchar.d32 = pChnReg->hccharn;
    if (hcchar.b.chen == 0)
    {
        ret = 0;
        return ret;
    }

    txsts.d32 = OTGReg->Core.gnptxsts;
    hcchar.b.chen = (txsts.b.nptxqspcavail)? 1 : 0;

    hcchar.b.chdis = 1;
    pChnReg->hccharn = hcchar.d32;

    if (hcchar.b.chen)
    {
        hc->HaltPending = 1;
        hc->HaltOnQueue = 0;
    }
    else
    {
        hc->HaltOnQueue = 1;
    }

    if (hc->HaltOnQueue)
    {
        GINTMSK_DATA gintmsk;
        gintmsk.d32 = OTGReg->Core.gintmsk;
        gintmsk.b.nptxfemp = 1;
        OTGReg->Core.gintmsk = gintmsk.d32;
    }

    return ret;
}


/*
Name:       UHCReleaseChn
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
void UHCReleaseChn(UHC_CHN_INFO * hc)
{
    HOST_REG *pHostReg = &OTGReg->Host;
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];
    USB_REQ  *urb = hc->urb;

    if (HC_XFER_XERR==hc->status)
    {
        if (urb->ErrCnt >= 3)
        {
            UHCCompleteUrb(urb, -HOST_XERR);
        }
    }

    hc->HaltPending = 0;
    hc->XferStarted = 0;
    pChnReg->hcintmaskn = 0;
    pChnReg->hcintn = 0xFFFFFFFF;
    pHostReg->haintmsk &= ~(0x1ul<<hc->chn);

    if (!urb->completed)
    {
        UHCInitChn(hc, hc->urb);
        UHCProcessXfer();
    }
}

/*
Name:       UHCResetPort
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
int32 UHCResetPort(HOST_DEV *pDev)
{
    volatile uint32 status;
    uint32 i, j;
    int32 ret=-HOST_ERR;
    uint8 change;
	uint8 state;

    for(i=0; i<3; i++)
    {
        //reset the port
        status = OTGReg->Host.hprt&(~0x0f);
        status |= (0x01<<8);
        OTGReg->Host.hprt = status;

        USBDelayMS(50);  //hi-speed 50ms,F/L speed 10ms
        USBDelayMS(10);  //datasheet 建议在加10ms清除

        status = OTGReg->Host.hprt&(~0x0f);
        status &= ~(0x01<<8);
        OTGReg->Host.hprt = status;

        UHCPortStatus(&state, &change);
        if (!state)
            return ret;

        for (j=0; j<50; j++)
        {
            if (OTGReg->Host.hprt & 0x04) //wait for port enable
                break;
            USBDelayMS(1);
        }

        if (j>=50)
            return ret;

        ret = HOST_OK;
        status=(OTGReg->Host.hprt >> 17) & 0x03;
        switch (status)
        {
            case 0:     //high speed
                gHostCtrl.PortSpeed = USB_SPEED_HS ;
                break;
            case 1:     //full speed
                gHostCtrl.PortSpeed = USB_SPEED_FS;
                break;
            case 2:     //low speed
                gHostCtrl.PortSpeed = USB_SPEED_LS;
                break;
            case 3:
            default:
                ret = -HOST_ERR;
                break;
        }

        if (HOST_OK == ret)
        {
            pDev->speed = gHostCtrl.PortSpeed;
            pDev->state = USB_STATE_DEFAULT;
            break;
        }
    }

    return ret;
}

/*
Name:       UHCPrtConnSts
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
uint8 UHCPrtConnSts(void)
{
    HPRT0_DATA hprt;

    hprt.d32 = OTGReg->Host.hprt;
    return (uint8)hprt.b.prtconnsts;
}

/*
Name:       UHCPortStatus
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
int32 UHCPortStatus(uint8 *status, uint8 *change)
{
    //DISABLE_USB_INT;
    if (change)
    {
		*change = gHostCtrl.PortChange;
        gHostCtrl.PortChange = 0;
    }

    if (status)
		*status = gHostCtrl.PortStatus;
    //ENABLE_USB_INT;

    return HOST_OK;
}

/*
Name:       UHCInit
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
int32 UHCInit(void)
{
    GINTMSK_DATA gintmsk;
//    GUSBCFG_DATA usbcfg;
    GAHBCFG_DATA ahbcfg;
    uint32 count;

    //USBSetClk();
    /*USB初始化*/
    DISABLE_USB_INT;

    OTGReg->Device.dctl |= 0x02;       //soft disconnect
    DEBUG("HOST1 CON0 is%08x",GRFReg->OTGPHY_CON0);
    DEBUG("HOST1 CON1 is%08x",GRFReg->OTGPHY_CON1);
    #if 1
    GRFReg->OTGPHY_CON0 = 3<<13 | 3<<29; //DP, DM 下拉15K电阻
    GRFReg->OTGPHY_CON1 = 0x00030001; //选择为host

    DEBUG("HOST CON0 is%08x",GRFReg->OTGPHY_CON0);
    DEBUG("HOST CON1 is%08x",GRFReg->OTGPHY_CON1);
    #endif
    USBResetPhy();

    memset((void*)&gHostCtrl, 0, sizeof(USBHOST_CTRL));

    /* Wait for AHB master IDLE state. */
    for (count=0; count<100000; count++)
    {
        if ((OTGReg->Core.grstctl & (1ul<<31))!=0)
            break;
        USBDelayUS(10);
    }

    OTGReg->ClkGate.pcgcctl = 0x00;               //Restart the Phy Clock

    //Core soft reset
    OTGReg->Core.grstctl |= 1<<0;
    for (count=0; count<10000; count++)
    {
        if ((OTGReg->Core.grstctl & (1<<0))==0)
            break;
    }
    #if 0
    OTGReg->Core.grstctl|=(0x10<<6) | (1<<5);       //Flush all Txfifo
    for (count=0; count<10000; count++)
    {
        if ((OTGReg->Core.grstctl & (1<<5))==0)
            break;
    }
    OTGReg->Core.grstctl|=1<<4;                     //Flush all Rxfifo
    for (count=0; count<10000; count++)
    {
        if ((OTGReg->Core.grstctl & (1<<4))==0)
            break;
    }

    //可以不用配置, ID接地,自动为HOST
    // 16bit phy if, force host mode
    usbcfg.d32 = OTGReg->Core.gusbcfg;
    usbcfg.b.forcehstmode = 1;
    usbcfg.b.forcedevmode = 0;
    usbcfg.b.phyif = 1;
    //OTGReg->Core.gusbcfg = usbcfg.d32;
    USBDelayMS(100);
    #endif

    for (count=0; count<1000; count++)
    {
        if ((OTGReg->Core.gintsts & 0x1)!=0)
            break;

        USBDelayMS(1);
    }

    #if 0 //smart 不需要配置
    OTGReg->Core.grxfsiz= 0x0208;
    OTGReg->Core.gnptxfsiz = 0x00800208;//0x04000208
    #endif

    OTGReg->Host.hprt |= (0x01<<12);          //power on the port
    OTGReg->Core.gintsts=0xffffffff;
    OTGReg->Core.gotgint=0xffffffff;

    gintmsk.d32 = 0;
    gintmsk.b.disconnint = 1;
    //gintmsk.b.conidstschng = 1; //Connector ID Status Change
    gintmsk.b.hchint = 1;
    gintmsk.b.prtint = 1;
    //gintmsk.b.nptxfemp = 1;
    gintmsk.b.rxflvl = 1;
    OTGReg->Core.gintmsk = gintmsk.d32;

    ahbcfg.d32 = OTGReg->Core.gahbcfg;
    ahbcfg.b.glblintrmsk = 1;
    ahbcfg.b.nptxfemplvl = DWC_GAHBCFG_TXFEMPTYLVL_EMPTY;
    OTGReg->Core.gahbcfg = ahbcfg.d32;      //Global Interrupt Mask, operates in Slave mode

    IntRegister(INT_ID23_USB, (void*)UHCIsr);
    ENABLE_USB_INT;

    return (HOST_OK);

}

/*
Name:       UHCDeInit
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_HOST_CODE_
void UHCDeInit(void)
{
    DISABLE_USB_INT;
}

#endif
