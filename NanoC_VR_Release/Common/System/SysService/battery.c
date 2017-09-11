/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   battery.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_BATTERY_

#include "SysInclude.h"

//#define BATTERY_DBG
#ifdef BATTERY_DBG
uint32 battery_dbg_cnt = 0;
#endif

#define CHARGE_SAMPLE_TIME      (5 * 100)           //采样间隔时间单位10mS
#define CHARGE_CHECKFULL_COUNT  (20)                //检测充电电压大于 阈值的次数
#define CHARGE_FULL_DELAY       (1*60*60*100/CHARGE_SAMPLE_TIME)    //小电流充电时间

#define BATTERY_BUF_SIZE        64
uint16 BatteryCnt;
uint16 BatteryBuffer[BATTERY_BUF_SIZE];

//int32  LastBattryVal;
//uint32 ChargeTimeMs = 0;
uint32 lastChargeTimeMs = 0;
uint32 chargeFullCheckCnt = 0;

/*
--------------------------------------------------------------------------------
  Function name : UINT32 BatteryInit(void)
  Author        : yangwenjie
  Description   : battery initial
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                yangwenjie      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void BatteryInit(void)
{    
	UINT32 battlevel;
	UINT32 i,j;
	uint32 temp;
    
	//Scu_Battery_Sns_Set(1);
	
    BatteryCnt = 0;
    
	for(i =  0; i < 18; i++)
	{
        Adc->ADC_CTRL &= ~ADC_START;
        temp = Adc->ADC_CTRL & (~ADC_CH_MASK);
        Adc->ADC_CTRL = temp | ADC_CHANEL_BATTERY | ADC_START;
        
        for (j = 0; j < 20; j++)
        {
            DelayMs(1);
            if ((Adc->ADC_STAS & 0x01) == 0)
                break;
        }
		battlevel = (UINT32)(Adc->ADC_DATA);
        //printf("j = %d, Adc Val = %d, battlevel =", j, battlevel);
		battlevel = (UINT32)RealBattValue(battlevel);
        //printf(" %d\n", battlevel);
		if (i >= 10)
		{
			gBattery.Batt_Value += battlevel;
		}
	}
	
	gBattery.Batt_Value >>= 3;	  
	
	for (gBattery.Batt_Level = 0; gBattery.Batt_Level < BATT_TOTAL_STEPS - 1; gBattery.Batt_Level++)
	{
		if (gBattery.Batt_Value < Batt_Level[gBattery.Batt_Level])
		{
			break;
		}
	}

	if((gBattery.Batt_Value <= BATT_EMPTY_VALUE+10)&(!CheckVbus()))
	{
        //DEBUG("Low Power, BatteryVal = %d\n", gBattery.Batt_Value);
        DelayMs(200);
        Scu_OOL_Power_Set(0);       //power down
		while(1);
	}
	
	if(gBattery.Batt_Level>0) gBattery.Batt_Level = gBattery.Batt_Level-1 ;

	if((gBattery.Batt_Value>BATT_FULL_VALUE) == TRUE)
		gBattery.Batt_Level = BATT_TOTAL_STEPS - 1;

	Adc->ADC_CTRL = AdcSamplingCh  | /*ADC_START | */ADC_POWERUP | ADC_INT_ENBALE;
}


/*
--------------------------------------------------------------------------------
  Function name : UINT32 RealBattValue(UINT16 adc)
  Author        : yangwenjie
  Description   : voltage sample
                  (ret/1024)*2.5*2*1000,enlarge voltage a thousand,improve conversion accuracy.
                  
  Input         : 
  Return        : real voltage value.

  History:     <author>         <time>         <version>       
                yangwenjie      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
UINT32 RealBattValue(UINT16 adc)
{
    int32  delta;
    int32  SetVol;
    uint32 i, buffer_max;
    uint32 temp = (Grf->MPMU_PWR_CON & MPMU_PWR_VID_LDO_MASK) >> 0;

    //temp = 2500;                    //ADC Ref = 2500mV
    temp = 3400 - temp * 100;       //LDO output adc sample reference.
    temp = (adc << 1) * temp >> 10;

    BatteryBuffer[BatteryCnt%BATTERY_BUF_SIZE] = temp;
    BatteryCnt++;

    buffer_max = BATTERY_BUF_SIZE;
    if (BatteryCnt < BATTERY_BUF_SIZE)
    {
        buffer_max = BatteryCnt;
    }
    
    temp = 0;
    for (i = 0; i < buffer_max; i++)
    {
        temp += (uint32)(BatteryBuffer[i]);
    }
    temp = temp/buffer_max;
    
    return (temp);
}

/*
--------------------------------------------------------------------------------
  Function name : void BatteryChargeInit(void)
  Author        : yangwnejie
  Description   : get voltage status.
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
                yangwenjie      2008-1-15          Ver1.0
  desc:         ORG  
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BatteryChargeInit(void)
{
    ChargeFullFlag     = 0;
    chargeFullCheckCnt = 0;
    lastChargeTimeMs   = SysTickCounter;

    Scu_USB_PowerPath_Enable(1);
    DelayMs(5);
    Scu_Charger_Enable_Set(1);              //charge Enable
    Scu_Charger_Current_Set(CHARGE_280mA);  //charge Current set
    //DEBUG("Charge Start, Charge Current set is 280mA!\n");
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 ChargeFullCheck(uint32 battval)
  Author        : yangwnejie
  Description   : get voltage status.
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
                yangwenjie      2008-1-15          Ver1.0
  desc:         ORG  
            1. eFuse 偏差大于8，说明电压上升，此时需要通过ADC检测
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
uint32 ChargeFullCheck(void)
{
    UINT32 batt_state = CHARGE_BATT;
    uint32 ChargeTimeMs = SysTickCounter;

    if (ChargeFullFlag == 0)
    {
        if (MPMU_PWR_CHARGE_IS_OVER == Scu_Charge_State_Get())
        {
            //DEBUG("Battery PMU charge Full!");
            batt_state =  CHARGE_FULL; 
        }

        if (batt_state == CHARGE_FULL)
        {
            if (ChargeFullFlag == 0)
            {
                ChargeFullFlag = 1;
                Scu_Charger_Enable_Set(0);      //charge Disable
                //DEBUG("Charge Full....");
            }
        }
    }
    else
    {
        batt_state = CHARGE_FULL;
    }
        
    return batt_state;
}

/*
--------------------------------------------------------------------------------
  Function name :void BatteryChargeDeInit(void)
  Author        : yangwnejie
  Description   : get voltage status.
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
                yangwenjie      2008-1-15          Ver1.0
  desc:         ORG  
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void BatteryChargeDeInit(void)
{
    Scu_USB_PowerPath_Enable(0);
    DelayMs(5);
    Scu_Charger_Enable_Set(0);      //charge Disable
    //DEBUG("Charge Disable!\n");
            
}

/*
--------------------------------------------------------------------------------
  Function name : UINT16 Battery_GetChargeState(void)
  Author        : yangwnejie
  Description   : get voltage status.
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
                yangwenjie      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
UINT16 Battery_GetChargeState(void)
{
    UINT32 i;
    UINT32 battlevel;
    UINT32 batt_state = CHARGE_NO;
    
    if (CheckMsg(MSG_CHARGE_ENABLE))
    { 
        if (gBattery.IsBatt_Charge != TRUE)
        {
            gBattery.IsBatt_Charge = TRUE;
            
            return CHARGE_BATT;
        }
        
        batt_state = ChargeFullCheck();
        
    }
    else
    {
        ChargeFullFlag = 0;
        batt_state  = CHARGE_NO;  
        
        if(TRUE == gBattery.IsBatt_Charge)
        {            
            for(i = 0; i < 50; i++)
            {
                battlevel = GetAdcData(ADC_CHANEL_BATTERY);
                battlevel = (UINT32)RealBattValue(battlevel);

                gBattery.Batt_Value += battlevel;
                gBattery.Batt_Value >>= 1;
                  
            }

            for(gBattery.Batt_Level = 0; gBattery.Batt_Level < BATT_TOTAL_STEPS - 1; gBattery.Batt_Level++)
            {
                if (gBattery.Batt_Value < Batt_Level[gBattery.Batt_Level])
                {
                    break;
                }
            }

            if(gBattery.Batt_Level > 0) 
            {
                gBattery.Batt_Level = gBattery.Batt_Level - 1;
            }

            if(gBattery.Batt_Value > BATT_FULL_VALUE) 
            {
                gBattery.Batt_Level = BATT_TOTAL_STEPS - 1;
            }
                
            
            gBattery.IsBatt_Charge = FALSE;
            gBattery.IsBattChange  = TRUE;
            
            SendMsg(MSG_BATTERY_UPDATE);
        }
    }

    return batt_state;
    
}

/*
--------------------------------------------------------------------------------
  Function name : void Battery_GetLevel(void)
  Author        : yangwenjie
  Description   : get voltage level
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
                yangwenjie      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
UINT32 Battery_GetLevel(void)
{
    UINT32 batt_adcval;
    UINT32 ChargeState;
    UINT32 i, alarm_state;
    
    batt_adcval = GetAdcData(ADC_CHANEL_BATTERY);
    gBattery.Batt_Value   = (UINT32)RealBattValue(batt_adcval); 
    alarm_state = Scu_DCout_Issue_State_Get();
    ChargeState = Battery_GetChargeState();

    ///////////////////////////////////////
    #ifdef BATTERY_DBG
    if (SysTickCounter - battery_dbg_cnt > 500)
    {
        battery_dbg_cnt = SysTickCounter;
        //printf("batt_adcval=%d, gBatt_Value,=%d, vbus=%d, charge_state=%d, alarmstate=0x%x\n", 
               //batt_adcval,gBattery.Batt_Value,ChargeState,Scu_Charge_State_Get(),alarm_state);
    }
    #else
    if (alarm_state != 0)
    {
        //DEBUG("alarm_state = 0x%x", alarm_state);
    }
    #endif
    /////////////////////////////////////

    if (alarm_state & 0x1e)     //zyz: Low Battery use ADC test
    {
        SendMsg(MSG_POWER_DOWN);
        return gBattery.Batt_Level;
    }
    
    switch (ChargeState)
    {
        case CHARGE_BATT:
        {
            if (SysTickCounter - BatterySystickCounterBack > 50)
            {
                if (++gBattery.Batt_Level > (BATT_TOTAL_STEPS - 1))
                {
                    gBattery.Batt_Level = 0;
                }
            
                BatterySystickCounterBack = SysTickCounter;
                SendMsg(MSG_BATTERY_UPDATE);
            }
        }
        break;
            
        case CHARGE_FULL:
        {
            gBattery.Batt_Level = (BATT_TOTAL_STEPS - 1);
            SendMsg(MSG_BATTERY_UPDATE);
        }
        break;
            
        case CHARGE_NO:
        {   
            if (gBattery.Batt_Value < BATT_POWEROFF_VALUE)
            {
                gBattery.Batt_LowCnt++;

                if(gBattery.Batt_LowCnt > BATT_POWEROFF_CNT)
                {
                    if(FALSE==gBattery.Batt_LowPower)
                    {
                        SendMsg(MSG_LOW_POWER);
                        gBattery.Batt_LowPower = TRUE;
                    }
                }

                return gBattery.Batt_Level;
            }   
            
            gBattery.Batt_LowCnt = 0;   

            for (i = 0; i < BATT_TOTAL_STEPS; i++)
            {                   
               if ((gBattery.Batt_Value) < Batt_Level[i])
               {  
                    if((Batt_Level[i] - gBattery.Batt_Value) < BATTERY_VALUE_SETP)
                    {
                        BatteryCounter ++;
                        if(BatteryCounter>15)
                        {  
                            gBattery.Batt_Level = i-1;                
                            BatteryCounter=0;
                            SendMsg(MSG_BATTERY_UPDATE);
                        }
                        BatteryCounter1 = 0;
                    }
                    else
                    {                               
                        BatteryCounter1 ++;
                        if(BatteryCounter1 > 15)
                        {
                            if( gBattery.IsBattChange == TRUE)
                            {
                                gBattery.Batt_Level = i-1; 
                                gBattery.IsBattChange =FALSE;
                            }
                            else
                            {                                   
                                if(i > gBattery.Batt_Level)
                                {
                                    return 0 ;
                                }
                                else
                                {
                                    gBattery.Batt_Level = i-1; 
                                }
                            }

                            SendMsg(MSG_BATTERY_UPDATE);
                        }
                        BatteryCounter = 0;
                    }
                    break;
               }
            }
        }
        break;

        default:
            break;
    }

    return gBattery.Batt_Level;
}

/*
********************************************************************************
*
*                         End of battery.c
*
********************************************************************************
*/

