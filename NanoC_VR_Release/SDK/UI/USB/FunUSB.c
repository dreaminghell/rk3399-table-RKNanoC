/*
********************************************************************************
*                   Copyright (c) 2008,CHENFEN
*                         All rights reserved.
*
* File Name：  FunUSB.c
*
* Description:
*
* History:      <author>          <time>        <version>
*             chenfen          2008-3-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_FUNUSB_

#include "SysInclude.h"
#include "FsInclude.h"
#include "AudioControl.h"
//#include "MusicWinInterface.h"
#include "MainMenu.h"
#include "USBHidRkvrData.h"
#include "MPU6500.h"
#ifdef _USB_

#include "FunUSB.h"
#include "USBControl.h"
#include "USBConfig.h"


#ifdef USB_AUDIO
#include "AudioControl.h"
#define SPECTRUM_LINE_M      10     // 10跟频谱线
#define USBAUDIO_VOL_MAX  MAX_VOLUME
extern char  SpectrumOut[SPECTRUM_LINE_M];


_ATTR_USB_UI_BSS_    UINT32              UsbAudioCurrentTime200MsBk;


/*
--------------------------------------------------------------------------------
  Function name : void MainMenuDisplay(void)
  Author        : ZHengYongzhi
  Description   : main menu display handle program

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

_ATTR_USB_UI_CODE_
void USBAudioDisplay(void)
{
//#ifdef DISPLAY_ENABLE
#if 0
    UINT8                   j, k,l;
    UINT16                  i, TempColor, TempBkColor, TempCharSize,TempTxtMode;
    UINT16 		            y, y1;
    UINT16                  *p2Unicode;
    UINT32                  CurrentTimeSec, TotalTimeSec;
    LCD_RECT                r;
    PICTURE_INFO_STRUCT     PicInfo;
    PicturePartInfo         PicPartInfo;
    char 					SpectrumOutTemp[SPECTRUM_LINE_M];//the size of Buffer is defined by real situation

    TempColor    = LCD_GetColor();
    TempBkColor  = LCD_GetBkColor();
    TempTxtMode  = LCD_SetTextMode(LCD_DRAWMODE_REV);
    TempCharSize = LCD_SetCharSize(FONT_12x12);
    LCD_SetColor(COLOR_BLACK);
    LCD_SetBkColor(COLOR_WHITE);

    if (CheckMsg(MSG_NEED_PAINT_ALL) || (GetMsg(MSG_USB_DISPLAY_ALL)))
    {
        //display backgroud picture,battery and hold icon
        DispPictureWithIDNum(IMG_ID_USBAUDIO_BACKGROUND);
        DispPictureWithIDNum(IMG_ID_MUSIC_BATTERYLEVEL0 + gBattery.Batt_Level);
        DispPictureWithIDNum(IMG_ID_MUSIC_FILETYPE_WAV);

        //if(TRUE==GetMsg(MSG_MUSIC_DISPFLAG_TOTAL_TIME))
        {
            CurrentTimeSec = 0;
            CurrentTimeSec = CurrentTimeSec / 1000;//turn ms to s,
            GetTimeHMS(CurrentTimeSec,&i,&j,&k);

            GetPictureInfoWithIDNum(IMG_ID_MUSIC_TOTLETIMENUM0, &PicInfo);
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + i % 10), PicInfo.xSize * 0, 0);    //hour
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_TOTLETIMENUMCOLON,        PicInfo.xSize * 1, 0);    //sperator
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + j / 10), PicInfo.xSize * 2, 0);    //minute
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + j % 10), PicInfo.xSize * 3, 0);
            DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_TOTLETIMENUMCOLON,        PicInfo.xSize * 4, 0);    //sperator
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + k / 10), PicInfo.xSize * 5, 0);    //second
            DispPictureWithIDNumAndXYoffset((IMG_ID_MUSIC_TOTLETIMENUM0 + k % 10), PicInfo.xSize * 6, 0);
        }

        //if (TRUE == GetMsg(MSG_MUSIC_DISPFLAG_FILENAME))
        {
            uint16 tempbuf[] = {' ','U','S','B',' ','A','u','d','i','o','.','.','.',0};
            GetPictureInfoWithIDNum(IMG_ID_MUSIC_FILENAMEBK, &PicInfo);
            r.x0 = PicInfo.x;
            r.y0 = PicInfo.y + (PicInfo.ySize - CH_CHAR_XSIZE_12) / 2;
            r.x1 = r.x0 + PicInfo.xSize;
            r.y1 = r.y0 + CH_CHAR_XSIZE_12 - 1 ;
            DispPictureWithIDNum(IMG_ID_MUSIC_FILENAMEBK);
            LCD_DispStringInRect(&r, &r, tempbuf, LCD_TEXTALIGN_LEFT);
        }

        //if(TRUE == GetMsg(MSG_MUSIC_DISPFLAG_BITRATE))
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_BPSICON);

            i = (1536000 /1000) /1000;
            j = (1536000 /1000) % 1000 / 100;
            k = (1536000 /1000) % 100 / 10;
            l = (1536000 /1000) % 10;

            GetPictureInfoWithIDNum(IMG_ID_MUSIC_BPSNUM0, &PicInfo);
            if(0 == i)
            {
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSBACKGROND, PicInfo.xSize * 0, 0);
            }
            else
            {
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSNUM0 + i, PicInfo.xSize * 0, 0);
            }
            if((0 == i) && (0 == j))
            {
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSBACKGROND, PicInfo.xSize * 1, 0);
            }
            else
            {
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSNUM0 + j, PicInfo.xSize * 1, 0);
            }
            if((0 == i) && (0 == j) && (0 == k))
            {
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSBACKGROND, PicInfo.xSize * 2, 0);
            }
            else
            {
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSNUM0 + k, PicInfo.xSize * 2, 0);
            }
            if((0 == i) && (0 == j) && (0 == k) && (0 == l))
            {
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSBACKGROND, PicInfo.xSize * 3, 0);
            }
            else
            {
                DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_BPSNUM0 + l, PicInfo.xSize * 3, 0);
            }
        }

        //the other diaplay element message display in here,
        SendMsg(MSG_USB_DISPFLAG_STATUS);
        SendMsg(MSG_USB_DISPFLAG_VOL);
        SendMsg(MSG_USB_DISPFLAG_EQ);
        SendMsg(MSG_USB_DISPFLAG_SPECTRUM);
        SendMsg(MSG_USB_DISPFLAG_SAMPLERATE);
    }

    if(TRUE == GetMsg(MSG_BATTERY_UPDATE))
    {
        DispPictureWithIDNum(IMG_ID_MUSIC_BATTERYLEVEL0 + gBattery.Batt_Level);
    }

    if (TRUE == GetMsg(MSG_USB_DISPFLAG_STATUS))
    {
        if(AUDIO_STATE_PLAY == MusicPlayState)
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_STATUS_PLAY);
        }
        else
        {
            DispPictureWithIDNum(IMG_ID_MUSIC_STATUS_PAUSE);
        }
    }

    if(TRUE == GetMsg(MSG_USB_DISPFLAG_SAMPLERATE))
    {

    }

    if (TRUE==GetMsg(MSG_USB_DISPFLAG_VOL))
    {
        DispPictureWithIDNum(IMG_ID_MUSIC_VOLBACKGROUND);

        GetPictureInfoWithIDNum(IMG_ID_MUSIC_VOLNUM0, &PicInfo);
        DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_VOLNUM0 + gSysConfig.OutputVolume / 10, (PicInfo.xSize * 0), 0);
        DispPictureWithIDNumAndXYoffset(IMG_ID_MUSIC_VOLNUM0 + gSysConfig.OutputVolume % 10, (PicInfo.xSize * 1), 0);
    }

    if (TRUE == GetMsg(MSG_USB_DISPFLAG_EQ))
    {
        DispPictureWithIDNum(IMG_ID_MUSIC_EQ_NOR + gSysConfig.MusicConfig.Eq.Mode);
    }

    if(TRUE == GetMsg(MSG_USB_DISPFLAG_SPECTRUM))
    {
        DispPictureWithIDNum(IMG_ID_MUSIC_SPECBACKGROUND);//draw sepectrum basic.

        GetPictureInfoWithIDNum(IMG_ID_MUSIC_SPEC, &PicInfo);
        //do insert value in here
        for (i = 0; i < SPECTRUM_LINE_M ; i++)
        {
            SpectrumOutTemp[i] = i * 2;
        }

        for (i = 0;i < 9;i++)
        {
            y1 = (SpectrumOutTemp[i] >> 1);//real pectrum height.
			if (y1 > 20)
			{
				y1 = 20;
			}
			y = 20 - y1;

 		    DisplayPicture_part(IMG_ID_MUSIC_SPEC, PicInfo.x + i * (PicInfo.xSize + 2)+3, PicInfo.y, y, y1);
        }
    }

    if(TRUE == GetMsg(MUSIC_UPDATESPECTRUM))
    {
        #ifdef _RK_SPECTRUM_
        CodecGetTime(&CurrentTimeSec);
        CurrentTimeSec = CurrentTimeSec / 200;    //refresh spectrum every 200ms

        if (CurrentTimeSec != UsbAudioCurrentTime200MsBk)
        {
            UsbAudioCurrentTime200MsBk = CurrentTimeSec;

            DispPictureWithIDNum(IMG_ID_MUSIC_SPECBACKGROUND);//draw sepectrum basic.

            GetPictureInfoWithIDNum(IMG_ID_MUSIC_SPEC, &PicInfo);
            //do insert value in here
            for (i = 0; i < SPECTRUM_LINE_M ; i++)
            {
                SpectrumOutTemp[i] = SpectrumOut[i];
            }

            for (i = 0;i < 9;i++)
            {
                y1 = (SpectrumOutTemp[i] >> 1);//real pectrum height.
    			if (y1 > 20)
    			{
    				y1 = 20;
    			}
    			y = 20 - y1;

     		    DisplayPicture_part(IMG_ID_MUSIC_SPEC, PicInfo.x + i * (PicInfo.xSize + 2)+3, PicInfo.y, y, y1);
            }
        }
        #endif
    }

    LCD_SetTextMode(TempTxtMode);
    LCD_SetBkColor(TempBkColor);
    LCD_SetColor(TempColor);
    LCD_SetCharSize(TempCharSize);
#else

#endif
}

#endif


/*
--------------------------------------------------------------------------------
  Function name : void FunUSBInit(void)
  Author        : zhuzhe
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
               chenfen       2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_UI_CODE_
void FunUSBInit(void *pArg)
{
    TASK_ARG TaskArg;
    if (((USB_WIN_ARG*)pArg)->FunSel == USB_CLASS_TYPE_UPGRADE)
    {
        SendMsg(MSG_MES_FIRMWAREUPGRADE);
        return;
    }

    //memset((uint8 *)&usbpcb, 0, sizeof(struct _usbPcb));

    if(((USB_WIN_ARG*)pArg)->FunSel != USB_CLASS_TYPE_HOST)
    {
        //#ifdef DISPLAY_ENABLE
        #if 0
        if( ((USB_WIN_ARG*)pArg)->FunSel &  (USB_CLASS_TYPE_MSC | USB_CLASS_TYPE_MTP))
        {
             DispPictureWithIDNum(IMG_ID_USB_BACKGROUND);

             #ifdef _SPINOR_
             ResourFilesClose();
             #endif
        }
        #endif

        //FunUSBSetMode(USB_MODE_DEV);
        if(GetMsg(MSG_USB_CONNECTED))
        {
            DEBUG("CONNECTED");
            //usbpcb.usbConnected = 1;

            TaskArg.Usb.FunSel = ((USB_WIN_ARG*)pArg)->FunSel;
            if (TRUE != ThreadCheck(pMainThread, &USBControlThread))
            {
                ModuleOverlay(MODULE_ID_USBCONTROL, MODULE_OVERLAY_ALL);
                ThreadCreat(&pMainThread, &USBControlThread, &TaskArg);
            }
        }
    }

    DEBUG("THE FUN USB INIT");
    SendMsg(MSG_USB_DISPLAY_ALL);
    SetPowerOffTimerDisable();
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuDeInit(void)
  Author        : zhuzhe
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_UI_CODE_
void FunUSBDeInit(void)
{
    pFUNCTION_USB pFUSB = &FUSBDevice;

    SetPowerOffTimerDisable();
    if (TRUE == ThreadCheck(pMainThread, &MusicThread))
    {
        ThreadDelete(&pMainThread, &MusicThread);
    }

    //#ifdef DISPLAY_ENABLE
    #if 0
    if( pFUSB->SelClassType &  (USB_CLASS_TYPE_MSC | USB_CLASS_TYPE_MTP))
    {
        #ifdef _SPINOR_
         ResourFilesOpen();
        #endif
    }
    #endif

    DEBUG("Exit");
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 FunUSBService(void)
  Author        : zhuzhe
  Description   : it is USB service handle that is used to handle window message,window service task etc.

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_UI_CODE_
UINT32 FunUSBService(void)
{
    int force_calib = 0;
    uint32  RetVal;
    AUDIO_THREAD_ARG AudioArg;
    TASK_ARG TaskArg;

    RetVal = RETURN_OK;

    if (GetMsg(MSG_MES_FIRMWAREUPGRADE))
    {
        DEBUG("Firmware Upgrade...");
        SysReboot(0x0000008c, 1);
    }

    if (GetMsg(MSG_ENTER_LOADER_USB))
    {
        SysReboot(0x00000000, 3);
    }

    if (GetMsg(MSG_SYS_REBOOT))
    {
        DEBUG("System Reboot...");
        SysReboot(0x00000000, 1);
    }

    if(GetMsg(MSG_USB_EXIT_FUSB))
    {
        ThreadDeleteAll(&pMainThread);
//        TaskSwitch(TASK_ID_BT, (TASK_ARG*)NULL);
//        RetVal = RETURN_FAIL;


        TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        RetVal = 1;
    }

#if 0
    if (GetMsg(MSG_VR_SUSPEND))
    {
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
        //AutoPowerOffDisableCounter = 0;
		//gSysConfig.SysClkBack = chip_freq.armclk;
		//SysFreqSet(48);
		
    }
#endif

    if (GetMsg(MSG_VR_RESUME))
    {
    	ClearMsg(MSG_VR_RESUME);
	    ScuDCoutSet(SCU_DCOUT_120);
	    SysFreqSet(gSysConfig.SysClkBack);

		GpioMuxSet(GPIOPortB_Pin7,IOMUX_GPIOB7_IO);
		Gpio_SetPinDirection(GPIOPortB_Pin7,GPIO_IN);
		GPIO_SetPinPull(GPIOPortB_Pin7,ENABLE);

		GpioMuxSet(GPIOPortC_Pin7,IOMUX_GPIOC7_I2C_SCL);
		GpioMuxSet(GPIOPortD_Pin0,IOMUX_GPIOD0_I2C_SDA);

		GpioMuxSet(GPIOPortC_Pin0,IOMUX_GPIOC1_IO); // HP_DET_H
		Gpio_SetPinDirection(GPIOPortC_Pin0,GPIO_OUT);
		Gpio_SetPinLevel(GPIOPortC_Pin0, GPIO_HIGH);

		GpioMuxSet(GPIOPortA_Pin0,IOMUX_GPIOC1_IO); // HP_HOOK
		Gpio_SetPinDirection(GPIOPortA_Pin0,GPIO_OUT);
		Gpio_SetPinLevel(GPIOPortA_Pin0, GPIO_HIGH);
		                                   
		GpioMuxSet(GPIOPortC_Pin1,IOMUX_GPIOC1_IO);
		Gpio_SetPinDirection(GPIOPortC_Pin1,GPIO_OUT);
		Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_HIGH);

		GpioMuxSet(GPIOPortD_Pin2,IOMUX_GPIOD3_IO);
		Gpio_SetPinDirection(GPIOPortD_Pin2 , GPIO_OUT);
		Gpio_SetPinLevel(GPIOPortD_Pin2, GPIO_HIGH);
		                            
#ifdef _TC358860XBG_
		PMU_EnterModule(PMU_USB);
		Tc358860XBG_init();
		LCD_BL_On();
		PMU_ExitModule(PMU_USB);
#endif

#ifdef _MPU6500_                                      	
		PMU_EnterModule(PMU_USB);        	
		MPU6500_Init(0);	//MPU6500(accel and gyro sensor) init
		PMU_ExitModule(PMU_USB); 
		gSysConfig.UsbSensor = 1;
#endif

#ifdef _TOUCH_GSL1680_
		StartTP();
#endif

		//gSysConfig.StartTP = 0;

    }

    return (RetVal);
}


/*
--------------------------------------------------------------------------------
  Function name : UINT32 FunUSBKey(void)
  Author        : ZHengYongzhi
  Description   : monitor the key handle of charge status

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
extern FUN_USB_DEV FUSBDevTab[];

/*touch keys **/
#define KEY_UP_MASK_BIT		1
#define KEY_DOWN_MASK_BIT	2
#define KEY_LEFT_MASK_BIT	4
#define KEY_RIGHT_MASK_BIT	8
#define KEY_ENTER_MASK_BIT	16
extern uint8 key_map;
extern int vr_nanoc_suspend;

_ATTR_USB_UI_CODE_
void HID_ReportData()
{
    int16 accel_data[3] = {0};
    int16 gyro_data[3] ={0};
	int16 itemperature;
	uint8 temperature[2] ={0};
	float tempreature;
    UINT32  TempKeyVal;

	RKVR_DATA_UN rkvr_data_un;

	if(vr_nanoc_suspend)
	{
	    return;
	}

	if(USBWriter_IsBusy(62)) {
		return;
	}

	memset(rkvr_data_un.buf, 0, sizeof(rkvr_data_un.buf));

    if (gSysConfig.UsbSensor == 1)
    {
        //获取Sensor 数据
        MPU6500_Read_Seq(accel_data,gyro_data,&itemperature);
		temperature[0] = itemperature&&0xff;
		temperature[1] =(itemperature >> 8 )&& 0xff;
		sensor_data_fill(&(rkvr_data_un.rkvr_data.sensor_data),accel_data,gyro_data,temperature);
    }

#ifdef _TOUCH_GSL1680_
	if(gSysConfig.StartTP == 1) {
		if(key_map&KEY_UP_MASK_BIT) {
			rkvr_data_un.rkvr_data.key_map.key_up_pressed = 1;
			key_map &= ~KEY_UP_MASK_BIT;
			printf("menu UP down\n");
			USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			return;
		}
		if(key_map&KEY_DOWN_MASK_BIT) {
			rkvr_data_un.rkvr_data.key_map.key_down_pressed = 1;
			key_map &= ~KEY_DOWN_MASK_BIT;
			printf("menu DOWN down\n");
			USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			return;
		}
		if(key_map&KEY_LEFT_MASK_BIT) {
			rkvr_data_un.rkvr_data.key_map.key_left_pressed = 1;
			key_map &= ~KEY_LEFT_MASK_BIT;
			printf("menu LEFT down\n");
			USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			return;
		}
		if(key_map&KEY_RIGHT_MASK_BIT) {
			rkvr_data_un.rkvr_data.key_map.key_right_pressed = 1;
			key_map &= ~KEY_RIGHT_MASK_BIT;
			printf("menu RIGHT down\n");
			USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			return;
		}
		if(key_map&KEY_ENTER_MASK_BIT) {
			rkvr_data_un.rkvr_data.key_map.key_enter_pressed = 1;
			key_map &= ~KEY_ENTER_MASK_BIT;
			printf("menu ENTER down\n");
			USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			return;
		}
	}
#endif

    TempKeyVal =  GetKeyVal();
    switch (TempKeyVal)
    {
    	case KEY_VAL_NB_DOWN:
			{
				printf("NB key down\n");
				rkvr_data_un.rkvr_data.key_map.key_home_down = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			}
			break;

		case KEY_VAL_NB_SHORT_UP:
        case KEY_VAL_NB_LONG_UP: 
			{
	            printf ("NB key up\n");
				rkvr_data_un.rkvr_data.key_map.key_home_up = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
        	}
            break;

		case KEY_VAL_UP_DOWN:
			{
				printf("volup key down\n");
				rkvr_data_un.rkvr_data.key_map.key_volup_down = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			}
			break;

        case KEY_VAL_UP_LONG_UP:     //volume increse
        case KEY_VAL_UP_SHORT_UP:
			{
	            printf ("volup key up\n");
				rkvr_data_un.rkvr_data.key_map.key_volup_up = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
        	}
            break;

		case KEY_VAL_DOWN_DOWN:
			{
				printf("voldn key down\n");
				rkvr_data_un.rkvr_data.key_map.key_voldn_down = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			}
			break;

		case KEY_VAL_DOWN_LONG_UP:
        case KEY_VAL_DOWN_SHORT_UP:
			{
	            printf ("voldn key up\n");
				rkvr_data_un.rkvr_data.key_map.key_voldn_up = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
        	}
            break;

		case KEY_VAL_ESC_DOWN:
			{
				printf("esc key down\n");
				rkvr_data_un.rkvr_data.key_map.key_esc_down = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			}
			break;

		case KEY_VAL_ESC_LONG_UP:
        case KEY_VAL_ESC_SHORT_UP:
			{
				printf("esc key up\n");
				rkvr_data_un.rkvr_data.key_map.key_esc_up = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			}
			break;

		case KEY_VAL_PLAY_DOWN:
			{
				printf("power key down\n");
				rkvr_data_un.rkvr_data.key_map.key_power_down = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
			}
			break;

		case KEY_VAL_PLAY_LONG_UP:
        case KEY_VAL_PLAY_SHORT_UP:
			{
	            printf ("power key up\n");
				rkvr_data_un.rkvr_data.key_map.key_power_up = 1;
				rkvr_data_un.rkvr_data.key_map.key_pressed = 1;
				USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf);
        	}
            break;
        default:
            {
				if (gSysConfig.UsbSensor == 1)
			    {
			    	rkvr_data_un.rkvr_data.key_map.key_pressed = 0;
			        USBWriteEp(HID_IN_EP_SENSOR ,62, rkvr_data_un.buf); //HID_IN_EP_SENSOR
			    }
        	}
            break;
    }

}


_ATTR_USB_UI_CODE_
UINT32 FunUSBKey(void)
{
    uint32  RetVal = RETURN_OK;
    HID_ReportData();
    return (RetVal);
#if 0
    uint32  i;
    TASK_ARG TaskArg;
    uint32  RetVal = RETURN_OK;
    UINT32  TempKeyVal;
    pFUNCTION_USB pFUSB = &FUSBDevice;
    //printf ("Get USB key\n");
    TempKeyVal =  GetKeyVal();


    switch (TempKeyVal)
    {
        case KEY_VAL_PLAY_SHORT_UP:
            break;
        case KEY_VAL_UP_SHORT_UP:
            break;

        case KEY_VAL_UP_PRESS:     //volume increse
            break;

        case KEY_VAL_DOWN_SHORT_UP:                                 //volume reduce
            break;

        case KEY_VAL_DOWN_PRESS:
            break;

        case KEY_VAL_ESC_SHORT_UP:
            #if 0
            ThreadDeleteAll(&pMainThread);
            TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
            TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
            RetVal = 1;
            return (RetVal);
            #endif
        break;

        default:
            break;
    }


    if ((pFUSB->SelClassType & USB_CLASS_TYPE_AUDIO))
    {
        //printf ("Get USB key 1\n");
        RetVal = UsbAudioHidControl(TempKeyVal);
    }

    return (RetVal);
#endif
}

/*
--------------------------------------------------------------------------------
  Function name : void FunUSBDisplay(void)
  Author        : ZHengYongzhi
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_UI_CODE_
void FunUSBDisplay(void)
{
//#ifdef DISPLAY_ENABLE
#if 0
    pFUNCTION_USB pFUSB = &FUSBDevice;
    #ifdef USB_AUDIO
    if (pFUSB->SelClassType & USB_CLASS_TYPE_AUDIO)
    {
        USBAudioDisplay();
    }
    else
    #endif
    if (pFUSB->SelClassType &  (USB_CLASS_TYPE_MSC | USB_CLASS_TYPE_MTP))
    {
        if (GetMsg(MSG_USB_DISPLAY_ALL))
        {
            //DispPictureWithIDNum(IMG_ID_USB_BACKGROUND);
        }
    }
#else

#endif
}

/*
********************************************************************************
*
*                         End of MainMenu.c
*
********************************************************************************
*/
#endif

