/*
********************************************************************************
*                   Copyright (c) 2009,chenfen
*                         All rights reserved.
*
* File Name��   UsbAdapterProbe.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             chenfen      2008-12-12          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_USB_ADAPTER

#include "SysInclude.h"
#include "FsInclude.h"

_ATTR_SYS_BSS_ UINT8  UsbAdapterProbeConnectFlag;
_ATTR_SYS_BSS_ UINT8  UsbAdapterProbeStartFlag;
_ATTR_SYS_BSS_ UINT32 UsbAdapterProbeSystickCounter;
_ATTR_SYS_BSS_ UINT32 UsbAdapterProbeLdoBak;


/*
--------------------------------------------------------------------------------
  Function name : usb_in_ep_intr
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void usb_in_ep_intr_hook(void)
{
    uint32 ep_intr;
    uint32 diepint;
    uint32 msk;
    DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;

    /* Read in the device interrupt bits */
    ep_intr = (dev_regs->daint & dev_regs->daintmsk) & 0xFFFF;
    if ( ep_intr & 0x01)
    {
        ep_intr = 0;     //�˵�0
    }
    else if ( ep_intr & 0x02)
    {
        ep_intr = 1;       //�˵�1
    }
    else
    {
        return;
    }

    /* Service the Device IN interrupts for each endpoint */

    msk = dev_regs->diepmsk | ((dev_regs->dtknqr4_fifoemptymsk & 0x01)<<7);   //<<7����Ϊmsk�Ǳ���?
    diepint = dev_regs->in_ep[ep_intr].diepint & msk;
                       
    /* Transfer complete */
    if (diepint & 0x01 ) 
    {
        /* Disable the NP Tx FIFO Empty Interrrupt */
        dev_regs->dtknqr4_fifoemptymsk = 0;

        /* Clear the bit in DIEPINTn for this interrupt */
        dev_regs->in_ep[ep_intr].diepint = 0x01;
    }
    /* Endpoint disable  */
    if (diepint & 0x02 ) 
    {
        /* Clear the bit in DIEPINTn for this interrupt */
        dev_regs->in_ep[ep_intr].diepint = 0x02;

    }
    /* AHB Error */
    if ( diepint & 0x04 ) 
    {
        /* Clear the bit in DIEPINTn for this interrupt */
        dev_regs->in_ep[ep_intr].diepint = 0x04;
    }
    /* TimeOUT Handshake (non-ISOC IN EPs) */
    if ( diepint & 0x08 ) 
    { 
        dev_regs->in_ep[ep_intr].diepint = 0x08;
    }
    /** IN Token received with TxF Empty */
    if ( diepint & 0x20 ) 
    {
        dev_regs->in_ep[ep_intr].diepint = 0x20;
    }           
    /** IN EP Tx FIFO Empty Intr */
    if ( diepint & 0x80 )
    {                   
        dev_regs->in_ep[ep_intr].diepint = 0x80;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : UsbAdpterProbeISR
  Author        : chenfen
  Description   : 
                  the device interrupts.  Many conditions can cause a
   * device interrupt. When an interrupt occurs, the device interrupt
   * service routine determines the cause of the interrupt and
   * dispatches handling to the appropriate function. These interrupt
   * handling functions are described below.
     *
   * All interrupt registers are processed from LSB to MSB.
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/012/10         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void UsbAdpterProbeISR(void)
{
    uint32 intr_status;
    USB_OTG_REG *  otg_core = (USB_OTG_REG * )USB_REG_BASE;

    DEBUG("PROBE ISR");
    intr_status = otg_core->Core.gintsts & otg_core->Core.gintmsk;  

    if (!intr_status) 
    {
        return ;
    }

    if(intr_status & (1<<10))    //Early Suspend
    {
        //UDEBUG("e suspend\n");
        otg_core->Core.gintsts = 1<<10;
    }
    if(intr_status & (1<<11))    //USB Suspend
    {
        //UDEBUG("suspend\n");
        otg_core->Core.gintsts = 1<<11;
        if (UsbAdapterProbeConnectFlag)
        {
            USBDEBUG("Reset->Suspend");
            UsbAdapterProbeConnectFlag = 0;
        }
        else
        {
            USBDEBUG("Suspend");
        }
    }
    if(intr_status & (1<<12))  //USB Reset
    {
        //UDEBUG("reset\n");
        USBDEBUG("Reset");
        UsbAdapterProbeConnectFlag = 1;      
        
        otg_core->Device.dcfg &= ~0x07f0;    
        otg_core->Device.dctl &= ~0x01;
        otg_core->Core.gintsts = 1<<12;

		DISABLE_USB_INT;  
        //otg_core->Device.dctl |= 0x02;         //soft disconnect
    }
    
    if(intr_status & (1<<13))  //Enumeration Done
    {        
        //otg_core->core_regs.gintsts = 1<<13;
        //UDEBUG("enum\n");
    }
    
    if(intr_status & (1<<30))  //USB VBUS�ж�  this interrupt is asserted when the utmiotg_bvalid signal goes high.
    {
        //UDEBUG("vbus\n");
        otg_core->Core.gintsts = 1<<30;
    }

    if(intr_status & (1<<18))       //IN�ж�
    {
        usb_in_ep_intr_hook();
    }
    
    if(intr_status & ((1<<22)|(1<<6)|(1<<7)|(1<<17)))
    {
        otg_core->Core.gintsts = intr_status & ((1<<22)|(1<<6)|(1<<7)|(1<<17));
    }
}

_ATTR_SYS_CODE_
void UsbAdpterProbeStart(void)
{
    USB_OTG_REG *  otg_core = (USB_OTG_REG * )USB_REG_BASE;
    int count = 0;

    if (UsbAdapterProbeStartFlag == 0)
    {
        //variable init
        UsbAdapterProbeConnectFlag = 0;
        UsbAdapterProbeStartFlag   = 1;

        DEBUG();
        #ifdef _FILE_DEBUG_
        SysDebugHookDeInit();
        SysDebugHookInit();
        #endif

        //CPU init
        {
            DISABLE_USB_INT;
            UsbAdapterProbeLdoBak = ScuLDOSet(SCU_LDO_33);
            ScuLDODisable();            
            PMU_EnterModule(PMU_USB); //sch for PMU_USB = PMU_MAX, so can disale
            PMU_Disable(); //pmu disable for usb otg stability
            
            ScuClockGateCtr(CLOCK_GATE_USBPHY, 1);
            ScuClockGateCtr(CLOCK_GATE_USBBUS, 1);
            
            Scu_otgphy_suspend(1);
            ScuSoftResetCtr(RST_USBPHY, 1);            
            ScuSoftResetCtr(RST_USBLGC, 1);
            ScuSoftResetCtr(RST_USBOTG, 1);            
            DelayUs(1);
            Scu_otgphy_suspend(0);
            DelayMs(1);
            
            ScuSoftResetCtr(RST_USBPHY, 0);  
            DelayUs(1);          
            ScuSoftResetCtr(RST_USBLGC, 0);
            ScuSoftResetCtr(RST_USBOTG, 0);


            //interrupt init
            IntRegister(INT_ID23_USB, (void*)UsbAdpterProbeISR);
            //ENABLE_USB_INT;
        }
        USBDEBUG("Detect Start");	
        //USBConnect(1);       //FULL SPEED
        #ifdef _USB_
        USBConnect(0);
        #endif
        UsbAdapterProbeSystickCounter = GetSysTick();        //start timer
    }
}

_ATTR_SYS_CODE_
void UsbAdpterProbeStop(void)
{
    DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;

    if (UsbAdapterProbeStartFlag == 1)
    {
        //Variable deinit
        UsbAdapterProbeStartFlag = 0;

        DEBUG();

        //Usb Stop
        DISABLE_USB_INT;
        dev_regs->dctl |= 0x02;       //soft disconnect
        
        //CPU DeInit
        {
            Scu_otgphy_suspend(1);

            IntUnregister(INT_ID23_USB); 

            ScuSoftResetCtr(RST_USBPHY, 1);            
            ScuSoftResetCtr(RST_USBLGC, 1);
            ScuSoftResetCtr(RST_USBOTG, 1);  
            
            ScuClockGateCtr(CLOCK_GATE_USBPHY, 0);
            ScuClockGateCtr(CLOCK_GATE_USBBUS, 0);

            PMU_Enable(); //UsbAdpterProbeStart -> PmuDisable()
            PMU_ExitModule(PMU_USB);
            ScuLDOEnable(); //UsbAdpterProbeStart->ScuLdoDisalbe()
            ScuLDOSet(UsbAdapterProbeLdoBak);
        }
    }
}

_ATTR_SYS_CODE_
int32 UsbAdpterProbe(void)
{
    uint32 SystickTmp;

    if (UsbAdapterProbeStartFlag == 1)
    {
        if (UsbAdapterProbeConnectFlag)
        {   
            UsbAdapterProbeStartFlag = 0;
            USBDEBUG("Detect Ok");
            return 1;
        }
        else
        {
            //probe timeout, it means charge            
            SystickTmp = GetSysTick();
            if ((SystickTmp - UsbAdapterProbeSystickCounter) > 300)    //1000ms��ʱ
            {
                //USBDEBUG("Detect TimeOut");
                #if 1
                UsbAdapterProbeSystickCounter = SystickTmp;
                #ifdef _FILE_DEBUG_
                SysDebugHookDeInit();
                SysDebugHookInit();
                #endif
                #else
                UsbAdpterProbeStop();
                #endif
		        return 2;
            }
        }
    }
    return 0;
}

/*
********************************************************************************
*
*                         End of UsbAdapterProbe.c
*
********************************************************************************
*/

