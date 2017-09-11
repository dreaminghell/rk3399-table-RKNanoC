/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: HCD.h
Desc: 

Author: chenfen@rock-chips.com
Date: 12-06-00
Notes:

$Log: $
 *
 *
*/

#ifndef _HCD_H
#define _HCD_H

/*-------------------------------- Includes ----------------------------------*/


/*------------------------------ Global Defines ------------------------------*/


#define     MAX_UHC_CHN                 1

#define     HC_CHN_FIFO(chn)            (OTG_REG_BASE +((chn+1)<<12))

#define     DWC_OTG_HC_PID_DATA0        0
#define     DWC_OTG_HC_PID_DATA2        1
#define     DWC_OTG_HC_PID_DATA1        2
#define     DWC_OTG_HC_PID_MDATA        3
#define     DWC_OTG_HC_PID_SETUP        3

#define     DWC_OTG_EP_TYPE_CONTROL	    0
#define     DWC_OTG_EP_TYPE_ISOC        1
#define     DWC_OTG_EP_TYPE_BULK	    2
#define     DWC_OTG_EP_TYPE_INTR	    3

#define     DWC_OTG_EP_SPEED_LOW	    0
#define     DWC_OTG_EP_SPEED_FULL	    1
#define     DWC_OTG_EP_SPEED_HIGH	    2

/* Macro used to clear one channel interrupt */
#define CLEAN_HC_INT(_hc_regs_, _intr_) \
do { \
	HCINT_DATA hcint_clear; \
    hcint_clear.d32 = 0;    \
    hcint_clear.b._intr_ = 1; \
	_hc_regs_->hcintn = hcint_clear.d32; \
} while (0)

#define DISABLE_HC_INT(_hc_regs_, _intr_) \
do { \
	HCINTMSK_DATA hcintmsk; \
    hcintmsk.d32 = _hc_regs_->hcintmaskn;  \
    hcintmsk.b._intr_ = 0; \
	_hc_regs_->hcintmaskn =  hcintmsk.d32; \
} while (0)


/*------------------------------ Global Typedefs -----------------------------*/

/*
 * Reasons for halting a host channel.
 */
typedef enum HC_XFER_STATUS
{
    HC_XFER_IDLE,
    HC_XFER_COMPL,
    HC_XFER_URB_COMPL,
    HC_XFER_ACK,
    HC_XFER_NAK,
    HC_XFER_NYET,
    HC_XFER_XERR,
    HC_XFER_STALL,
    HC_XFER_BABBLE,
    HC_XFER_TIMEOUT,
} HC_XFER_STATUS_e;


/**
 * Phases for control transfers.
 */
typedef enum DWC_OTG_CTRL_PHASE {
	DWC_OTG_CONTROL_SETUP,
	DWC_OTG_CONTROL_DATA,
	DWC_OTG_CONTROL_STATUS
} DWC_OTG_CTRL_PHASE_e;


typedef	volatile struct tagUHC_CHN_INFO
{
    uint8           chn;
    uint8           valid;

    /** Device to access */
    uint8           DevAddr;
    /** EP to access */
    uint8           EpNum;
    /** EP direction. 0: OUT, 1: IN */
    uint8           EpIsIn;
	/**
	 * EP speed.
	 * One of the following values:
	 *	- DWC_OTG_EP_SPEED_LOW
	 *	- DWC_OTG_EP_SPEED_FULL
	 *	- DWC_OTG_EP_SPEED_HIGH
	 */
    uint8           speed;
	/**
	 * Endpoint type.
	 * One of the following values:
	 *	- DWC_OTG_EP_TYPE_CONTROL: 0
	 *	- DWC_OTG_EP_TYPE_ISOC: 1
	 *	- DWC_OTG_EP_TYPE_BULK: 2
	 *	- DWC_OTG_EP_TYPE_INTR: 3
	 */
    uint8           EpType;
	/** Max packet size in bytes */
    uint16          MaxPkt;
	/**
	 * PID for initial transaction.
	 * 0: DATA0,<br>
	 * 1: DATA2,<br>
	 * 2: DATA1,<br>
	 * 3: MDATA (non-Control EP),
	 *	  SETUP (Control EP)
	 */
	uint8           PidStart;

    uint8           *XferBuf;              //中断数据接收或发送数据用到的buf地址  
    uint32          XferLen;               //需要传输的数据长度
    uint32          XferCnt;               //已经发送或接收的数据长度
    /** Packet count at start of transfer.*/
    uint16          StartPktCnt;
    /**
    * Flag to indicate whether the transfer has been started. Set to 1 if
    * it has been started, 0 otherwise.
    */
	uint8           XferStarted;
	/**
	 * Set to 1 to indicate that a PING request should be issued on this
	 * channel. If 0, process normally.
	 */
    uint8           DoPing;
	/**
	 * Set to 1 to indicate that the error count for this transaction is
	 * non-zero. Set to 0 if the error count is 0.
	 */
    uint8           ErrorState;
    
    uint32          status;
    /**
    * Set to 1 if the host channel has been halted, but the core is not
    * finished flushing queued requests. Otherwise 0.
    */
    uint8           HaltPending;
    uint8           HaltOnQueue;

    USB_REQ         *urb;
         
}UHC_CHN_INFO, *pUHC_CHN_INFO;


typedef volatile struct tagUSBHOST_CTRL 
{
    uint8           PortStatus; 
    uint8           PortChange;
    uint8           PortSpeed;

    UHC_CHN_INFO    ChnInfo[MAX_UHC_CHN];
    
}USBHOST_CTRL, *pUSBHOST_CTRL;

/*----------------------------- External Variables ---------------------------*/

extern USBHOST_CTRL gHostCtrl;


/*------------------------- Global Function Prototypes -----------------------*/

extern void UHCRxPkt(UHC_CHN_INFO *hc, uint32 bytes);

extern void UHCSaveToggle(UHC_CHN_INFO* hc, USB_REQ *urb);

extern int32 UHCUpdateXfer(UHC_CHN_INFO* hc, USB_REQ *urb, uint32 ChnStatus);

extern void UHCCompleteUrb(USB_REQ *urb, int32 status);

extern void UHCCompleteXfer(UHC_CHN_INFO* hc, HCHN_REG *pChnReg, uint32 status);

extern int32 UHCProcessXfer(void);

extern  void UHCReleaseChn(UHC_CHN_INFO * hc);

extern  UHC_CHN_INFO* UHCGetChn(int8 chn);

extern  int32 UHCHaltChn(UHC_CHN_INFO* hc, uint32 status);

extern  void UHCInitChn(UHC_CHN_INFO *hc, USB_REQ *urb);


extern int32 UHCStartWaitXfer(USB_REQ *urb, int32 timeout);

extern int32 UHCResetPort(HOST_DEV *pDev);

extern int32 UHCPortStatus(uint8 *status, uint8 *change);

extern uint8 UHCPrtConnSts(void);

extern  int32 UHCInit(void);

extern  void UHCDeInit(void);

extern void UHCIsr(void);

#endif
