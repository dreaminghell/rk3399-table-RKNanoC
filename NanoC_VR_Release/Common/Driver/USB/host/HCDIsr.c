/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: HCDIsr.c
Desc: 

Author: chenfen@rock-chips.com
Date: 12-12-29
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


/*-------------------------- Forward Declarations ----------------------------*/

/* ------------------------------- Globals ---------------------------------- */

/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/

/*------------------------ Function Implement --------------------------------*/

/*
Name:       UHCNptxfempIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCNptxfempIsr(void)
{
    UHCProcessXfer(); 
}

/*
Name:       UHCRxflvlIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCRxflvlIsr(void)
{
    int8 chn;
    GRXSTSH_DATA grxsts;

    grxsts.d32 = OTGReg->Core.grxstsp;

    chn = grxsts.b.chnum;

    switch (grxsts.b.pktsts) 
    {
        case DWC_GRXSTS_PKTSTS_IN:
        /* Read the data into the host buffer. */
        if (grxsts.b.bcnt > 0)
        {
            UHCRxPkt(UHCGetChn(chn), grxsts.b.bcnt);
        }
    
        case DWC_GRXSTS_PKTSTS_IN_XFER_COMP:
        case DWC_GRXSTS_PKTSTS_DATA_TOGGLE_ERR:
        case DWC_GRXSTS_PKTSTS_CH_HALTED:
        /* Handled in interrupt, just ignore data */
            break;
        default:
            //DWC_ERROR ("RX_STS_Q Interrupt: Unknown status %d\n", grxsts.b.pktsts);
            break;
    }
}

/*
Name:       UHCPortIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCPortIsr(void)
{
    HPRT0_DATA hprt;
    HPRT0_DATA clean_hprt;
    USBHOST_CTRL *pHost = &gHostCtrl;

    hprt.d32 = OTGReg->Host.hprt;
    clean_hprt.d32 = hprt.d32;
    
	/* Clear appropriate bits in HPRT0 to clear the interrupt bit in
	 * GINTSTS */

	clean_hprt.b.prtena = 0;
	clean_hprt.b.prtconndet = 0; 
	clean_hprt.b.prtenchng = 0;
	clean_hprt.b.prtovrcurrchng = 0; 
	/* Port Connect Detected 
	 * Set flag and clear if detected */
    if(hprt.b.prtconndet)
    {
        clean_hprt.b.prtconndet = 1;
        pHost->PortChange = 1;
        pHost->PortStatus = (hprt.b.prtconnsts)? 1 : 0;
    }
    if (hprt.b.prtenchng)
    {
        clean_hprt.b.prtenchng = 1;
        //if (hprt.b.prtena == 1)
    }
    if(hprt.b.prtovrcurrchng)
    {
        /** Overcurrent Change Interrupt */
        clean_hprt.b.prtovrcurrchng = 1;
        //disable the port and power off
        clean_hprt.b.prtpwr = 0;
    }

    /* Clear Port Interrupts */
    OTGReg->Host.hprt = clean_hprt.d32;
}

/*
Name:       UHCXfercompIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCXfercompIsr(UHC_CHN_INFO* hc)
{
    HCHN_REG *pChnReg = &OTGReg->Host.hchn[hc->chn];
    USB_REQ   *urb    = hc->urb;
    uint32 status = HC_XFER_COMPL; 

    DISABLE_HC_INT(pChnReg, xfercompl);
    switch (hc->EpType) /** 0: Control, 1: Isoc, 2: Bulk, 3: Intr */
    {
        
        case DWC_OTG_EP_TYPE_CONTROL:
            switch (urb->CtrlPhase)
            {   
                case DWC_OTG_CONTROL_SETUP:
                    if (urb->BufLen > 0) 
                        urb->CtrlPhase = DWC_OTG_CONTROL_DATA;
                    else
                        urb->CtrlPhase = DWC_OTG_CONTROL_STATUS;;
                    break;
                case DWC_OTG_CONTROL_DATA:
                    UHCUpdateXfer(hc, urb, HC_XFER_COMPL);
                    urb->CtrlPhase = DWC_OTG_CONTROL_STATUS;
                    break;
                case DWC_OTG_CONTROL_STATUS:
                    UHCCompleteUrb(urb, 0);
                    status = HC_XFER_URB_COMPL;
                    break;
                default:
                    break;       
            }

            UHCCompleteXfer(hc, pChnReg, status);
            break;
        case DWC_OTG_EP_TYPE_BULK:
            UHCUpdateXfer(hc, urb, HC_XFER_COMPL);
            UHCSaveToggle(hc, urb);
            UHCCompleteUrb(urb, 0);
            status = HC_XFER_URB_COMPL;
            UHCCompleteXfer(hc, pChnReg, status);
            break;
            
        default:
            break;
    }

	//CLEAN_HC_INT(pChnReg, xfercomp);
    //DISABLE_HC_INT(pChnReg, xfercompl);
}

/*
Name:       UHCStallIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCStallIsr(UHC_CHN_INFO* hc)
{ 
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];

    UHCCompleteUrb(hc->urb, -HOST_STALL);
    
    if (hc->EpType == DWC_OTG_EP_TYPE_BULK)
    {
        /*
        * USB protocol requires resetting the data toggle for bulk
        * and interrupt endpoints when a CLEAR_FEATURE(ENDPOINT_HALT)
        * setup command is issued to the endpoint. Anticipate the
        * CLEAR_FEATURE command since a STALL has occurred and reset
        * the data toggle now.
        */
        hc->urb->DataToggle = 0;
    }

    UHCHaltChn(hc, HC_XFER_STALL);                //Disable Channel
     
    DISABLE_HC_INT(pChnReg, stall);
}

/*
Name:       UHCAckIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCAckIsr(UHC_CHN_INFO* hc)
{
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];
    USB_REQ     *urb = hc->urb;

    //Reset Error Count
    urb->ErrCnt = 0;
 
    if (urb->PingState)
    {
        urb->PingState = 0;
        UHCHaltChn(hc, HC_XFER_ACK);
    }

    DISABLE_HC_INT(pChnReg, ack);
}

/*
Name:       UHCNakIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCNakIsr(UHC_CHN_INFO* hc)
{
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];
    USB_REQ     *urb = hc->urb;

    switch (hc->EpType) /** 0: Control, 1: Isoc, 2: Bulk, 3: Intr */
    {
        case DWC_OTG_EP_TYPE_CONTROL:
        case DWC_OTG_EP_TYPE_BULK:
            //Reset Error Count
            urb->ErrCnt = 0;
            
            if (!urb->PingState)
            {
                UHCUpdateXfer(hc, urb, HC_XFER_NAK);
                UHCSaveToggle(hc, urb);
                
                if (USB_SPEED_HS==hc->speed && !hc->EpIsIn) 
                {
                    urb->PingState = 1;
                }
            }
            
            UHCHaltChn(hc, HC_XFER_NAK);
            break;
        default:
            break;
    }

    CLEAN_HC_INT(pChnReg, nak);
    DISABLE_HC_INT(pChnReg, nak);
}


/*
Name:       UHCNyetIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCNyetIsr(UHC_CHN_INFO* hc)
{
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];
    USB_REQ     *urb = hc->urb;

    printf("NYET\n");
    //Reset Error Count
    urb->ErrCnt = 0;
    urb->PingState = 1;

    UHCUpdateXfer(hc, urb, HC_XFER_NYET);
    UHCSaveToggle(hc, urb);

    /*
    * Halt the channel and re-start the transfer so the PING
    * protocol will start.
    */
    UHCHaltChn(hc, HC_XFER_NYET);
    
    DISABLE_HC_INT(pChnReg, nyet);
}

/*
Name:       UHCXacterrIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCXacterrIsr(UHC_CHN_INFO* hc)
{
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];
    USB_REQ     *urb = hc->urb;

    //Reset Error Count
    urb->ErrCnt++;
    if (!urb->PingState)
    {
        UHCUpdateXfer(hc, urb, HC_XFER_XERR);
        UHCSaveToggle(hc, urb);
        
        if (USB_SPEED_HS==hc->speed && !hc->EpIsIn) 
        {
            urb->PingState = 1;
        }
    }
    UHCHaltChn(hc, HC_XFER_XERR);
    
    DISABLE_HC_INT(pChnReg, xacterr);
}

/*
Name:       UHCBabbleIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCBabbleIsr(UHC_CHN_INFO* hc)
{
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];

    UHCCompleteUrb(hc->urb, -HOST_BABBLE);
    UHCHaltChn(hc, HC_XFER_BABBLE);

    DISABLE_HC_INT(pChnReg, bblerr);
}

/*
Name:       UHCTglerrIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCTglerrIsr(UHC_CHN_INFO* hc)
{
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];

    if (hc->EpIsIn) //ep_is_in
    {
        hc->urb->ErrCnt = 0;
	} 
    else 
    {
		;//DWC_ERROR("Data Toggle Error on OUT transfer,""channel %d\n", _hc->hc_num);
	}

    DISABLE_HC_INT(pChnReg, datatglerr);
}

/*
Name:       UHCChhltdIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCChhltdIsr(UHC_CHN_INFO* hc)
{
    //HCHN_REG *pChnReg  = &OTGReg->Host.hchn[hc->chn];
    UHCReleaseChn(hc);
    //CLEAN_HC_INT(pChnReg, chhltd);
    //DISABLE_HC_INT(pChnReg, chhltd);
}

/*
Name:       UHCChnIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void UHCChnIsr(int8 chn)
{
    HCINT_DATA hcint;
    HCINTMSK_DATA hcintmsk;
    HCHN_REG *pChnReg  = &OTGReg->Host.hchn[chn];
    UHC_CHN_INFO* hc = UHCGetChn(chn);
    
    hcint.d32 = pChnReg->hcintn;
    hcintmsk.d32 = pChnReg->hcintmaskn;
 
    hcint.d32 = hcint.d32 & hcintmsk.d32;
    
    if ((hcint.b.chhltd) && (hcint.d32 != 0x2)) 
    {
        hcint.b.chhltd = 0;
    }

    if (hcint.b.xfercomp) 
    {
        UHCXfercompIsr(hc);
        /*
         * If NYET occurred at same time as Xfer Complete, the NYET is
         * handled by the Xfer Complete interrupt handler. Don't want
         * to call the NYET interrupt handler in this case.
         */
        hcint.b.nyet = 0;
    }
    if (hcint.b.chhltd) 
    {
        UHCChhltdIsr(hc);
    }
    if (hcint.b.ahberr)
    {
        ;
    }
    if (hcint.b.stall) 
    {
        UHCStallIsr(hc);
    }
    if (hcint.b.nak)
    {
        UHCNakIsr(hc);
    }
    if (hcint.b.ack) 
    {
        UHCAckIsr(hc);
    }
    if (hcint.b.nyet)
    {
        UHCNyetIsr(hc);
    }
    if (hcint.b.xacterr)
    {
        UHCXacterrIsr(hc);
    }
    if ( hcint.b.bblerr) 
    {
        UHCBabbleIsr(hc);
    }

    if (hcint.b.frmovrun) 
    {
        /**
        * Handles a host channel frame overrun interrupt. This handler may be called
        * in either DMA mode or Slave mode.
        */
    }
    if (hcint.b.datatglerr) 
    {
        UHCTglerrIsr(hc);
    }
}

/*
Name:       UHCIsr
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
void UHCIsr(void)
{
    GINTSTS_DATA gintsts;
    USBHOST_CTRL *pHost = &gHostCtrl;

    gintsts.d32 = OTGReg->Core.gintsts;
    //DISABLE_USB_INT;
    /* Check if HOST Mode */
    if (DWC_HOST_MODE == gintsts.b.curmod)
    {
        gintsts.d32 &= OTGReg->Core.gintmsk;
        if (!gintsts.d32)
            return;
    }
    else
    {
        return;
    }
    
    if(gintsts.b.disconnint)
    {
       
        pHost->PortStatus = 0;
        pHost->PortChange = 1;
    }
    
    if(gintsts.b.prtint)
    {
        
        UHCPortIsr();
    }

    if (gintsts.b.rxflvl)
    {
       
        UHCRxflvlIsr();
    }
    
    if (gintsts.b.nptxfemp)
    {
        
        UHCNptxfempIsr();
    }

    if (gintsts.b.hchint)
    {
        int8 chn;
        uint32 haint = OTGReg->Host.haint & OTGReg->Host.haintmsk;

        for(chn=0; chn<MAX_UHC_CHN; chn++)
        {
            if(haint & (1<<chn))
            {
                UHCChnIsr(chn);
            }
        }
    }
    ENABLE_USB_INT;
    OTGReg->Core.gintsts = gintsts.d32;//write clear
}

#endif
