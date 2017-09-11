/*
********************************************************************************
*          Copyright (C),2004-2007, Fuzhou Rockchip Electronics Co.,Ltd.
*                             All Rights Reserved
*
*Description: AD sampling,battery voltage check program.
*
$Header: /cvs_database/cvs_server_rk26/mp3_project/RK260x_SDK/System/Driver/Adc/Adc.c,v 1.1.1.5 2007/12/27 07:58:59 ke Exp $
$Author: ke $
$Date: 2007/12/27 07:58:59 $ 
$Revision: 1.1.1.5 $ 
********************************************************************************
*/
#define _IN_ADC_

#include "DriverInclude.h"
#include "interrupt.h"

/*
--------------------------------------------------------------------------------
  Function name : void AdcInit(void)
  Author        : Zhengyongzhi
  Description   : ad sampling initial
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
              anzhiguo     2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void AdcInit(void)
{
    AdcSamplingCounter = 0;
    AdcPendCounter     = 0;
    AdcFlag            = ADC_FINISH;
    AdcSamplingCh      = AdcSamplingChConfig[AdcSamplingCounter];
	Adc->ADC_CTRL      = ADC_POWERUP ;
}

/*
--------------------------------------------------------------------------------
  Function name : void AdcInit(void)
  Author        : Zhengyongzhi
  Description   : 
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
              anzhiguo     2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AdcPowerDown(void)
{
	Adc->ADC_CTRL &= ~ADC_POWERUP ;
}

_ATTR_SYS_CODE_
void AdcPowerUp(void)
{
	Adc->ADC_CTRL |= ADC_POWERUP ;
}

/*
--------------------------------------------------------------------------------
  Function name : void AdcStart(uint8 StartCh)
  Author        : Zhengyongzhi
  Description   : start ad sampling.
  				  AD sampling is started in systick and timer interruption,if adc is processing,
  				  then suspend the channel.after finish this ad sampling, start the interrupt in
  				  interrupt service again.
  Input         : null
  Return        : null
  History:     <author>         <time>         <version>       
                anzhiguo     2009-3-24         Ver1.0
  desc:          ORG //register system interruption, used for key scan and battery detect.
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void AdcStart(uint8 StartCh)
{
    uint8  ch;
    uint32 temp;
    uint32 IsEnable;
    
//    IsEnable = IsrDisable(INT_ID29_TIMER);
    
    if ((StartCh == ADC_MIC) || (StartCh == ADC_CHANEL_FM1))
    {
        ch = StartCh;
    }
    else
    {
        if (++AdcSamplingCounter >= sizeof(AdcSamplingChConfig))
        {
            AdcSamplingCounter = 0;
        }
        ch  = AdcSamplingChConfig[AdcSamplingCounter];
    }

    if ((AdcFlag & ADC_FINISH) || (AdcPendCounter > 5))
    {
        AdcPendCounter = 0;
        //直接启动ADC
        Adc->ADC_CTRL &= ~ADC_START;
        temp = Adc->ADC_CTRL & (~ADC_CH_MASK);
        Adc->ADC_CTRL = temp | ch | ADC_START;
        AdcSamplingCh  = ch;
        AdcFlag &= ~ADC_FINISH;
    }
    else
    {
        //挂起ADC通道
        AdcFlag |= (ADC_CH0_PEND << ch);
        AdcPendCounter ++;
    }
    
//    IsrEnable(INT_ID29_TIMER, IsEnable);
}

/*
--------------------------------------------------------------------------------
  Function name : __irq void AdcIsr(void)
  Author        : Zhengyongzhi
  Description   : interruption of ad sampling.sampling ad data, and detect whether is channel suspended.
                  
  Input         : null
  Return        : null
  History:     <author>         <time>         <version>       
                anzhiguo     2009-3-24         Ver1.0
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
__irq void AdcIsr(void)
{
    uint8  ch;
    uint32 temp;
    
    
    AdcData[AdcSamplingCh] = (UINT16)(Adc->ADC_DATA);
    
    Adc->ADC_CTRL &= ~ADC_INT_CLEAR;
    //Adc->ADC_CTRL &= ~ADC_POWERUP;
    //SCU_ExitModule(SCU_ADC);
    
    AdcFlag |= (0x01 << AdcSamplingCh);
    AdcFlag |= ADC_FINISH;
    
    for (ch = 0; ch < 5; ch++)
    {
        if (AdcFlag & (ADC_CH0_PEND << ch))
        {
            Adc->ADC_CTRL &= ~ADC_START;
            temp = Adc->ADC_CTRL & (~ADC_CH_MASK);        
            Adc->ADC_CTRL = temp | ch | ADC_START;
            
            AdcSamplingCh  = ch;
            AdcFlag &= ~ADC_FINISH;
            AdcFlag &= ~(ADC_CH0_PEND << ch);

            AdcPendCounter--;            
            break;
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 CheckAdcState(uint8 ch)
  Author        : Zhengyongzhi
  Description   : 
                  
  Input         : null
  Return        : null
  History:     <author>         <time>         <version>       
                anzhiguo     2009-3-24         Ver1.0
  desc:          ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 CheckAdcState(uint8 ch)
{
    uint32 temp;

    temp = (AdcFlag & (0x01 << ch));
    
    if (temp != 0) 
    {
        AdcFlag &= ~((uint32)0x01 << ch);
    }

    return(temp);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetAdcData(uint8 ch)
  Author        : Zhengyongzhi
  Description   : 
                  
  Input         : null
  Return        : null
  History:     <author>         <time>         <version>       
                anzhiguo     2009-3-24         Ver1.0
  desc:          ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
uint32 GetAdcData(uint8 ch)
{
    return(AdcData[ch]);
}

/*
********************************************************************************
*
*                         End of Adc.c
*
********************************************************************************
*/

