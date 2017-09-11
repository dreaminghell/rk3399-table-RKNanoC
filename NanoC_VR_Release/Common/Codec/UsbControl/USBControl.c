/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name:       USBControl.C
*
* Description:
*
* History:      <author>          <time>        <version>
*                 ZS      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#define _IN_USB_CONTROL_

#include "SysInclude.h"

#ifdef _USB_

#include "FsInclude.h"
#include "USBConfig.h"
#include "USBControl.h"

#define REMOTE_WAKEUP
#include "MPU6500.h"

#include "Thread.h"
#include "AudioControl.h"
/*
--------------------------------------------------------------------------------

   Forward Declaration

--------------------------------------------------------------------------------
*/
extern int vr_nanoc_suspend;
extern int32 FUSBFsgInit(void);
extern void  FUSBFsgDeInit(void);
extern int32 FUSBSerialInit(void);

extern int32 USBAudioInit(void *arg);
extern int32 USBAudioThread(void);
extern void USBAudioDeInit(void);

/*
*-------------------------------------------------------------------------------
*
*                           Struct Data define
*
*-------------------------------------------------------------------------------
*/
_ATTR_USBCONTROL_DATA_
FUN_USB_DEV FUSBDevTab[] =
{
    #ifdef USB_MSC
    {
        USB_CLASS_TYPE_MSC,
        FUSBFsgInit,
        FsgThread,
        FUSBFsgDeInit,
    },
    #endif

    #ifdef USB_AUDIO
    {
        USB_CLASS_TYPE_AUDIO,
        USBAudioInit,
        USBAudioThread,
        USBAudioDeInit,
    },
    #endif

    #ifdef USB_SERIAL
    {
        USB_CLASS_TYPE_SERIAL,
        FUSBSerialInit,
        SrlThread,
        SrlDeInit,
    }
    #endif
};


#ifdef USB_MSC
#include "AudioControl.h"

/*
--------------------------------------------------------------------------------
  Function name : void UsbBusyHook(void)
  Author        : ZhengYongzhi
  Description   : USB busy status flag function

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
void FUSBBusyHook(void)
{

}

/*
--------------------------------------------------------------------------------
  Function name : void UsbReadyHook(void)
  Author        : ZhengYongzhi
  Description   : USB Ready status flag function

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
void FUSBReadyHook(void)
{

}

/*
--------------------------------------------------------------------------------
  Function name : void UsbConnectedHook(void)
  Author        : ZhengYongzhi
  Description   : USB Connected status flag function

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
void FUSBConnectHook(void)
{

}

/*
--------------------------------------------------------------------------------
  Function name : void UsbRKMSCHook(uint32 cmd)
  Author        : ZhengYongzhi
  Description   : USB busy status display function

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
void FUSBRKCmdHook(uint32 cmd, uint32 param)
{
    if (cmd == 0xFFFFFFFE)
    {
        if (0 == param)
            SendMsg(MSG_MES_FIRMWAREUPGRADE);//system backgroud check this message ,and execute the firmware update
        else
            SendMsg(MSG_ENTER_LOADER_USB);
    }

    #ifdef _CDROM_
    else if (cmd == 0xFFFFFFF6)       // Show User Disk, not enum the cdrom
    {
        gSysConfig.bShowCdrom = 0;
        SendMsg(MSG_SYS_REBOOT);
    }
    #endif
}

/*
--------------------------------------------------------------------------------
Name:       FUSBFsgUpdateHook
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
void FUSBUpdateHook(void)
{
    FUSBDevice.StorageUpdate = 1;
}

_ATTR_USB_MSC_CODE_
void FUSBWriteHook(void)
{
    if (FUSBDevice.StorageStatus != 1)
    {
        FUSBDevice.StorageStatus = 1;
        FUSBDevice.MscStatusStartCounter = SysTickCounter;
    }
}


_ATTR_USB_MSC_CODE_
void FUSBReadHook(void)
{
    if (FUSBDevice.StorageStatus != 2)
    {
        FUSBDevice.StorageStatus = 2;
        FUSBDevice.MscStatusStartCounter = SysTickCounter;
    }
}

_ATTR_USB_MSC_CODE_
void FUSBIdleHook(void)
{
    FUSBDevice.StorageStatus = 0;
    FUSBDevice.MscStatusStartCounter = SysTickCounter;
}

/*
--------------------------------------------------------------------------------
  Function name : void UsbShowCdrom(void)
  Author        : ZhengYongzhi
  Description   : USB busy status display function

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
BOOL FUSBShowCdrom(void)
{
    #ifdef _CDROM_
        return bShowCdrom;
    #else
        return 0;
    #endif

}

/*
--------------------------------------------------------------------------------
  Function name :  void USBMSCInit(void )
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
static int32 FUSBFsgInit(void)
{

    return FsgInit((void*)0);
}

/*
--------------------------------------------------------------------------------
  Function name :  void USBMSCDeInit(void)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
static void FUSBFsgDeInit(void)
{
    FsgDeInit();
    //-----------------------------------------------------------------
    {
        #ifdef MEDIA_MODULE
        if(FUSBDevice.StorageUpdate)
        {
            gSysConfig.MedialibPara.MediaUpdataFlag = 1;
        }
        #endif

        #ifdef AUDIOHOLDONPLAY
        if (FUSBDevice.StorageUpdate)    //Memap switch,clear breakpoint.
        {
            gSysConfig.MusicConfig.HoldOnPlaySaveFlag = 0 ;//zs 2009-5-18
        }
        #endif

        #ifdef _CDROM_
        bShowCdrom = 1;
        #endif
    }
    //-----------------------------------------------------------------

    #if (NAND_DRIVER == 1)
    FtlRefreshHook();
    #endif

    #if((defined (_SDCARD_) || defined (_MULT_DISK_) )&& (!defined(_SPINOR_)) )
    gSysConfig.Memory =  FLASH0;
    FileSysSetup(gSysConfig.Memory);
    #else
    FileSysSetup(gSysConfig.Memory);
    #endif
    /*
    gSysConfig.Memory =  FLASH0;
    FileSysSetup(gSysConfig.Memory);
    */

    //zyz: After USB Transfer, need get Flash & Card free memory
    SendMsg(MSG_FLASH_MEM0_UPDATE);
}

/*
--------------------------------------------------------------------------------
  Function name : UsbBGetVetsion
  Author        : ZhengYongzhi
  Description   : USB get system version number.

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
extern uint16 LoaderVer;
_ATTR_USB_MSC_CODE_
void FUSBGetVetsion(PRKNANO_VERSION pVersion)
{
    pVersion->dwBootVer = (uint16)LoaderVer;
    pVersion->dwFirmwareVer = (((uint32)gSysConfig.MasterVersion & 0xFF) << 24) |
                              (((uint32)gSysConfig.SlaveVersion  & 0xFF) << 16) |
                               ((uint32)gSysConfig.SmallVersion & 0xFFFF);
}

/*
--------------------------------------------------------------------------------
  Function name : FwGetDevInfo
  Author        :
  Description   : USB get Device Info.

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_MSC_CODE_
int32 FwGetDevInfo(pPRODUCT_DEVINFO pInfo)
{
    #if 0
    uint32 temp = 3;

    memset((char*)pInfo, 0, sizeof(PRODUCT_DEVINFO));

    //pInfo->DevInfo = "DEVINFO";
    pInfo->DevInfo[0]='D';
    pInfo->DevInfo[1]='E';
    pInfo->DevInfo[2]='V';
    pInfo->DevInfo[3]='I';
    pInfo->DevInfo[4]='N';
    pInfo->DevInfo[5]='F';
    pInfo->DevInfo[6]='O';

    pInfo->size = 0x80;

    //pInfo->VendorID = "SONY    ";
    pInfo->VendorID[0]='S';
    pInfo->VendorID[1]='O';
    pInfo->VendorID[2]='N';
    pInfo->VendorID[3]='Y';
    pInfo->VendorID[4]=' ';
    pInfo->VendorID[5]=' ';
    pInfo->VendorID[6]=' ';
    pInfo->VendorID[7]=' ';

    //pInfo->ProductID = "NWZ-B172F       ";
    pInfo->ProductID[ 0]='N';
    pInfo->ProductID[ 1]='W';
    pInfo->ProductID[ 2]='Z';
    pInfo->ProductID[ 3]='-';
    pInfo->ProductID[ 4]='B';
    pInfo->ProductID[ 5]='1';
    pInfo->ProductID[ 6]='7';
    pInfo->ProductID[ 7]='2';
    pInfo->ProductID[ 8]='F';
    pInfo->ProductID[ 9]=' ';
    pInfo->ProductID[10]=' ';
    pInfo->ProductID[11]=' ';
    pInfo->ProductID[12]=' ';
    pInfo->ProductID[13]=' ';
    pInfo->ProductID[14]=' ';
    pInfo->ProductID[15]=' ';

    //pInfo->ProductRevision = "1.02";
    pInfo->ProductRevision[0]    = (gSysConfig.MasterVersion & 0x0f) + 0x30;
    pInfo->ProductRevision[1]    = '.';
    pInfo->ProductRevision[2]    = ((gSysConfig.SlaveVersion  & 0xf0) >> 4) + 0x30;
    pInfo->ProductRevision[3]    =  (gSysConfig.SlaveVersion  & 0x0f) + 0x30;

    //pInfo->ProductSubrevision = "01  ";
    pInfo->ProductSubrevision[0] = ((gSysConfig.SmallVersion  & 0xf0) >> 4) + 0x30;
    pInfo->ProductSubrevision[1] =  (gSysConfig.SmallVersion  & 0x0f) + 0x30;
    pInfo->ProductSubrevision[2] = 0x20;
    pInfo->ProductSubrevision[3] = 0x20;

    //pInfo->StorageSize = "  4G";
    pInfo->StorageSize[0] =' ';
    pInfo->StorageSize[1] =' ';
    pInfo->StorageSize[2] = (temp + 1) + 0x30;
    pInfo->StorageSize[3] = 'G';

    //pInfo->SerialNum = "400745";
    pInfo->SerialNum[0]='4';
    pInfo->SerialNum[1]='0';
    pInfo->SerialNum[2]='0';
    pInfo->SerialNum[3]='7';
    pInfo->SerialNum[4]='4';
    pInfo->SerialNum[5]='5';
    #endif

    return 0;
}

#endif

#ifdef REMOTE_WAKEUP
_ATTR_USBCONTROL_CODE_
static void Remote_WakeUpEnable(void)
{
	uint32 count;
	DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;
	USB_OTG_REG *  otg_core = (USB_OTG_REG * )USB_REG_BASE;

	otg_core->Device.dctl |= 0x01;
}

_ATTR_USBCONTROL_CODE_
static void Remote_WakeUpDisable(void)
{
	uint32 count;
	DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;
	USB_OTG_REG *  otg_core = (USB_OTG_REG * )USB_REG_BASE;

	otg_core->Device.dctl &= ~0x01;
}
#endif

#ifdef USB_SERIAL
/*
--------------------------------------------------------------------------------
Name:       _ATTR_USB_MSC_CODE_
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
--------------------------------------------------------------------------------
*/
_ATTR_USB_SRL_CODE_
static int32 FUSBSerialInit(void)
{
    return SrlInit((void*)0);
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : void Os_idle(void)
  Author        : ZHengYongzhi
  Description   : system enter idle

  Input         :

  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
extern UDC_DRIVER UDCDriver;
_ATTR_USBCONTROL_CODE_
void USBControlIdle(void)
{
    UINT32 KeyValue;
    UINT32 usb_remotewakeup = 0;

	DEBUG("Enter USBControlIdle...\n");

    //Enter Idle
	BL_Off();
    MPU6500_Write(MPU6500_PWR_MGMT_1,0x40);
    
    GpioMuxSet(GPIOPortC_Pin1,IOMUX_GPIOC1_IO); // PWR_ON_H
    Gpio_SetPinDirection(GPIOPortC_Pin1,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_LOW);

    GpioMuxSet(GPIOPortC_Pin7,IOMUX_GPIOC1_IO); // FCS2/SCL
    Gpio_SetPinDirection(GPIOPortC_Pin7,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin7, GPIO_LOW);

    GpioMuxSet(GPIOPortD_Pin0,IOMUX_GPIOC1_IO); // FCS3/SDA
    Gpio_SetPinDirection(GPIOPortD_Pin0,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortD_Pin0, GPIO_LOW);

    GpioMuxSet(GPIOPortB_Pin7,IOMUX_GPIOC1_IO); // FCS3/SDA
    Gpio_SetPinDirection(GPIOPortB_Pin7,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortB_Pin7, GPIO_LOW);       

    GpioMuxSet(GPIOPortC_Pin6,IOMUX_GPIOC1_IO); // Switch_RST_H
    Gpio_SetPinDirection(GPIOPortC_Pin6,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin6, GPIO_LOW);

    GpioMuxSet(GPIOPortC_Pin0,IOMUX_GPIOC1_IO); // HP_DET_H
    Gpio_SetPinDirection(GPIOPortC_Pin0,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin0, GPIO_LOW);

    GpioMuxSet(GPIOPortA_Pin0,IOMUX_GPIOC1_IO); // HP_HOOK
    Gpio_SetPinDirection(GPIOPortA_Pin0,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortA_Pin0, GPIO_LOW);

    gSysConfig.UsbSensor = 0;

	gSysConfig.SysClkBack = chip_freq.armclk;
	SysFreqSet(48);

	while(!GetMsg(MSG_USB_SUSPEND)) 
	{
		if(vr_nanoc_suspend == 0)
		{
			return;
		}
		DelayMs(3);
	}

	printf("wait for key event...\n");

    while(1)
    {
#ifdef REMOTE_WAKEUP	
        KeyValue =  GetKeyVal();

        switch (KeyValue)
        {
            case KEY_VAL_PLAY_LONG_UP:
    		case KEY_VAL_PLAY_SHORT_UP:
                printf ("power key up--->start remote wakeup...\n");
                usb_remotewakeup = 1;
                break;

            default:
                break;
        }
        if (usb_remotewakeup == 1)
        {
        	DelayMs(200);
            Remote_WakeUpEnable();
            DelayMs(10);
            Remote_WakeUpDisable();
			usb_remotewakeup = 0;
			KeyReset();
			break;
        }
#endif
		if (vr_nanoc_suspend == 0)
		{
			KeyReset();
			break;
		}

		if (CheckVbus() == 0)
        {
            DEBUG("VBUS Disconnected....");
            break;
        }

        if (CheckMsg(MSG_POWER_DOWN))
        {
            DEBUG("Low Power....");
            break;
        }
        DelayMs(10);
    }

    DEBUG("Exit USBControlIdle..\n");
}

/*
--------------------------------------------------------------------------------
  Function name :  void USBControlInit(void *pArg)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_USBCONTROL_CODE_
void USBControlInit(void *pArg)
{
    uint32 i;
    int32 ret;
    pFUN_USB_DEV   pDev = &FUSBDevTab[0];
    pFUNCTION_USB  pFUSB = &FUSBDevice;
    uint32 USBClassType = ((USB_WIN_ARG*)pArg)->FunSel;

    #ifdef DISPLAY_ENABLE
    //BLOffDisable();
    #endif

    //ScuLDODisable(); sch for enter usb mode,already Disable

    if (USBClassType & USB_CLASS_TYPE_MSC)
    {
        ModuleOverlay(MODULE_ID_USB_MSC, MODULE_OVERLAY_ALL);
    }
    if (USBClassType & USB_CLASS_TYPE_SERIAL)
    {
        ModuleOverlay(MODULE_ID_USB_SERIAL, MODULE_OVERLAY_ALL);
    }
    if (USBClassType & USB_CLASS_TYPE_AUDIO)
    {
        ModuleOverlay(MODULE_ID_USB_AUDIO, MODULE_OVERLAY_ALL);
    }


    //Notify Start
	#if 0
    {
        if (USBClassType & USB_CLASS_TYPE_MSC)
        {
            playVoiceNotify(VOICE_ID_USB_MSC_MODE,PALY_MODE_BLOCK, PALY_TIME_ONCE );
        }
        else if (USBClassType & USB_CLASS_TYPE_SERIAL)
        {
            playVoiceNotify(VOICE_ID_USB_AUDIO_MODE,PALY_MODE_BLOCK, PALY_TIME_ONCE );
        }
        else if (USBClassType & USB_CLASS_TYPE_AUDIO)
        {
            playVoiceNotify(VOICE_ID_USB_AUDIO_MODE, PALY_MODE_BLOCK, PALY_TIME_ONCE);
        }

    }
    #endif



    memset(pFUSB, 0, sizeof(FUNCTION_USB));
    pFUSB->SelClassType = USBClassType;

    USBDriverInit();


    // Install USB Device
    for(i=0; i<sizeof(FUSBDevTab)/sizeof(FUN_USB_DEV); i++, pDev++)
    {
        if (USBClassType & pDev->ClassType)
        {
            ret = pDev->FUInit();
            if (ret < 0)
            {
                //printf("FUInit err");
            }
            else
            {
                FUSBDevice.DevNum++;
            }
        }
        DEBUG("FUSBDevice.DevNum = %d", i);
    }

    USBDEBUG("Reconnect");
    if ((USBClassType & USB_CLASS_TYPE_AUDIO))
    {
        USBReConnect(1);
    }
    else
    {
        USBReConnect(0);
    }

    ret = USBWaitConnect(10000); //wait 2s
    if (ret == 0)
    {
        //..????
        USBDEBUG("USB connect error!");
        USBConnect(1);
        ret = USBWaitConnect(5000); //wait 2s
    }
    else
    {
        //..????,????
        USBDEBUG("Reconnect OK!");
    }

    //wait for Notyfy finish
    #if 0
    {
        while(FALSE == CheckAndStopVoiceNotify())
        {
            ;
        }
    }
    #endif

    #ifdef USB_PHY_TEST
    ret = USBWaitConnect(10000);
    if (ret)
    {
        DEVICE_REG *dev_regs = (DEVICE_REG *)USB_DEV_BASE;

        DEBUG("USB Test Start");

        Scu_USB_PowerPath_Enable(0);
        DelayMs(5);
        Scu_Charger_Enable_Set(0);

        dev_regs->dctl=(dev_regs->dctl & (~(0x07<<4))) | (((0x04)%8)<<4);
        DelayMs(100);

        while(1)
        {
            if (Scu_PlayOn_Level_Get() == 0)        //?Play???
    		{
    			break;
    		}
            DelayMs(100);
        }
        DEBUG("USB Test End");
    }
    #endif

    UsbIdleCounter = SysTickCounter;
    //ClearMsg(MSG_USB_DISCONNECT);
}

/*
--------------------------------------------------------------------------------
  Function name :   UINT32 USBControlService(void)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_USBCONTROL_CODE_
UINT32 USBControlService(void)
{
    int32 ret = 0;
    uint32 i;
    pFUN_USB_DEV  pDev = &FUSBDevTab[0];
    pFUNCTION_USB pFUSB = &FUSBDevice;

    if ((FALSE == CheckMsg(MSG_VBUS_INSERT)) || (CheckMsg(MSG_SYS_FW_UPGRADE)))
    {
        USBDEBUG("USB MODULE EXIT");
        SendMsg(MSG_NEED_BACKLIGHT_ON);
        SendMsg(MSG_NEED_PAINT_ALL);
        SendMsg(MSG_USB_EXIT_FUSB);
        return 0;
    }

#ifdef REMOTE_WAKEUP
    if(GetMsg(MSG_VR_SUSPEND) || GetMsg(MSG_USB_DISCONNECT))
    {
    	ClearMsg(MSG_VR_SUSPEND);
		ClearMsg(MSG_USB_DISCONNECT);
        if ((SysTickCounter - UsbIdleCounter) > 80 * 2)
        {
            USBControlIdle();
            UsbIdleCounter = SysTickCounter;
        }
    }

    if (GetMsg(MSG_USB_RESUMED))
    {
//        if (CheckVbus())
//        {
//            SendMsg(MSG_NEED_BACKLIGHT_ON);
//            SendMsg(MSG_NEED_PAINT_ALL);
//            BatteryChargeInit();
//        }
       USBDEBUG("MSG_USB_RESUMED");
    }
#else
    if(GetMsg(MSG_USB_SUSPEND))
    {
        SendMsg(MSG_USB_EXIT_FUSB);
        return 0;
    }
#endif

    for(i=0; i<sizeof(FUSBDevTab)/sizeof(FUN_USB_DEV); i++, pDev++)
    {
        if (pFUSB->SelClassType & pDev->ClassType)
        {
            if (!pDev->FUThead)
                continue;

            ret = pDev->FUThead();
            if (ret < 0)
            {
                printf("FUThead err");
            }
        }
    }

    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name :  void USBControlDeInit(void)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
extern UINT32 UsbAdapterProbeLdoBak;
_ATTR_USBCONTROL_CODE_
void USBControlDeInit(void)
{
    uint32 i;
    pFUN_USB_DEV pDev = &FUSBDevTab[0];
    pFUNCTION_USB pFUSB = &FUSBDevice;


    DISABLE_USB_INT;

    Scu_otgphy_suspend(1);

    ScuSoftResetCtr(RST_USBPHY, 1);
    ScuSoftResetCtr(RST_USBLGC, 1);
    ScuSoftResetCtr(RST_USBOTG, 1);

    ScuClockGateCtr(CLOCK_GATE_USBPHY, 0);
    ScuClockGateCtr(CLOCK_GATE_USBBUS, 0);

    IntUnregister(INT_ID23_USB);

    for(i=0; i<sizeof(FUSBDevTab)/sizeof(FUN_USB_DEV); i++, pDev++)
    {
        if (pFUSB->SelClassType & pDev->ClassType)
        {
            pDev->FUDeinit();
        }
    }

    FUSBDevice.DevNum = 0;

    //#ifdef DISPLAY_ENABLE
    #if 0
    BLOffEnable();
    #endif

    ScuLDOEnable();//UsbAdpterProbeStart -> ScuLdoDisable();
    ScuLDOSet(UsbAdapterProbeLdoBak);
    PMU_Enable(); //UsbAdpterProbeStart -> Pmu_Disable();
    PMU_ExitModule(PMU_USB);
}

#endif
/*
********************************************************************************
*
*                         End of USBControl.c
*
********************************************************************************
*/




