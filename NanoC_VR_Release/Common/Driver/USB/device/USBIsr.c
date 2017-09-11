/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: USBIsr.c
Desc:

Author: chenfen
Date: 13-07-26
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/
#include "SysInclude.h"

#ifdef _USB_

#include "USBConfig.h"



#ifdef UDEBUG(...)
#undef UDEBUG(...)
#define UDEBUG(...)
#endif
/*------------------------------------ Defines -------------------------------*/

/*----------------------------------- Typedefs -------------------------------*/

/*-------------------------- Forward Declarations ----------------------------*/

extern UDC_DRIVER UDCDriver;

/* ------------------------------- Globals ---------------------------------- */

/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

/*
Name:       USBHandleEp0
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_DRIVER_CODE_
static void USBHandleEp0(pUDC_DRIVER pUDC, uint32 event)
{
    USB_CTRL_REQUEST *CtrlRequest = &pUDC->CtrlRequest;
    USB_EP0_REQ *req = &pUDC->ep0req;
    int32  TRACE_LEN;

    TRACE_LEN = TRACE_ENTER(__FUNCTION__);

	switch (pUDC->ep0state)
    {
    	//case EP0_DISCONNECT:
    	//	break;
    	case EP0_IDLE:
            if (event == UDC_EV_SETUP_DONE)
            {
                USBDeviceSetup(pUDC);
            }
            break;

        case EP0_IN_DATA_PHASE:
            if (event == UDC_EV_IN_XFER_COMPL)
            {
                if(req->XferLen >= req->NeedLen)
                {
                    pUDC->ep0state = EP0_IDLE;
                    req->NeedLen = 0;
                }
                else
                {
                    uint16 size = MIN(EP0_PACKET_SIZE, (req->NeedLen-req->XferLen));

                    USBWriteEp0(size, req->buf+req->XferLen);
                    req->XferLen += size;
                }
            }
            break;
        case EP0_OUT_DATA_PHASE:
            if (event == UDC_EV_OUT_XFER_COMPL)
            {
                if(req->XferLen >= req->NeedLen)
                {
                    if (req->complete)
                        req->complete(pUDC->pDev);

                    USBInEp0Ack();
                    pUDC->ep0state = EP0_IDLE;
                    req->NeedLen = 0;
                }
            }
            break;
        //case EP0_IN_STATUS_PHASE:
        //case EP0_OUT_STATUS_PHASE:
        //case EP0_STALL:
        //    break;
        default:
            break;
    }

    TRACE_EXIT(TRACE_LEN);
}

/*
Name:       USBInEpIsr
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_DRIVER_CODE_
static void USBInEpIsr(pUDC_DRIVER pUDC)
{
    uint32 ep_intr;
    uint32 diepint, msk;
    uint32 epnum = 0;
    DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;
    pUSB_DEVICE  pDev = pUDC->pDev;
    int32  TRACE_LEN;

    TRACE_LEN = TRACE_ENTER(__FUNCTION__);

    /* Read in the device interrupt bits */
    ep_intr = (dev_regs->daint & dev_regs->daintmsk) & 0xFFFF;

    /* Service the Device IN interrupts for each endpoint */
    while (ep_intr)
    {
        if (ep_intr&0x1)
        {
            msk = dev_regs->diepmsk | ((dev_regs->dtknqr4_fifoemptymsk & 0x01)<<7);   //<<7是因为msk是保留?
            diepint = dev_regs->in_ep[epnum].diepint & msk;

            UDEBUG("in ep%d intr=0x%x\n", epnum, diepint);

            /* Transfer complete */
            if ( diepint & 0x01 )
            {
                /* Disable the NP Tx FIFO Empty Interrrupt */
                dev_regs->dtknqr4_fifoemptymsk = 0;

                /* Clear the bit in DIEPINTn for this interrupt */
                dev_regs->in_ep[epnum].diepint = 0x01;

                if (epnum == 0)
                {
                    USBHandleEp0(pUDC, UDC_EV_IN_XFER_COMPL);
                }
                else
                {
                    pDev->request(epnum, UDC_EV_IN_XFER_COMPL, 0, pDev);
                }
            }

            /* Endpoint disable  */
            if (  diepint & 0x02 )
            {
                /* Clear the bit in DIEPINTn for this interrupt */
                dev_regs->in_ep[epnum].diepint = 0x02;

            }
            /* AHB Error */
            if ( diepint & 0x04 )
            {
                /* Clear the bit in DIEPINTn for this interrupt */
                dev_regs->in_ep[epnum].diepint = 0x04;
            }
            /* TimeOUT Handshake (non-ISOC IN EPs) */
            if ( diepint & 0x08 )
            {
                dev_regs->in_ep[epnum].diepint = 0x08;
            }

            /** IN Token received with TxF Empty */
            if ( diepint & 0x20 )
            {
                dev_regs->in_ep[epnum].diepint = 0x20;
            }
            /** IN EP Tx FIFO Empty Intr */
            if ( diepint & 0x80 )
            {
                dev_regs->in_ep[epnum].diepint = 0x10;
            }
        }

		epnum++;
		ep_intr >>=1;
    }

    TRACE_EXIT(TRACE_LEN);

}

/*
Name:       USBOutEpIsr
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_DRIVER_CODE_
static void USBOutEpIsr(pUDC_DRIVER pUDC)
{
    uint32 ep_intr;
    uint32 doepint;
    uint32 epnum = 0;
    pUSB_DEVICE  pDev = pUDC->pDev;
    DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;
    int32  TRACE_LEN;

    TRACE_LEN = TRACE_ENTER(__FUNCTION__);

    /* Read in the device interrupt bits */
    ep_intr = (dev_regs->daint & dev_regs->daintmsk) >> 16;

    while(ep_intr)
    {
        if (ep_intr & 0x1)
        {
            doepint = dev_regs->out_ep[epnum].doepint & dev_regs->doepmsk;

            /* Transfer complete */
            if ( doepint & 0x01 )
            {
                /* Clear the bit in DOEPINTn for this interrupt */
                dev_regs->out_ep[epnum].doepint = 0x01;
                //if (epnum)
                //    dev_regs->out_ep[epnum].doepctl |= (1ul<<26) | (1ul<<31);
            }

            /* Endpoint disable  */
            if ( doepint & 0x02 )
            {
                /* Clear the bit in DOEPINTn for this interrupt */
                dev_regs->out_ep[epnum].doepint = 0x02;
            }
            /* AHB Error */
            if ( doepint & 0x04 )
            {
                dev_regs->out_ep[epnum].doepint = 0x04;
            }
            /* Setup Phase Done (contorl EPs) */
            if ( doepint & 0x08 )
            {
                dev_regs->out_ep[epnum].doepint = 0x08;
            }
         }
        epnum++;
        ep_intr >>=1;
    }

    TRACE_EXIT(TRACE_LEN);
}

/*
Name:       USBRxLevelIsr
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_DRIVER_CODE_
static void USBRxLevelIsr(pUDC_DRIVER pUDC)
{
    USB_OTG_REG *  otg_core = (USB_OTG_REG * )USB_REG_BASE;
    DEVICE_GRXSTS_DATA status;
    pUSB_DEVICE  pDev = pUDC->pDev;
    int32  TRACE_LEN;
    uint32 tempGRX;
    TRACE_LEN = TRACE_ENTER(__FUNCTION__);

    /* Disable the Rx Status Queue Level interrupt */
    otg_core->Core.gintmsk &= ~(1<<4);

    /* Get the Status from the top of the FIFO */
    status.d32 = otg_core->Core.grxstsp;

    switch (status.b.pktsts)
    {
        case 0x01: //Global OUT NAK
            break;
        case 0x02: //OUT data packet received;
            if (0 != status.b.epnum)
            {
                pDev->request(status.b.epnum, UDC_EV_OUT_PKT_RCV, status.b.bcnt|(status.b.fn<<16), pDev);
            }
            else
            {
                USB_EP0_REQ *req = &pUDC->ep0req;
                if (status.b.bcnt)
                {
                    USBReadEp(0, status.b.bcnt, req->buf+req->XferLen);
                    req->XferLen += status.b.bcnt;
                }
            }
            break;
        case 0x03://OUT transfer completed
            if (0 == status.b.epnum)
            {
                otg_core->Device.out_ep[0].doepctl = (1ul<<15) | (1ul<<26) | (1ul<<31);
                USBHandleEp0(pUDC, UDC_EV_OUT_XFER_COMPL);
            }
            else
            {
                otg_core->Device.out_ep[status.b.epnum].doepctl |= (1ul<<26) | (1ul<<31);
                pDev->request(status.b.epnum, UDC_EV_OUT_XFER_COMPL, 0, pDev);
            }
            break;

        case 0x04: //SETUP transaction completed
            otg_core->Device.out_ep[0].doepctl = (1ul<<15) | (1ul<<26) | (1ul<<31);   //Active ep, Clr Nak, endpoint enable
            break;
        case 0x06: //SETUP data packet received
            if(status.b.bcnt > 0)
            {
                USBReadEp(0, 8, &pUDC->CtrlRequest);
            }
            USBHandleEp0(pUDC, UDC_EV_SETUP_DONE);
            break;
        default:
            break;
    }


    /* Enable the Rx Status Queue Level interrupt */
    otg_core->Core.gintmsk |= (1<<4);

    /* Clear interrupt */
    otg_core->Core.gintsts = (1<<4);
    TRACE_EXIT(TRACE_LEN);
}

/*
Name:       USBResetIsr
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_DRIVER_CODE_
/*static*/ void USBEnumDoneIsr(pUDC_DRIVER pUDC)
{
    DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;
    pUSB_DEVICE  pDev = pUDC->pDev;
    /* Set the MPS of the IN EP based on the enumeration speed */
    uint8 speed = USB_SPEED_FULL;
    int32  TRACE_LEN;

    TRACE_LEN = TRACE_ENTER(__FUNCTION__);

    switch ((dev_regs->dsts >> 1) & 0x03)
    {
        case 0: //HS_PHY_30MHZ_OR_60MHZ:
            speed = USB_SPEED_HIGH;
            USBDEBUG("SPEED_HIGH");
            dev_regs->in_ep[0].diepctl &= ~0x03;  //64bytes MPS
            break;

        case 1: //FS_PHY_30MHZ_OR_60MHZ:
        case 3: //FS_PHY_48MHZ:
            USBDEBUG("SPEED_FULL");
            dev_regs->in_ep[0].diepctl &= ~0x03;  //64bytes MPS
            break;

        case 2: //LS_PHY_6MHZ
        default:
            dev_regs->in_ep[0].diepctl |= 0x03;   //8bytes MPS
            USBDEBUG("SPEED_LOW");
            break;
    }

    pUDC->ep0state = EP0_IDLE;
    pDev->speed = speed;
    pDev->request(0, UDC_EV_ENUM_DONE, speed, pDev);

    /* setup EP0 to receive SETUP packets */
    dev_regs->out_ep[0].doeptsiz = (1ul << 29) | (1ul << 19) | 0x40; //supcnt = 1, pktcnt = 1, xfersize = 64*1
    /* Enable OUT EP for receive */
    dev_regs->out_ep[0].doepctl |= (1ul<<31);

    dev_regs->dctl |= 1<<8;         //clear global IN NAK

    TRACE_EXIT(TRACE_LEN);
}

/*
Name:       USBResetIsr
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_DRIVER_CODE_
static void USBResetIsr(void)
{
    uint32 count;
    DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;
    USB_OTG_REG *  otg_core = (USB_OTG_REG * )USB_REG_BASE;

    /* Clear the Remote Wakeup Signalling */
    otg_core->Device.dctl &= ~0x01;

    /* Set NAK for all OUT EPs */

    /* Flush the NP Tx FIFO */
    otg_core->Core.grstctl |= ( 0x10<<6) | (1<<5);     //Flush all Txfifo
    for (count=0; count<10000; count++)
    {
        if ((otg_core->Core.grstctl & (1<<5))==0)
            break;
    }

    /* Flush the Learning Queue. */
    otg_core->Core.grstctl |= 1<<3;

    /* Reset Device Address */
    otg_core->Device.dcfg &= ~0x07f0;

    /* setup EP0 to receive SETUP packets */

    /* Clear interrupt */
    otg_core->Core.gintsts = 0xFFFFFFFF;
}

/*
Name:       USBDeviceIsr
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_USB_DRIVER_CODE_
void USBDeviceIsr(void)
{
    uint32 intr_status;
    USB_OTG_REG *  otg_core = (USB_OTG_REG * )USB_REG_BASE;
    pUDC_DRIVER pUDC = &UDCDriver;
    pUSB_DEVICE pDev = pUDC->pDev;
    int32  TRACE_LEN;

    TRACE_LEN = TRACE_ENTER(__FUNCTION__);

    intr_status = otg_core->Core.gintsts & otg_core->Core.gintmsk;

    if (!intr_status)
    {
        TRACE_EXIT(TRACE_LEN);
        return;
    }

    if (intr_status & (1<<4))   //RxFIFO Non-Empty
    {
        USBRxLevelIsr(pUDC);
    }
    if (intr_status & (1<<5))   //Non-periodic TxFIFO Empty
    {
        otg_core->Core.gintmsk &= ~(1<<5);
        otg_core->Device.dtknqr4_fifoemptymsk = 0;
    }
    if(intr_status & (1<<10))    //Early Suspend
    {
        USBDEBUG("early suspend");
        otg_core->Core.gintsts = 1<<10;
    }
    if(intr_status & (1<<11))    //USB Suspend
    {
        if (0 == CheckVbus())
        {
            USBDEBUG("Disconnect\n");
            pUDC->connected = 0;
            pDev->disconnect(pDev);
        }
        else
        {
            if (pUDC->connected)
            {
                USBDEBUG("Suspend\n");
                pUDC->suspend = 1;
                pDev->suspend(pDev);
            }
        }
        otg_core->Core.gintsts = 1<<11;
    }

    if(intr_status & (1<<12))  //USB Reset
    {
        USBDEBUG("RESET\n");
        USBResetIsr();
        pUDC->suspend = 0;
        otg_core->Core.gintsts = 1<<12;
    }
    if(intr_status & (1<<13))  //Enumeration Done
    {
        USBDEBUG("enum done isr");
        USBEnumDoneIsr(pUDC);
        otg_core->Core.gintsts = 1<<13;
    }
    if(intr_status & (1<<18))       //IN中断
    {
        //DEBUG("InEpIsr");
        USBInEpIsr(pUDC);
    }
    if(intr_status & (1<<19))       //OUT中断
    {
        USBOutEpIsr(pUDC);
    }
    if(intr_status & (1<<30))  //USB VBUS中断  this interrupt is asserted when the utmiotg_bvalid signal goes high.
    {
        USBDEBUG("VBUS INT");
        otg_core->Core.gintsts = 1<<30;
    }
    if(intr_status & (1ul<<31))    //resume
    {
        USBDEBUG("resume");
        pUDC->suspend = 0;
        pDev->resume(pDev);
        otg_core->Core.gintsts = 1ul<<31;
    }
    if(intr_status & ((1<<22)|(1<<6)|(1<<7)|(1<<17)))
    {
        otg_core->Core.gintsts = intr_status & ((1<<22)|(1<<6)|(1<<7)|(1<<17));
    }

    TRACE_EXIT(TRACE_LEN);
}

#endif

