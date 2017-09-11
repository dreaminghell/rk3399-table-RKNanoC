/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   main.c
*
* Description:
*
* History:      <author>          <time>        <version>
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_MAIN_

#include "SysInclude.h"
#include "FsInclude.h"
#include "MainMenu.h"
#include "PowerOn_Off.h"
#include "Hold.h"



#include "FunUSB.h"

#ifdef _RADIO_
#include "FmControl.h"
#endif

#include "tc358860xbg.h"

#ifdef MEDIA_MODULE
extern  void FavoBlockInit(void);
#endif

/*
--------------------------------------------------------------------------------
  Function name : __error__(char *pcFilename, unsigned long ulLine)
  Author        : ZHengYongzhi
  Description   : test program.

  Input         :
  Return        : TRUE

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#ifdef DEBUG
_ATTR_SYS_CODE_
void __error__(char *pcFilename, unsigned long ulLine)
{
    while(1);
}
#endif

_ATTR_SYS_INIT_DATA_ uint16 MediaInitStr[]   = {'M','e','d','i','a',' ','I','n','i','t','.','.','.',0};
_ATTR_SYS_INIT_DATA_ uint16 LowFormatStr[]   = {'L','o','w','F','o','r','m','a','.','.','.',0};
_ATTR_SYS_INIT_DATA_ uint16 SysAreaCopyStr[] = {'S','y','s','A','r','e','a','C','o','p','y','.','.','.',0};

//lable must be 8.3 format.
_ATTR_SYS_INIT_DATA_ uint8  VolumeName[] =
{
	'R',
	'o',
	'c',
	'k',
	'C',
	'h',
	'i',
	'p',
    '\0'
};

/*
--------------------------------------------------------------------------------
  Function name : void SysCpuInit(void)
  Author        : ZHengYongzhi
  Description   : PLL seting. disable interruption,PWM Etc.

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysCpuInit(void)
{
    Scu_OOL_Power_Set(1);    //power on

    ScuLDOEnable();
    ScuDCoutEnable();
    PMU_Enable();

    ScuDCoutSet(SCU_DCOUT_120);
    ScuLDOSet(SCU_LDO_30);

    //#ifdef DISPLAY_ENABLE
    #if 1
    BacklightLDOBackup = SCU_LDO_30;
    #endif

    Scu_Force_PWM_Mode_Set(0);
    Scu_Threshold_Set(PFM_26MA);
    Scu_Battery_Sns_Set(1);

    SysFreqSet(24);
    SetAdcFreq(1);          // 1M
    SetI2sFreq(12);         // I2s clk = 12M
    SetUartFreq(24);        // uart clk = 24M

    Scu_otgphy_suspend(1);
    Scu_mic_pga_volset(0);
    Scu_mic_boosten_set(0);
    Scu_mic_pga_powerup(0);

    ScuClockGateInit();
 }

/*
--------------------------------------------------------------------------------
  Function name : void SysVariableInit(void)
  Author        : ZHengYongzhi
  Description   : system global variable initialization.

  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysVariableInit(void)
{
    //auto power off time initalization.
    HoldState                      = HOLD_STATE_OFF;
    SysTickCounter                 = 0;
    AutoPowerOffDisableCounter     = 0;

    gBattery.IsBattChange          = TRUE;
    chip_freq.armclk = 0;
    gSysConfig.UsbSensor           = 0;
	gSysConfig.StartTP             = 0;

    //#ifdef DISPLAY_ENABLE
    #if 0
    IsBackLightOn  = FALSE;
    LCD_TEXTFORT   = 0xffff;
    LCD_IMAGEINDEX = 0xffff;
    #endif
}

/*
--------------------------------------------------------------------------------
  Function name : BoardInit
  Author        : ZHengYongzhi
  Description   : hardware initializtion
                  GPIO and AD initializtion，low power level check.
                  LCD and Codec initializtion.
  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void GpioInit(void)
{

#ifdef RKNANOC_EVK_V20

    //-------------------------------------------------------------------------
    //Flash: Nand
    GpioMuxSet(GPIOPortA_Pin0,IOMUX_GPIOA0_FLAS_CS0);
    GpioMuxSet(GPIOPortA_Pin1,IOMUX_GPIOA1_FLAS_CS1);
    GpioMuxSet(GPIOPortA_Pin2,IOMUX_GPIOA2_FLAS_ALE);
    GpioMuxSet(GPIOPortA_Pin3,IOMUX_GPIOA3_FLAS_RDY);
    GpioMuxSet(GPIOPortA_Pin4,IOMUX_GPIOA4_FLAS_RDN);
    GpioMuxSet(GPIOPortA_Pin5,IOMUX_GPIOA5_FLAS_WRN);
    GpioMuxSet(GPIOPortA_Pin6,IOMUX_GPIOA6_FLAS_CLE);

    //-------------------------------------------------------------------------
    //LCD:
    GpioMuxSet(GPIOPortA_Pin7,IOMUX_GPIOA7_LCD_CSN);
    GpioMuxSet(GPIOPortB_Pin0,IOMUX_GPIOB0_LCD_WRN);

    //-------------------------------------------------------------------------
    //I2S & SDMMC:default GPIO input
	GpioMuxSet(GPIOPortC_Pin1,IOMUX_GPIOC1_IO);
    GpioMuxSet(GPIOPortC_Pin2,IOMUX_GPIOC2_IO);
    GpioMuxSet(GPIOPortC_Pin3,IOMUX_GPIOC3_IO);
    GpioMuxSet(GPIOPortC_Pin4,IOMUX_GPIOC4_IO);
    GpioMuxSet(GPIOPortC_Pin5,IOMUX_GPIOC5_IO);
    GpioMuxSet(GPIOPortC_Pin6,IOMUX_GPIOC6_IO);

    Gpio_SetPinDirection(GPIOPortC_Pin1,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin2,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin3,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin4,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin5,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin6,GPIO_IN);

    GPIO_SetPinPull(GPIOPortC_Pin1,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin2,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin3,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin4,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin5,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin6,ENABLE);

    //-------------------------------------------------------------------------
    //UART:
    GpioMuxSet(GPIOPortB_Pin5,IOMUX_GPIOB5_UART_TXD);
    GpioMuxSet(GPIOPortB_Pin6,IOMUX_GPIOB6_UART_RXD);
    //uart cts
    GpioMuxSet(GPIOPortB_Pin7,IOMUX_GPIOB7_IO);
    Gpio_SetPinDirection(GPIOPortB_Pin7,GPIO_IN);
    GPIO_SetPinPull(GPIOPortB_Pin7,ENABLE);
    //uart rts
    GpioMuxSet(GPIOPortC_Pin0,IOMUX_GPIOC0_IO);
    Gpio_SetPinDirection(GPIOPortC_Pin0,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin0, GPIO_LOW);

    //-------------------------------------------------------------------------
    //I2C:
    //i2c&flas_cs iomux: defaule GPIO input
    GpioMuxSet(GPIOPortC_Pin7,IOMUX_GPIOC7_IO);
    Gpio_SetPinDirection(GPIOPortC_Pin7,GPIO_IN);
    GPIO_SetPinPull(GPIOPortC_Pin7,ENABLE);

    GpioMuxSet(GPIOPortD_Pin0,IOMUX_GPIOD0_IO);
    Gpio_SetPinDirection(GPIOPortD_Pin0,GPIO_IN);
    GPIO_SetPinPull(GPIOPortD_Pin0,ENABLE);

    //-------------------------------------------------------------------------
    //SPI: no use
    GpioMuxSet(GPIOPortB_Pin2,IOMUX_GPIOB2_IO);     //no use
    Gpio_SetPinDirection(GPIOPortB_Pin2,GPIO_IN);
    GPIO_SetPinPull(GPIOPortB_Pin2,ENABLE);

    GpioMuxSet(GPIOPortB_Pin4,IOMUX_GPIOB4_IO);     //no use
    Gpio_SetPinDirection(GPIOPortB_Pin4,GPIO_IN);
    GPIO_SetPinPull(GPIOPortB_Pin4,ENABLE);

    //-------------------------------------------------------------------------
    //IO CONTROL:
    //BT_POWER
    GpioMuxSet(BT_POWER,IOMUX_GPIOB3_IO);
    Gpio_SetPinDirection(BT_POWER, GPIO_OUT);
    Gpio_SetPinLevel(BT_POWER, GPIO_LOW);

    //SD Dect
    GpioMuxSet(GPIOPortD_Pin2,IOMUX_GPIOD2_IO);
    Gpio_SetPinDirection(GPIOPortD_Pin2, GPIO_IN);
    GPIO_SetPinPull(GPIOPortD_Pin2, ENABLE);

    //pwm_ctrl
    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD3_IO);
	Gpio_SetPinDirection(BACKLIGHT_PIN , GPIO_OUT);
	Gpio_SetPinLevel(BACKLIGHT_PIN, GPIO_LOW);

    //Virtual uart
    GpioMuxSet(VIRTUAL_UART_TX,IOMUX_GPIOB1_IO);
    Gpio_SetPinDirection(VIRTUAL_UART_TX,GPIO_OUT);
    Gpio_SetPinLevel(VIRTUAL_UART_TX, GPIO_LOW);

    //no use
    GpioMuxSet(GPIOPortA_Pin1,IOMUX_GPIOA1_IO);
    Gpio_SetPinDirection(GPIOPortA_Pin1,GPIO_IN);
    GPIO_SetPinPull(GPIOPortA_Pin1,ENABLE);

    //-------------------------------------------------------------------------
    //8bit data port iomux
    //DataPortMuxSet(IO_LCD);

#endif

#ifdef RKNANOC_BT_EVB_V10

    //-------------------------------------------------------------------------
    //Flash: spi0 flash IO configer
    GpioMuxSet(GPIOPortA_Pin3,IOMUX_GPIOA3_SPI_RXDP0);
    GpioMuxSet(GPIOPortA_Pin4,IOMUX_GPIOA4_SPI_TXDP0);
    GpioMuxSet(GPIOPortA_Pin5,IOMUX_GPIOA5_SPI_CLKP0);
    GpioMuxSet(GPIOPortA_Pin6,IOMUX_GPIOA6_SPI_CSN0);

    //-------------------------------------------------------------------------
    //LCD:
    GpioMuxSet(GPIOPortA_Pin2,IOMUX_GPIOA2_LCDRS);
    GpioMuxSet(GPIOPortB_Pin0,IOMUX_GPIOB0_LCD_WRN);

    //-------------------------------------------------------------------------
    //I2S & SDMMC:default GPIO input
	GpioMuxSet(GPIOPortC_Pin1,IOMUX_GPIOC1_IO);
    GpioMuxSet(GPIOPortC_Pin2,IOMUX_GPIOC2_IO);
    GpioMuxSet(GPIOPortC_Pin3,IOMUX_GPIOC3_IO);
    GpioMuxSet(GPIOPortC_Pin4,IOMUX_GPIOC4_IO);
    GpioMuxSet(GPIOPortC_Pin5,IOMUX_GPIOC5_IO);
    GpioMuxSet(GPIOPortC_Pin6,IOMUX_GPIOC6_IO);

    Gpio_SetPinDirection(GPIOPortC_Pin1,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin2,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin3,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin4,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin5,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortC_Pin6,GPIO_IN);

    GPIO_SetPinPull(GPIOPortC_Pin1,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin2,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin3,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin4,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin5,ENABLE);
    GPIO_SetPinPull(GPIOPortC_Pin6,ENABLE);

    //-------------------------------------------------------------------------
    //UART:
    GpioMuxSet(GPIOPortB_Pin5,IOMUX_GPIOB5_UART_TXD);
    GpioMuxSet(GPIOPortB_Pin6,IOMUX_GPIOB6_UART_RXD);
    //uart cts
    GpioMuxSet(GPIOPortB_Pin7,IOMUX_GPIOB7_IO);
    Gpio_SetPinDirection(GPIOPortB_Pin7,GPIO_IN);
    GPIO_SetPinPull(GPIOPortB_Pin7,ENABLE);
    //uart rts
    GpioMuxSet(GPIOPortC_Pin0,IOMUX_GPIOC0_IO);
    Gpio_SetPinDirection(GPIOPortC_Pin0,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin0, GPIO_LOW);

    //-------------------------------------------------------------------------
    //I2C:
    //i2c&flas_cs iomux: defaule GPIO input
    GpioMuxSet(GPIOPortC_Pin7,IOMUX_GPIOC7_I2C_SCL);
    GpioMuxSet(GPIOPortD_Pin0,IOMUX_GPIOD0_I2C_SDA);

    //-------------------------------------------------------------------------
    //IO CONTROL:
    // PA_CTRL
    GpioMuxSet(PA_CTRL,IOMUX_GPIOB3_IO);
    Gpio_SetPinDirection(PA_CTRL,GPIO_OUT);
    Gpio_SetPinLevel(PA_CTRL, GPIO_LOW);

    //BT_POWER
    GpioMuxSet(BT_POWER,IOMUX_GPIOD2_IO);
    Gpio_SetPinDirection(BT_POWER, GPIO_OUT);
    Gpio_SetPinLevel(BT_POWER, GPIO_LOW);

    //MIC_CTRL
    GpioMuxSet(MIC_CTRL,IOMUX_GPIOA0_IO);
    Gpio_SetPinDirection(MIC_CTRL,GPIO_OUT);
    Gpio_SetPinLevel(MIC_CTRL, GPIO_LOW);

    //pwm_ctrl
    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD3_IO);
	Gpio_SetPinDirection(BACKLIGHT_PIN , GPIO_OUT);
	Gpio_SetPinLevel(BACKLIGHT_PIN, GPIO_LOW);

    //RTC_INT
    GpioMuxSet(RTC_INT_DET,IOMUX_GPIOB1_IO);
    Gpio_SetPinDirection(RTC_INT_DET,GPIO_IN);
    GPIO_SetPinPull(RTC_INT_DET,ENABLE);

    //PHONE_DET
    GpioMuxSet(PHONE_DET,IOMUX_GPIOB2_IO);
    Gpio_SetPinDirection(PHONE_DET,GPIO_IN);
    GPIO_SetPinPull(PHONE_DET,ENABLE);

    //LINEIN_DET
    GpioMuxSet(LINEIN_DET,IOMUX_GPIOB4_IO);
    Gpio_SetPinDirection(LINEIN_DET,GPIO_IN);
    GPIO_SetPinPull(LINEIN_DET,ENABLE);

    //no use
    GpioMuxSet(GPIOPortA_Pin1,IOMUX_GPIOA1_IO);
    Gpio_SetPinDirection(GPIOPortA_Pin1,GPIO_IN);
    GPIO_SetPinPull(GPIOPortA_Pin1,ENABLE);

    //-------------------------------------------------------------------------
    //8bit data port iomux
    DataPortMuxSet(IO_LCD);
#endif

}

/*
--------------------------------------------------------------------------------
  Function name : BoardInit
  Author        : ZHengYongzhi
  Description   : hardware initialization.
                  GPIO and AD initialization. low power level check.
                  lcd and Codec initial.
  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void BoardInit(void)
{
    //Gpio initialization.
    GpioInit();

    UARTInit(24*1000*1000,115200,UART_DATA_8B,UART_ONE_STOPBIT,UART_PARITY_DISABLE);
    //DEBUG("board initial...");

    //ADC initialization.
    AdcInit();

    //power on level check.
    BatteryInit();

    //Codec initialization.
    I2sCs(I2S_IN);
    I2SInit(I2S_SLAVE_MODE);
    ScuClockGateCtr(CLOCK_GATE_I2S, 0);

    Codec_PowerOnInitial();

    //key initialization.
    KeyInit();
}

/*
--------------------------------------------------------------------------------
  Function name : BoardInit
  Author        : ZHengYongzhi
  Description   : Flash and file system Etc initialization.
  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#if (defined(_SPINOR_) && defined(DISPLAY_ENABLE))
void ResourFilesOpen(void)
{
    if ((hImageFile = FileOpen("\\","IMAGEXXXUIS", "R")) == -1)//打开文件成功，加载磁盘
    {
        hImageFile = -1;
        DEBUG("Open File ERROR");
    }
    if ((hFont16File = FileOpen("\\","FONT16XXBIN", "R")) == -1)//打开文件成功，加载磁盘
    {
        hFont16File = -1;
        DEBUG("Open File ERROR");
    }
    if ((hFont12File = FileOpen("\\","FONT12XXBIN", "R")) == -1)//打开文件成功，加载磁盘
    {
        hFont12File = -1;
        DEBUG("Open File ERROR");
    }
    if ((hGBKFile = FileOpen("\\","GBKXXXXXBIN", "R")) == -1)//打开文件成功，加载磁盘
    {
        hGBKFile = -1;
        DEBUG("Open File ERROR");
    }
    if ((hMenuFile = FileOpen("\\","MENUXXXXRES", "R")) == -1)//打开文件成功，加载磁盘
    {
        hMenuFile = -1;
        DEBUG("Open File ERROR");
    }
}

void ResourFilesClose(void)
{
    FileClose(hImageFile);
    FileClose(hMenuFile);
    FileClose(hGBKFile);
    FileClose(hFont12File);
    FileClose(hFont16File);

    hImageFile  = -1;
    hMenuFile   = -1;
    hGBKFile    = -1;
    hFont12File = -1;
    hFont16File = -1;
}
#endif

_ATTR_SYS_INIT_CODE_
void MediaInit(void)
{
    uint32 FreqBak, LDOLevelBak;
    uint16 TempColor;
    HANDLE  hMainFile;
    uint8  SysInitDispX     = 2;
    uint8  SysInitDispY     = 2;

    DEBUG("Enter");

    FreqBak = chip_freq.armclk;
    PMU_EnterModule(PMU_MEDIA);

    MemDevInit(MEM_DEV_MASK_ALL_FLASH);

    SetDataDiskSize(SYS_DATA_DISK_SIZE);
    #ifdef _MULT_DISK_
    SetUserDisk2Size(SYS_USER_DISK2_SIZE);
    #endif

    #ifdef MEDIA_MODULE
    MediaInfoAddr = FtlGetMediaBaseSecAddr();
    FavoBlockInit();  //get favorite information saved address.
    #endif

    LoadSysInformation();

    #if (defined(_SPINOR_) && defined(_SDCARD_))
        ModuleOverlay(MODULE_ID_SDMMC, MODULE_OVERLAY_ALL);

        if (CheckCard() == 1)
        {
            MemDevInit(MEM_DEV_MASK_SD);
            MDScanDev(1,MEM_DEV_SCAN_SD);

//			DelayMs(100);	//加延时避免SD卡未扫描完成而导致后面文件系统建立失败。

            gSysConfig.Memory = CARD;
            FileSysSetup(gSysConfig.Memory);
        }
    #else

        gSysConfig.Memory = FLASH0;
        FileSysSetup(gSysConfig.Memory);
    #endif

    #ifdef _MULT_DISK_
    //if there is file _$ROCK$.USB in FLASH_DISK0,then mount FLASH_DISK1 when connect USB.
    #ifdef _CDROM_
    bShowFlash1 = 1;
    bShowCdrom = gSysConfig.bShowCdrom;
    if (0 == bShowCdrom)
    {
        gSysConfig.bShowCdrom = 1;
        SaveSysInformation();
    }

    #else
    bShowFlash1 = 0;
    if ((hMainFile = FileOpen("\\","ROCKCHIPBIN", "R")) != NOT_OPEN_FILE)//after open file ok,mount disk.
    {
        FileClose(hMainFile);
        bShowFlash1 = 1;
    }
    #endif
    #endif

    #ifdef  DISK_VOLUME
    VolumeCreate(VolumeName);
    #endif

    //system poweron, need get memory info
    FlashTotalMemory();
    SendMsg(MSG_FLASH_MEM_UPDATE);
    SendMsg(MSG_SDCARD_MEM_UPDATE);

    FLASH_PROTECT_ON();
    PMU_ExitModule(PMU_MEDIA);

}

/*
--------------------------------------------------------------------------------
  Function name : void ModuleInit(void)
  Author        : ZHengYongzhi
  Description   : power on module initialization, breakpoint resume.
  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void ModuleInit(void)
{
    TASK_ARG TaskArg;

    if (CheckVbus())
    {
        TaskSwitch(TASK_ID_CHARGE, NULL);
    }
    else
    {
        TaskArg.MainMenu.MenuID = MAINMENU_ID_MUSIC;
    	TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
    }

    pMainThread = (THREAD*)NULL;
}

/*
--------------------------------------------------------------------------------
  Function name : void SysTickInit(void)
  Author        : ZHengYongzhi
  Description   : system tick initialization.
  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysTickInit(void)
{
    SysTickDisable();

    SysTickClkSourceSet(NVIC_SYSTICKCTRL_CLKIN);

    SysTickPeriodSet(10);

    SysTickEnable();

    DEBUG("Systick initial: periodset 10ms");
}

/*
--------------------------------------------------------------------------------
  Function name : void IntrruptInit(void)
  Author        : ZHengYongzhi
  Description   : system breakpoint initialization
  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysIntrruptInit(void)
{
    IntMasterEnable();

    //set priority.
    IrqPriorityInit();
    //......
    //register interrupt functions.
    IntRegister(FAULT_ID15_SYSTICK, (void*)SysTickHandler);
    IntRegister(INT_ID16_DMA,       (void*)DmaInt);
    //IntRegister(INT_ID26_GPIO, (void*)GpioInt);   //test GPIO INT
    IntRegister(INT_ID30_ADC,       (void*)AdcIsr);

    //IntRegister(INT_ID46_PWR_CHARGE , hci_serve);

    IntRegister(INT_ID39_OOL_PLAYON,  (void*)playon_wakeup_int);
    IntRegister(INT_ID40_PWR_5V_READY,(void*)vbus_wakeup_int);

    //interrupt enable.
    IntEnable(FAULT_ID4_MPU);
    IntEnable(FAULT_ID5_BUS);
    IntEnable(FAULT_ID6_USAGE);
    IntEnable(FAULT_ID15_SYSTICK);
    IntEnable(INT_ID16_DMA);
    //IntEnable(INT_ID26_GPIO);                     //test GPIO INT
    IntEnable(INT_ID30_ADC);

    //DEBUG("system INT initial: DMA & Systick INT enable");
}

/*
--------------------------------------------------------------------------------
  Function name : void poweron(void)
  Author        : ZHengYongzhi
  Description   : system tick initialization.
  Input         :
  Return        :

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void poweron(void)
{
    uint32 ms, SysFreq;

    if(CheckVbus())
    {
        return;
    }

    ms = 50;

    SysFreq = GetPllSysFreq();

    ms *= SysFreq * 1000 / 3;

    do
    {
        //printf("xxx\n");
        if(Scu_PlayOn_Level_Get())
        {
            Scu_OOL_Power_Set(0);
        }
    }while (ms--);
}



/*
--------------------------------------------------------------------------------
  Function name : INT Main(void)
  Author        : ZHengYongzhi
  Description   : this the main funtion for tasks manage,and call task main window callback function.

  Input         :
  Return        : TRUE

  History:     <author>         <time>         <version>
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
UINT32 Main(void)
{
	int force_calib = 0;
	UINT32 KeyVal;
    //initialization for global variable.
    SysVariableInit();

    //PLL setting,disable interrupt and PWM.
    SysCpuInit();

    //ADC initializtion, low power check,LCD initialization, Codec initialization.
    BoardInit();

    poweron();

    //interruption initialization.
    SysIntrruptInit();

    IntPendingClear(INT_ID26_GPIO);
    IntRegister(INT_ID26_GPIO, (void*)GpioInt);
    IntEnable(INT_ID26_GPIO);

#if defined(_FUSB302_)
	PMU_EnterModule(PMU_USB);
    fusb302_init();  //Fusb302 init
    PMU_ExitModule(PMU_USB);
#endif

    //Flash initial,get system parameter, file system initial
    MediaInit();

    //module initialization.
    ModuleInit();

    //system tick initialization.
    SysTickInit();

    SysServiceInit();

	//print version
	printf("\n");
	printf("--------------------V1.3-------------------\n");
	printf("------------------RAYKEN LCD---------------\n");
	printf("\n");

    //Moudle init after System init

    printf("Toshiba vedio changeIC init start\n");
#ifdef _TC358870XBG_
	PMU_EnterModule(PMU_USB);
    Tc358870XBG_init(); //Tc358870xbg init
    PMU_ExitModule(PMU_USB);
#endif

#ifdef _TC358860XBG_
	PMU_EnterModule(PMU_USB);
    Tc358860XBG_init();
	PMU_ExitModule(PMU_USB);
#endif
    printf("Toshiba vedio changeIC init end\n");

#ifdef _MPU6500_
	PMU_EnterModule(PMU_USB);
	KeyVal = GetKeyVal();
	//printf("KeyVal=%d\n", KeyVal);
	if(KeyVal&KEY_VAL_NB_DOWN) {
		force_calib = 1;
	} else {
		force_calib = 0;
	}
	//printf("force_calib=%d\n", force_calib);
	MPU6500_Init(force_calib);	//MPU6500(accel and gyro sensor) init
	PMU_ExitModule(PMU_USB);
#endif

#ifdef _TOUCH_GSL1680_
	StartTP();
#endif


    DEBUG("System Start!");
    OSStart(&pMainWin, &pMainThread);

    microlibuse();

    return (TRUE);
}
/*
********************************************************************************
*
*                         End of main.c
*
********************************************************************************
*/

