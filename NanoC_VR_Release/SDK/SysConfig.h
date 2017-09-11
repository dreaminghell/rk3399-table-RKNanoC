/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   Main.h
*
* Description:
*
* History:      <author>          <time>        <version>
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__

/*
*-------------------------------------------------------------------------------
*
*                            Debug Config
*
*-------------------------------------------------------------------------------
*/
//#define _JTAG_DEBUG_
#define _UART_DEBUG_

#ifdef _UART_DEBUG_
#define DEBUG(format,...)    printf("FILE: %s, FUNCTION: %s, LINE: %d: "format"\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define USBDEBUG(format,...) printf("\nFILE: %s, LINE: %d: "format, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(format,...)
#define USBDEBUG(format,...)
#endif

/*
*-------------------------------------------------------------------------------
*
*                            EVK Board Configuration
*
*-------------------------------------------------------------------------------
*/
#define RKNANOC_EVK_V20

/*
*-------------------------------------------------------------------------------
*
* Memory Device Option
* Select which memory device are used
*
*-------------------------------------------------------------------------------
*/
//SPI NOR Flash Support
#define _SPINOR_
#ifdef _SPINOR_
    #define FW_IN_DEV       2       //Firmware Stored in: 1:nandflash 2:sipnor 3:emmc 4:sd card
    //#define _SDCARD_                //SPI Flash must support SDCARD for user disk

    #define DISPLAY_ENABLE
#endif

/*
*-------------------------------------------------------------------------------
*
*                            Key Driver Configuration
*
*-------------------------------------------------------------------------------
*/
//Key Num configer
#define KEY_NUM_4		    4
#define KEY_NUM_5		    5
#define KEY_NUM_6		    6
#define KEY_NUM_7		    7
#define KEY_NUM_8		    8
#define KEY_NUM  	        KEY_NUM_7

//Key Val Define
#define KEY_VAL_NONE        ((UINT32)0x0000)
#define KEY_VAL_MASK        ((UINT32)0x0fffffff)
#define KEY_VAL_UNMASK      ((UINT32)0xf0000000)

#define KEY_VAL_PLAY        ((UINT32)0x0001 << 0)
#define KEY_VAL_MENU        ((UINT32)0x0001 << 1)
#define KEY_VAL_FFD         ((UINT32)0x0001 << 2)
#define KEY_VAL_FFW         ((UINT32)0x0001 << 3)
#define KEY_VAL_UP          ((UINT32)0x0001 << 4)
#define KEY_VAL_DOWN        ((UINT32)0x0001 << 5)
#define KEY_VAL_ESC         ((UINT32)0x0001 << 6)
#define KEY_VAL_NB         	((UINT32)0x0001 << 7)

#define KEY_VAL_VOL         KEY_VAL_ESC
#define KEY_VAL_UPGRADE     KEY_VAL_MENU
#define KEY_VAL_POWER       KEY_VAL_PLAY
#define KEY_VAL_HOLD        (KEY_VAL_MENU | KEY_VAL_PLAY)

//Bit position define for AD keys.
#define KEY_VAL_ADKEY2		KEY_VAL_MENU
#define KEY_VAL_ADKEY3		KEY_VAL_UP
#define KEY_VAL_ADKEY4      KEY_VAL_DOWN
#define KEY_VAL_ADKEY5      KEY_VAL_NB
#define KEY_VAL_ADKEY6      KEY_VAL_ESC
#define KEY_VAL_ADKEY7      KEY_VAL_PLAY

//AdKey Reference Voltage define
#define ADKEY2_MIN          ((0   +   0) / 2)
#define ADKEY2_MAX          ((0   + 113) / 2)

#define ADKEY3_MIN          ((113 +   0) / 2)
#define ADKEY3_MAX          ((113 + 250) / 2)

#define ADKEY4_MIN          ((250 + 113) / 2)
#define ADKEY4_MAX          ((250 + 395) / 2)

#define ADKEY5_MIN          ((395 + 250) / 2)
#define ADKEY5_MAX          ((395 + 590) / 2)

#define ADKEY6_MIN          ((590 + 395) / 2)
#define ADKEY6_MAX          ((590 + 725) / 2)

#define ADKEY7_MIN          ((725 + 590) / 2)
#define ADKEY7_MAX          ((725 + 1023) / 2)

/*
*-------------------------------------------------------------------------------
*
*                            GPIO Config
*
*-------------------------------------------------------------------------------
*/
#ifdef RKNANOC_EVK_V20
    #define GPIO_HOST_CTS       GPIOPortB_Pin7
    #define GPIO_HOST_RTS       GPIOPortC_Pin0

    #define BT_POWER            GPIOPortB_Pin3

    #define SD_DET              GPIOPortD_Pin2
    #define BACKLIGHT_PIN       GPIOPortD_Pin3
    #define VIRTUAL_UART_TX     GPIOPortB_Pin1

#endif

/*
*-------------------------------------------------------------------------------
*
*                            Battery and charge configer
*
*-------------------------------------------------------------------------------
*/
//degree for battery power.
#define BATTERY_LEVEL       5
#define BL_LEVEL_MAX        5

#define BL_PWM_RATE_MIN     10
#define BL_PWM_RATE_MAX     80
#define BL_PWM_RATE_STEP    ((BL_PWM_RATE_MAX - BL_PWM_RATE_MIN) / (BL_LEVEL_MAX))

/*
*-------------------------------------------------------------------------------
*
*                            Application Modules Option
*
*-------------------------------------------------------------------------------
*/
//configer Fusb302
#define _FUSB302_
#define _TOUCH_GSL1680_
//configer sensor
#define _MPU6500_

//configer TC3588xx
#define _TC358860XBG_
//#define _TC358870XBG_

//configer mipi lcd manufacturer
//#define _AUO_2P1_60FPS_LCD_
//#define _AUO_3P81_75FPS_LCD_
//#define _SHARP_2P89_75FPS_LCD_
#define _RAYKEN_5P46_60FPS_LCD_

//----------------------------------//
//	  AUO3.81 use _DP_MIPI_2LANE_		//
//----------------------------------//
#ifdef _AUO_3P81_75FPS_LCD_
#define _DP_MIPI_2LANE_
#else
#define _DP_MIPI_4LANE_
#endif

//configer EDID
//#define _EEPROM_EDID_
#define _GPIO_EDID_

//configer music player
#define _MUSIC_
#ifdef _MUSIC_
    #define _MUSIC_LRC_
    #define AUDIOHOLDONPLAY          //hold on play switch.
#endif


//-----------------------------------------------------------------------------
//configer USB function.
#define _USB_
#ifdef _USB_
    //#define USB_IF_TEST                 //USB test mode
	//#define _USBHOST_

    //#define USB_MSC
    #define USB_AUDIO
    #ifdef USB_AUDIO
        #define _USBAUDIO_DECODE_
    #endif
#endif




//-----------------------------------------------------------------------------
//configer battery charge win function.
#define _CHARGE_
#ifdef _CHARGE_

#endif

/*
********************************************************************************
*
*                         End of Include.h
*
********************************************************************************
*/

#endif

