/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: USBD.c
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

/*-------------------------- Forward Declarations ----------------------------*/

/* ------------------------------- Globals ---------------------------------- */

/*-------------------------------- Local Statics: ----------------------------*/
_ATTR_USB_HOST_BSS_ static HOST_DEV gHostDevCache[MAX_USB_DEV];
_ATTR_USB_HOST_BSS_ static USB_REQ gURBCache[MAX_USB_REQ];

_ATTR_USB_HOST_BSS_ static HOST_DEV_DRIVER *HostDevDrvTab[MAX_USB_DRV];


/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

/*
Name:       HostSwap16
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
uint16 HostSwap16(uint16 input)
{
    return ((input>>8)|(input<<8));
}

/*
Name:       HostSwap32
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
uint32 HostSwap32(uint32 input)
{
    return ((input>>24)|(input<<24)|(input>>8&0xff00)|(input<<8&0xff0000));
}

/*
Name:       HostAllocDev
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static HOST_DEV * HostAllocDev(void)
{
    HOST_DEV *pDev = &gHostDevCache[0];

    if (!pDev->valid)
    {
        memset(pDev, 0, sizeof(HOST_DEV));
        pDev->valid = 1;
        return pDev;
    }

    return NULL;
}

/*
Name:       HostFreeDev
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void HostFreeDev(HOST_DEV *pDev)
{
    pDev->valid=0;
}

/*
Name:       HostAllocReq
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static USB_REQ * HostAllocUrb(void)
{
    USB_REQ *urb = &gURBCache[0];
    
    if (!urb->valid)
    {
        memset(urb, 0, sizeof(USB_REQ));

        urb->valid = 1;;
        return urb;
    }
    
    return NULL;
}

/*
Name:       HostFreeReq
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void HostFreeUrb(USB_REQ *urb)
{
    urb->valid = 0;
}

/*
Name:       HostCreatePipe
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
uint32 HostCreatePipe(HOST_DEV * dev, uint8 epnum, uint8 epdir)
{
    HOST_EP_DESC *Ep;
    uint32 pipe;
    
    if (epdir)
        Ep = dev->EpIn[epnum];
    else    
        Ep = dev->EpOut[epnum];

    pipe = (dev->DevNum<<18) |(Ep->bmAttributes<<16)|(epdir << 15)
            |(epnum << 11)| Ep->wMaxPacketSize;

    return pipe;
}

/*
Name:       HostCtrlMsg
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 HostCtrlMsg(HOST_DEV *pDev, uint32 pipe, uint8 bmRequestType, uint8 bRequest, uint16  wValue, 
                        uint16 wIndex, void *data, uint32 size, int timeout)
{
    int32 ret;
    USB_CTRL_REQ CtrlReq;
    USB_REQ * urb = NULL;

	CtrlReq.bmRequestType = bmRequestType;
	CtrlReq.bRequest = bRequest;
	CtrlReq.wValue = wValue;
	CtrlReq.wIndex = wIndex;
	CtrlReq.wLength = size;

    urb = HostAllocUrb();

    urb->pipe.d32 = pipe;
    urb->SetupPkt = &CtrlReq;
    urb->TransBuf = data;
    urb->BufLen = size;
    
    urb->DataToggle = 1;
    urb->CtrlPhase = DWC_OTG_CONTROL_SETUP;
    urb->pDev = pDev;

    ret =  UHCStartWaitXfer(urb, timeout);

    HostFreeUrb(urb);

    return ret;    
}

/*
Name:       HostBulkMsg
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 HostBulkMsg(HOST_DEV *pDev, uint32 pipe, void * data, int len, int timeout)
{
    int32 ret;
    USB_REQ * urb = NULL;
    uint8 epdir = USBPipeDir(pipe);
    uint8 epnum = USBPipeEpNum(pipe);

    urb = HostAllocUrb();

    urb->pipe.d32 = pipe;
    urb->TransBuf = data;
    urb->BufLen = len;

    urb->pDev = pDev;
    urb->DataToggle = USBGetToggle(pDev, epnum, epdir);

    ret =  UHCStartWaitXfer(urb, timeout);
    
    USBSetToggle(pDev, epnum, epdir, urb->DataToggle);

    HostFreeUrb(urb);

    return ret;
}

/*
Name:       HostDisconnect
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
void HostDisconnect(void)
{
    HOST_DEV *pDev = &gHostDevCache[0];
    
    if (pDev->valid)
    {
        if (pDev->intf.drv)
        {
            pDev->intf.drv->disconnet(&pDev->intf);
        }
        HostFreeDev(pDev);
    }
}

/*
Name:       HostParseInterface
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 HostParseInterface(HOST_DEV *pDev, uint8* buf, uint32 size)
{
    int32 i, numep, ret = -HOST_ERR;
    HOST_EP_DESC *ep;
    uint8 epnum;
    uint8 epdir; /** 0: OUT, 1: IN */
    HOST_IF_DESC* ifdesc;
    HOST_INTERFACE *intf = &pDev->intf;

    ifdesc = &intf->IfDesc;
    memcpy(ifdesc, buf, USB_DT_INTERFACE_SIZE);
    numep = ifdesc->bNumEndpoints;
    if (numep > MAX_HOST_EP)
    {
        numep = MAX_HOST_EP;
        ifdesc->bNumEndpoints = numep;
    }

    buf += ifdesc->bLength;
    size -= ifdesc->bLength;

	if (numep > 0)
    {
        for (i=0; i<numep && size>0; i++)
        {
            ep = (HOST_EP_DESC*)buf; 
            if (ep->bDescriptorType != USB_DT_ENDPOINT)
                return -HOST_ERR;
            
            epdir = ep->bEndpointAddress >> 7;
            epnum = ep->bEndpointAddress & 0x0F;

            ep = &intf->EpDesc[i];
            memcpy(ep, buf, USB_DT_ENDPOINT_SIZE);            
            (epdir)? (pDev->EpIn[epnum]=ep) : (pDev->EpOut[epnum]=ep);
            
            buf += USB_DT_ENDPOINT_SIZE;
            size -=USB_DT_ENDPOINT_SIZE;
        }
        intf->pDev = pDev;
        ret = HOST_OK;
    }

    return ret;
}

/*
Name:       HostParseConfig
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 HostParseConfig(HOST_DEV *pDev, uint8* buf, uint32 size)
{
    int32 ret = -HOST_ERR;
    uint32 nintf;
    USB_DESC_HEADER *header;
    HOST_CONFIG_DESC *config = &pDev->CfgDesc;

    memcpy(config, buf, USB_DT_CONFIG_SIZE);
    #if 1
    nintf = config->bNumInterfaces;
    if (nintf > USB_MAXINTERFACES)
    {
        nintf = USB_MAXINTERFACES;
        config->bNumInterfaces = nintf;
    }
    #endif
    buf += config->bLength;
    size -= config->bLength;

    header = (USB_DESC_HEADER *)buf;
    
    if (USB_DT_INTERFACE == header->bDescriptorType)
    {
        ret = HostParseInterface(pDev, buf, size);
        if (ret < 0)
            return ret;
    }

    return ret;
}

/*
Name:       HostDevMatch
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 HostDevMatch(HOST_DEV *pDev, HOST_DEV_DRIVER *pDrv)
{
    USB_DEV_ID *id = (USB_DEV_ID *)pDrv->DevIDTab;
    HOST_IF_DESC* ifdesc = &pDev->intf.IfDesc;

    #if 0
    if ((USB_DEVICE_ID_MATCH_VENDOR & id->MatchFlags)
        && pDev->DevDesc.idVendor != id->idVendor)
    {
        return -HOST_ERR;
    }

    if ((USB_DEVICE_ID_MATCH_PRODUCT & id->MatchFlags)
        && pDev->DevDesc.idVendor != id->idProduct)
    {
        return -HOST_ERR;
    }
    #endif
    if ((USB_DEVICE_ID_MATCH_INT_CLASS & id->MatchFlags)
        && ifdesc->bInterfaceClass != id->bInterfaceClass)
    {
        return -HOST_ERR;
    }

    if ((USB_DEVICE_ID_MATCH_INT_SUBCLASS & id->MatchFlags)
        && ifdesc->bInterfaceSubClass != id->bInterfaceSubClass)
    {
        return -HOST_ERR;
    }

    if ((USB_DEVICE_ID_MATCH_DEV_PROTOCOL & id->MatchFlags)
        && ifdesc->bInterfaceProtocol!= id->bInterfaceProtocol)
    {
        return -HOST_ERR;
    }

    return HOST_OK;
}

/*
Name:       HostBindDriver
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 HostBindDriver(HOST_DEV *pDev)
{
    int32 i, ret = -HOST_ERR;
    HOST_DEV_DRIVER *pDrv;

    for(i=0; i<MAX_USB_DRV; i++)
    {
        pDrv = HostDevDrvTab[i];
        if (pDrv)
        {
            ret = HostDevMatch(pDev, pDrv);
            if (!ret)
            {
                ret = pDrv->probe(&pDev->intf);
                if (!ret)
                {
                    pDev->intf.drv = pDrv;
                    break;
                }
            }
        }
    }
    
    return ret;
}

/*
Name:       HostEp0ReInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static void HostEp0ReInit(HOST_DEV *pDev, uint16 wMaxPacketSize)
{
    HOST_EP_DESC *ep0 = &pDev->ep0;

    ep0->bmAttributes = USB_ENDPOINT_XFER_CONTROL;
    ep0->wMaxPacketSize = wMaxPacketSize;
    
    pDev->EpOut[0] = ep0;
    pDev->EpIn[0] = ep0;
}

/*
Name:       HostSetAddr
Desc:       
Param:      
Return:     <0: ERROR,  =0: OK 

Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 HostSetAddr(HOST_DEV *pDev)
{
    int32 ret, i;
    uint8 devnum = 1;
    
    for(i=0; i<2; i++) 
    {
        ret = HostCtrlMsg(pDev, HostCreatePipe(pDev, 0, 0),
				 0, USB_REQ_SET_ADDRESS, devnum, 0,
				NULL, 0, USB_CTRL_SET_TIMEOUT);

        if (ret >= 0)
        {
            pDev->DevNum = devnum;
            pDev->state = USB_STATE_ADDRESS;
            break;
        }
        USBDelayMS(200);
    }

    return ret;
}

/*
Name:       HostSetConfig
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 HostSetConfig(HOST_DEV *pDev, uint16 config)
{
    int32 ret;

    ret = HostCtrlMsg(pDev, HostCreatePipe(pDev, 0, 0),
             0, USB_REQ_SET_CONFIGURATION, config, 0,
            NULL, 0, USB_CTRL_SET_TIMEOUT);

    if (ret < 0)
        return ret;

    pDev->state = USB_STATE_CONFIGURED;
    return HOST_OK;
}

/*
Name:       HostGetDesc
Desc:        
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 HostGetDesc(HOST_DEV *pDev, uint8 type, uint8 index, void *buf, int size)
{
    int32 i, ret;
 
    for (i = 0; i < 3; i++) 
    { 
        ret = HostCtrlMsg(pDev, HostCreatePipe(pDev, 0, 1),  USB_DIR_IN, USB_REQ_GET_DESCRIPTOR,
                    (type << 8) + index, 0, buf, size, USB_CTRL_GET_TIMEOUT);
        if (ret <= 0)
        {
            continue;
        }
        if (ret>1 && ((uint8 *)buf)[1] != type)
        {
            ret = -HOST_ERR;
            continue;
        }
        break;
    }
    
    return ret;
}


/*
Name:       HostClearFeature
Desc:        
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 HostClearFeature(HOST_DEV *pDev, uint32 pipe, uint8 type, uint16 wValue, uint16 wIndex)
{
    int32 ret;
    ret = HostCtrlMsg(pDev, pipe, type, USB_REQ_CLEAR_FEATURE, 
                      wValue, wIndex, NULL, 0, USB_CTRL_SET_TIMEOUT);
    if (ret < 0)
        return ret;
    
    //  toggle is reset on clear
    USBSetToggle(pDev, USBPipeEpNum(pipe), USBPipeDir(pipe), 0);

    return ret;
}

/*
Name:       HostNewDev
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 HostNewDev(HOST_DEV *pDev)
{
	int32 ret = -HOST_ERR;
    uint16 length;
    uint32 buf[256/4];
    HOST_CONFIG_DESC *config;
    uint32 ncfg = pDev->DevDesc.bNumConfigurations;

    if (ncfg > USB_MAXCONFIG)
    {
        ncfg = USB_MAXCONFIG;
    }

    if (ncfg < 1)
        return -HOST_ERR;

    config = (HOST_CONFIG_DESC *)buf;
    
    ret = HostGetDesc(pDev, USB_DT_CONFIG, 0, buf, USB_DT_CONFIG_SIZE);
    if (ret < 0)
        goto FAIL;
    
    length = config->wTotalLength;
    if (length > 256)
    {
        ret = -HOST_ERR;
        goto FAIL;
    }
    
    ret = HostGetDesc(pDev, USB_DT_CONFIG, 0, buf, length);
    if (ret < 0)
        goto FAIL;
    
    ret = HostParseConfig(pDev, (uint8*)buf, length);
    if (ret < 0)
        goto FAIL;

    ret = HostSetConfig(pDev, 1);
    if (ret < 0)
        goto FAIL;
    
FAIL:

    return ret;
}

/*
Name:       HostPortInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
static int32 HostPortInit(HOST_DEV *pDev)
{
    int32 ret;
    uint16    wMaxPacketSize;
    HOST_DEV_DESC buf[64/4];
    HOST_DEV_DESC *pDesc;

    ret = UHCResetPort(pDev);
    if (ret < 0)
        return ret;

    switch (pDev->speed)
    {
        case USB_SPEED_HS:     //high speed
            wMaxPacketSize = 64;
            break;
        case USB_SPEED_FS:     //full speed
            wMaxPacketSize = 64;
            break;
        case USB_SPEED_LS:     //low speed
            wMaxPacketSize = 8;
            break;

        default:
            break;
    }

    HostEp0ReInit(pDev, wMaxPacketSize);

    USBDelayMS(10);
    ret = HostGetDesc(pDev, USB_DT_DEVICE, 0, buf, 64);
    if (ret < 0)
        return ret;
    
    pDesc = (HOST_DEV_DESC *)buf;
    if (wMaxPacketSize != pDesc->bMaxPacketSize0)
    {
        HostEp0ReInit(pDev, pDesc->bMaxPacketSize0);
    }

    #if 0
    ret = UHCResetPort(pDev); // windows 和linux 会进行二次复位, 协议无此要求
    if (ret < 0)
        return ret;
    #endif
    ret = HostSetAddr(pDev);
    if (ret < 0)
        return ret;

    USBDelayMS(5);
    ret = HostGetDesc(pDev, USB_DT_DEVICE, 0, buf, USB_DT_DEVICE_SIZE);
    if (ret < 0)
        return ret;

    memcpy(&pDev->DevDesc, buf, USB_DT_DEVICE_SIZE);

    return ret;
}

/*
Name:       HostPortchange
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 HostPortchange(uint8 status)
{
    HOST_DEV * pDev = NULL;
    int32 ret;

    /* Disconnect any existing devices under this port */
    HostDisconnect();
    
    if (status)
    {
        USBDelayMS(100);    //等待电源稳定

        status = UHCPrtConnSts();
        if (!status)
            goto FAIL;

        pDev = HostAllocDev();
        pDev->state = USB_STATE_POWERED;
        
        //ENABLE_USB_INT;
        ret = HostPortInit(pDev);
        if (ret < 0)
            goto FAIL;
        
        ret = HostNewDev(pDev);
        if (ret < 0)
            goto FAIL;
        SendMsg( MSG_USB_HOST_CONNECTED);

        ret = HostBindDriver(pDev);
        if (ret < 0)
            goto FAIL;

        return ret;
    }
    
FAIL:
    if (pDev)
    {        
        //DISABLE_USB_INT;
        HostFreeDev(pDev);
    }

    return -HOST_ERR;
}

/*
Name:       HostEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
void HostEvent(void)
{
    uint8 status;
    uint8 change;

    while(1)
    {
        UHCPortStatus(&status, &change);

        if (change)
        {
            HostPortchange(status);
        }
    }
}

/*
Name:       HostRegDriver
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 HostRegDrv(HOST_DEV_DRIVER * pDrv)
{
    uint32 i;

    for (i=0; i<MAX_USB_DRV; i++)
    {
        if (NULL == HostDevDrvTab[i])
        {
             HostDevDrvTab[i] = pDrv;
             return HOST_OK;
        }         
    }

    return -HOST_ERR;
}


/*
Name:       HostUnRegDriver
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
void HostUnRegDrv(HOST_DEV_DRIVER * pDrv)
{
    uint32 i;

    for (i=0; i<MAX_USB_DRV; i++)
    {

        if (pDrv == HostDevDrvTab[i])
        {
             HostDevDrvTab[i] = NULL;
        }         
    }
}

/*
Name:       HostInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_USB_HOST_CODE_
int32 HostInit(void)
{
    int32 ret;

    memset(gHostDevCache, 0, sizeof(gHostDevCache));
    memset(gURBCache, 0, sizeof(gURBCache));
    memset(HostDevDrvTab, 0, sizeof(HostDevDrvTab));
    
    ret = UHCInit();

    return ret;
}

#if 0
/*
Name:       HostTest
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
void HostTest(void)
{
    HostInit();

    MscInit();

    HostEvent();
}
#endif

#endif
