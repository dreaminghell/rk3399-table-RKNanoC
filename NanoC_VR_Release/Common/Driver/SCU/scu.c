/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name£º   Scu.c
*
* Description:  each module open,close and reset and some related usb PHY registers setting.
*
* History:      <author>          <time>        <version>
*               anzhiguo          2009-3-24         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_SCU_
#include "SysConfig.h"
#include "DriverInclude.h"

/*
--------------------------------------------------------------------------------
  Function name : ScuClockSet()
  Author        : anzhiguo
  Description   : selet open/close module by its id.

  Input         : clk_id module id
                  Enable :open flag: 1--open this id module.0--close this id module

  Return        : null

  History:     <author>         <time>         <version>
               anzhiguo       2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void ScuClockGateCtr(eCLOCK_GATE clk_id, BOOL Enable)
{
    UINT8  group;
    UINT8  num;

    group  = clk_id / 16 ;
    num    = clk_id % 16;

    if (Enable)
    {
        Scu->CLKGATE[group] = (uint32)(0x00010000) << (num);
    }
    else
    {
        Scu->CLKGATE[group] = (uint32)(0x00010001) << (num);
    }
    Delay10cyc(5);
}

/*
--------------------------------------------------------------------------------
  Function name : ScuModuleReset()
  Author        : anzhiguo
  Description   : reset module by id.

  Input         : clk_id module id
                  Reset:   1:  reset
                           0:  release reset
  Return        : null

  History:     <author>         <time>         <version>
               anzhiguo     2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void ScuSoftResetCtr(eSOFT_RST Module_id, BOOL Reset)
{
    UINT8  group;
    UINT8  num;

    group  = Module_id / 16 ;
    num    = Module_id % 16;

    if (Reset)
    {
        Scu->SOFTRST[group] = (uint32)(0x00010001) << (num);
    }
    else
    {
        Scu->SOFTRST[group] = (uint32)(0x00010000) << (num);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void ScuLDOEnable(void)
  Author        : zyz
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_DATA_ uint8 LDODisable = 0;

_ATTR_DRIVER_CODE_
void ScuLDOEnable(void)
{
    LDODisable = 0;
}

/*
--------------------------------------------------------------------------------
  Function name : void ScuLDODisable(void)
  Author        : zyz
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void ScuLDODisable(void)
{
    LDODisable = 1;
}

/*
--------------------------------------------------------------------------------
  Function name : void ScuLDOSet(uint32 Level)
  Author        : anzhiguo

  Input         : clk_id
                  Reset:   1:  reset
                           0:  release reset
  Return        : null

  History:     <author>         <time>         <version>
               anzhiguo     2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 ScuLDOSet(uint32 Level)
{
    uint32 temp;

    temp = (Grf->MPMU_PWR_CON & MPMU_PWR_VID_LDO_MASK) >> 0;

    if (LDODisable == 0)
    {
        Grf->MPMU_PWR_CON = (MPMU_PWR_VID_LDO_MASK << 16) | (Level << 0);
    }

    //#ifdef DISPLAY_ENABLE
    #if 1
    BacklightLDOBackup = temp;
    #endif

    return(temp);
}

/*
--------------------------------------------------------------------------------
  Function name : void ScuLDOEnable(void)
  Author        : zyz
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_DATA_ uint8 DCoutDisable = 0;

_ATTR_DRIVER_CODE_
void ScuDCoutEnable(void)
{
    DCoutDisable = 0;
}

/*
--------------------------------------------------------------------------------
  Function name : void ScuLDODisable(void)
  Author        : zyz
  Description   :

  Input         :
  Return        :

  History:     <author>         <time>         <version>
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void ScuDCoutDisable(void)
{
    DCoutDisable = 1;
}

/*
--------------------------------------------------------------------------------
  Function name : void ScuLDOSet(uint32 Level)
  Author        : anzhiguo

  Input         : clk_id
                  Reset:   1:  reset
                           0:  release reset
  Return        : null

  History:     <author>         <time>         <version>
               anzhiguo     2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 ScuDCoutSet(uint32 Level)
{
    uint32 temp;

    temp = (Grf->MPMU_PWR_CON & MPMU_PWR_VID_BUCK_MASK) >> 6;
    if (LDODisable == 0)
    {
        Grf->MPMU_PWR_CON = (MPMU_PWR_VID_BUCK_MASK << 16) | (Level << 6);
    }

    return(temp);
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_Force_PWM_Mode_Set(BOOL Enable)
  Author           :
  Description     :  SW can charge PFM and PWM mode current threshold
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_Force_PWM_Mode_Set(BOOL Enable)
{
    if (Enable)
    {
        Grf->MPMU_PWR_CON = ((MPMU_PWR_FORCE_PWM_MASK << 16) | MPMU_PWR_FORCE_PWM);
    }
    else
    {
        Grf->MPMU_PWR_CON = ((MPMU_PWR_FORCE_PWM_MASK << 16) | MPMU_PWR_UNFORCE_PWM);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_Threshold_Set(Threshold_Value Value)
  Author           :
  Description     :  SW can charge PFM and PWM mode current threshold
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_Threshold_Set(Threshold_Value Value)
{
    Grf->MPMU_PWR_CON = ((MPMU_PFM_THRESHOLD_MASK << 16) | (Value));
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_Battery_Sns_Set(BOOL Enable)
  Author           :
  Description     :  Enable Batter Sences Sample
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_Battery_Sns_Set(BOOL Enable)
{
    if(Enable)
    {
        Grf->MPMU_PWR_CON = ((MPMU_PWR_BAT_SNS_MASK << 16) | MPMU_PWR_BAT_SNS_ENABLE);
    }
    else
    {
        Grf->MPMU_PWR_CON = ((MPMU_PWR_BAT_SNS_MASK << 16) | MPMU_PWR_BAT_SNS_DISABLE);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_Charger_Enable_Set(void)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_Charger_Enable_Set(BOOL Enable)
{
    if (Enable)
    {
        Grf->MPMU_PWR_CON = ((MPMU_PWR_CHARGE_ENABLE_MASK << 16) | MPMU_PWR_CHARGE_ENABLE);
    }
    else
    {
        Grf->MPMU_PWR_CON = ((MPMU_PWR_CHARGE_ENABLE_MASK << 16) | MPMU_PWR_CHARGE_DISABLE);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_Charger_Current_Set(Charge_Current_Value Value)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_Charger_Current_Set(Charge_Current_Value Value)
{
    Grf->MPMU_PWR_CON = ((MPMU_PWR_CHARGE_CURRENT_MASK << 16) | Value);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 Scu_Charge_State_Get(void)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 Scu_Charge_State_Get(void)
{
    return ((Grf->MPMU_STATE  & MPMU_PWR_CHARGE_STATE_MASK) >> 10);
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_USB_PowerPath_Enable(BOOL Enable)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_USB_PowerPath_Enable(BOOL Enable)
{
    if (Enable)
    {
        RTCReg->RTC_CTRL &= ~(0x01ul);
    }
    else
    {
        RTCReg->RTC_CTRL |= 0x01ul;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : BOOL Scu_DCout_Issue_State_Get(void)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 Scu_DCout_Issue_State_Get(void)
{
    return ((Grf->MPMU_STATE & MPMU_PWR_DCOUT_ALRAM_MASK) >> 5);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 Scu_PlayOn_State_Get(void)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 Scu_PlayOn_Level_Get(void)
{
    if (Grf->MPMU_STATE & MPMU_PWR_PLAY_LEVEL_MASK)
    {
        return 1;
    }
    return 0;
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 Scu_Play_Pause_Get(void)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 Scu_Play_Pause_Get(void)
{
    if (Grf->MPMU_STATE & MPMU_PWR_PLAY_PAUSE_MASK)
    {
        return 1;
    }
    return 0;
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 Scu_Adapter_State_Get(void)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 Scu_Adapter_State_Get(void)
{
    if (Grf->MPMU_STATE & MPMU_PWR_5V_READY)
    {
        return 1;
    }
    return 0;
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_OOL_Power_Set(BOOL Enable)
  Author           :
  Description     :  System PowerDown Or PowerUp
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_OOL_Power_Set(BOOL Enable)
{
    if(Enable)
    {
        Grf->MPMU_PWR_CON = ((MPMU_PWR_BAT_POWER_MASK << 16) | MPMU_PWR_OOL_POWER_UP);
    }
    else
    {
        Grf->MPMU_PWR_CON = ((MPMU_PWR_BAT_POWER_MASK << 16) | MPMU_PWR_OOL_POWER_DOWN);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_saradc_mic_powerup(BOOL Enable)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_mic_pga_powerup(BOOL Enable)
{
    if(Enable)
    {
        Grf->SARADC_CON = ((MPMU_SARADC_MIC_POWER_MASK << 16) | MPMU_SARADC_MIC_POWERUP);
    }
    else
    {
        Grf->SARADC_CON = ((MPMU_SARADC_MIC_POWER_MASK << 16) | MPMU_SARADC_MIC_POWERDOWN);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_mic_pga_volset(uint32 vol)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_mic_pga_volset(uint32 vol)
{
    Grf->SARADC_CON = ((MPMU_SARADC_MIC_VOL_MASK << 16) | ((vol & 0x0f) << 1));
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_mic_boosten_set(BOOL Enable)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_mic_boosten_set(BOOL Enable)
{
    if(Enable)
    {
        Grf->SARADC_CON = ((MPMU_SARADC_MIC_BOOST_MASK << 16) | MPMU_SARADC_MIC_BOOSTEN);
    }
    else
    {
        Grf->SARADC_CON = ((MPMU_SARADC_MIC_BOOST_MASK << 16) | MPMU_SARADC_MIC_BOOSTDIS);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Scu_mic_boosten_set(BOOL Enable)
  Author           :
  Description     :
  Input             :
  Return           :

  History:     <author>         <time>         <version>
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_mic_ctriref_half_set(BOOL Enable)
{
    if(Enable)
    {
        Grf->SARADC_CON = ((MPMU_SARADC_MIC_CTRIREF_MASK << 16) | MPMU_SARADC_MIC_CTRIREFEN);
    }
    else
    {
        Grf->SARADC_CON = ((MPMU_SARADC_MIC_CTRIREF_MASK << 16) | MPMU_SARADC_MIC_CTRIREFDIS);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : ScuUsbPHYSet()
  Author        : anzhiguo
  Description   : usb PHY control registers setting.



  Input         :

  Return        : null

  History:     <author>         <time>         <version>
               anzhiguo       2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Scu_otgphy_suspend(BOOL Enable)
{
    if (Enable)
    {
        //Grf->OTGPHY_CON0 = (((OTGPHY_SUSPEND_MASK | OTGPHY_SOFT_CON_MASK) << 16) | (OTGPHY_SUSPEND_EN | OTGPHY_SOFT_CON_EN));
        Grf->OTGPHY_CON0 = (((OTGPHY_SUSPEND_MASK | OTGPHY_SOFT_CON_MASK) << 16) | (OTGPHY_SUSPEND_DIS | OTGPHY_SOFT_CON_EN));
    }
    else
    {
        Grf->OTGPHY_CON0 = (((OTGPHY_SUSPEND_MASK | OTGPHY_SOFT_CON_MASK) << 16) | (OTGPHY_SUSPEND_DIS | OTGPHY_SOFT_CON_DIS));
    }
    Delay100cyc(5);     //zyz: need delay
}

/*
--------------------------------------------------------------------------------
  Function name : int32 SCU_Init(void)
  Author        :
  Description   :

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
               anzhiguo     2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
int32 ScuClockGateInit(void)
{
    //Clock Gate Init
    Scu->CLKGATE[0] = (((uint32)(0x00010000) << (CLOCK_GATE_DRAM  - 0)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_IRAM  - 0)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_TROM1 - 0)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_TROM0 - 0)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_BTROM - 0)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_LCDC  - 0)) |
                        #ifdef _NANDFLASH_
                       ((uint32)(0x00010000) << (CLOCK_GATE_BCH   - 0)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_NANDC - 0)) |
                        #else
                       ((uint32)(0x00010001) << (CLOCK_GATE_BCH   - 0)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_NANDC - 0)) |
                        #endif
                       ((uint32)(0x00010000) << (CLOCK_GATE_GPIO  - 0)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_I2C   - 0)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_I2S   - 0)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_PWM   - 0)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_ADC   - 0)) |
                       ((uint32)(0         )                          )
                      );

    Scu->CLKGATE[1] = (((uint32)(0x00010001) << (CLOCK_GATE_TIMER  - 16)) |
                        #ifdef _SPINOR_
                        ((uint32)(0x00010000) << (CLOCK_GATE_SPI    - 16)) |
                        #else
                        ((uint32)(0x00010001) << (CLOCK_GATE_SPI    - 16)) |
                        #endif
                        #ifdef _SDCARD_
                       ((uint32)(0x00010000) << (CLOCK_GATE_SDMMC  - 16)) |
                        #else
                        ((uint32)(0x00010001) << (CLOCK_GATE_SDMMC  - 16)) |
                        #endif
                       ((uint32)(0x00010000) << (CLOCK_GATE_DMA    - 16)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_SYNTH  - 16)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_IMDCT  - 16)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_USBPHY - 16)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_USBBUS - 16)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_UART   - 16)) |
                       ((uint32)(0x00010001) << (CLOCK_GATE_EFUSE  - 16)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_RTC    - 16)) |
                       ((uint32)(0x00010000) << (CLOCK_GATE_GRF    - 16)) |
                       ((uint32)(0         )                            )
                      );
    Delay100cyc(50);
    #if 0
    //Soft Reset Init
    Scu->SOFTRST[0] = (((uint32)(0x00010001) << (RST_ARM   - 0)) |
                       ((uint32)(0x00010001) << (RST_I2C   - 0)) |
                       ((uint32)(0x00010001) << (RST_I2S   - 0)) |
                       ((uint32)(0x00010001) << (RST_PWM   - 0)) |
                       ((uint32)(0x00010001) << (RST_ADC   - 0)) |
                       ((uint32)(0x00010001) << (RST_GPIO  - 0)) |
                       ((uint32)(0x00010001) << (RST_UART  - 0)) |
                       ((uint32)(0x00010001) << (RST_EFUSE - 0)) |
                       ((uint32)(0x00010001) << (RST_RTC   - 0)) |
                       ((uint32)(0x00010001) << (RST_GRF   - 0)) |
                       ((uint32)(0x00010001) << (RST_TIMER - 0)) |
                       ((uint32)(0x00010001) << (RST_SPI   - 0)) |
                       ((uint32)(0x00010001) << (RST_SDMMC - 0)) |
                       ((uint32)(0x00010001) << (RST_DMA   - 0)) |
                       ((uint32)(0x00010001) << (RST_SYNTH - 0)) |
                       ((uint32)(0x00010001) << (RST_IMDCT - 0)) |
                       ((uint32)(0         )                   )
                      );
    Scu->SOFTRST[1] = (((uint32)(0x00010001) << (RST_LCDC   - 16)) |
                       ((uint32)(0x00010001) << (RST_USBPHY - 16)) |
                       ((uint32)(0x00010001) << (RST_USBLGC - 16)) |
                       ((uint32)(0x00010001) << (RST_USBOTG - 16)) |
                       ((uint32)(0x00010001) << (RST_NANDC  - 16)) |
                       ((uint32)(0         )                     )
                      );
    #endif
    return(0);
}

/*
********************************************************************************
*
*                         End of Scu.c
*
********************************************************************************
*/


