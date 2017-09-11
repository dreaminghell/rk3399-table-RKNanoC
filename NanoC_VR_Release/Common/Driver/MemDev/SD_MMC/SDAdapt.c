/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: SDAdapt.c
Desc: 

Author: chenfen
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/

#include "SDConfig.h"


#if (SDMMC_DRIVER==1)
#include "SysInclude.h"

/*------------------------------------ Defines -------------------------------*/


/*----------------------------------- Typedefs -------------------------------*/


/*-------------------------- Forward Declarations ----------------------------*/


/* ------------------------------- Globals ---------------------------------- */


/*-------------------------------- Local Statics: ----------------------------*/
_ATTR_SD_BSS_  SDOAM_EVENT_T gEvent[SDC_MAX];
_ATTR_SD_DATA_ int32 SDA_DMAChannel = -1;

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

/*
Name:       SDA_Delay
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
void SDA_Delay(uint32 us)
{
    DelayUs(us);
}


/*
Name:       SDA_CreateEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
pEVENT SDA_CreateEvent(uint8 nSDCPort)
{
    gEvent[nSDCPort].event = 0;
    return (&gEvent[nSDCPort].event);
}

/*
Name:       SDA_SetEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
void SDA_SetEvent(pEVENT handle, uint32 event)
{
    *handle |= event;
}

/*
Name:       SDOAM_GetEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
int32 SDOAM_GetEvent(pEVENT handle, uint32 event, uint32 timeout)
{
    if (NO_TIMEOOUT == timeout)
    {
        while (!(*((volatile uint32 *)handle) & event))
        {
             SDA_Delay(1);
        }
    }
    else if (0 == timeout) 
    {
        if (!(*((volatile uint32 *)handle) & event))
        {
            return SDC_NO_EVENT;
        }     
    }
    else
    {
        while ((!(*((volatile uint32 *)handle) & event)))
        {
            if (timeout == 0)
                return SDC_NO_EVENT;
            timeout--;
            SDA_Delay(1); 
        }
    }
    
    *handle &= ~(event);
    return SDC_SUCCESS;
}


/*
Name:       SDA_DMAStart
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
bool SDA_DMAStart(SDMMC_PORT_E nSDCPort, uint32 DstAddr, uint32 SrcAddr, uint32 size, bool rw, pFunc CallBack)
{
    uint32 timeout = 0;
    eDMA_CHN channel;
        
    if (rw)
    {
        //write
        DMA_CFGX DmaCfg = {DMA_CTLL_SDMMC_WR, DMA_CFGL_SDMMC_WR, DMA_CFGH_SDMMC_WR,0};
        channel = 1;//DmaGetChannel();
        //if ((channel != DMA_FALSE)/* && (channel < DMA_CHN2)*/)
        {
            SDA_DMAChannel = DmaStart((uint32)(channel),(uint32)SrcAddr,(uint32)DstAddr, size,&DmaCfg, CallBack);
        }
    }
    else
    {
        //read        
        DMA_CFGX DmaCfg = {DMA_CTLL_SDMMC_RD, DMA_CFGL_SDMMC_RD, DMA_CFGH_SDMMC_RD,0};
        channel = 1;//DmaGetChannel();
        //if ((channel != DMA_FALSE)/* && (channel < DMA_CHN2)*/)
        {
            SDA_DMAChannel = DmaStart((uint32)(channel),(uint32)SrcAddr,(uint32)DstAddr,size,&DmaCfg, CallBack);
        }
    }
    return TRUE;
}

/*
Name:       SDA_DMAStop
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
bool SDA_DMAStop(SDMMC_PORT_E nSDCPort)
{
    uint32 timeout = 20000;
    
    if (SDA_DMAChannel != -1)
    {
        while(timeout--)
        {
            if (DMA_SUCCESS == DmaGetState(SDA_DMAChannel))
            {
                break;
            }
            DelayUs(10);
        }
    }
    SDA_DMAChannel = -1;
    
    return TRUE;
}

/*
Name:       SDA_GetSrcClkFreq
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
uint32 SDA_GetSrcClkFreq(void)
{
    return 1000*GetSdmmcFreq();  //usbplus = 24MHz
}


/*
Name:       SDA_SetSrcClkDiv
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
int32 SDA_SetSrcClkDiv(SDMMC_PORT_E nSDCPort, uint32 div)
{
    //CRUReg->CRU_CLKSEL_MMC_CON = (div-1)<<1 | 0x1f<<17;
	return SDC_SUCCESS;
}

/*
Name:       SDA_SetSrcClk
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
void SDA_SetSrcClk(SDMMC_PORT_E nSDCPort, bool enable)
{
    CRUReg->CRU_CLKGATE_CON1 = ((TRUE == enable)? 0:1)<<2 | 1<<18; //clk_sdmmc_gate
}

/*
Name:       SDA_RegISR
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
bool SDA_RegISR(SDMMC_PORT_E nSDCPort, pFunc Routine)
{
    IntRegister(INT_ID18_SDMMC, (void*)Routine);
    return TRUE;

}

/*
Name:       SDA_EnableIRQ
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
void SDA_EnableIRQ(SDMMC_PORT_E nSDCPort)
{
	//EnableCPUInt();
    nvic->Irq.Enable[0] = 0x00000004;

}


/*
Name:       SDA_SetIOMux
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
bool SDA_SetIOMux(SDMMC_PORT_E SDCPort, HOST_BUS_WIDTH width)
{
    return TRUE;
}

/*
Name:       SDA_SetIOMux
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_DRIVERLIB_CODE_
bool SDA_CheckCard(SDMMC_PORT_E SDCPortd)
{
    #if (CONFIG_EMMC_SPEC==1)
        return TRUE;
    #else

        #ifdef RKNANOC_BT_EVB_V10
        return TRUE;
        #endif

        #ifdef RKNANOC_EVK_V20
        return (0 == Gpio_GetPinLevel(SD_DET))? TRUE:FALSE;
        #endif
    #endif
}

/*
Name:       SDA_Init
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
void SDA_Init(void)
{
    //CRUReg->CRU_CLKSEL_MMC_CON = 1<<0 | 0<<1 | 1<<16 | 0x1f<<17; // 1: 24MHz, mmc_div_sel=0, clk_mmc = pllout/(1+mmc_div_sel)
    #if (CONFIG_EMMC_SPEC==1)
    GRFReg->GPIO0A_IOMUX = 1<<0 | 1<<6 | 1<<8 | 1<<12 | 3<<16 | 3<<22 | 3<< 24 | 3<<28; ////sdmmc_pwr_en sdmmc_cclk_p1, sdmmc_rstn_p1,sdmmc_cmd_p1
    GRFReg->IOMUX_CON0 = 0xFFFFFFFF; //sdmmc_data0_p1~sdmmc_data7_p1
    GRFReg->IOMUX_CON1 = 1<<0 | 1<<16; //sdmmc_xxx_p1 available for sdmmc
    #else
    //sdmmc_cmd_p0, sdmmc_clk_p0, sdmmc_data0_p0~sdmmc_data3_p0
    GRFReg->GPIO0C_IOMUX = 1<<2 | 3<<4 | 3<<6 | 3<<8 | 3<<10 | 3<<12 \
                           | 3<<18 | 3<<20 | 3<<22 | 3<<24 | 3<<26 | 3<<28;  
    GRFReg->IOMUX_CON1 = 0<<0 | 1<<16;//sdmmc_xxx_p0 available for sdmmc
    #endif
}

#endif
