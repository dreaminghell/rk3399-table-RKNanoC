/*
********************************************************************************
*                   Copyright (c) 2008,Huweiguo
*                         All rights reserved.
*
* File Name£º  Backlight.c
*
* Description:
*
*
* History:      <author>          <time>        <version>
*               yangwenjie        2009-01-19         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_BACKLIGHT_

#include "SysInclude.h"

/*
--------------------------------------------------------------------------------
  Function name : void BL_SetBright(BL_LIGHT_en_t light)
  Author        : yangwenjie
  Description   : setting backlight

  Input         :

  Return        :

  History:     <author>         <time>         <version>
              yangwejie    2009-1-5        Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BL_SetLevel(uint8 Level)
{
    uint8 TmpPwmRate;

    if (Level <= BL_LEVEL_MAX)
    {
        TmpPwmRate = BL_PWM_RATE_MIN + BL_PWM_RATE_STEP * Level;
    }
    else
    {
        TmpPwmRate = BL_PWM_RATE_MIN / 2;
    }
	PwmRateSet(BL_PWM_CH,TmpPwmRate,PWM_FREQ);
}


/*
--------------------------------------------------------------------------------
  Function name : void BL_DeInit(void)
  Author        : yangwenjie
  Description   :

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void LCD_BL_On(void)
{
#ifdef _SHARP_2P89_75FPS_LCD_
	GpioMuxSet(GPIOPortB_Pin3,IOMUX_GPIOB3_IO);
	Gpio_SetPinDirection(GPIOPortB_Pin3,GPIO_OUT);
	Gpio_SetPinLevel(GPIOPortB_Pin3, GPIO_HIGH);
#endif
	ScuLDOSet(BacklightLDOBackup);
    PMU_EnterModule(PMU_BLON);
    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD3_PWM2);

    PWM_Start(BL_PWM_CH);
    BL_SetLevel(1); // 3 gSysConfig.BLevel
    printf("Backlight on\n");
}


/*
--------------------------------------------------------------------------------
  Function name : void BL_DeInit(void)
  Author        : yangwenjie
  Description   : close backlight

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BL_Off(void)
{
    PWM_Stop(BL_PWM_CH);

    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD3_IO);
	Gpio_SetPinDirection(BACKLIGHT_PIN , GPIO_OUT);
	Gpio_SetPinLevel(BACKLIGHT_PIN, GPIO_LOW);

       GpioMuxSet(GPIOPortD_Pin2,IOMUX_GPIOD3_IO);
	Gpio_SetPinDirection(GPIOPortD_Pin2 , GPIO_OUT);
	Gpio_SetPinLevel(GPIOPortD_Pin2, GPIO_LOW);

#ifdef _SHARP_2P89_75FPS_LCD_
	Gpio_SetPinLevel(GPIOPortB_Pin3, GPIO_LOW);
#endif

    PMU_ExitModule(PMU_BLON);
    BacklightLDOBackup = ScuLDOSet(SCU_LDO_28);
}


//#ifdef DISPLAY_ENABLE
#if 0

/*
--------------------------------------------------------------------------------
  Function name : void BL_SetBright(BL_LIGHT_en_t light)
  Author        : yangwenjie
  Description   : setting backlight

  Input         :

  Return        :

  History:     <author>         <time>         <version>
              yangwejie    2009-1-5        Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BL_SetLevel(uint8 Level)
{
    uint8 TmpPwmRate;

    if (Level < BL_LEVEL_MAX)
    {
        TmpPwmRate = BL_PWM_RATE_MIN + BL_PWM_RATE_STEP * Level;
    }
    else
    {
        TmpPwmRate = BL_PWM_RATE_MIN / 2;
    }
	PwmRateSet(BL_PWM_CH,TmpPwmRate,PWM_FREQ);
}
/*
--------------------------------------------------------------------------------
  Function name : void BL_DeInit(void)
  Author        : yangwenjie
  Description   : close backlight

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BL_Compensate(uint8 Level)
{
    uint8 TmpPwmRate;

    if (Level < BL_LEVEL_MAX)
    {
        Level = (BL_LEVEL_MAX - 1) - Level;
		if(Level == 0 )
        	TmpPwmRate = BL_PWM_RATE_MIN + BL_PWM_RATE_STEP * Level+55;
		else if(Level == 1 )
			TmpPwmRate = BL_PWM_RATE_MIN + BL_PWM_RATE_STEP * Level+45;
		else if(Level == 2)
			TmpPwmRate = BL_PWM_RATE_MIN + BL_PWM_RATE_STEP * Level+35;
		else if(Level == 3)
			TmpPwmRate = BL_PWM_RATE_MIN + BL_PWM_RATE_STEP * Level+28;
		else
			TmpPwmRate = BL_PWM_RATE_MIN + BL_PWM_RATE_STEP * Level+20;
    }
    PwmRateSet(BL_PWM_CH, TmpPwmRate,PWM_FREQ);
}

/*
--------------------------------------------------------------------------------
  Function name : void BL_DeInit(void)
  Author        : yangwenjie
  Description   :

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BL_On(void)
{
    ScuLDOSet(BacklightLDOBackup);
    PMU_EnterModule(PMU_BLON);

    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD3_PWM2);

    PWM_Start(BL_PWM_CH);
    BL_SetLevel(gSysConfig.BLevel);

	BacklightSystickCounterBack	= SysTickCounter;

    ClearMsg(MSG_BL_OFF);
}

/*
--------------------------------------------------------------------------------
  Function name : void BL_DeInit(void)
  Author        : yangwenjie
  Description   : close backlight

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BL_Off(void)
{
    PWM_Stop(BL_PWM_CH);

    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD3_IO);
	Gpio_SetPinDirection(BACKLIGHT_PIN , GPIO_OUT);
	Gpio_SetPinLevel(BACKLIGHT_PIN, GPIO_LOW);

    PMU_ExitModule(PMU_BLON);
    BacklightLDOBackup = ScuLDOSet(SCU_LDO_28);

    SendMsg(MSG_BL_OFF);
}

/*
--------------------------------------------------------------------------------
  Function name : void BLOffEnable(void)
  Author        : ZhengYongzhi
  Description   : close backlight enable.

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BLOffEnable(void)
{
    ClearMsg(MSG_BL_OFF_DISABLE);
    BacklightSystickCounterBack = SysTickCounter;
}

/*
--------------------------------------------------------------------------------
  Function name : void BLOffDisable(void)
  Author            : ZhengYongzhi
  Description      : close backlight disable.

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BLOffDisable(void)
{
    SendMsg(MSG_BL_OFF_DISABLE);
}

/*
--------------------------------------------------------------------------------
  Function name : void BLOffEnable(void)
  Author        : ZhengYongzhi
  Description   : close backlight enable

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void LcdStandbyEnable(void)
{
    ClearMsg(MSG_LCD_STANDBY_DISABLE);
}

/*
--------------------------------------------------------------------------------
  Function name : void BLOffDisable(void)
  Author            : ZhengYongzhi
  Description      : ½ûÖ¹¹Ø±Õ±³¹â

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void LcdStandbyDisable(void)
{
    SendMsg(MSG_LCD_STANDBY_DISABLE);
}

/*
--------------------------------------------------------------------------------
  Function name : void BL_DeInit(void)
  Author        : yangwenjie
  Description   : backlight detect

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BacklightDetec(void)
{
    UINT16 Count;
    uint32 Batt, Temp;

    if((FALSE == IsBackLightOn) && (FALSE == CheckMsg(MSG_BL_OFF_DISABLE)))
    {
        if(gSysSetBLTimeArray[gSysConfig.BLtime])
        {
            Count = SysTickCounter - BacklightSystickCounterBack;
            if(Count>gSysSetBLTimeArray[gSysConfig.BLtime]*100)
            {
                if(gSysSetBLModeArray[gSysConfig.BLmode] == BL_DARK)
                {
                    BL_Off();
                    if (FALSE == CheckMsg(MSG_LCD_STANDBY_DISABLE))
                    {
                        LcdStandby();
                    }
                    Os_idle();
                }

                if(gSysSetBLModeArray[gSysConfig.BLmode] == BL_HALFDARK)
                {
                    BL_SetLevel(5);
                }
                IsBackLightOn = TRUE;

				return;
            }
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void BL_DeInit(void)
  Author        : yangwenjie
  Description   : backlight resume

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
/*
void BL_Resume(void)
{
    BacklightSystickCounterBack = SysTickCounter;

    if(IsBackLightOn == TRUE)
	{
	    IsBackLightOn = FALSE;

        if(gSysSetBLModeArray[gSysConfig.BLmode] == BL_DARK)
        {
            if (FALSE == CheckMsg(MSG_LCD_STANDBY_DISABLE))
            {
                LcdWakeUp();
            }

            SendMsg(MSG_NEED_BACKLIGHT_ON);
            SendMsg(MSG_NEED_PAINT_ALL);
        }
        else
        {
            BL_SetLevel(gSysConfig.BLevel);
            BacklightSystickCounterBack	= SysTickCounter;
        }
        KeyReset();
	}
}
*/
/*
********************************************************************************
*
*                         End of backlight.c
*
********************************************************************************
*/
#endif

