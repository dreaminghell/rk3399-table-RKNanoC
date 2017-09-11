/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  SysService.c
*
* Description:
*
* History:      <author>          <time>        <version>
*             ZhengYongzhi      2008-9-13          1.0
*    desc:
********************************************************************************
*/
#define _IN_SYSSERVICE_

#include "SysInclude.h"
#include "UsbAdapterProbe.h"

#include "FsInclude.h"
#include "Mainmenu.h"
#include "AudioControl.h"
#include "PowerOn_Off.h"

#include "FunUSB.h"
#include "gslx680.h"

extern BOOLEAN VideoPause(void);

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : 系统服务程序，该程序调用系统消息、系统线程、外设检测等后台需要
                  完成的工作

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
uint32 UsbInsertAntiShake = 0;
uint32 UsbEjectAntiShake  = 0;
_ATTR_SYS_BSS_ uint32 USBDebounceCount;
_ATTR_SYS_BSS_ uint8  USBVbusStatus;
_ATTR_SYS_BSS_ uint8  VbusDetStatus;


_ATTR_SYS_CODE_
void USBStatusDetectReset(void)
{
    USBDebounceCount = 0;
    USBVbusStatus = 0;
    VbusDetStatus = 0;
    ClearMsg(MSG_USB_CONNECTED);
    ClearMsg(MSG_VBUS_INSERT);

    OTGReg->Device.dctl |= 0x02;
    Scu_otgphy_suspend(1);

    GRFReg->OTGPHY_CON0 = 1<<5 | 3<<29 | 3<<2;
    GRFReg->OTGPHY_CON1 = 0x00030000;   //选择为Dev
    USBResetPhy();
}

_ATTR_SYS_CODE_
uint32 USBStatusDetect(void)
{
    uint32 RetVal = 0;
    int32  UsbAdapterRet = 0;
    uint8  VbusStatus = 0;
    uint8  VbusChanged = 0;
    WIN   *pWin;
    TASK_ARG TaskArg;

    VbusStatus = CheckVbus();
    if (USBVbusStatus != VbusStatus)
    {
        USBVbusStatus = VbusStatus;
        USBDEBUG("VBUS Change: ", VbusStatus);
        VbusChanged = 1;
    }

    if(VbusChanged)
    {
        VbusDetStatus = 0;
        if (VbusStatus)
        {
            VbusDetStatus = 1;
        }
        USBDebounceCount = SysTickCounter;
    }
    else
    {
        if (VbusStatus)
        {
            if (1 == VbusDetStatus)
            {
                if ((SysTickCounter-USBDebounceCount) > 50) // 500ms 防抖 有溢出风险
                {
                    if (FALSE == CheckMsg(MSG_VBUS_INSERT))
                    {
                        VbusDetStatus = 2;

                        USBDEBUG("VBUS INSET");
                        SendMsg(MSG_VBUS_INSERT);

                        ThreadDeleteAll(&pMainThread);

                        AutoPowerOffDisable();
                        SetPowerOffTimerDisable();

                        ScuClockGateCtr(CLOCK_GATE_RTC, 1); //插入USB后要进行充电控制和检测，需要打开clk
                        SendMsg(MSG_CHARGE_ENABLE);
                        BatteryChargeInit();
						#ifdef _USB_
                        if(gSysConfig.UsbConfig.UsbMode != 2)   // 0: MSC; 1: USB Audio; 2: USB Host
                        {
                            UsbAdpterProbeStart();
                        }
						#endif
                    }
                }
            }
            else if (2 == VbusDetStatus)
            {
                UsbAdapterRet = UsbAdpterProbe();

                if (1 == UsbAdapterRet) // connnect ok
                {
                    VbusDetStatus = 0;

                    SendMsg(MSG_USB_CONNECTED);
					#ifdef _USB_
                    if (gSysConfig.UsbConfig.UsbMode == 0)          // 0: MSC; 1: USB Audio; 2: USB Host
                    {
                        TaskArg.Usb.FunSel = USB_CLASS_TYPE_MSC;
                    }
                    else// if (gSysConfig.UsbConfig.UsbMode == 1)     // 0: MSC; 1: USB Audio; 2: USB Host
                    {
                        TaskArg.Usb.FunSel = USB_CLASS_TYPE_AUDIO;
                    }
					#endif
                    if (GetMsg(MSG_MES_FIRMWAREUPGRADE))
                    {
                        TaskArg.Usb.FunSel = USB_CLASS_TYPE_UPGRADE;
                    }

                    TaskSwitch(TASK_ID_USB, &TaskArg);

                    SendMsg(MSG_CHARGE_ENABLE);
                    RetVal = RETURN_FAIL;

                    //charge...
                    SendMsg(MSG_CHARGE_START);
                }
                else if (2 == UsbAdapterRet) // connnect time out
                {
                    //TaskSwitch(TASK_ID_BT, (TASK_ARG*)NULL);
                    //RetVal = RETURN_FAIL;
                }
            }
        }
        else
        {
            //if ((SysTickCounter-USBDebounceCount) > 20) // 200ms 防抖 有溢出风险
            {
                if (CheckMsg(MSG_VBUS_INSERT))
                {
                    USBDEBUG("VBUS REMOVE");
                    ClearMsg(MSG_VBUS_INSERT);

                    UsbAdpterProbeStop();

                    AutoPowerOffEnable();
                    //SetPowerOffTimerEnable();

                    BatteryChargeDeInit();

                    ClearMsg(MSG_CHARGE_ENABLE);
                    ClearMsg(MSG_CHARGE_START);
                    ScuClockGateCtr(CLOCK_GATE_RTC, 1); //插入USB后要进行充电控制和检测，需要打开clk

                    //TaskSwitch(TASK_ID_BT, (TASK_ARG*)NULL);
                    //RetVal = RETURN_FAIL;
                }
            }
        }
    }

    return (RetVal);
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 USBHostDetect(void)
  Author        : ZHengYongzhi
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#ifdef _USBHOST_
_ATTR_SYS_CODE_
uint32 USBHostDetect(void)
{
    uint32 RetVal = 0;

    if (gSysConfig.UsbConfig.UsbMode == 2)  // 0: MSC; 1: USB Audio; 2: USB Host
    {

    }

    return (RetVal);
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : 系统服务程序，该程序调用系统消息、系统线程、外设检测等后台需要
                  完成的工作

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

#ifdef _SDCARD_
_ATTR_SYS_CODE_
void SDCardEnable(void)
{
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : 系统服务程序，该程序调用系统消息、系统线程、外设检测等后台需要
                  完成的工作

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SDCardDisable(void)
{
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : 系统服务程序，该程序调用系统消息、系统线程、外设检测等后台需要
                  完成的工作

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
uint32 SDCardStatusDetect(void)
{
    uint32 RetVal = 0;
    TASK_ARG TaskArg;

    //if (CheckCard() == 0)
    if(SDA_CheckCard(0) == 0)
    {
        if (FALSE == CheckMsg(MSG_SDCARD_EJECT))
        {
            DEBUG();
            SendMsg(MSG_SDCARD_EJECT);
            SendMsg(MSG_SDCARD_UPDATE);
            SendMsg(MSG_SDCARD_MEM_UPDATE);
            SendMsg(MSG_SYS_RESUME);
            PMU_EnterModule(PMU_BLON);

            #if 0
			#if (2 != FW_IN_DEV)
            if (gSysConfig.Memory == CARD)
			#endif
            {
                //CheckCard();

                #ifdef _RADIO_
                if (TRUE != ThreadCheck(pMainThread, &FMThread))
                #endif
                {
                    ThreadDeleteAll(&pMainThread);
                }

                #ifdef _MUSIC_
                if (Task.TaskID == TASK_ID_MUSIC)
                {
                    RetVal = RETURN_FAIL;
                    TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
                    TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
                }
        		#endif

				#ifdef _VIDEO_//ylz++
				if (Task.TaskID == TASK_ID_VIDEO)
				{
					RetVal = RETURN_FAIL;
					TaskArg.MainMenu.MenuID = MAINMENU_ID_VIDEO;
					TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
				}
				#endif

                #ifdef _RECORD_
                if (Task.TaskID == TASK_ID_RECORD)
                {
                    RetVal = RETURN_FAIL;
                    TaskArg.MainMenu.MenuID = MAINMENU_ID_RECORD;
                    TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
                }
                #endif

                #ifdef _EBOOK_
                if (Task.TaskID == TASK_ID_EBOOK)
                {
                    RetVal = RETURN_FAIL;
                    TaskArg.MainMenu.MenuID = MAINMENU_ID_EBOOK;
                    TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
                }
                #endif


                #ifdef _PICTURE_//ylz++
                if (Task.TaskID == TASK_ID_PICTURE)
                {
                    RetVal = RETURN_FAIL;
                    TaskArg.MainMenu.MenuID = MAINMENU_ID_PICTURE;
                    TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
                }
                #endif

				#if (FW_IN_DEV != 2)
                gSysConfig.Memory = FLASH0;
                FileSysSetup(gSysConfig.Memory);//dgl second filesystem
                #endif

			    //SDM_DeInit();//add by dgl
            }

            //SDCardDisable();

            #endif

        }
    }
    else
    {
        if (CheckMsg(MSG_SDCARD_EJECT))
        {
            ClearMsg(MSG_SDCARD_EJECT);
            SendMsg(MSG_SDCARD_UPDATE);
            SendMsg(MSG_SDCARD_MEM_UPDATE);
            SendMsg(MSG_SYS_RESUME);
            PMU_EnterModule(PMU_BLON);
        }
    }

    return (RetVal);
}
#endif

_ATTR_SYS_CODE_
void StartTP(void)
{
#if defined(_TOUCH_GSL1680_)
	/*gslx680 init**/
	PMU_EnterModule(PMU_USB);
	gslx680_init_chip();
	check_mem_data();
	PMU_ExitModule(PMU_USB);
#endif
}

uint32 RemoveDevStatusDetect(void)
{

    uint32 DevID;
    uint32 RetVal = 0;
    TASK_ARG TaskArg;
    DevID = GetMemDevID(gSysConfig.Memory);

    if(RETURN_DEV_REMOVE ==MDScanDev(0, 1 << MD_MAJOR(DevID)))
    {
        DEBUG("DevID = %d",DevID);

        #ifdef _RADIO_
        if (TRUE != ThreadCheck(pMainThread, &FMThread))
        #endif
        {
            ThreadDeleteAll(&pMainThread);
        }

        #ifdef _MUSIC_
        if (Task.TaskID == TASK_ID_MUSIC)
        {
            RetVal = RETURN_FAIL;
            TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
            TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        }
		#endif

		#ifdef _VIDEO_//ylz++
		if (Task.TaskID == TASK_ID_VIDEO)
		{
			RetVal = RETURN_FAIL;
			TaskArg.MainMenu.MenuID = MAINMENU_ID_VIDEO;
			TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
		}
		#endif

        #ifdef _RECORD_
        if (Task.TaskID == TASK_ID_RECORD)
        {
            RetVal = RETURN_FAIL;
            TaskArg.MainMenu.MenuID = MAINMENU_ID_RECORD;
            TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        }
        #endif

        #ifdef _EBOOK_
        if (Task.TaskID == TASK_ID_EBOOK)
        {
            RetVal = RETURN_FAIL;
            TaskArg.MainMenu.MenuID = MAINMENU_ID_EBOOK;
            TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        }
        #endif


        #ifdef _PICTURE_//ylz++
        if (Task.TaskID == TASK_ID_PICTURE)
        {
            RetVal = RETURN_FAIL;
            TaskArg.MainMenu.MenuID = MAINMENU_ID_PICTURE;
            TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        }
        #endif

		#if (FW_IN_DEV != 2)
        gSysConfig.Memory = FLASH0;
        FileSysSetup(gSysConfig.Memory);//dgl second filesystem
        #endif


    }
    if(RETURN_DEV_INSERT ==MDScanDev(0, 1 << MD_MAJOR(DevID)))
    {

    }

    return RetVal;
}

/*
--------------------------------------------------------------------------------
  Function name : AdcKey_SysTimer_Handler()
  Author        : anzhiguo
  Description   : ADC Key Timer Isr

  Input         : null

  Return        : null

  History:     <author>         <time>         <version>
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AutoPowerDownTimerISR(void)
{
    SendMsg(MSG_POWER_DOWN);
}

_ATTR_SYS_CODE_
void SetPowerDownTimerISR(void)
{
    SetPowerOffFlag = 0;
    SendMsg(MSG_POWER_DOWN);
}

_ATTR_SYS_DATA_
SYSTICK_LIST AutoPowerDownTimer =
{
    NULL,
    0,
    10 * 60 * 100,
    1,
    AutoPowerDownTimerISR,
};

_ATTR_SYS_DATA_
SYSTICK_LIST SetPowerDownTimer =
{
    NULL,
    0,
    0,
    1,
    SetPowerDownTimerISR,
};

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AutioPowerOffTimerRest(void)
{
    AutoPowerDownTimer.Counter = 0;
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AutoPowerOffEnable(void)
{
    if (AutoPowerOffDisableCounter > 0)
    {
        AutoPowerOffDisableCounter--;

        if (AutoPowerOffDisableCounter == 0)
        {
            SystickTimerStart(&AutoPowerDownTimer);
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AutoPowerOffDisable(void)
{
    if (AutoPowerOffDisableCounter == 0)
    {
        SystickTimerStop(&AutoPowerDownTimer);
    }
    AutoPowerOffDisableCounter++;
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SetPowerOffTimerEnable(void)
{
    uint32 PowerOffBuff[6] = {0, 10*60*100, 15*60*100, 30*60*100, 60*60*100, 120*60*100};

    if (gSysConfig.ShutTime)
    {
        SetPowerDownTimer.Counter = 0;
        SetPowerDownTimer.Reload = PowerOffBuff[gSysConfig.ShutTime];

        if (SetPowerOffFlag == 0)
        {
            SetPowerOffFlag = 1;
            SystickTimerStart(&SetPowerDownTimer);
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SetPowerOffTimerDisable(void)
{
    if (SetPowerOffFlag == 1)
    {
        SetPowerOffFlag = 0;
        SystickTimerStop(&SetPowerDownTimer);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
uint32 PowerOffDetec(void)
{
    uint32 RetVal = RETURN_OK;
    TASK_ARG TaskArg;

    //--------------------------------------------------------------------------
    if (GetMsg(MSG_POWER_DOWN))
    {
        #ifdef _MUSIC_
        if (TRUE == ThreadCheck(pMainThread, &MusicThread))
        {
            gSysConfig.MusicConfig.HoldOnPlaySaveFlag = 1;
        }
        else
        {
            gSysConfig.MusicConfig.HoldOnPlaySaveFlag = HoldOnPlayInfo.HoldMusicGetSign ;
        }
        #endif


        ThreadDeleteAll(&pMainThread);

        if ((CheckVbus()) && (0 == Scu_DCout_Issue_State_Get()))
        {
            //TaskArg.Usb.FunSel = FUN_USB_CHARGE;
            TaskSwitch(TASK_ID_CHARGE, &TaskArg);
            RetVal = RETURN_FAIL;
        }
        else
        {

            SaveSysInformation();

            #if (NAND_DRIVER == 1)
            FtlClose();
            #endif

            PowerOff();
        }
    }

    return(RetVal);
}

//#ifdef DISPLAY_ENABLE
#if 0

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SysResume(void)
{
    if(TRUE == GetMsg(MSG_SYS_RESUME))
    {
        //KeyReset();
        BL_Resume();
        AutioPowerOffTimerRest();
    }
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : void SysBatteryStateChk(void)

  Author        : yangwenjie
  Description   : shutdown check

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             yangwenjie     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SysBatteryStateCheck(void)
{
    WIN    * PowerOffWin;

    if (CheckAdcState(ADC_CHANEL_BATTERY))
    {
        Battery_GetLevel();

        //low voltage check,
        if(TRUE == GetMsg(MSG_LOW_POWER))
        {
            SendMsg(MSG_SYS_RESUME);

			#ifdef _VIDEO_//ylz++
			if (TRUE == ThreadCheck(pMainThread, &VideoThread))
			{
				VideoPause();
			}
			#endif

            PowerOffWin = WinGetFocus(pMainWin);
            WinCreat(PowerOffWin, &LowPowerWin,NULL);
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : 系统服务程序，该程序调用系统消息、系统线程、外设检测等后台需要
                  完成的工作

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysServiceInit(void)
{
    //DEBUG("system service initial...");

    //#ifdef DISPLAY_ENABLE
    #if 0
    LcdStandbyEnable();
    BLOffEnable();
    #endif

    AutoPowerOffDisableCounter = 1;
    AutoPowerOffEnable();

    SetPowerOffTimerEnable();
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : system service progarm,this program will call system backgroud message,system thread
                  and devices check etc to finish work.,

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
UINT32 SysService(void)
{
    uint32 retval;

    watchdog_reset();

    //#ifdef DISPLAY_ENABLE
    #if 0
    BacklightDetec();
    #endif

    #ifdef _SDCARD_
    if (gSysConfig.SDEnable)
    {
        if (RETURN_FAIL == SDCardStatusDetect())
        {
            return(RETURN_FAIL);
        }
    }
    #endif

	#ifdef _TOUCH_GSL1680_
	if(gSysConfig.StartTP == 1)
	{
		GSLX680_Read();
	}
	#endif

    #ifdef _USBHOST_
    if (RETURN_FAIL == USBHostDetect())
    {
        return(RETURN_FAIL);
    }
    #endif

    if (RETURN_FAIL == USBStatusDetect())
    {
        return(RETURN_FAIL);
    }

    //if (RETURN_FAIL == PowerOffDetec())
    //{
    //    return(RETURN_FAIL);
    //}
    if (RETURN_FAIL == RemoveDevStatusDetect())
    {
        return(RETURN_FAIL);
    }

    //VoiceNotifyService();

    SysBatteryStateCheck();

    //#ifdef DISPLAY_ENABLE
    #if 0
    SysResume();
    #endif

    return(RETURN_OK);
}

/*
********************************************************************************
*
*                         End of SysService.c
*
********************************************************************************
*/

