/*
TC358860XBG Slave Port address is 0x68, (binary 1101_000x) where x = 1 for read and x = 0 for write.
The slave address can be changed to 0x0E (binary 0001_110x) by a weak pull up to pin HPD during boot
time. 0xE1 and 0x1E
*/
//uint8 SLave_Address = 0x1F;   //tc585570 int low
//uint8 SLave_Address = 0xD1;   //tc358860 int low
/*
TC358870XBG Support 2 I2C Slave Addresses (0x0F & 0x1F)
selected through boot-strap pin (INT)
*/


#include "DriverInclude.h"

#include "Tc358870xbg.h"
#include "Gpio.h"

#define HDMI_3GLASS
//#define HDMI_MIPI
//#define HDMI_MIPI_COLORBAR

void i2c1_uh2d_read8(uint16 address,uint8* data)
{
    i2c1_reg16_read8 (0x1F, address, data); //1F
}

void i2c1_uh2d_read16(uint16 address,uint8* data)
{
    i2c1_reg16_read16 (0x1F, address, data);
}

void i2c1_uh2d_read32(uint16 address,uint8* data)
{
    i2c1_reg16_read32 (0x1F, address, data);
}

void i2c1_uh2d_write8(uint16 address,uint8 data)
{
    i2c1_reg16_write8 (0x1F, address, data);
}

void i2c1_uh2d_write16(uint16 address,uint16 data)
{
    i2c1_reg16_write16 (0x1F, address, data);
}

void i2c1_uh2d_write32(uint16 address,uint32 data)
{
    i2c1_reg16_write32 (0x1F, address, data);
}



void dcs_write_1P(uint8 cmd, uint16 delay)
{

    uint16 regdata;

    regdata=0x0000;
    regdata|=cmd;

    i2c1_uh2d_write16(0x0504,0x0005);
    i2c1_uh2d_write16(0x0504,regdata);

    DelayUs(200);
    if (delay != 0)
        DelayMs(delay);

}

void dcs_write_2P(uint8 cmd1, uint8 cmd2, uint16 delay)
{
    uint16 regdata=0x0000;
    uint16 tmp = 0x0000;
    regdata=cmd1;
    tmp = cmd2;
    regdata|=(tmp<<8);

    i2c1_uh2d_write16(0x0504,0x0015);
    i2c1_uh2d_write16(0x0504,regdata);

    DelayUs(200);
    if (delay != 0)
        DelayMs(delay);
}

void LCD_init_LP(void)
{
#if 1

/*0*/   dcs_write_1P(0x11, 200); // dcs_write_1P(cmd, delayms)

/*1*/   dcs_write_2P(0xff, 0x01, 0);

/*2*/   dcs_write_2P(0xfb, 0x01,10);

/*3*/   dcs_write_2P(0x00, 0x4a, 0);

/*4*/   dcs_write_2P(0x01, 0x33, 0);

/*5*/   dcs_write_2P(0x02, 0x53, 0);


/*6*/   dcs_write_2P(0x03, 0x55, 0);
/*7*/   dcs_write_2P(0x04, 0x55, 0);

/*8*/   dcs_write_2P(0x05, 0x33, 0);

/*9*/   dcs_write_2P(0x06, 0x22, 0);


/*10*/  dcs_write_2P(0x08, 0x16, 10);

/*11*/  dcs_write_2P(0x09, 0x94, 0);

/*12*/  dcs_write_2P(0x0b, 0x97, 0);


/*13*/  dcs_write_2P(0x0c, 0x97, 0);

        dcs_write_2P(0x0d, 0x2f, 0);     //14


        dcs_write_2P( 0x0e, 0x3d, 0);    //15

        dcs_write_2P( 0x0f, 0xa7, 0);    //16



        dcs_write_2P( 0x36, 0x73, 10);   //17


        dcs_write_2P( 0x6d, 0x33, 0);    //18


        dcs_write_2P( 0x6f, 0x02, 0);    //19


        dcs_write_2P( 0x37, 0x02, 10);   //20


        dcs_write_2P( 0xff, 0x05, 0);    //21


        dcs_write_2P( 0xfb, 0x01, 10);   //22


        dcs_write_2P( 0x01, 0x00, 0);//23


        dcs_write_2P( 0x02, 0x8d, 0);//24


        dcs_write_2P( 0x03, 0x8d, 0);//25


        dcs_write_2P( 0x04, 0x8d, 0);//26


        dcs_write_2P( 0x06, 0x33, 0);//27


        dcs_write_2P( 0x07, 0x01, 0);//28


        dcs_write_2P( 0x08, 0x00, 0);//29


        dcs_write_2P( 0x09, 0x49, 10);//30

        dcs_write_2P( 0x0a, 0x46, 0);//31


        dcs_write_2P( 0x0d, 0x0d, 0);//32

        dcs_write_2P( 0x0e, 0x16, 0);//33


        dcs_write_2P( 0x0f, 0x17, 0);//34


        dcs_write_2P( 0x10, 0x53, 0);//35


        dcs_write_2P( 0x11, 0x00, 0);//36


        dcs_write_2P( 0x12, 0x00, 0);//37


        dcs_write_2P( 0x14, 0x01, 10);//38


        dcs_write_2P( 0x15, 0x00, 0);//39


        dcs_write_2P( 0x16, 0x05, 0);//40


        dcs_write_2P( 0x17, 0x00, 0);//41


        dcs_write_2P( 0x19, 0x7f, 0);//42


        dcs_write_2P( 0x1a, 0xff, 0);//43


        dcs_write_2P( 0x1b, 0x0f, 0);//44


        dcs_write_2P( 0x1c, 0x00, 0);//45


        dcs_write_2P( 0x1d, 0x00, 10);//46

        dcs_write_2P( 0x1e, 0x00, 0);//47


        dcs_write_2P( 0x1f, 0x07, 0);//48


        dcs_write_2P( 0x20, 0x00, 0);//49


        dcs_write_2P( 0x21, 0x00, 0);//50

        dcs_write_2P( 0x22, 0x55, 0);//51


        dcs_write_2P( 0x23, 0x0d, 0);//52


        dcs_write_2P( 0x2d, 0x02, 0);//53


        dcs_write_2P( 0x3b, 0x8d, 10);//54


        dcs_write_2P( 0x3d, 0x8a, 0);//55


        dcs_write_2P( 0x7b, 0xa1, 10);//56


        dcs_write_2P( 0x83, 0x03, 10);//57


        dcs_write_2P( 0xa0, 0xc2, 0);//58


        dcs_write_2P( 0xa2, 0x10, 0);//59


        dcs_write_2P( 0xa6, 0x25, 0);//60


        dcs_write_2P( 0xbb, 0x08, 0);//61


        dcs_write_2P( 0xbc, 0x08, 10);//62


        dcs_write_2P( 0x28, 0x01, 0);//63


        dcs_write_2P( 0x32, 0x08, 0);//64


        dcs_write_2P( 0x33, 0xb8, 0);//65


        dcs_write_2P( 0x36, 0x05, 0);//66


        dcs_write_2P( 0x37, 0x03, 0);//67


        dcs_write_2P( 0x43, 0x00, 0);//68


        dcs_write_2P( 0x4b, 0x21, 0);//69


        dcs_write_2P( 0x4c, 0x03, 10);//70


        dcs_write_2P( 0x50, 0x21, 0);//71


        dcs_write_2P( 0x51, 0x03, 0);//72


        dcs_write_2P( 0x58, 0x21, 0);//73


        dcs_write_2P( 0x59, 0x03, 0);//74


        dcs_write_2P( 0x5d, 0x21, 0);//75


        dcs_write_2P( 0x5e, 0x03, 0);//76


        dcs_write_2P( 0x6c, 0x03, 0);//77


        dcs_write_2P( 0x6d, 0x03, 10);//78


        dcs_write_2P( 0x6f, 0x25, 0);//79


        dcs_write_2P( 0x89, 0xc1, 0);//80


        dcs_write_2P( 0x9e, 0x00, 0);//81


        dcs_write_2P( 0x9f, 0x10, 0);//82


        dcs_write_2P( 0x2a, 0x11, 0);//83


        dcs_write_2P( 0x35, 0x0b, 0);//84


        dcs_write_2P( 0x2f, 0x01, 0);//85


        dcs_write_2P( 0x2d, 0x03, 10);//86


        dcs_write_2P( 0xff, 0x00, 0);//87


        dcs_write_2P( 0xfb, 0x01, 0);//88


        dcs_write_2P( 0xff, 0x01, 0);//89


        dcs_write_2P( 0xfb, 0x01, 0);//90


        dcs_write_2P( 0xff, 0x02, 0);//91


        dcs_write_2P( 0xfb, 0x01, 0);//92


        dcs_write_2P( 0xff, 0x03, 0);//93


        dcs_write_2P( 0xfb, 0x01, 0);//94


        dcs_write_2P( 0xff, 0x04, 0);//95


        dcs_write_2P( 0xfb, 0x01, 0);//96


        dcs_write_2P( 0xff, 0x05, 0);//97


        dcs_write_2P( 0xfb, 0x01, 19);//98


        dcs_write_2P( 0xff, 0x00, 0);//99


        dcs_write_2P( 0xfb, 0x01, 0);//100


        dcs_write_2P( 0xba, 0x03, 0);//101
#endif
        dcs_write_1P(0x11, 200);
        dcs_write_1P(0x29, 200);

}




/**************************************************************************************
*   Name    : Tc358870XBG_init
*   Function: Tc358870XBG init Use I2C bus.
*   Return  : TRUE  -- send seccess
*             FALSE -- send failure
*   explain :
**************************************************************************************/
BOOL Tc358870XBG_init()//2ping add  寄存器数据模式
{
    BOOL ret = TRUE;
    uint8 sys_status    = 0;

    //拉低INT 配置Slave为0x1F
    GpioMuxSet(GPIOPortC_Pin5,IOMUX_GPIOC5_IO);
    Gpio_SetPinDirection(GPIOPortC_Pin5,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin5, GPIO_LOW);
    DelayMs(10);
    //拉低ResetN
    GpioMuxSet(GPIOPortC_Pin6,IOMUX_GPIOC6_IO);
    Gpio_SetPinDirection(GPIOPortC_Pin6,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin6, GPIO_LOW);
    DelayMs(100);
    Gpio_SetPinLevel(GPIOPortC_Pin6, GPIO_HIGH);
    Gpio_SetPinDirection(GPIOPortC_Pin5,GPIO_IN);
    //GPIO_SetPinPull(GPIOPortC_Pin5,ENABLE);

    //打开背光
    LCD_BL_On();
    DelayMs(100);

/***************************************************************/
#ifdef HDMI_3GLASS
    // Initialization to receive HDMI signal
    // Software Reset
    i2c1_uh2d_write16(0x0004,0x0004); // ConfCtl0
    i2c1_uh2d_write16(0x0002,0x3F01); // SysCtl
    i2c1_uh2d_write16(0x0002,0x0000); // SysCtl
    i2c1_uh2d_write16(0x0006,0x0008); // ConfCtl1
    // DSI-TX0 Transition Timing
    i2c1_uh2d_write32(0x0108,0x00000001); // DSI_TX_CLKEN
    i2c1_uh2d_write32(0x010C,0x00000001); // DSI_TX_CLKSEL
    i2c1_uh2d_write32(0x02A0,0x00000001); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x02AC,0x000090BD); // MIPI_PLL_CNF
    DelayMs(1);
    i2c1_uh2d_write32(0x02A0,0x00000003); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x0118,0x00000014); // LANE_ENABLE
    i2c1_uh2d_write32(0x0120,0x00001770); // LINE_INIT_COUNT
    i2c1_uh2d_write32(0x0124,0x00000000); // HSTX_TO_COUNT
    i2c1_uh2d_write32(0x0128,0x00000101); // FUNC_ENABLE
    i2c1_uh2d_write32(0x0130,0x00010000); // DSI_TATO_COUNT
    i2c1_uh2d_write32(0x0134,0x00005000); // DSI_PRESP_BTA_COUNT
    i2c1_uh2d_write32(0x0138,0x00010000); // DSI_PRESP_LPR_COUNT
    i2c1_uh2d_write32(0x013C,0x00010000); // DSI_PRESP_LPW_COUNT
    i2c1_uh2d_write32(0x0140,0x00010000); // DSI_PRESP_HSR_COUNT
    i2c1_uh2d_write32(0x0144,0x00010000); // DSI_PRESP_HSW_COUNT
    i2c1_uh2d_write32(0x0148,0x00001000); // DSI_PR_TO_COUNT
    i2c1_uh2d_write32(0x014C,0x00010000); // DSI_LRX-H_TO_COUNT
    i2c1_uh2d_write32(0x0150,0x00000160); // FUNC_MODE
    i2c1_uh2d_write32(0x0154,0x00000001); // DSI_RX_VC_ENABLE
    i2c1_uh2d_write32(0x0158,0x000000C8); // IND_TO_COUNT
    i2c1_uh2d_write32(0x0168,0x0000002A); // DSI_HSYNC_STOP_COUNT
    i2c1_uh2d_write32(0x0170,0x000002AB); // APF_VDELAYCNT
    i2c1_uh2d_write32(0x017C,0x00000081); // DSI_TX_MODE
    i2c1_uh2d_write32(0x018C,0x00000001); // DSI_HSYNC_WIDTH
    i2c1_uh2d_write32(0x0190,0x00000104); // DSI_HBPR
    i2c1_uh2d_write32(0x01A4,0x00000000); // DSI_RX_STATE_INT_MASK
    i2c1_uh2d_write32(0x01C0,0x00000015); // DSI_LPRX_THRESH_COUNT
    i2c1_uh2d_write32(0x0214,0x00000000); // APP_SIDE_ERR_INT_MASK
    i2c1_uh2d_write32(0x021C,0x00000080); // DSI_RX_ERR_INT_MASK
    i2c1_uh2d_write32(0x0224,0x00000000); // DSI_LPTX_INT_MASK
    i2c1_uh2d_write32(0x0254,0x00000006); // LPTXTIMECNT
    i2c1_uh2d_write32(0x0258,0x00240204); // TCLK_HEADERCNT
    i2c1_uh2d_write32(0x025C,0x000D0008); // TCLK_TRAILCNT
    i2c1_uh2d_write32(0x0260,0x00140006); // THS_HEADERCNT
    i2c1_uh2d_write32(0x0264,0x00004268); // TWAKEUPCNT
    i2c1_uh2d_write32(0x0268,0x0000000F); // TCLK_POSTCNT
    i2c1_uh2d_write32(0x026C,0x000D0008); // THS_TRAILCNT
    i2c1_uh2d_write32(0x0270,0x00000020); // HSTXVREGCNT
    i2c1_uh2d_write32(0x0274,0x0000001F); // HSTXVREGEN
    i2c1_uh2d_write32(0x0278,0x00060005); // BTA_COUNT
    i2c1_uh2d_write32(0x027C,0x00000002); // DPHY_TX ADJUST
    i2c1_uh2d_write32(0x011C,0x00000001); // DSITX_START
    // DSI-TX1 Transition Timing
    i2c1_uh2d_write32(0x0308,0x00000001); // DSI_TX_CLKEN
    i2c1_uh2d_write32(0x030C,0x00000001); // DSI_TX_CLKSEL
    i2c1_uh2d_write32(0x04A0,0x00000001); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x04AC,0x000090BD); // MIPI_PLL_CNF
    DelayMs(1);
    i2c1_uh2d_write32(0x04A0,0x00000003); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x0318,0x00000014); // LANE_ENABLE
    i2c1_uh2d_write32(0x0320,0x00001770); // LINE_INIT_COUNT
    i2c1_uh2d_write32(0x0324,0x00000000); // HSTX_TO_COUNT
    i2c1_uh2d_write32(0x0328,0x00000101); // FUNC_ENABLE
    i2c1_uh2d_write32(0x0330,0x00010000); // DSI_TATO_COUNT
    i2c1_uh2d_write32(0x0334,0x00005000); // DSI_PRESP_BTA_COUNT
    i2c1_uh2d_write32(0x0338,0x00010000); // DSI_PRESP_LPR_COUNT
    i2c1_uh2d_write32(0x033C,0x00010000); // DSI_PRESP_LPW_COUNT
    i2c1_uh2d_write32(0x0340,0x00010000); // DSI_PRESP_HSR_COUNT
    i2c1_uh2d_write32(0x0344,0x00010000); // DSI_PRESP_HSW_COUNT
    i2c1_uh2d_write32(0x0348,0x00001000); // DSI_PR_TO_COUNT
    i2c1_uh2d_write32(0x034C,0x00010000); // DSI_LRX-H_TO_COUNT
    i2c1_uh2d_write32(0x0350,0x00000160); // FUNC_MODE
    i2c1_uh2d_write32(0x0354,0x00000001); // DSI_RX_VC_ENABLE
    i2c1_uh2d_write32(0x0358,0x000000C8); // IND_TO_COUNT
    i2c1_uh2d_write32(0x0368,0x0000002A); // DSI_HSYNC_STOP_COUNT
    i2c1_uh2d_write32(0x0370,0x000002AB); // APF_VDELAYCNT
    i2c1_uh2d_write32(0x037C,0x00000081); // DSI_TX_MODE
    i2c1_uh2d_write32(0x038C,0x00000001); // DSI_HSYNC_WIDTH
    i2c1_uh2d_write32(0x0390,0x00000104); // DSI_HBPR
    i2c1_uh2d_write32(0x03A4,0x00000000); // DSI_RX_STATE_INT_MASK
    i2c1_uh2d_write32(0x03C0,0x00000015); // DSI_LPRX_THRESH_COUNT
    i2c1_uh2d_write32(0x0414,0x00000000); // APP_SIDE_ERR_INT_MASK
    i2c1_uh2d_write32(0x041C,0x00000080); // DSI_RX_ERR_INT_MASK
    i2c1_uh2d_write32(0x0424,0x00000000); // DSI_LPTX_INT_MASK
    i2c1_uh2d_write32(0x0454,0x00000006); // LPTXTIMECNT
    i2c1_uh2d_write32(0x0458,0x00240204); // TCLK_HEADERCNT
    i2c1_uh2d_write32(0x045C,0x000D0008); // TCLK_TRAILCNT
    i2c1_uh2d_write32(0x0460,0x00140006); // THS_HEADERCNT
    i2c1_uh2d_write32(0x0464,0x00004268); // TWAKEUPCNT
    i2c1_uh2d_write32(0x0468,0x0000000F); // TCLK_POSTCNT
    i2c1_uh2d_write32(0x046C,0x000D0008); // THS_TRAILCNT
    i2c1_uh2d_write32(0x0470,0x00000020); // HSTXVREGCNT
    i2c1_uh2d_write32(0x0474,0x0000001F); // HSTXVREGEN
    i2c1_uh2d_write32(0x0478,0x00060005); // BTA_COUNT
    i2c1_uh2d_write32(0x047C,0x00000002); // DPHY_TX ADJUST
    i2c1_uh2d_write32(0x031C,0x00000001); // DSITX_START
    // Command Transmission Before Video Start
    i2c1_uh2d_write16(0x0500,0x0004); // CMD_SEL
    i2c1_uh2d_write32(0x0110,0x00000016); // MODE_CONFIG
    i2c1_uh2d_write32(0x0310,0x00000016); // MODE_CONFIG
    // LCD Initialization
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x00B0); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x01D6); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0004); //
    i2c1_uh2d_write16(0x0504,0x18B3); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x00B4); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0003); //
    i2c1_uh2d_write16(0x0504,0x3AB6); //
    i2c1_uh2d_write16(0x0504,0x00C3); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x04BE); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0004); //
    i2c1_uh2d_write16(0x0504,0x00C3); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x00C5); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0005); //
    i2c1_uh2d_write16(0x0504,0x00C0); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0024); //
    i2c1_uh2d_write16(0x0504,0x04C1); //
    i2c1_uh2d_write16(0x0504,0x0061); //
    i2c1_uh2d_write16(0x0504,0x2930); //
    i2c1_uh2d_write16(0x0504,0x1910); //
    i2c1_uh2d_write16(0x0504,0x6163); //
    i2c1_uh2d_write16(0x0504,0xE6B4); //
    i2c1_uh2d_write16(0x0504,0x7BDC); //
    i2c1_uh2d_write16(0x0504,0x39EF); //
    i2c1_uh2d_write16(0x0504,0xDAD7); //
    i2c1_uh2d_write16(0x0504,0x8C08); //
    i2c1_uh2d_write16(0x0504,0x08B1); //
    i2c1_uh2d_write16(0x0504,0x8254); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0200); //
    i2c1_uh2d_write16(0x0504,0x2360); //
    i2c1_uh2d_write16(0x0504,0x0003); //
    i2c1_uh2d_write16(0x0504,0x11FF); //
    DelayUs(400);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0009); //
    i2c1_uh2d_write16(0x0504,0x08C2); //
    i2c1_uh2d_write16(0x0504,0x010A); //
    i2c1_uh2d_write16(0x0504,0x0707); //
    i2c1_uh2d_write16(0x0504,0x00F0); //
    i2c1_uh2d_write16(0x0504,0x0004); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x000F); //
    i2c1_uh2d_write16(0x0504,0x70C4); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0501); //
    i2c1_uh2d_write16(0x0504,0x0001); //
    DelayUs(400);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0016); //
    i2c1_uh2d_write16(0x0504,0x5AC6); //
    i2c1_uh2d_write16(0x0504,0x2D2D); //
    i2c1_uh2d_write16(0x0504,0x0103); //
    i2c1_uh2d_write16(0x0504,0x0102); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x1506); //
    i2c1_uh2d_write16(0x0504,0x5A08); //
    DelayUs(400);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0010); //
    i2c1_uh2d_write16(0x0504,0xFFCB); //
    i2c1_uh2d_write16(0x0504,0xFFFF); //
    i2c1_uh2d_write16(0x0504,0x00FF); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x5400); //
    i2c1_uh2d_write16(0x0504,0x07E0); //
    i2c1_uh2d_write16(0x0504,0xE02A); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    DelayUs(400);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x32CC); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x000E); //
    i2c1_uh2d_write16(0x0504,0x82D7); //
    i2c1_uh2d_write16(0x0504,0x21FF); //
    i2c1_uh2d_write16(0x0504,0x8C8E); //
    i2c1_uh2d_write16(0x0504,0x87F1); //
    i2c1_uh2d_write16(0x0504,0x7E3F); //
    i2c1_uh2d_write16(0x0504,0x0010); //
    i2c1_uh2d_write16(0x0504,0x8F00); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0003); //
    i2c1_uh2d_write16(0x0504,0x00D9); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0005); //
    i2c1_uh2d_write16(0x0504,0x11D0); //
    i2c1_uh2d_write16(0x0504,0x1717); //
    i2c1_uh2d_write16(0x0504,0x00FD); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0011); //
    i2c1_uh2d_write16(0x0504,0xCDD2); //
    i2c1_uh2d_write16(0x0504,0x2B2B); //
    i2c1_uh2d_write16(0x0504,0x1233); //
    i2c1_uh2d_write16(0x0504,0x3333); //
    i2c1_uh2d_write16(0x0504,0x7733); //
    i2c1_uh2d_write16(0x0504,0x3377); //
    i2c1_uh2d_write16(0x0504,0x3333); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    DelayUs(400);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0008); //
    i2c1_uh2d_write16(0x0504,0x06D5); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x5001); //
    i2c1_uh2d_write16(0x0504,0x5001); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x001F); //
    i2c1_uh2d_write16(0x0504,0x01C7); //
    i2c1_uh2d_write16(0x0504,0x130C); //
    i2c1_uh2d_write16(0x0504,0x2C1D); //
    i2c1_uh2d_write16(0x0504,0x473C); //
    i2c1_uh2d_write16(0x0504,0x3C58); //
    i2c1_uh2d_write16(0x0504,0x4F44); //
    i2c1_uh2d_write16(0x0504,0x655C); //
    i2c1_uh2d_write16(0x0504,0x756C); //
    i2c1_uh2d_write16(0x0504,0x0C01); //
    i2c1_uh2d_write16(0x0504,0x1D13); //
    i2c1_uh2d_write16(0x0504,0x392B); //
    i2c1_uh2d_write16(0x0504,0x5443); //
    i2c1_uh2d_write16(0x0504,0x4138); //
    i2c1_uh2d_write16(0x0504,0x5A4D); //
    i2c1_uh2d_write16(0x0504,0x6A63); //
    i2c1_uh2d_write16(0x0504,0x0074); //
    DelayUs(800);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0014); //
    i2c1_uh2d_write16(0x0504,0x01C8); //
    i2c1_uh2d_write16(0x0504,0xFF00); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x00FC); //
    i2c1_uh2d_write16(0x0504,0xFF00); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x00FC); //
    i2c1_uh2d_write16(0x0504,0xFF00); //
    i2c1_uh2d_write16(0x0504,0x0000); //
    i2c1_uh2d_write16(0x0504,0x0FFC); //
    DelayUs(400);
    i2c1_uh2d_write16(0x0504,0x8029); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x03B0); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8039); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0xFF51); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8039); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x0C53); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x8039); //
    i2c1_uh2d_write16(0x0504,0x0002); //
    i2c1_uh2d_write16(0x0504,0x0055); //
    DelayUs(200);
    i2c1_uh2d_write16(0x0504,0x0005); //
    i2c1_uh2d_write16(0x0504,0x0029); //
    DelayMs(200);
    i2c1_uh2d_write16(0x0504,0x0005); //
    i2c1_uh2d_write16(0x0504,0x0011); //
    DelayMs(200);
    // Split Control
    i2c1_uh2d_write16(0x5000,0x0000); // STX0_CTL
    i2c1_uh2d_write16(0x500C,0x84E0); // STX0_FPX
    i2c1_uh2d_write16(0x5080,0x0000); // STX1_CTL
    // HDMI PHY
    i2c1_uh2d_write8(0x8410,0x03); // PHY CTL
    i2c1_uh2d_write8(0x8413,0x3F); // PHY_ENB
    i2c1_uh2d_write8(0x8420,0x06); // EQ_BYPS
    i2c1_uh2d_write8(0x84F0,0x31); // APLL_CTL
    i2c1_uh2d_write8(0x84F4,0x01); // DDCIO_CTL
    // HDMI Clock
    i2c1_uh2d_write16(0x8540,0x12C0); // SYS_FREQ0_1
    i2c1_uh2d_write8(0x8630,0x00); // LOCKDET_FREQ0
    i2c1_uh2d_write16(0x8631,0x0753); // LOCKDET_REF1_2
    i2c1_uh2d_write8(0x8670,0x02); // NCO_F0_MOD
    i2c1_uh2d_write16(0x8A0C,0x12C0); // CSC_SCLK0_1
    // HDMI Interrupt Mask, Clear
    i2c1_uh2d_write8(0x850B,0xFF); // MISC_INT
    i2c1_uh2d_write8(0x851B,0xFD); // MISC_INTM
    // Interrupt Control (TOP level)
    i2c1_uh2d_write16(0x0014,0x0FBF); // IntStatus
    i2c1_uh2d_write16(0x0016,0x0DBF); // IntMask
    // EDID
    i2c1_uh2d_write8(0x85E0,0x01); // EDID_MODE
    i2c1_uh2d_write16(0x85E3,0x0100); // EDID_LEN1_2
    // EDID Data
    i2c1_uh2d_write8(0x8C00,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C01,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C02,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C03,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C04,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C05,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C06,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C07,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C08,0x3E); // EDID_RAM
    i2c1_uh2d_write8(0x8C09,0xD2); // EDID_RAM
    i2c1_uh2d_write8(0x8C0A,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C0B,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C0C,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C0D,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C0E,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C0F,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C10,0x1C); // EDID_RAM
    i2c1_uh2d_write8(0x8C11,0x15); // EDID_RAM
    i2c1_uh2d_write8(0x8C12,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C13,0x03); // EDID_RAM
    i2c1_uh2d_write8(0x8C14,0x80); // EDID_RAM
    i2c1_uh2d_write8(0x8C15,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C16,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C17,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8C18,0x0A); // EDID_RAM
    i2c1_uh2d_write8(0x8C19,0x0D); // EDID_RAM
    i2c1_uh2d_write8(0x8C1A,0xC9); // EDID_RAM
    i2c1_uh2d_write8(0x8C1B,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C1C,0x57); // EDID_RAM
    i2c1_uh2d_write8(0x8C1D,0x47); // EDID_RAM
    i2c1_uh2d_write8(0x8C1E,0x98); // EDID_RAM
    i2c1_uh2d_write8(0x8C1F,0x27); // EDID_RAM
    i2c1_uh2d_write8(0x8C20,0x12); // EDID_RAM
    i2c1_uh2d_write8(0x8C21,0x48); // EDID_RAM
    i2c1_uh2d_write8(0x8C22,0x4C); // EDID_RAM
    i2c1_uh2d_write8(0x8C23,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C24,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C25,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C26,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C27,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C28,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C29,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2A,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2B,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2C,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2D,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2E,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2F,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C30,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C31,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C32,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C33,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C34,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C35,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C36,0xEC); // EDID_RAM
    i2c1_uh2d_write8(0x8C37,0x68); // EDID_RAM
    i2c1_uh2d_write8(0x8C38,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C39,0xDC); // EDID_RAM
    i2c1_uh2d_write8(0x8C3A,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8C3B,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C3C,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8C3D,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C3E,0x64); // EDID_RAM
    i2c1_uh2d_write8(0x8C3F,0x28); // EDID_RAM
    i2c1_uh2d_write8(0x8C40,0x76); // EDID_RAM
    i2c1_uh2d_write8(0x8C41,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C42,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8C43,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8C44,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C45,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C46,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C47,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8C48,0xEC); // EDID_RAM
    i2c1_uh2d_write8(0x8C49,0x68); // EDID_RAM
    i2c1_uh2d_write8(0x8C4A,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C4B,0xDC); // EDID_RAM
    i2c1_uh2d_write8(0x8C4C,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8C4D,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C4E,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8C4F,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C50,0x64); // EDID_RAM
    i2c1_uh2d_write8(0x8C51,0x28); // EDID_RAM
    i2c1_uh2d_write8(0x8C52,0x76); // EDID_RAM
    i2c1_uh2d_write8(0x8C53,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C54,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8C55,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8C56,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C57,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C58,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C59,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8C5A,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C5B,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C5C,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C5D,0xFC); // EDID_RAM
    i2c1_uh2d_write8(0x8C5E,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C5F,0x54); // EDID_RAM
    i2c1_uh2d_write8(0x8C60,0x6F); // EDID_RAM
    i2c1_uh2d_write8(0x8C61,0x73); // EDID_RAM
    i2c1_uh2d_write8(0x8C62,0x68); // EDID_RAM
    i2c1_uh2d_write8(0x8C63,0x69); // EDID_RAM
    i2c1_uh2d_write8(0x8C64,0x62); // EDID_RAM
    i2c1_uh2d_write8(0x8C65,0x61); // EDID_RAM
    i2c1_uh2d_write8(0x8C66,0x2D); // EDID_RAM
    i2c1_uh2d_write8(0x8C67,0x55); // EDID_RAM
    i2c1_uh2d_write8(0x8C68,0x48); // EDID_RAM
    i2c1_uh2d_write8(0x8C69,0x32); // EDID_RAM
    i2c1_uh2d_write8(0x8C6A,0x44); // EDID_RAM
    i2c1_uh2d_write8(0x8C6B,0x0A); // EDID_RAM
    i2c1_uh2d_write8(0x8C6C,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C6D,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C6E,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C6F,0xFD); // EDID_RAM
    i2c1_uh2d_write8(0x8C70,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C71,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8C72,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8C73,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C74,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C75,0x1D); // EDID_RAM
    i2c1_uh2d_write8(0x8C76,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C77,0x0A); // EDID_RAM
    i2c1_uh2d_write8(0x8C78,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C79,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7A,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7B,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7C,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7D,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7E,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C7F,0xC1); // EDID_RAM
    i2c1_uh2d_write8(0x8C80,0x02); // EDID_RAM
    i2c1_uh2d_write8(0x8C81,0x03); // EDID_RAM
    i2c1_uh2d_write8(0x8C82,0x1A); // EDID_RAM
    i2c1_uh2d_write8(0x8C83,0x71); // EDID_RAM
    i2c1_uh2d_write8(0x8C84,0x47); // EDID_RAM
    i2c1_uh2d_write8(0x8C85,0xC6); // EDID_RAM
    i2c1_uh2d_write8(0x8C86,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C87,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C88,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C89,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C8A,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C8B,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C8C,0x23); // EDID_RAM
    i2c1_uh2d_write8(0x8C8D,0x09); // EDID_RAM
    i2c1_uh2d_write8(0x8C8E,0x07); // EDID_RAM
    i2c1_uh2d_write8(0x8C8F,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C90,0x83); // EDID_RAM
    i2c1_uh2d_write8(0x8C91,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C92,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C93,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C94,0x65); // EDID_RAM
    i2c1_uh2d_write8(0x8C95,0x03); // EDID_RAM
    i2c1_uh2d_write8(0x8C96,0x0C); // EDID_RAM
    i2c1_uh2d_write8(0x8C97,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C98,0x10); // EDID_RAM
    i2c1_uh2d_write8(0x8C99,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C9A,0xEC); // EDID_RAM
    i2c1_uh2d_write8(0x8C9B,0x68); // EDID_RAM
    i2c1_uh2d_write8(0x8C9C,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C9D,0xDC); // EDID_RAM
    i2c1_uh2d_write8(0x8C9E,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8C9F,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CA0,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8CA1,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CA2,0x64); // EDID_RAM
    i2c1_uh2d_write8(0x8CA3,0x28); // EDID_RAM
    i2c1_uh2d_write8(0x8CA4,0x76); // EDID_RAM
    i2c1_uh2d_write8(0x8CA5,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CA6,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8CA7,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8CA8,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CA9,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CAA,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CAB,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8CAC,0xEC); // EDID_RAM
    i2c1_uh2d_write8(0x8CAD,0x68); // EDID_RAM
    i2c1_uh2d_write8(0x8CAE,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CAF,0xDC); // EDID_RAM
    i2c1_uh2d_write8(0x8CB0,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CB1,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CB2,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8CB3,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CB4,0x64); // EDID_RAM
    i2c1_uh2d_write8(0x8CB5,0x28); // EDID_RAM
    i2c1_uh2d_write8(0x8CB6,0x76); // EDID_RAM
    i2c1_uh2d_write8(0x8CB7,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CB8,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8CB9,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8CBA,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CBB,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CBC,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CBD,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8CBE,0xEC); // EDID_RAM
    i2c1_uh2d_write8(0x8CBF,0x68); // EDID_RAM
    i2c1_uh2d_write8(0x8CC0,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CC1,0xDC); // EDID_RAM
    i2c1_uh2d_write8(0x8CC2,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CC3,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CC4,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8CC5,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CC6,0x64); // EDID_RAM
    i2c1_uh2d_write8(0x8CC7,0x28); // EDID_RAM
    i2c1_uh2d_write8(0x8CC8,0x76); // EDID_RAM
    i2c1_uh2d_write8(0x8CC9,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CCA,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8CCB,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8CCC,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CCD,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CCE,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CCF,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8CD0,0xEC); // EDID_RAM
    i2c1_uh2d_write8(0x8CD1,0x68); // EDID_RAM
    i2c1_uh2d_write8(0x8CD2,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CD3,0xDC); // EDID_RAM
    i2c1_uh2d_write8(0x8CD4,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CD5,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CD6,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8CD7,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CD8,0x64); // EDID_RAM
    i2c1_uh2d_write8(0x8CD9,0x28); // EDID_RAM
    i2c1_uh2d_write8(0x8CDA,0x76); // EDID_RAM
    i2c1_uh2d_write8(0x8CDB,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CDC,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8CDD,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8CDE,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CDF,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE0,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE1,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8CE2,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE3,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE4,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE5,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE6,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE7,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE8,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE9,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEA,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEB,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEC,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CED,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEE,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEF,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF0,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF1,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF2,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF3,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF4,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF5,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF6,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF7,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF8,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF9,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFA,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFB,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFC,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFD,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFE,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFF,0xEB); // EDID_RAM
    // HDCP Setting
    // Video Color Format Setting
    i2c1_uh2d_write8(0x8A02,0x42); // VOUT_SYNC0
    // HDMI SYSTEM
    i2c1_uh2d_write8(0x8543,0x02); // DDC_CTL
    i2c1_uh2d_write8(0x8544,0x10); // HPD_CTL
    // HDMI Audio Setting
    i2c1_uh2d_write8(0x8600,0x00); // AUD_Auto_Mute
    i2c1_uh2d_write8(0x8602,0xF3); // Auto_CMD0
    i2c1_uh2d_write8(0x8603,0x02); // Auto_CMD1
    i2c1_uh2d_write8(0x8604,0x0C); // Auto_CMD2
    i2c1_uh2d_write8(0x8606,0x05); // BUFINIT_START
    i2c1_uh2d_write8(0x8607,0x00); // FS_MUTE
    i2c1_uh2d_write8(0x8652,0x02); // SDO_MODE1
    i2c1_uh2d_write32(0x8671,0x020C49BA); // NCO_48F0A_D
    i2c1_uh2d_write32(0x8675,0x01E1B089); // NCO_44F0A_D
    i2c1_uh2d_write8(0x8680,0x00); // AUD_MODE
    // Let HDMI Source start access
    i2c1_uh2d_write8(0x854A,0x01); // INIT_END
    // HDMI Signal Detection
    // Wait until HDMI sync is established
    // By Interrupt   In Interrupt Service Routine.

    //Wait until 0x8520 bit[7]=1
    while (!(sys_status&0x80))
    {
        i2c1_uh2d_read8(0x8520,&sys_status); // SYS_STATUS
        printf ("\n(0x80)note status--sysstatus=%x\n",sys_status);
    }
    printf ("3glass-----1\n");

    // Sequence: Check bit7 of 8x8520
    i2c1_uh2d_write8(0x850B,0xFF); // MISC_INT
    i2c1_uh2d_write16(0x0014,0x0FBF); // IntStatus
    // By Polling
    //i2c1_uh2d_read8(0x8520); // SYS_STATUS
    while (!(sys_status&0x80))
    {
        i2c1_uh2d_read8(0x8520,&sys_status); // SYS_STATUS
        printf ("\n(0x80)note status--sysstatus=%x\n",sys_status);
    }
    printf ("3glass-----2\n");

    // Sequence: Check bit7 of 8x8520
    // Start Video TX
    i2c1_uh2d_write16(0x0004,0x0C37); // ConfCtl0
    i2c1_uh2d_write16(0x0006,0x0000); // ConfCtl1
    // Command Transmission After Video Start.
    i2c1_uh2d_write32(0x0110,0x00000006); // MODE_CONFIG
    i2c1_uh2d_write32(0x0310,0x00000006); // MODE_CONFIG
    DelayMs(1000);
#endif

#ifdef HDMI_MIPI //HDMI->MIPI
    //ID adress 0x0000

    // Software Reset
    i2c1_uh2d_write16(0x0004,0x0004); // ConfCtl0
    i2c1_uh2d_write16(0x0002,0x3F01); // SysCtl
    i2c1_uh2d_write16(0x0002,0x0000); // SysCtl
    i2c1_uh2d_write16(0x0006,0x0008); // ConfCtl1
    // DSI-TX0 Transition Timing
    i2c1_uh2d_write32(0x0108,0x00000001); // DSI_TX_CLKEN
    i2c1_uh2d_write32(0x010C,0x00000001); // DSI_TX_CLKSEL
    i2c1_uh2d_write32(0x02A0,0x00000001); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x02AC,0x000094BD); // MIPI_PLL_CNF
    DelayMs(1);
    i2c1_uh2d_write32(0x02A0,0x00000003); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x0118,0x00000014); // LANE_ENABLE
    i2c1_uh2d_write32(0x0120,0x00001770); // LINE_INIT_COUNT
    i2c1_uh2d_write32(0x0124,0x00000000); // HSTX_TO_COUNT
    i2c1_uh2d_write32(0x0128,0x00000101); // FUNC_ENABLE
    i2c1_uh2d_write32(0x0130,0x00010000); // DSI_TATO_COUNT
    i2c1_uh2d_write32(0x0134,0x00005000); // DSI_PRESP_BTA_COUNT
    i2c1_uh2d_write32(0x0138,0x00010000); // DSI_PRESP_LPR_COUNT
    i2c1_uh2d_write32(0x013C,0x00010000); // DSI_PRESP_LPW_COUNT
    i2c1_uh2d_write32(0x0140,0x00010000); // DSI_PRESP_HSR_COUNT
    i2c1_uh2d_write32(0x0144,0x00010000); // DSI_PRESP_HSW_COUNT
    i2c1_uh2d_write32(0x0148,0x00001000); // DSI_PR_TO_COUNT
    i2c1_uh2d_write32(0x014C,0x00010000); // DSI_LRX-H_TO_COUNT
    i2c1_uh2d_write32(0x0150,0x00000140); // FUNC_MODE
    i2c1_uh2d_write32(0x0154,0x00000001); // DSI_RX_VC_ENABLE
    i2c1_uh2d_write32(0x0158,0x000000C8); // IND_TO_COUNT
    i2c1_uh2d_write32(0x0168,0x0000002A); // DSI_HSYNC_STOP_COUNT
    i2c1_uh2d_write32(0x0170,0x000002CA); // APF_VDELAYCNT
    i2c1_uh2d_write32(0x017C,0x00000081); // DSI_TX_MODE
    i2c1_uh2d_write32(0x018C,0x00000001); // DSI_HSYNC_WIDTH
    i2c1_uh2d_write32(0x0190,0x00000104); // DSI_HBPR
    i2c1_uh2d_write32(0x01A4,0x00000000); // DSI_RX_STATE_INT_MASK
    i2c1_uh2d_write32(0x01C0,0x00000015); // DSI_LPRX_THRESH_COUNT
    i2c1_uh2d_write32(0x0214,0x00000000); // APP_SIDE_ERR_INT_MASK
    i2c1_uh2d_write32(0x021C,0x00000080); // DSI_RX_ERR_INT_MASK
    i2c1_uh2d_write32(0x0224,0x00000000); // DSI_LPTX_INT_MASK
    i2c1_uh2d_write32(0x0254,0x00000003); // LPTXTIMECNT
    i2c1_uh2d_write32(0x0258,0x00140303); // TCLK_HEADERCNT
    i2c1_uh2d_write32(0x025C,0x00060003); // TCLK_TRAILCNT
    i2c1_uh2d_write32(0x0260,0x00080003); // THS_HEADERCNT
    i2c1_uh2d_write32(0x0264,0x00004268); // TWAKEUPCNT
    i2c1_uh2d_write32(0x0268,0x0000000F); // TCLK_POSTCNT
    i2c1_uh2d_write32(0x026C,0x00060004); // THS_TRAILCNT
    i2c1_uh2d_write32(0x0270,0x00000020); // HSTXVREGCNT
    i2c1_uh2d_write32(0x0274,0x0000001F); // HSTXVREGEN
    i2c1_uh2d_write32(0x0278,0x00030002); // BTA_COUNT
    i2c1_uh2d_write32(0x027C,0x00000002); // DPHY_TX ADJUST
    i2c1_uh2d_write32(0x011C,0x00000001); // DSITX_START
    // DSI-TX1 Transition Timing
    i2c1_uh2d_write32(0x0308,0x00000001); // DSI_TX_CLKEN
    i2c1_uh2d_write32(0x030C,0x00000001); // DSI_TX_CLKSEL
    i2c1_uh2d_write32(0x04A0,0x00000001); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x04AC,0x000094BD); // MIPI_PLL_CNF
    DelayMs(1);
    i2c1_uh2d_write32(0x04A0,0x00000003); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x0318,0x00000014); // LANE_ENABLE
    i2c1_uh2d_write32(0x0320,0x00001770); // LINE_INIT_COUNT
    i2c1_uh2d_write32(0x0324,0x00000000); // HSTX_TO_COUNT
    i2c1_uh2d_write32(0x0328,0x00000101); // FUNC_ENABLE
    i2c1_uh2d_write32(0x0330,0x00010000); // DSI_TATO_COUNT
    i2c1_uh2d_write32(0x0334,0x00005000); // DSI_PRESP_BTA_COUNT
    i2c1_uh2d_write32(0x0338,0x00010000); // DSI_PRESP_LPR_COUNT
    i2c1_uh2d_write32(0x033C,0x00010000); // DSI_PRESP_LPW_COUNT
    i2c1_uh2d_write32(0x0340,0x00010000); // DSI_PRESP_HSR_COUNT
    i2c1_uh2d_write32(0x0344,0x00010000); // DSI_PRESP_HSW_COUNT
    i2c1_uh2d_write32(0x0348,0x00001000); // DSI_PR_TO_COUNT
    i2c1_uh2d_write32(0x034C,0x00010000); // DSI_LRX-H_TO_COUNT
    i2c1_uh2d_write32(0x0350,0x00000140); // FUNC_MODE
    i2c1_uh2d_write32(0x0354,0x00000001); // DSI_RX_VC_ENABLE
    i2c1_uh2d_write32(0x0358,0x000000C8); // IND_TO_COUNT
    i2c1_uh2d_write32(0x0368,0x0000002A); // DSI_HSYNC_STOP_COUNT
    i2c1_uh2d_write32(0x0370,0x000002CA); // APF_VDELAYCNT
    i2c1_uh2d_write32(0x037C,0x00000081); // DSI_TX_MODE
    i2c1_uh2d_write32(0x038C,0x00000001); // DSI_HSYNC_WIDTH
    i2c1_uh2d_write32(0x0390,0x00000104); // DSI_HBPR
    i2c1_uh2d_write32(0x03A4,0x00000000); // DSI_RX_STATE_INT_MASK
    i2c1_uh2d_write32(0x03C0,0x00000015); // DSI_LPRX_THRESH_COUNT
    i2c1_uh2d_write32(0x0414,0x00000000); // APP_SIDE_ERR_INT_MASK
    i2c1_uh2d_write32(0x041C,0x00000080); // DSI_RX_ERR_INT_MASK
    i2c1_uh2d_write32(0x0424,0x00000000); // DSI_LPTX_INT_MASK
    i2c1_uh2d_write32(0x0454,0x00000003); // LPTXTIMECNT
    i2c1_uh2d_write32(0x0458,0x00140303); // TCLK_HEADERCNT
    i2c1_uh2d_write32(0x045C,0x00060003); // TCLK_TRAILCNT
    i2c1_uh2d_write32(0x0460,0x00080003); // THS_HEADERCNT
    i2c1_uh2d_write32(0x0464,0x00004268); // TWAKEUPCNT
    i2c1_uh2d_write32(0x0468,0x0000000F); // TCLK_POSTCNT
    i2c1_uh2d_write32(0x046C,0x00060004); // THS_TRAILCNT
    i2c1_uh2d_write32(0x0470,0x00000020); // HSTXVREGCNT
    i2c1_uh2d_write32(0x0474,0x0000001F); // HSTXVREGEN
    i2c1_uh2d_write32(0x0478,0x00030002); // BTA_COUNT
    i2c1_uh2d_write32(0x047C,0x00000002); // DPHY_TX ADJUST
    i2c1_uh2d_write32(0x031C,0x00000001); // DSITX_START
    // Command Transmission Before Video Start
    i2c1_uh2d_write16(0x0500,0x0004); // CMD_SEL
    i2c1_uh2d_write32(0x0110,0x00000016); // MODE_CONFIG
    i2c1_uh2d_write32(0x0310,0x00000016); // MODE_CONFIG
    // LCD Initialization

    LCD_init_LP ();

    // Split Control
    i2c1_uh2d_write16(0x5000,0x0000); // STX0_CTL
    i2c1_uh2d_write16(0x500C,0x84E0); // STX0_FPX
    i2c1_uh2d_write16(0x5080,0x0000); // STX1_CTL
    // HDMI PHY
    i2c1_uh2d_write8(0x8410,0x03); // PHY CTL
    i2c1_uh2d_write8(0x8413,0x3F); // PHY_ENB
    i2c1_uh2d_write8(0x8420,0x06); // EQ_BYPS
    i2c1_uh2d_write8(0x84F0,0x31); // APLL_CTL
    i2c1_uh2d_write8(0x84F4,0x01); // DDCIO_CTL
    // HDMI Clock
    i2c1_uh2d_write16(0x8540,0x12C0); // SYS_FREQ0_1
    i2c1_uh2d_write8(0x8630,0x00); // LOCKDET_FREQ0
    i2c1_uh2d_write16(0x8631,0x0753); // LOCKDET_REF1_2
    i2c1_uh2d_write8(0x8670,0x02); // NCO_F0_MOD
    i2c1_uh2d_write16(0x8A0C,0x12C0); // CSC_SCLK0_1
    // HDMI Interrupt Mask, Clear
    i2c1_uh2d_write8(0x850B,0xFF); // MISC_INT
    i2c1_uh2d_write8(0x851B,0xFD); // MISC_INTM
    // Interrupt Control (TOP level)
    i2c1_uh2d_write16(0x0014,0x0FBF); // IntStatus
    i2c1_uh2d_write16(0x0016,0x0DBF); // IntMask
    // EDID
    i2c1_uh2d_write8(0x85E0,0x01); // EDID_MODE
    i2c1_uh2d_write16(0x85E3,0x0100); // EDID_LEN1_2
    // EDID Data
    i2c1_uh2d_write8(0x8C00,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C01,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C02,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C03,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C04,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C05,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C06,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C07,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C08,0x52); // EDID_RAM
    i2c1_uh2d_write8(0x8C09,0x62); // EDID_RAM
    i2c1_uh2d_write8(0x8C0A,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C0B,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C0C,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C0D,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C0E,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C0F,0x88); // EDID_RAM
    i2c1_uh2d_write8(0x8C10,0x1C); // EDID_RAM
    i2c1_uh2d_write8(0x8C11,0x15); // EDID_RAM
    i2c1_uh2d_write8(0x8C12,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C13,0x03); // EDID_RAM
    i2c1_uh2d_write8(0x8C14,0x80); // EDID_RAM
    i2c1_uh2d_write8(0x8C15,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C16,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C17,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8C18,0x0A); // EDID_RAM
    i2c1_uh2d_write8(0x8C19,0x0D); // EDID_RAM
    i2c1_uh2d_write8(0x8C1A,0xC9); // EDID_RAM
    i2c1_uh2d_write8(0x8C1B,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C1C,0x57); // EDID_RAM
    i2c1_uh2d_write8(0x8C1D,0x47); // EDID_RAM
    i2c1_uh2d_write8(0x8C1E,0x98); // EDID_RAM
    i2c1_uh2d_write8(0x8C1F,0x27); // EDID_RAM
    i2c1_uh2d_write8(0x8C20,0x12); // EDID_RAM
    i2c1_uh2d_write8(0x8C21,0x48); // EDID_RAM
    i2c1_uh2d_write8(0x8C22,0x4C); // EDID_RAM
    i2c1_uh2d_write8(0x8C23,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C24,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C25,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C26,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C27,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C28,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C29,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2A,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2B,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2C,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2D,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2E,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C2F,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C30,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C31,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C32,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C33,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C34,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C35,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C36,0xE0); // EDID_RAM
    i2c1_uh2d_write8(0x8C37,0x2E); // EDID_RAM
    i2c1_uh2d_write8(0x8C38,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C39,0x70); // EDID_RAM
    i2c1_uh2d_write8(0x8C3A,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8C3B,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C3C,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8C3D,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8C3E,0x60); // EDID_RAM
    i2c1_uh2d_write8(0x8C3F,0x08); // EDID_RAM
    i2c1_uh2d_write8(0x8C40,0xA2); // EDID_RAM
    i2c1_uh2d_write8(0x8C41,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C42,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8C43,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8C44,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C45,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C46,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C47,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8C48,0xE0); // EDID_RAM
    i2c1_uh2d_write8(0x8C49,0x2E); // EDID_RAM
    i2c1_uh2d_write8(0x8C4A,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C4B,0x70); // EDID_RAM
    i2c1_uh2d_write8(0x8C4C,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8C4D,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C4E,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8C4F,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8C50,0x60); // EDID_RAM
    i2c1_uh2d_write8(0x8C51,0x08); // EDID_RAM
    i2c1_uh2d_write8(0x8C52,0xA2); // EDID_RAM
    i2c1_uh2d_write8(0x8C53,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C54,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8C55,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8C56,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C57,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C58,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C59,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8C5A,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C5B,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C5C,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C5D,0xFC); // EDID_RAM
    i2c1_uh2d_write8(0x8C5E,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C5F,0x54); // EDID_RAM
    i2c1_uh2d_write8(0x8C60,0x6F); // EDID_RAM
    i2c1_uh2d_write8(0x8C61,0x73); // EDID_RAM
    i2c1_uh2d_write8(0x8C62,0x68); // EDID_RAM
    i2c1_uh2d_write8(0x8C63,0x69); // EDID_RAM
    i2c1_uh2d_write8(0x8C64,0x62); // EDID_RAM
    i2c1_uh2d_write8(0x8C65,0x61); // EDID_RAM
    i2c1_uh2d_write8(0x8C66,0x2D); // EDID_RAM
    i2c1_uh2d_write8(0x8C67,0x55); // EDID_RAM
    i2c1_uh2d_write8(0x8C68,0x48); // EDID_RAM
    i2c1_uh2d_write8(0x8C69,0x32); // EDID_RAM
    i2c1_uh2d_write8(0x8C6A,0x44); // EDID_RAM
    i2c1_uh2d_write8(0x8C6B,0x0A); // EDID_RAM
    i2c1_uh2d_write8(0x8C6C,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C6D,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C6E,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C6F,0xFD); // EDID_RAM
    i2c1_uh2d_write8(0x8C70,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C71,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8C72,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8C73,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C74,0xFF); // EDID_RAM
    i2c1_uh2d_write8(0x8C75,0x1D); // EDID_RAM
    i2c1_uh2d_write8(0x8C76,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C77,0x0A); // EDID_RAM
    i2c1_uh2d_write8(0x8C78,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C79,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7A,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7B,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7C,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7D,0x20); // EDID_RAM
    i2c1_uh2d_write8(0x8C7E,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C7F,0x11); // EDID_RAM
    i2c1_uh2d_write8(0x8C80,0x02); // EDID_RAM
    i2c1_uh2d_write8(0x8C81,0x03); // EDID_RAM
    i2c1_uh2d_write8(0x8C82,0x1A); // EDID_RAM
    i2c1_uh2d_write8(0x8C83,0x71); // EDID_RAM
    i2c1_uh2d_write8(0x8C84,0x47); // EDID_RAM
    i2c1_uh2d_write8(0x8C85,0xC6); // EDID_RAM
    i2c1_uh2d_write8(0x8C86,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C87,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C88,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C89,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C8A,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C8B,0x46); // EDID_RAM
    i2c1_uh2d_write8(0x8C8C,0x23); // EDID_RAM
    i2c1_uh2d_write8(0x8C8D,0x09); // EDID_RAM
    i2c1_uh2d_write8(0x8C8E,0x07); // EDID_RAM
    i2c1_uh2d_write8(0x8C8F,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C90,0x83); // EDID_RAM
    i2c1_uh2d_write8(0x8C91,0x01); // EDID_RAM
    i2c1_uh2d_write8(0x8C92,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C93,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C94,0x65); // EDID_RAM
    i2c1_uh2d_write8(0x8C95,0x03); // EDID_RAM
    i2c1_uh2d_write8(0x8C96,0x0C); // EDID_RAM
    i2c1_uh2d_write8(0x8C97,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C98,0x10); // EDID_RAM
    i2c1_uh2d_write8(0x8C99,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8C9A,0xE0); // EDID_RAM
    i2c1_uh2d_write8(0x8C9B,0x2E); // EDID_RAM
    i2c1_uh2d_write8(0x8C9C,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8C9D,0x70); // EDID_RAM
    i2c1_uh2d_write8(0x8C9E,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8C9F,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CA0,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8CA1,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CA2,0x60); // EDID_RAM
    i2c1_uh2d_write8(0x8CA3,0x08); // EDID_RAM
    i2c1_uh2d_write8(0x8CA4,0xA2); // EDID_RAM
    i2c1_uh2d_write8(0x8CA5,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CA6,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8CA7,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8CA8,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CA9,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CAA,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CAB,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8CAC,0xE0); // EDID_RAM
    i2c1_uh2d_write8(0x8CAD,0x2E); // EDID_RAM
    i2c1_uh2d_write8(0x8CAE,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CAF,0x70); // EDID_RAM
    i2c1_uh2d_write8(0x8CB0,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CB1,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CB2,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8CB3,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CB4,0x60); // EDID_RAM
    i2c1_uh2d_write8(0x8CB5,0x08); // EDID_RAM
    i2c1_uh2d_write8(0x8CB6,0xA2); // EDID_RAM
    i2c1_uh2d_write8(0x8CB7,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CB8,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8CB9,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8CBA,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CBB,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CBC,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CBD,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8CBE,0xE0); // EDID_RAM
    i2c1_uh2d_write8(0x8CBF,0x2E); // EDID_RAM
    i2c1_uh2d_write8(0x8CC0,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CC1,0x70); // EDID_RAM
    i2c1_uh2d_write8(0x8CC2,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CC3,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CC4,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8CC5,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CC6,0x60); // EDID_RAM
    i2c1_uh2d_write8(0x8CC7,0x08); // EDID_RAM
    i2c1_uh2d_write8(0x8CC8,0xA2); // EDID_RAM
    i2c1_uh2d_write8(0x8CC9,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CCA,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8CCB,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8CCC,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CCD,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CCE,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CCF,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8CD0,0xE0); // EDID_RAM
    i2c1_uh2d_write8(0x8CD1,0x2E); // EDID_RAM
    i2c1_uh2d_write8(0x8CD2,0xA0); // EDID_RAM
    i2c1_uh2d_write8(0x8CD3,0x70); // EDID_RAM
    i2c1_uh2d_write8(0x8CD4,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CD5,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CD6,0x14); // EDID_RAM
    i2c1_uh2d_write8(0x8CD7,0x50); // EDID_RAM
    i2c1_uh2d_write8(0x8CD8,0x60); // EDID_RAM
    i2c1_uh2d_write8(0x8CD9,0x08); // EDID_RAM
    i2c1_uh2d_write8(0x8CDA,0xA2); // EDID_RAM
    i2c1_uh2d_write8(0x8CDB,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CDC,0xC0); // EDID_RAM
    i2c1_uh2d_write8(0x8CDD,0x78); // EDID_RAM
    i2c1_uh2d_write8(0x8CDE,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CDF,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE0,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE1,0x18); // EDID_RAM
    i2c1_uh2d_write8(0x8CE2,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE3,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE4,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE5,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE6,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE7,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE8,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CE9,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEA,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEB,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEC,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CED,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEE,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CEF,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF0,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF1,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF2,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF3,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF4,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF5,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF6,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF7,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF8,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CF9,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFA,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFB,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFC,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFD,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFE,0x00); // EDID_RAM
    i2c1_uh2d_write8(0x8CFF,0xD3); // EDID_RAM
    // HDCP Setting
    // Video Color Format Setting
    i2c1_uh2d_write8(0x8A02,0x42); // VOUT_SYNC0
    // HDMI SYSTEM
    i2c1_uh2d_write8(0x8543,0x02); // DDC_CTL
    i2c1_uh2d_write8(0x8544,0x10); // HPD_CTL
    // HDMI Audio Setting
    i2c1_uh2d_write8(0x8600,0x00); // AUD_Auto_Mute
    i2c1_uh2d_write8(0x8602,0xF3); // Auto_CMD0
    i2c1_uh2d_write8(0x8603,0x02); // Auto_CMD1
    i2c1_uh2d_write8(0x8604,0x0C); // Auto_CMD2
    i2c1_uh2d_write8(0x8606,0x05); // BUFINIT_START
    i2c1_uh2d_write8(0x8607,0x00); // FS_MUTE
    i2c1_uh2d_write8(0x8652,0x02); // SDO_MODE1
    i2c1_uh2d_write32(0x8671,0x020C49BA); // NCO_48F0A_D
    i2c1_uh2d_write32(0x8675,0x01E1B089); // NCO_44F0A_D
    i2c1_uh2d_write8(0x8680,0x00); // AUD_MODE
    // Let HDMI Source start access
    i2c1_uh2d_write8(0x854A,0x01); // INIT_END
    // HDMI Signal Detection
    // Wait until HDMI sync is established
    // By Interrupt   In Interrupt Service Routine.

    //wait until 0x8520 bit[7]=1
    while (!(sys_status&0x80))
    {
        i2c1_uh2d_read8(0x8520,&sys_status); // SYS_STATUS
        printf ("\n(0x80)note status--sysstatus=%x\n",sys_status);
    }

    // Sequence: Check bit7 of 8x8520
    // Start Video TX
    i2c1_uh2d_write16(0x0004,0x0C37); // ConfCtl0
    i2c1_uh2d_write16(0x0006,0x0000); // ConfCtl1
    // Command Transmission After Video Start.
    i2c1_uh2d_write32(0x0110,0x00000006); // MODE_CONFIG
    i2c1_uh2d_write32(0x0310,0x00000006); // MODE_CONFIG

#endif

/************************************************************/
#if HDMI_MIPI_COLORBAR //Color Bar
    /*
    //拉低INT 配置Slave为0x1F
    //GpioMuxSet(GPIOPortC_Pin5,IOMUX_GPIOC5_IO);
    //Gpio_SetPinDirection(GPIOPortC_Pin5,GPIO_OUT);
    //Gpio_SetPinLevel(GPIOPortC_Pin5, GPIO_LOW);
    //DelayMs(10);
    //拉低ResetN
    GpioMuxSet(GPIOPortC_Pin6,IOMUX_GPIOC6_IO);
    Gpio_SetPinDirection(GPIOPortC_Pin6,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin6, GPIO_LOW);
    DelayMs(100);
    Gpio_SetPinLevel(GPIOPortC_Pin6, GPIO_HIGH);
    //Gpio_SetPinDirection(GPIOPortC_Pin5,GPIO_IN);
    #if 1
    //打开背光
    BL_On();
    #endif
    */
    // Software Reset
    ret = i2c1_uh2d_write16(0x0004,0x0004); // ConfCtl0
    if (ret != TRUE)
    {
        printf ("I2C Write Failure\n");
    }
    return 1;
    i2c1_uh2d_write16(0x0002,0x3F01); // SysCtl
    i2c1_uh2d_write16(0x0002,0x0000); // SysCtl
    i2c1_uh2d_write16(0x0006,0x0000); // ConfCtl1
    i2c1_uh2d_write16(0x7000,0x0009); // CB_CTL


    // DSI-TX1 Transition Timing
    i2c1_uh2d_write32(0x0308,0x00000001); // DSI_TX_CLKEN
    i2c1_uh2d_write32(0x030C,0x00000001); // DSI_TX_CLKSEL
    i2c1_uh2d_write32(0x04A0,0x00000001); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x04AC,0x000094BD); // MIPI_PLL_CNF
    DelayMs(1);
    i2c1_uh2d_write32(0x04A0,0x00000003); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x0318,0x00000014); // LANE_ENABLE
    i2c1_uh2d_write32(0x0320,0x00001770); // LINE_INIT_COUNT
    i2c1_uh2d_write32(0x0324,0x00000000); // HSTX_TO_COUNT
    i2c1_uh2d_write32(0x0328,0x00000101); // FUNC_ENABLE
    i2c1_uh2d_write32(0x0330,0x00010000); // DSI_TATO_COUNT
    i2c1_uh2d_write32(0x0334,0x00005000); // DSI_PRESP_BTA_COUNT
    i2c1_uh2d_write32(0x0338,0x00010000); // DSI_PRESP_LPR_COUNT
    i2c1_uh2d_write32(0x033C,0x00010000); // DSI_PRESP_LPW_COUNT
    i2c1_uh2d_write32(0x0340,0x00010000); // DSI_PRESP_HSR_COUNT
    i2c1_uh2d_write32(0x0344,0x00010000); // DSI_PRESP_HSW_COUNT
    i2c1_uh2d_write32(0x0348,0x00001000); // DSI_PR_TO_COUNT
    i2c1_uh2d_write32(0x034C,0x00010000); // DSI_LRX-H_TO_COUNT
    i2c1_uh2d_write32(0x0350,0x00000140); // FUNC_MODE
    i2c1_uh2d_write32(0x0354,0x00000001); // DSI_RX_VC_ENABLE
    i2c1_uh2d_write32(0x0358,0x000000C8); // IND_TO_COUNT
    i2c1_uh2d_write32(0x0368,0x0000002A); // DSI_HSYNC_STOP_COUNT
    i2c1_uh2d_write32(0x0370,0x000002FD); // APF_VDELAYCNT
    i2c1_uh2d_write32(0x037C,0x00000081); // DSI_TX_MODE
    i2c1_uh2d_write32(0x038C,0x00000001); // DSI_HSYNC_WIDTH
    i2c1_uh2d_write32(0x0390,0x00000104); // DSI_HBPR
    i2c1_uh2d_write32(0x03A4,0x00000000); // DSI_RX_STATE_INT_MASK
    i2c1_uh2d_write32(0x03C0,0x00000015); // DSI_LPRX_THRESH_COUNT
    i2c1_uh2d_write32(0x0414,0x00000000); // APP_SIDE_ERR_INT_MASK
    i2c1_uh2d_write32(0x041C,0x00000080); // DSI_RX_ERR_INT_MASK
    i2c1_uh2d_write32(0x0424,0x00000000); // DSI_LPTX_INT_MASK
    i2c1_uh2d_write32(0x0454,0x00000003); // LPTXTIMECNT
    i2c1_uh2d_write32(0x0458,0x00140303); // TCLK_HEADERCNT
    i2c1_uh2d_write32(0x045C,0x00060003); // TCLK_TRAILCNT
    i2c1_uh2d_write32(0x0460,0x00080003); // THS_HEADERCNT
    i2c1_uh2d_write32(0x0464,0x00004268); // TWAKEUPCNT
    i2c1_uh2d_write32(0x0468,0x0000000F); // TCLK_POSTCNT
    i2c1_uh2d_write32(0x046C,0x00060004); // THS_TRAILCNT
    i2c1_uh2d_write32(0x0470,0x00000020); // HSTXVREGCNT
    i2c1_uh2d_write32(0x0474,0x0000001F); // HSTXVREGEN
    i2c1_uh2d_write32(0x0478,0x00030002); // BTA_COUNT
    i2c1_uh2d_write32(0x047C,0x00000002); // DPHY_TX ADJUST
    i2c1_uh2d_write32(0x031C,0x00000001); // DSITX_START
    // DSI-TX0 Transition Timing
    i2c1_uh2d_write32(0x0108,0x00000001); // DSI_TX_CLKEN
    i2c1_uh2d_write32(0x010C,0x00000001); // DSI_TX_CLKSEL
    i2c1_uh2d_write32(0x02A0,0x00000001); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x02AC,0x000094BD); // MIPI_PLL_CNF
    DelayMs(1);
    i2c1_uh2d_write32(0x02A0,0x00000003); // MIPI_PLL_CONTROL
    i2c1_uh2d_write32(0x0118,0x00000014); // LANE_ENABLE
    i2c1_uh2d_write32(0x0120,0x00001770); // LINE_INIT_COUNT
    i2c1_uh2d_write32(0x0124,0x00000000); // HSTX_TO_COUNT
    i2c1_uh2d_write32(0x0128,0x00000101); // FUNC_ENABLE
    i2c1_uh2d_write32(0x0130,0x00010000); // DSI_TATO_COUNT
    i2c1_uh2d_write32(0x0134,0x00005000); // DSI_PRESP_BTA_COUNT
    i2c1_uh2d_write32(0x0138,0x00010000); // DSI_PRESP_LPR_COUNT
    i2c1_uh2d_write32(0x013C,0x00010000); // DSI_PRESP_LPW_COUNT
    i2c1_uh2d_write32(0x0140,0x00010000); // DSI_PRESP_HSR_COUNT
    i2c1_uh2d_write32(0x0144,0x00010000); // DSI_PRESP_HSW_COUNT
    i2c1_uh2d_write32(0x0148,0x00001000); // DSI_PR_TO_COUNT
    i2c1_uh2d_write32(0x014C,0x00010000); // DSI_LRX-H_TO_COUNT
    i2c1_uh2d_write32(0x0150,0x00000140); // FUNC_MODE
    i2c1_uh2d_write32(0x0154,0x00000001); // DSI_RX_VC_ENABLE
    i2c1_uh2d_write32(0x0158,0x000000C8); // IND_TO_COUNT
    i2c1_uh2d_write32(0x0168,0x0000002A); // DSI_HSYNC_STOP_COUNT
    i2c1_uh2d_write32(0x0170,0x000002FD); // APF_VDELAYCNT
    i2c1_uh2d_write32(0x017C,0x00000081); // DSI_TX_MODE
    i2c1_uh2d_write32(0x018C,0x00000001); // DSI_HSYNC_WIDTH
    i2c1_uh2d_write32(0x0190,0x00000104); // DSI_HBPR
    i2c1_uh2d_write32(0x01A4,0x00000000); // DSI_RX_STATE_INT_MASK
    i2c1_uh2d_write32(0x01C0,0x00000015); // DSI_LPRX_THRESH_COUNT
    i2c1_uh2d_write32(0x0214,0x00000000); // APP_SIDE_ERR_INT_MASK
    i2c1_uh2d_write32(0x021C,0x00000080); // DSI_RX_ERR_INT_MASK
    i2c1_uh2d_write32(0x0224,0x00000000); // DSI_LPTX_INT_MASK
    i2c1_uh2d_write32(0x0254,0x00000003); // LPTXTIMECNT
    i2c1_uh2d_write32(0x0258,0x00140303); // TCLK_HEADERCNT
    i2c1_uh2d_write32(0x025C,0x00060003); // TCLK_TRAILCNT
    i2c1_uh2d_write32(0x0260,0x00080003); // THS_HEADERCNT
    i2c1_uh2d_write32(0x0264,0x00004268); // TWAKEUPCNT
    i2c1_uh2d_write32(0x0268,0x0000000F); // TCLK_POSTCNT
    i2c1_uh2d_write32(0x026C,0x00060004); // THS_TRAILCNT
    i2c1_uh2d_write32(0x0270,0x00000020); // HSTXVREGCNT
    i2c1_uh2d_write32(0x0274,0x0000001F); // HSTXVREGEN
    i2c1_uh2d_write32(0x0278,0x00030002); // BTA_COUNT
    i2c1_uh2d_write32(0x027C,0x00000002); // DPHY_TX ADJUST
    i2c1_uh2d_write32(0x011C,0x00000001); // DSITX_START
    // Command Transmission Before Video Start
    i2c1_uh2d_write16(0x0500,0x0004);     // CMD_SEL
    i2c1_uh2d_write32(0x0110,0x00000016); // MODE_CONFIG
    i2c1_uh2d_write32(0x0310,0x00000016); // MODE_CONFIG


    // LCD Initialization
    printf ("LCD init start\n");

    LCD_init_LP();

    printf ("LCD init end\n");
    // Split Control
    i2c1_uh2d_write16(0x5000,0x0000); // STX0_CTL
    i2c1_uh2d_write16(0x500C,0x84E0); // STX0_FPX
    i2c1_uh2d_write16(0x5080,0x0000); // STX1_CTL
    // Color Bar Setting
    i2c1_uh2d_write16(0x7000,0x000D); // CB_CTL
    i2c1_uh2d_write16(0x7008,0x0008); // CB_HSW
    i2c1_uh2d_write16(0x700A,0x0002); // CB_VSW
    i2c1_uh2d_write16(0x700C,0x0610); // CB_Htotal
    i2c1_uh2d_write16(0x700E,0x0514); // CB_Vtotal
    i2c1_uh2d_write16(0x7010,0x05A0); // CB_Hact //1440(5A0) //720(2D0)
    i2c1_uh2d_write16(0x7012,0x0500); // CB_Vact //1280 //1280
    i2c1_uh2d_write16(0x7014,0x0010); // CB_Hstart
    i2c1_uh2d_write16(0x7016,0x000A); // CB_Vstart
    // Start Video TX
    i2c1_uh2d_write16(0x0004,0x0C37); // ConfCtl0
    // Command Transmission After Video Start. HS command TX
    i2c1_uh2d_write32(0x0110,0x00000006); // MODE_CONFIG
    i2c1_uh2d_write32(0x0310,0x00000006); // MODE_CONFIG

    DelayMs(50);
    i2c1_uh2d_write16(0x0504,0x0005); //
    i2c1_uh2d_write16(0x0504,0x0029); // STX0_FPX
    DelayMs(50);
    return ret;
#endif
/************************************************************/

}


