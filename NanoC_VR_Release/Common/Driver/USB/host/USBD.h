/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: USBD.h
Desc: 

Author: chenfen@rock-chips.com
Date: 12-06-20
Notes:

$Log: $
 *
 *
*/

#ifndef _USBD_H
#define _USBD_H

/*-------------------------------- Includes ----------------------------------*/

/*------------------------------ Global Defines ------------------------------*/

#define     MAX_USB_DEV                 1
#define     MAX_USB_DRV                 1
#define     MAX_HOST_EP                 4
#define     MAX_USB_REQ                 1

#define     USB_MAXCONFIG			    1	
#define     USB_MAXINTERFACES	        1   

#define     HOST_OK                     0
#define     HOST_ERR                    1
#define     HOST_STALL                  2
#define     HOST_NAK                    3
#define     HOST_NYET                   4
#define     HOST_XERR                   5
#define     HOST_TGLERR                 6
#define     HOST_BABBLE                 7
#define     HOST_NO_QUEUE               8
#define     HOST_NO_SPACE               9
#define     HOST_NO_DATA                10
#define     HOST_TIMEOUT                11
#define     HOST_NO_READY               12
#define     HOST_NO_DEV                 13


#define     USB_CNTL_TIMEOUT            100 /* 100ms timeout */
#define     USB_CTRL_GET_TIMEOUT	    5000
#define     USB_CTRL_SET_TIMEOUT	    5000

/* Some useful macros to use to create struct usb_device_id */
#define     USB_DEVICE_ID_MATCH_VENDOR		    0x0001
#define     USB_DEVICE_ID_MATCH_PRODUCT		    0x0002
#define     USB_DEVICE_ID_MATCH_DEV_LO		    0x0004
#define     USB_DEVICE_ID_MATCH_DEV_HI		    0x0008
#define     USB_DEVICE_ID_MATCH_DEV_CLASS		0x0010
#define     USB_DEVICE_ID_MATCH_DEV_SUBCLASS	0x0020
#define     USB_DEVICE_ID_MATCH_DEV_PROTOCOL	0x0040
#define     USB_DEVICE_ID_MATCH_INT_CLASS		0x0080
#define     USB_DEVICE_ID_MATCH_INT_SUBCLASS	0x0100
#define     USB_DEVICE_ID_MATCH_INT_PROTOCOL	0x0200

#define     USB_DEVICE_ID_MATCH_DEVICE \
		    (USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT)

#define     USB_DEVICE_ID_MATCH_INT_INFO \
		    (USB_DEVICE_ID_MATCH_INT_CLASS | \
		    USB_DEVICE_ID_MATCH_INT_SUBCLASS | \
		    USB_DEVICE_ID_MATCH_INT_PROTOCOL)

#define     USBPipeEpNum(pipe)          ((pipe>>11) & 0xf)

#define     USBPipeDir(pipe)            ((pipe>>15) & 0x1)

#define     USBIsCtrlPipe(pipe)         (!((pipe>>16) & 0x1))

#define     USBPipeType(pipe)           ((pipe>>16) & 0x3)

#define     USBPipeDev(pipe)            ((pipe>>18) & 0x7f)

#define     USBGetToggle(dev, ep, out)  (((dev)->toggle[out] >> (ep)) & 1)
#define	    USBDoToggle(dev, ep, out)   ((dev)->toggle[out] ^= (1 << (ep)))
#define     USBSetToggle(dev, ep, out, bit) \
		    ((dev)->toggle[out] = ((dev)->toggle[out] & ~(1 << (ep))) | \
		    ((bit) << (ep)))

//Standard Feature Selectors
#define     USB_ENDPOINT_HALT		    0	/* IN/OUT will STALL */

#define     HostCleanEpStall(pDev, pipe, wIndex)    \
            HostClearFeature(pDev, pipe, USB_RECIP_ENDPOINT, USB_ENDPOINT_HALT, wIndex)


#define     USBEpNum(ep)    (ep->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)

#define     USBEpType(ep)   (ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)

#define     USBEpDirIn(ep)  ((ep->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN)

/*------------------------------ Global Typedefs -----------------------------*/


typedef	struct tagHOST_INTERFACE
{
    HOST_IF_DESC        IfDesc;
    HOST_EP_DESC        EpDesc[MAX_HOST_EP];
    struct tagHOST_DEV  *pDev;
    struct tagHOST_DEV_DRIVER *drv;
}HOST_INTERFACE, *pHOST_INTERFACE;


typedef	struct tagHOST_DEV
{
    uint8               valid;
    uint8               DevNum;
    uint8               speed;
    uint8               state;
    uint16              toggle[2];        /*one bit for each endpoint   [0] = OUT, [1] = IN*/
    HOST_EP_DESC        ep0;                          
    HOST_DEV_DESC       DevDesc;
    HOST_CONFIG_DESC    CfgDesc;
    HOST_INTERFACE      intf;

    HOST_EP_DESC        *EpIn[16];
    HOST_EP_DESC        *EpOut[16];

}HOST_DEV, *pHOST_DEV;


typedef struct tagUSB_DEV_ID
{
	/* which fields to match against? */
	uint16		        MatchFlags;

	/* Used for product specific matches; range is inclusive */
	uint16		        idVendor;
	uint16		        idProduct;

	/* Used for interface class matches */
	uint8		        bInterfaceClass;
	uint8		        bInterfaceSubClass;
	uint8		        bInterfaceProtocol;
} USB_DEV_ID;


typedef	struct tagHOST_DEV_DRIVER
{
    const char          *name;
    int32               (*probe)(HOST_INTERFACE *);
    void                (*disconnet)(HOST_INTERFACE *);
    //int32             (*suspend)(struct tagHOST_DEV *);
    //int32             (*resume)(struct tagHOST_DEV *);
    const USB_DEV_ID    *DevIDTab;
}HOST_DEV_DRIVER, *pHOST_DEV_DRIVER;


typedef union tagHOST_PIPE
{
    /** raw register data */
    uint32 d32;

        /** register bits */
	struct 
	{
		/** Maximum packet size in bytes */
		unsigned mps : 11;

		/** Endpoint number */
		unsigned epnum : 4;

		/** 0: OUT, 1: IN */
		unsigned epdir : 1;

		/** 0: Control, 1: Isoc, 2: Bulk, 3: Intr */
		unsigned eptype : 2;

		/** Device address */
		unsigned devaddr : 7;

        //unsigned pid : 2;

        unsigned reserved25_31 : 7;

	} b;
} HOST_PIPE;


//typedef void (*USB_COMPLETE)(struct USB_REQ *);
typedef	struct tagUSB_REQ
{
    uint8               valid;
    HOST_DEV            *pDev;
    HOST_PIPE           pipe;
    USB_CTRL_REQ        *SetupPkt;
    uint8               CtrlPhase;
    void                *TransBuf;
    uint32              BufLen;
    uint32              ActualLen;
    uint32              StatusBuf[1];
	uint8               DataToggle;
	/** Ping state if 1. */
	uint8               PingState;
    //void              *context;
	uint8 		        ErrCnt;
    int32               status;
    uint8               completed;
}USB_REQ, *pUSB_REQ;


/*----------------------------- External Variables ---------------------------*/


/*------------------------- Global Function Prototypes -----------------------*/

extern uint16 HostSwap16(uint16 input);

extern uint32 HostSwap32(uint32 input);

extern int32 HostInit(void);

extern void HostEvent(void);

extern uint32 HostCreatePipe(HOST_DEV * dev, uint8 epnum, uint8 epdir);

extern int32 HostCtrlMsg(HOST_DEV *pDev, uint32 pipe, uint8 bmRequestType, uint8 bRequest, uint16  wValue, 
                        uint16 wIndex, void *data, uint32 size, int timeout);

extern int32 HostBulkMsg(HOST_DEV *pDev, uint32 pipe, void * data, int len, int timeout);

extern int32 HostRegDrv(HOST_DEV_DRIVER * pDrv);

extern void HostUnRegDrv(HOST_DEV_DRIVER * pDrv);

extern int32 HostClearFeature(HOST_DEV *pDev, uint32 pipe, uint8 type, uint16 wValue, uint16 wIndex);

extern int32 HostSetConfig(HOST_DEV *pDev, uint16 config);

extern int32 HostGetDesc(HOST_DEV *pDev, uint8 type, uint8 index, void *buf, int size);

extern int32 HostPortchange(uint8 status);


#endif
