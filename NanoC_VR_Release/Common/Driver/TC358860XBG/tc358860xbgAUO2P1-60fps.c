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
#ifdef _AUO_2P1_60FPS_LCD_

#include "DriverInclude.h"

#include "Tc358860xbg.h"
#include "Gpio.h"
#include "SysConfig.h"

BOOL i2c1_edp2dsi_write32(uint16 address, uint32 data)
{
    i2c1_reg16_write32(0xD1, address, data);
}

BOOL i2c1_edp2dsi_write16(uint16 address, uint16 data)
{
    i2c1_reg16_write16(0xD1, address, data);
}


BOOL i2c1_edp2dsi_write8(uint16 address, uint8 data)
{
    i2c1_reg16_write8(0xD1, address, data);
}


uint32 i2c1_edp2dsi_read32(uint16 address, uint8 *data)
{
    uint32 rdata = 0;
    uint32 rdata_tmp1 = 0;
    uint32 rdata_tmp2 = 0;
    uint32 rdata_tmp3 = 0;
    uint32 rdata_tmp4 = 0;
    i2c1_reg16_read32(0xD1, address, data);
    rdata_tmp1 = data[0];
    rdata_tmp2 = data[1];
    rdata_tmp3 = data[2];
    rdata_tmp4 = data[3];

    rdata = ((rdata_tmp4<<24)|(rdata_tmp3<<16)|(rdata_tmp2<<8)|rdata_tmp1);
    return rdata;

}

uint16 i2c1_edp2dsi_read16(uint16 address, uint8 *data)
{
    uint16 rdata = 0;
    uint16 rdata_tmp1 = 0;
    uint16 rdata_tmp2 = 0;
    i2c1_reg16_read16(0xD1, address, data);
    rdata_tmp1 = data[0];
    rdata_tmp2 = data[1];

    rdata = ((rdata_tmp2<<8)|rdata_tmp1);
    return rdata;

}

uint8 i2c1_edp2dsi_read8(uint16 address, uint8 *data)
{
    uint8 rdata = 0;
    i2c1_reg16_read8(0xD1, address, data);
    rdata = data[0];

    return rdata;

}

void i2c1_edidrom_read8 (uint8 address, uint8 *data)
{
    i2c_read_data(0xA1, address, data, 1);
}


void i2c1_edidrom_write8 (uint8 address, uint8 data)
{
    i2c_write_data(0xA1, address, data, 1);
}




void dcs_write_1P_mode(uint8 cmd, uint16 delay)
{
    uint32 regdata;
    uint8 data[2] = {0};

    //regdata=0x81000015;
    regdata=0x81000005;
    regdata|=cmd<<8;
    //printf ("1P:regdata=%x\n",regdata);

    i2c1_edp2dsi_write32(0x42FC,regdata);
    DelayUs(200);
    i2c1_edp2dsi_read16(0x4200,data);

    if (delay != 0)
        DelayMs(delay);
}

void dcs_write_2P_mode(uint8 cmd1, uint8 cmd2, uint16 delay)
{
    uint32 regdata;
    uint8 data[2] = {0};

    regdata=0x81000015;
    regdata|=cmd1<<8;
    regdata|=cmd2<<16;
    //printf ("2P:regdata=%x\n",regdata);


    i2c1_edp2dsi_write32(0x42FC,regdata);
    DelayUs(200);
    i2c1_edp2dsi_read16(0x4200,data);

    if (delay != 0)
        DelayMs(delay);
}

void LCD_init_LPMode(void)
{
#if 1

/*0*/   dcs_write_1P_mode(0x11, 200); // dcs_write_1P_mode(cmd, delayms)

/*1*/   dcs_write_2P_mode(0xff, 0x01, 0);

/*2*/   dcs_write_2P_mode(0xfb, 0x01,10);

/*3*/   dcs_write_2P_mode(0x00, 0x4a, 0);

/*4*/   dcs_write_2P_mode(0x01, 0x33, 0);

/*5*/   dcs_write_2P_mode(0x02, 0x53, 0);


/*6*/   dcs_write_2P_mode(0x03, 0x55, 0);
/*7*/   dcs_write_2P_mode(0x04, 0x55, 0);
        //i2c1_uh2d_read16(0x0270, test_data1);
/*8*/   dcs_write_2P_mode(0x05, 0x33, 0);

/*9*/   dcs_write_2P_mode(0x06, 0x22, 0);


/*10*/  dcs_write_2P_mode(0x08, 0x16, 10);

/*11*/  dcs_write_2P_mode(0x09, 0x94, 0);

/*12*/  dcs_write_2P_mode(0x0b, 0x97, 0);


/*13*/  dcs_write_2P_mode(0x0c, 0x97, 0);

        dcs_write_2P_mode(0x0d, 0x2f, 0);     //14


        dcs_write_2P_mode( 0x0e, 0x3d, 0);    //15
        //i2c1_uh2d_read16(0x0270, test_data1);

        dcs_write_2P_mode( 0x0f, 0xa7, 0);    //16



        dcs_write_2P_mode( 0x36, 0x73, 10);   //17


        dcs_write_2P_mode( 0x6d, 0x33, 0);    //18


        dcs_write_2P_mode( 0x6f, 0x02, 0);    //19


        dcs_write_2P_mode( 0x37, 0x02, 10);   //20


        dcs_write_2P_mode( 0xff, 0x05, 0);    //21


        dcs_write_2P_mode( 0xfb, 0x01, 10);   //22


        dcs_write_2P_mode( 0x01, 0x00, 0);//23


        dcs_write_2P_mode( 0x02, 0x8d, 0);//24


        dcs_write_2P_mode( 0x03, 0x8d, 0);//25


        dcs_write_2P_mode( 0x04, 0x8d, 0);//26


        dcs_write_2P_mode( 0x06, 0x33, 0);//27


        dcs_write_2P_mode( 0x07, 0x01, 0);//28


        dcs_write_2P_mode( 0x08, 0x00, 0);//29


        dcs_write_2P_mode( 0x09, 0x49, 10);//30
        //i2c1_uh2d_read16(0x0270, test_data1);

        dcs_write_2P_mode( 0x0a, 0x46, 0);//31


        dcs_write_2P_mode( 0x0d, 0x0d, 0);//32

        dcs_write_2P_mode( 0x0e, 0x16, 0);//33


        dcs_write_2P_mode( 0x0f, 0x17, 0);//34


        dcs_write_2P_mode( 0x10, 0x53, 0);//35


        dcs_write_2P_mode( 0x11, 0x00, 0);//36


        dcs_write_2P_mode( 0x12, 0x00, 0);//37


        dcs_write_2P_mode( 0x14, 0x01, 10);//38


        dcs_write_2P_mode( 0x15, 0x00, 0);//39


        dcs_write_2P_mode( 0x16, 0x05, 0);//40


        dcs_write_2P_mode( 0x17, 0x00, 0);//41


        dcs_write_2P_mode( 0x19, 0x7f, 0);//42


        dcs_write_2P_mode( 0x1a, 0xff, 0);//43


        dcs_write_2P_mode( 0x1b, 0x0f, 0);//44


        dcs_write_2P_mode( 0x1c, 0x00, 0);//45


        dcs_write_2P_mode( 0x1d, 0x00, 10);//46

        dcs_write_2P_mode( 0x1e, 0x00, 0);//47


        dcs_write_2P_mode( 0x1f, 0x07, 0);//48


        dcs_write_2P_mode( 0x20, 0x00, 0);//49


        dcs_write_2P_mode( 0x21, 0x00, 0);//50
        //i2c1_uh2d_read16(0x0270, test_data1); //亮起比较慢

        dcs_write_2P_mode( 0x22, 0x55, 0);//51


        dcs_write_2P_mode( 0x23, 0x0d, 0);//52


        dcs_write_2P_mode( 0x2d, 0x02, 0);//53


        dcs_write_2P_mode( 0x3b, 0x8d, 10);//54


        dcs_write_2P_mode( 0x3d, 0x8a, 0);//55


        dcs_write_2P_mode( 0x7b, 0xa1, 10);//56


        dcs_write_2P_mode( 0x83, 0x03, 10);//57


        dcs_write_2P_mode( 0xa0, 0xc2, 0);//58


        dcs_write_2P_mode( 0xa2, 0x10, 0);//59


        dcs_write_2P_mode( 0xa6, 0x25, 0);//60


        dcs_write_2P_mode( 0xbb, 0x08, 0);//61


        dcs_write_2P_mode( 0xbc, 0x08, 10);//62


        dcs_write_2P_mode( 0x28, 0x01, 0);//63


        dcs_write_2P_mode( 0x32, 0x08, 0);//64


        dcs_write_2P_mode( 0x33, 0xb8, 0);//65


        dcs_write_2P_mode( 0x36, 0x05, 0);//66


        dcs_write_2P_mode( 0x37, 0x03, 0);//67


        dcs_write_2P_mode( 0x43, 0x00, 0);//68


        dcs_write_2P_mode( 0x4b, 0x21, 0);//69


        dcs_write_2P_mode( 0x4c, 0x03, 10);//70


        dcs_write_2P_mode( 0x50, 0x21, 0);//71


        dcs_write_2P_mode( 0x51, 0x03, 0);//72


        dcs_write_2P_mode( 0x58, 0x21, 0);//73


        dcs_write_2P_mode( 0x59, 0x03, 0);//74


        dcs_write_2P_mode( 0x5d, 0x21, 0);//75


        dcs_write_2P_mode( 0x5e, 0x03, 0);//76


        dcs_write_2P_mode( 0x6c, 0x03, 0);//77


        dcs_write_2P_mode( 0x6d, 0x03, 10);//78


        dcs_write_2P_mode( 0x6f, 0x25, 0);//79


        dcs_write_2P_mode( 0x89, 0xc1, 0);//80


        dcs_write_2P_mode( 0x9e, 0x00, 0);//81


        dcs_write_2P_mode( 0x9f, 0x10, 0);//82


        dcs_write_2P_mode( 0x2a, 0x11, 0);//83


        dcs_write_2P_mode( 0x35, 0x0b, 0);//84


        dcs_write_2P_mode( 0x2f, 0x01, 0);//85


        dcs_write_2P_mode( 0x2d, 0x03, 10);//86


        dcs_write_2P_mode( 0xff, 0x00, 0);//87


        dcs_write_2P_mode( 0xfb, 0x01, 0);//88


        dcs_write_2P_mode( 0xff, 0x01, 0);//89


        dcs_write_2P_mode( 0xfb, 0x01, 0);//90


        dcs_write_2P_mode( 0xff, 0x02, 0);//91


        dcs_write_2P_mode( 0xfb, 0x01, 0);//92


        dcs_write_2P_mode( 0xff, 0x03, 0);//93


        dcs_write_2P_mode( 0xfb, 0x01, 0);//94


        dcs_write_2P_mode( 0xff, 0x04, 0);//95


        dcs_write_2P_mode( 0xfb, 0x01, 0);//96


        dcs_write_2P_mode( 0xff, 0x05, 0);//97


        dcs_write_2P_mode( 0xfb, 0x01, 19);//98


        dcs_write_2P_mode( 0xff, 0x00, 0);//99


        dcs_write_2P_mode( 0xfb, 0x01, 0);//100


        dcs_write_2P_mode( 0xba, 0x03, 0);//101
        //i2c1_uh2d_read16(0x0270, test_data1);
#endif
        dcs_write_1P_mode(0x11, 200);
        dcs_write_1P_mode(0x29, 200);

}


void Edidrom_write()
{
    uint8 rdata_0x01 = 0; //0xFF
    uint8 rdata_0x50 = 0; //0x68
    uint8 rdata_0xA0 = 0; //0x14
    uint8 rdata_0xFF = 0; //0x28

    i2c1_edidrom_read8(0x01, &rdata_0x01);
    DelayMs(10);
    i2c1_edidrom_read8(0x50, &rdata_0x50);
    DelayMs(10);
    i2c1_edidrom_read8(0xA0, &rdata_0xA0);
    DelayMs(10);
    i2c1_edidrom_read8(0xFF, &rdata_0xFF);
    DelayMs(10);
    if ((rdata_0x01==0xFF)&&(rdata_0x50==0x68)&&(rdata_0xA0==0x14)&&(rdata_0xFF==0x28))
    {
        printf ("EDID already been written \n");
		return;
    }


    i2c1_edidrom_write8(0x00,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x01,0xFF); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x02,0xFF); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x03,0xFF); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x04,0xFF); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x05,0xFF); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x06,0xFF); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x07,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x08,0x52); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x09,0x62); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x0A,0x88); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x0B,0x88); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x0C,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x0D,0x88); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x0E,0x88); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x0F,0x88); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x10,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x11,0x19); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x12,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x13,0x03); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x14,0x80); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x15,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x16,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x17,0x78); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x18,0x0A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x19,0x0D); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x1A,0xC9); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x1B,0xA0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x1C,0x57); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x1D,0x47); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x1E,0x98); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x1F,0x27); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x20,0x12); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x21,0x48); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x22,0x4C); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x23,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x24,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x25,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x26,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x27,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x28,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x29,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x2A,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x2B,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x2C,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x2D,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x2E,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x2F,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x30,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x31,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x32,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x33,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x34,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x35,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x36,0x02); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x37,0x3A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x38,0xA0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x39,0xD0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x3A,0x51); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x3B,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x3C,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x3D,0x50); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x3E,0x68); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x3F,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x40,0xA2); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x41,0x40); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x42,0x90); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x43,0x80); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x44,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x45,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x46,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x47,0x18); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x48,0x02); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x49,0x3A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x4A,0xA0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x4B,0xD0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x4C,0x51); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x4D,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x4E,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x4F,0x50); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x50,0x68); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x51,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x52,0xA2); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x53,0x40); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x54,0x90); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x55,0x80); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x56,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x57,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x58,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x59,0x18); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x5A,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x5B,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x5C,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x5D,0xFC); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x5E,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x5F,0x54); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x60,0x38); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x61,0x36); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x62,0x30); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x63,0x2D); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x64,0x57); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x65,0x51); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x66,0x51); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x67,0x48); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x68,0x44); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x69,0x34); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x6A,0x6B); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x6B,0x0A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x6C,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x6D,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x6E,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x6F,0xFD); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x70,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x71,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x72,0x64); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x73,0x05); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x74,0xFF); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x75,0x35); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x76,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x77,0x0A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x78,0x20); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x79,0x20); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x7A,0x20); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x7B,0x20); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x7C,0x20); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x7D,0x20); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x7E,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x7F,0x71); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x80,0x02); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x81,0x03); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x82,0x1A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x83,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x84,0x47); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x85,0x84); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x86,0x13); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x87,0x03); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x88,0x02); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x89,0x07); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x8A,0x06); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x8B,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x8C,0x23); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x8D,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x8E,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x8F,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x90,0x83); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x91,0x01); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x92,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x93,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x94,0x65); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x95,0x03); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x96,0x0C); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x97,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x98,0x10); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x99,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x9A,0x02); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x9B,0x3A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x9C,0xA0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x9D,0xD0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x9E,0x51); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0x9F,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA0,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA1,0x50); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA2,0x68); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA3,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA4,0xA2); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA5,0x40); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA6,0x90); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA7,0x80); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA8,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xA9,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xAA,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xAB,0x18); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xAC,0x02); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xAD,0x3A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xAE,0xA0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xAF,0xD0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB0,0x51); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB1,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB2,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB3,0x50); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB4,0x68); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB5,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB6,0xA2); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB7,0x40); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB8,0x90); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xB9,0x80); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xBA,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xBB,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xBC,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xBD,0x18); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xBE,0x02); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xBF,0x3A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC0,0xA0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC1,0xD0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC2,0x51); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC3,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC4,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC5,0x50); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC6,0x68); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC7,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC8,0xA2); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xC9,0x40); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xCA,0x90); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xCB,0x80); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xCC,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xCD,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xCE,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xCF,0x18); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD0,0x02); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD1,0x3A); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD2,0xA0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD3,0xD0); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD4,0x51); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD5,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD6,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD7,0x50); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD8,0x68); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xD9,0x14); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xDA,0xA2); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xDB,0x40); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xDC,0x90); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xDD,0x80); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xDE,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xDF,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE0,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE1,0x18); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE2,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE3,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE4,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE5,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE6,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE7,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE8,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xE9,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xEA,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xEB,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xEC,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xED,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xEE,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xEF,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF0,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF1,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF2,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF3,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF4,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF5,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF6,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF7,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF8,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xF9,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xFA,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xFB,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xFC,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xFD,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xFE,0x00); // EDID_Data
    DelayMs(10);
    i2c1_edidrom_write8(0xFF,0x28); // EDID_Data
    DelayMs(10);

}

const uint8 edid_data[] = 
{
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0x52,0x62,0x88,0x88,0x00,0x88,0x88,0x88,
	0x01,0x19,0x01,0x03,0x80,0x00,0x00,0x78,
	0x0A,0x0D,0xC9,0xA0,0x57,0x47,0x98,0x27,
	0x12,0x48,0x4C,0x00,0x00,0x00,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x3A,
	0xA0,0xD0,0x51,0x00,0x14,0x50,0x68,0x14,
	0xA2,0x40,0x90,0x80,0x00,0x00,0x00,0x18,
	0x02,0x3A,0xA0,0xD0,0x51,0x00,0x14,0x50,
	0x68,0x14,0xA2,0x40,0x90,0x80,0x00,0x00,
	0x00,0x18,0x00,0x00,0x00,0xFC,0x00,0x54,
	0x38,0x36,0x30,0x2D,0x57,0x51,0x51,0x48,
	0x44,0x34,0x6B,0x0A,0x00,0x00,0x00,0xFD,
	0x00,0x14,0x64,0x05,0xFF,0x35,0x00,0x0A,
	0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x71,
	0x02,0x03,0x1A,0x01,0x47,0x84,0x13,0x03,
	0x02,0x07,0x06,0x01,0x23,0x00,0x00,0x00,
	0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,
	0x10,0x00,0x02,0x3A,0xA0,0xD0,0x51,0x00,
	0x14,0x50,0x68,0x14,0xA2,0x40,0x90,0x80,
	0x00,0x00,0x00,0x18,0x02,0x3A,0xA0,0xD0,
	0x51,0x00,0x14,0x50,0x68,0x14,0xA2,0x40,
	0x90,0x80,0x00,0x00,0x00,0x18,0x02,0x3A,
	0xA0,0xD0,0x51,0x00,0x14,0x50,0x68,0x14,
	0xA2,0x40,0x90,0x80,0x00,0x00,0x00,0x18,
	0x02,0x3A,0xA0,0xD0,0x51,0x00,0x14,0x50,
	0x68,0x14,0xA2,0x40,0x90,0x80,0x00,0x00,
	0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x28
};

/**************************************************************************************
*   Name    : Tc358860XBG_init
*   Function: Tc358860XBG init Use I2C bus.
*   Input   : reg   -- address of would send register.
              pdata -- the pointer of would send data.
*             size  -- the size of would send data.
*   Return  : TRUE  -- send seccess
*             FALSE -- send failure
*   explain :
**************************************************************************************/
BOOL Tc358860XBG_init()
{
	uint8 data8 = 0;
    uint8 data32[4] = {0};
    uint8 rdata8 = 0;
    uint32 rdata32 = 0;

#if 1
    //拉低INT 配置Slave为0xD1
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
#endif

    //EDID Write--Save the screen information
#ifdef _EEPROM_EDID_
	Edidrom_write();
#endif

    tc358860xgb_sync_init();

#ifdef _DP_MIPI_4LANE_ //DP->MIPI 4 Lane Amy
	// IO Voltahge Setting
	i2c1_edp2dsi_write32(0x0800,0x00000001); // IOB_CTRL1
	// Boot Settings
	i2c1_edp2dsi_write32(0x1000,0x00006978); // BootWaitCount
	i2c1_edp2dsi_write32(0x1004,0x00040902); // Boot Set0
	i2c1_edp2dsi_write32(0x1008,0x03B8000A); // Boot Set1 //4lane:0x03B8000A 2lane:0x038C000A
	i2c1_edp2dsi_write32(0x100C,0x03000321); // Boot Set2 //4lane:0x03000321 2lane:0x030003B1
	i2c1_edp2dsi_write32(0x1010,0x01040020); // Boot Set3
	i2c1_edp2dsi_write32(0x1014,0x00000001); // Boot Ctrl
	DelayMs(1);

	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000002){;} // Check if 0x1018<bit2:0> is expected value
	// Internal PCLK Setting for Non Preset or REFCLK=26MHz
	i2c1_edp2dsi_write8(0xB005,0x0A); // SET CG_VIDPLL_CTRL1
	i2c1_edp2dsi_write8(0xB006,0x03); // SET CG_VIDPLL_CTRL2
	i2c1_edp2dsi_write8(0xB007,0x16); // SET CG_VIDPLL_CTRL3
	i2c1_edp2dsi_write8(0xB008,0x00); // SET CG_VIDPLL_CTRL4
	i2c1_edp2dsi_write8(0xB009,0x21); // SET CG_VIDPLL_CTRL5
	i2c1_edp2dsi_write8(0xB00A,0x07); // SET CG_VIDPLL_CTRL6
	// DSI Clock setting for Non Preset or REFCLK=26MHz
	i2c1_edp2dsi_write32(0x41B0,0x00002624); // MIPI_PLL_CNF
	i2c1_edp2dsi_write32(0x41BC,0x00000A01); // MIPI_PLL_PARA
	i2c1_edp2dsi_write32(0x41C0,0x00000030); // MIPI_PLL_FIX
	i2c1_edp2dsi_write32(0x1014,0x00000003); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000006){;} // Check if 0x1018<bit2:0> is expected value
	// Additional Setting for eDP
	i2c1_edp2dsi_write8(0x8200,0x01); // Sink Count
	i2c1_edp2dsi_write8(0x8003,0x41); // Max Downspread
	i2c1_edp2dsi_write8(0xB400,0x0D); // AL Mode Control Link
	// DPRX CAD Register Setting
	i2c1_edp2dsi_write8(0xB88E,0xFF); // Set CR_OPT_WCNT0
	i2c1_edp2dsi_write8(0xB88F,0xFF); // Set CR_OPT_WCNT1
	i2c1_edp2dsi_write8(0xB89A,0xFF); // Set CR_OPT_WCNT2
	i2c1_edp2dsi_write8(0xB89B,0xFF); // Set CR_OPT_WCNT3
	i2c1_edp2dsi_write8(0xB800,0x0E); // Set CDR_PHASE_LP_EN
	i2c1_edp2dsi_write8(0xBB26,0x02); // RX_VREG_VALUE
	i2c1_edp2dsi_write8(0xBB01,0x20); // RX_VREG_ENABLE
	i2c1_edp2dsi_write8(0xB8C0,0xF1); // RX_CDR_LUT1
	i2c1_edp2dsi_write8(0xB8C1,0xF1); // RX_CDR_LUT2
	i2c1_edp2dsi_write8(0xB8C2,0xF0); // RX_CDR_LUT3
	i2c1_edp2dsi_write8(0xB8C3,0xF0); // RX_CDR_LUT4
	i2c1_edp2dsi_write8(0xB8C4,0xF0); // RX_CDR_LUT5
	i2c1_edp2dsi_write8(0xB8C5,0xF0); // RX_CDR_LUT6
	i2c1_edp2dsi_write8(0xB8C6,0xF0); // RX_CDR_LUT7
	i2c1_edp2dsi_write8(0xB8C7,0xF0); // RX_CDR_LUT8
	i2c1_edp2dsi_write8(0xB80B,0x00); // PLL_CP1P1
	i2c1_edp2dsi_write8(0xB833,0x00); // PLL_CP1P2
	i2c1_edp2dsi_write8(0xB85B,0x00); // PLL_CP1P3
	i2c1_edp2dsi_write8(0xB810,0x00); // PLL_CP2P1
	i2c1_edp2dsi_write8(0xB838,0x00); // PLL_CP2P2
	i2c1_edp2dsi_write8(0xB860,0x00); // PLL_CP2P3
	i2c1_edp2dsi_write8(0xB815,0x00); // PLL_CP2P4
	i2c1_edp2dsi_write8(0xB83D,0x00); // PLL_CP2P5
	i2c1_edp2dsi_write8(0xB865,0x00); // PLL_CP2P6
	i2c1_edp2dsi_write8(0xB81A,0x00); // PLL_CP2P7
	i2c1_edp2dsi_write8(0xB842,0x00); // PLL_CP2P8
	i2c1_edp2dsi_write8(0xB86A,0x00); // PLL_CP2P9
	i2c1_edp2dsi_write8(0xB81F,0x00); // PLL_CP3P1
	i2c1_edp2dsi_write8(0xB847,0x00); // PLL_CP3P2
	i2c1_edp2dsi_write8(0xB86F,0x00); // PLL_CP3P3
	i2c1_edp2dsi_write8(0xB824,0x00); // PLL_CP4P1
	i2c1_edp2dsi_write8(0xB84C,0x00); // PLL_CP4P2
	i2c1_edp2dsi_write8(0xB874,0x00); // PLL_CP4P3
	i2c1_edp2dsi_write8(0xB829,0x00); // PLL_CP4P4
	i2c1_edp2dsi_write8(0xB851,0x00); // PLL_CP4P5
	i2c1_edp2dsi_write8(0xB879,0x00); // PLL_CP4P6
	i2c1_edp2dsi_write8(0xB82E,0x00); // PLL_CP5P7
	i2c1_edp2dsi_write8(0xB856,0x00); // PLL_CP5P2
	i2c1_edp2dsi_write8(0xB87E,0x00); // PLL_CP5P3
	i2c1_edp2dsi_write8(0xBB90,0x10); // ctle_em_data_rate_control_0[7:0]
	i2c1_edp2dsi_write8(0xBB91,0x0F); // ctle_em_data_rate_control_1[7:0]
	i2c1_edp2dsi_write8(0xBB92,0xF6); // ctle_em_data_rate_control_2[7:0]
	i2c1_edp2dsi_write8(0xBB93,0x10); // ctle_em_data_rate_control_3[7:0]
	i2c1_edp2dsi_write8(0xBB94,0x0F); // ctle_em_data_rate_control_4[7:0]
	i2c1_edp2dsi_write8(0xBB95,0xF6); // ctle_em_data_rate_control_5[7:0]
	i2c1_edp2dsi_write8(0xBB96,0x10); // ctle_em_data_rate_control_6[7:0]
	i2c1_edp2dsi_write8(0xBB97,0x0F); // ctle_em_data_rate_control_7[7:0]
	i2c1_edp2dsi_write8(0xBB98,0xF6); // ctle_em_data_rate_control_8[7:0]
	i2c1_edp2dsi_write8(0xBB99,0x10); // ctle_em_data_rate_control_A[7:0]
	i2c1_edp2dsi_write8(0xBB9A,0x0F); // ctle_em_data_rate_control_B[7:0]
	i2c1_edp2dsi_write8(0xBB9B,0xF6); // ctle_em_data_rate_control_0[7:0]
	i2c1_edp2dsi_write8(0xB88A,0x03); // CR_OPT_CTRL
	i2c1_edp2dsi_write8(0xB896,0x03); // EQ_OPT_CTRL
	i2c1_edp2dsi_write8(0xBBD1,0x07); // ctle_em_contro_1
	i2c1_edp2dsi_write8(0xBBB0,0x07); // eye_configuration_0
	i2c1_edp2dsi_write8(0xB88B,0x04); // CR_OPT_MIN_EYE_VALID
	i2c1_edp2dsi_write8(0xB88C,0x45); // CR_OPT_WCNT0_EYE
	i2c1_edp2dsi_write8(0xB88D,0x05); // CT_OPT_WCNT1_EYE
	i2c1_edp2dsi_write8(0xB897,0x04); // EQ_OPT_MIN_EYE_VALID
	i2c1_edp2dsi_write8(0xB898,0xE0); // EQ_OPT_WCNT0_FEQ
	i2c1_edp2dsi_write8(0xB899,0x2E); // EQ_OPT_WCNT1_FEQ
	i2c1_edp2dsi_write8(0x800E,0x00); // TRAINING_AUX_RD_INTERVAL
	i2c1_edp2dsi_write32(0x1014,0x00000007); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000007){;} // Check if 0x1018<bit2:0> is expected value
	// Video Size Related Setting for Non Preset
	i2c1_edp2dsi_write32(0x0110,0x000002D0); // HTIM2_LEFT
	i2c1_edp2dsi_write32(0x0124,0x000002D0); // HTIM2_RIGHT
	i2c1_edp2dsi_write32(0x0148,0x000005A0); // DPRX_HTIM2
	i2c1_edp2dsi_write32(0x2920,0x3E0B0870); // DSI0_PIC_SYN_PKT_A
	i2c1_edp2dsi_write32(0x3920,0x3E0B0870); // DSI1_PIC_SYN_PKT_A
	// eDP Settings for Link Training
	while((rdata8=i2c1_edp2dsi_read8(0xB631, &data8))!=0x01){;} // Check if 0xB631<bit1:0>=01b.
	//i2c1_edp2dsi_write8(0x8000,0x11); // DPCD Rev
	i2c1_edp2dsi_write8(0x8001,0x14); // Max Link Rate //0X14
	i2c1_edp2dsi_write8(0x8002,0x04); // Max Lane Count
	i2c1_edp2dsi_write8(0xB608,0x0B); // Set AUXTXHSEN
	i2c1_edp2dsi_write8(0xB800,0x1E); // Set CDR_PHASE_LP_EN
	i2c1_edp2dsi_write8(0x8700,0x00); // DPCD 0700h

	// For DP EDID Read
	i2c1_edp2dsi_write8(0xB201,0x30); //
	i2c1_edp2dsi_write16(0xB202,0x0000); //
#ifdef _GPIO_EDID_
	i2c1_edp2dsi_write32(0xB204,0x00000011); // 	
#else
	i2c1_edp2dsi_write32(0xB204,0x00000004); // 
#endif

	// VSYNC monitor output setting for DP connection
	i2c1_edp2dsi_write32(0x5010,0x016A0000); // Monitor Signal Selection
	i2c1_edp2dsi_write32(0x008C,0x00000040); // GPIOOUTMODE
	i2c1_edp2dsi_write32(0x0080,0x00000002); // GPIOC
	// I2C direction control for DP connection
	i2c1_edp2dsi_write32(0x00B0,0x00000001); // I2CMD_SL


	//EDID Write--Save the screen information
#if defined _GPIO_EDID_
	printf ("AUO2.1 60fps -- start read edid!\n");
	edid_send_init();
	DelayMs(600);
	edid_send_deinit();
#elif defined _EEPROM_EDID_
	edid_eeprom_send_init();
	DelayMs(600);
	edid_send_deinit();
#endif

#if !defined _GPIO_EDID_ && !defined _EEPROM_EDID_
	DelayMs(500);
#endif

	
	printf ("AUO2.1 60fps -- 4LANE start traning!\n");

	// Start Link Training
	// By detecting VSYNC monitor output (INT rising edge)
	// I2C direction control for DP connection
	// Check if VSYNC is detected on monitor signal
	i2c1_edp2dsi_write32(0x00B0,0x00000000); // I2CMD_SL
	i2c1_edp2dsi_write32(0x00B0,0x00000000); // I2CMD_SL
	i2c1_edp2dsi_write32(0x00B0,0x00000000); // I2CMD_SL

	// After Link Training finishes
	// Check Link Training Status
	while((rdata8=i2c1_edp2dsi_read8(0x8202, &data8))!=0x77){;} // Check if 0x8203 is expected value.
	printf ("Lane 1 trainning ok\n");

	while((rdata8=i2c1_edp2dsi_read8(0x8203, &data8))!=0x77){;} // Check if 0x8203 is expected value.
	printf ("Lane 2,3 trainning ok\n");

	while(((rdata8=i2c1_edp2dsi_read8(0x8204, &data8)) & 0x01)!=0x01){;} // Check if 0x8204 is expected value.
	printf ("Lane 4 trainning ok\n");

	// DSI Transition Time Setting for Non Preset
	i2c1_edp2dsi_write32(0x4154,0x00000006); // PPI_DPHY_LPTXTIMECNT
	i2c1_edp2dsi_write32(0x4158,0x00140003); // PPI_DPHY_TCLK_HEADERCNT
	i2c1_edp2dsi_write32(0x415C,0x00060002); // PPI_DPHY_TCLK_TRAILCNT
	i2c1_edp2dsi_write32(0x4160,0x000A0003); // PPI_DPHY_THS_HEADERCNT
	i2c1_edp2dsi_write32(0x4164,0x00002710); // PPI_DPHY_TWAKEUPCNT
	i2c1_edp2dsi_write32(0x4168,0x0000000A); // PPI_DPHY_TCLK_POSTCNT
	i2c1_edp2dsi_write32(0x416C,0x00070005); // PPI_DPHY_THSTRAILCNT
	i2c1_edp2dsi_write32(0x4178,0x00060006); // PPI_DSI_BTA_COUNT
	// DSI Start
	i2c1_edp2dsi_write32(0x407C,0x00000081); // DSI_DSITX_MODE
	i2c1_edp2dsi_write32(0x4050,0x00000000); // DSI_FUNC_MODE
	i2c1_edp2dsi_write32(0x401C,0x00000001); // DSI_DSITX_START
	DelayUs(100);
	while((rdata32=i2c1_edp2dsi_read32(0x4060, data32))!=0x00000003){;} // Check if 0x2060/4060<bit1:0>=11b.
	// GPIO setting for LCD control.  (Depends on LCD specification and System configuration)
	i2c1_edp2dsi_write32(0x0804,0x00000000); // IOB_CTRL2
	i2c1_edp2dsi_write32(0x0080,0x0000000F); // GPIOC
	i2c1_edp2dsi_write32(0x0084,0x0000000F); // GPIOO
	i2c1_edp2dsi_write32(0x0084,0x00000000); // GPIOO
	i2c1_edp2dsi_write32(0x0084,0x0000000F); // GPIOO
	DelayMs(50);
	// DSI Hs Clock Mode
	i2c1_edp2dsi_write32(0x4050,0x00000020); // DSI_FUNC_MODE
	DelayMs(100);
	// Command Transmission Before Video Start. (Depeds on LCD specification)
	// LCD Initialization

	// LCD Initialization
	LCD_init_LPMode();

	i2c1_edp2dsi_write32(0x2A10,0x80040010); // DSI0_CQMODE
	i2c1_edp2dsi_write32(0x3A10,0x80040010); // DSI1_CQMODE
	i2c1_edp2dsi_write32(0x2A04,0x00000001); // DSI0_VideoSTART
	i2c1_edp2dsi_write32(0x3A04,0x00000001); // DSI1_VideoSTART
	// Check if eDP video is coming
	i2c1_edp2dsi_write32(0x0154,0x00000001); // Set_DPVideoEn
	// Command Transmission After Video Start. (Depends on LCD specification)
	//LCD_BL_On();
#endif
	
#ifdef _DP_MIPI_1LANE_ //DP->MIPI 1 Lane
	// IO Voltahge Setting
	i2c1_edp2dsi_write32(0x0800,0x00000001); // IOB_CTRL1
	// Boot Settings
	i2c1_edp2dsi_write32(0x1000,0x00006978); // BootWaitCount
	i2c1_edp2dsi_write32(0x1004,0x00040902); // Boot Set0
	i2c1_edp2dsi_write32(0x1008,0x03B8000A); // Boot Set1
	i2c1_edp2dsi_write32(0x100C,0x03000321); // Boot Set2
	i2c1_edp2dsi_write32(0x1010,0x01040020); // Boot Set3
	i2c1_edp2dsi_write32(0x1014,0x00000001); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018,data32))!=0x00000002){;} // Check if 0x1018<bit2:0> is expected value
	// Internal PCLK Setting for Non Preset or REFCLK=26MHz
	i2c1_edp2dsi_write8(0xB005,0x0A); // SET CG_VIDPLL_CTRL1
	i2c1_edp2dsi_write8(0xB006,0x03); // SET CG_VIDPLL_CTRL2
	i2c1_edp2dsi_write8(0xB007,0x16); // SET CG_VIDPLL_CTRL3
	i2c1_edp2dsi_write8(0xB008,0x00); // SET CG_VIDPLL_CTRL4
	i2c1_edp2dsi_write8(0xB009,0x21); // SET CG_VIDPLL_CTRL5
	i2c1_edp2dsi_write8(0xB00A,0x07); // SET CG_VIDPLL_CTRL6
	// DSI Clock setting for Non Preset or REFCLK=26MHz
	i2c1_edp2dsi_write32(0x41B0,0x00002624); // MIPI_PLL_CNF
	i2c1_edp2dsi_write32(0x41BC,0x00000A01); // MIPI_PLL_PARA
	i2c1_edp2dsi_write32(0x41C0,0x00000030); // MIPI_PLL_FIX
	i2c1_edp2dsi_write32(0x1014,0x00000003); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018,data32))!=0x00000006){;} // Check if 0x1018<bit2:0> is expected value
	// Additional Setting for eDP
	i2c1_edp2dsi_write8(0x8200,0x01); // Sink Count
	i2c1_edp2dsi_write8(0x8003,0x41); // Max Downspread
	i2c1_edp2dsi_write8(0xB400,0x0D); // AL Mode Control Link
	// DPRX CAD Register Setting
	i2c1_edp2dsi_write8(0xB88E,0xFF); // Set CR_OPT_WCNT0
	i2c1_edp2dsi_write8(0xB88F,0xFF); // Set CR_OPT_WCNT1
	i2c1_edp2dsi_write8(0xB89A,0xFF); // Set CR_OPT_WCNT2
	i2c1_edp2dsi_write8(0xB89B,0xFF); // Set CR_OPT_WCNT3
	i2c1_edp2dsi_write8(0xB800,0x0E); // Set CDR_PHASE_LP_EN
	i2c1_edp2dsi_write8(0xBB26,0x02); // RX_VREG_VALUE
	i2c1_edp2dsi_write8(0xBB01,0x20); // RX_VREG_ENABLE
	i2c1_edp2dsi_write8(0xB8C0,0xF1); // RX_CDR_LUT1
	i2c1_edp2dsi_write8(0xB8C1,0xF1); // RX_CDR_LUT2
	i2c1_edp2dsi_write8(0xB8C2,0xF0); // RX_CDR_LUT3
	i2c1_edp2dsi_write8(0xB8C3,0xF0); // RX_CDR_LUT4
	i2c1_edp2dsi_write8(0xB8C4,0xF0); // RX_CDR_LUT5
	i2c1_edp2dsi_write8(0xB8C5,0xF0); // RX_CDR_LUT6
	i2c1_edp2dsi_write8(0xB8C6,0xF0); // RX_CDR_LUT7
	i2c1_edp2dsi_write8(0xB8C7,0xF0); // RX_CDR_LUT8
	i2c1_edp2dsi_write8(0xB80B,0x00); // PLL_CP1P1
	i2c1_edp2dsi_write8(0xB833,0x00); // PLL_CP1P2
	i2c1_edp2dsi_write8(0xB85B,0x00); // PLL_CP1P3
	i2c1_edp2dsi_write8(0xB810,0x00); // PLL_CP2P1
	i2c1_edp2dsi_write8(0xB838,0x00); // PLL_CP2P2
	i2c1_edp2dsi_write8(0xB860,0x00); // PLL_CP2P3
	i2c1_edp2dsi_write8(0xB815,0x00); // PLL_CP2P4
	i2c1_edp2dsi_write8(0xB83D,0x00); // PLL_CP2P5
	i2c1_edp2dsi_write8(0xB865,0x00); // PLL_CP2P6
	i2c1_edp2dsi_write8(0xB81A,0x00); // PLL_CP2P7
	i2c1_edp2dsi_write8(0xB842,0x00); // PLL_CP2P8
	i2c1_edp2dsi_write8(0xB86A,0x00); // PLL_CP2P9
	i2c1_edp2dsi_write8(0xB81F,0x00); // PLL_CP3P1
	i2c1_edp2dsi_write8(0xB847,0x00); // PLL_CP3P2
	i2c1_edp2dsi_write8(0xB86F,0x00); // PLL_CP3P3
	i2c1_edp2dsi_write8(0xB824,0x00); // PLL_CP4P1
	i2c1_edp2dsi_write8(0xB84C,0x00); // PLL_CP4P2
	i2c1_edp2dsi_write8(0xB874,0x00); // PLL_CP4P3
	i2c1_edp2dsi_write8(0xB829,0x00); // PLL_CP4P4
	i2c1_edp2dsi_write8(0xB851,0x00); // PLL_CP4P5
	i2c1_edp2dsi_write8(0xB879,0x00); // PLL_CP4P6
	i2c1_edp2dsi_write8(0xB82E,0x00); // PLL_CP5P7
	i2c1_edp2dsi_write8(0xB856,0x00); // PLL_CP5P2
	i2c1_edp2dsi_write8(0xB87E,0x00); // PLL_CP5P3
	i2c1_edp2dsi_write8(0xBB90,0x10); // ctle_em_data_rate_control_0[7:0]
	i2c1_edp2dsi_write8(0xBB91,0x0F); // ctle_em_data_rate_control_1[7:0]
	i2c1_edp2dsi_write8(0xBB92,0xF6); // ctle_em_data_rate_control_2[7:0]
	i2c1_edp2dsi_write8(0xBB93,0x10); // ctle_em_data_rate_control_3[7:0]
	i2c1_edp2dsi_write8(0xBB94,0x0F); // ctle_em_data_rate_control_4[7:0]
	i2c1_edp2dsi_write8(0xBB95,0xF6); // ctle_em_data_rate_control_5[7:0]
	i2c1_edp2dsi_write8(0xBB96,0x10); // ctle_em_data_rate_control_6[7:0]
	i2c1_edp2dsi_write8(0xBB97,0x0F); // ctle_em_data_rate_control_7[7:0]
	i2c1_edp2dsi_write8(0xBB98,0xF6); // ctle_em_data_rate_control_8[7:0]
	i2c1_edp2dsi_write8(0xBB99,0x10); // ctle_em_data_rate_control_A[7:0]
	i2c1_edp2dsi_write8(0xBB9A,0x0F); // ctle_em_data_rate_control_B[7:0]
	i2c1_edp2dsi_write8(0xBB9B,0xF6); // ctle_em_data_rate_control_0[7:0]
	i2c1_edp2dsi_write8(0xB88A,0x03); // CR_OPT_CTRL
	i2c1_edp2dsi_write8(0xB896,0x03); // EQ_OPT_CTRL
	i2c1_edp2dsi_write8(0xBBD1,0x07); // ctle_em_contro_1
	i2c1_edp2dsi_write8(0xBBB0,0x07); // eye_configuration_0
	i2c1_edp2dsi_write8(0xB88B,0x04); // CR_OPT_MIN_EYE_VALID
	i2c1_edp2dsi_write8(0xB88C,0x45); // CR_OPT_WCNT0_EYE
	i2c1_edp2dsi_write8(0xB88D,0x05); // CT_OPT_WCNT1_EYE
	i2c1_edp2dsi_write8(0xB897,0x04); // EQ_OPT_MIN_EYE_VALID
	i2c1_edp2dsi_write8(0xB898,0xE0); // EQ_OPT_WCNT0_FEQ
	i2c1_edp2dsi_write8(0xB899,0x2E); // EQ_OPT_WCNT1_FEQ
	i2c1_edp2dsi_write8(0x800E,0x00); // TRAINING_AUX_RD_INTERVAL
	i2c1_edp2dsi_write32(0x1014,0x00000007); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018,data32))!=0x00000007){;} // Check if 0x1018<bit2:0> is expected value
	// Video Size Related Setting for Non Preset
	i2c1_edp2dsi_write32(0x0110,0x000002D0); // HTIM2_LEFT
	i2c1_edp2dsi_write32(0x0124,0x000002D0); // HTIM2_RIGHT
	i2c1_edp2dsi_write32(0x0148,0x000005A0); // DPRX_HTIM2
	i2c1_edp2dsi_write32(0x2920,0x3E0B0870); // DSI0_PIC_SYN_PKT_A
	i2c1_edp2dsi_write32(0x3920,0x3E0B0870); // DSI1_PIC_SYN_PKT_A
	// eDP Settings for Link Training
	while((rdata8=i2c1_edp2dsi_read8(0xB631,&data8))!=0x01){;} // Check if 0xB631<bit1:0>=01b.
	printf ("init ------1\n");
	i2c1_edp2dsi_write8(0x8001,0x14); // Max Link Rate 5.4Gb/s(0x14) 0x0A
	i2c1_edp2dsi_write8(0x8002,0x01); // Max Lane Count
	i2c1_edp2dsi_write8(0xB608,0x0B); // Set AUXTXHSEN
	i2c1_edp2dsi_write8(0xB800,0x1E); // Set CDR_PHASE_LP_EN
	i2c1_edp2dsi_write8(0x8700,0x00); // DPCD 0700h

	// For DP EDID Read
	i2c1_edp2dsi_write8(0xB201,0x30); //
	i2c1_edp2dsi_write16(0xB202,0x0000); //
#ifdef _EEPROM_EDID_
	i2c1_edp2dsi_write32(0xB204,0x00000004); // 	
#else
	i2c1_edp2dsi_write32(0xB204,0x00000011); // 
#endif

	// VSYNC monitor output setting for DP connection
	i2c1_edp2dsi_write32(0x5010,0x016A0000); // Monitor Signal Selection
	i2c1_edp2dsi_write32(0x008C,0x00000040); // GPIOOUTMODE
	i2c1_edp2dsi_write32(0x0080,0x00000002); // GPIOC
	// I2C direction control for DP connection
	i2c1_edp2dsi_write32(0x00B0,0x00000001); // I2CMD_SL

	//EDID Write--Save the screen information
#if defined _GPIO_EDID_
	printf ("AUO2.1 60fps -- start read edid!\n");
	edid_send_init();
	DelayMs(1000);
	edid_send_deinit();
#elif defined _EEPROM_EDID_
	edid_eeprom_send_init();
	DelayMs(1000);
	edid_send_deinit();
#endif

#if !defined _GPIO_EDID_ && !defined _EEPROM_EDID_
	DelayMs(1000);
#endif
	
	printf ("1LANE start traning!\n");

	// Start Link Training
	// By detecting VSYNC monitor output (INT rising edge)
	// I2C direction control for DP connection
	// Check if VSYNC is detected on monitor signal
	i2c1_edp2dsi_write32(0x00B0,0x00000000); // I2CMD_SL
	// After Link Training finishes
	// Check Link Training Status
	while((rdata8=i2c1_edp2dsi_read8(0x8202,&data8))!=0x07){;} // Check if 0x8203 is expected value.
	while(((rdata8=i2c1_edp2dsi_read8(0x8204,&data8)) & 0x01)!=0x01){;} // Check if 0x8204 is expected value.
	// DSI Transition Time Setting for Non Preset
	i2c1_edp2dsi_write32(0x4154,0x00000006); // PPI_DPHY_LPTXTIMECNT
	i2c1_edp2dsi_write32(0x4158,0x00140003); // PPI_DPHY_TCLK_HEADERCNT
	i2c1_edp2dsi_write32(0x415C,0x00060002); // PPI_DPHY_TCLK_TRAILCNT
	i2c1_edp2dsi_write32(0x4160,0x000A0003); // PPI_DPHY_THS_HEADERCNT
	i2c1_edp2dsi_write32(0x4164,0x00002710); // PPI_DPHY_TWAKEUPCNT
	i2c1_edp2dsi_write32(0x4168,0x0000000A); // PPI_DPHY_TCLK_POSTCNT
	i2c1_edp2dsi_write32(0x416C,0x00070005); // PPI_DPHY_THSTRAILCNT
	i2c1_edp2dsi_write32(0x4178,0x00060006); // PPI_DSI_BTA_COUNT
	// DSI Start
	i2c1_edp2dsi_write32(0x407C,0x00000081); // DSI_DSITX_MODE
	i2c1_edp2dsi_write32(0x4050,0x00000000); // DSI_FUNC_MODE
	i2c1_edp2dsi_write32(0x401C,0x00000001); // DSI_DSITX_START
	DelayUs(100);
	while((rdata32=i2c1_edp2dsi_read32(0x4060,data32))!=0x00000003){;} // Check if 0x2060/4060<bit1:0>=11b.
	// GPIO setting for LCD control.  (Depends on LCD specification and System configuration)
	i2c1_edp2dsi_write32(0x0804,0x00000000); // IOB_CTRL2
	i2c1_edp2dsi_write32(0x0080,0x0000000F); // GPIOC
	i2c1_edp2dsi_write32(0x0084,0x0000000F); // GPIOO
	i2c1_edp2dsi_write32(0x0084,0x00000000); // GPIOO
	i2c1_edp2dsi_write32(0x0084,0x0000000F); // GPIOO
	DelayMs(50);
	// DSI Hs Clock Mode
	i2c1_edp2dsi_write32(0x4050,0x00000020); // DSI_FUNC_MODE
	DelayMs(100);
	// Command Transmission Before Video Start. (Depeds on LCD specification)
	// LCD Initialization
	LCD_init_LPMode();


	i2c1_edp2dsi_write32(0x2A10,0x80040010); // DSI0_CQMODE
	i2c1_edp2dsi_write32(0x3A10,0x80040010); // DSI1_CQMODE
	i2c1_edp2dsi_write32(0x2A04,0x00000001); // DSI0_VideoSTART
	i2c1_edp2dsi_write32(0x3A04,0x00000001); // DSI1_VideoSTART
	// Check if eDP video is coming
	i2c1_edp2dsi_write32(0x0154,0x00000001); // Set_DPVideoEn
	// Command Transmission After Video Start. (Depends on LCD specification)
	LCD_BL_On();
	
#endif
	
	
#ifdef _DP_MIPI_2LANE_ //DP->MIPI 2lane
	// IO Voltahge Setting
	i2c1_edp2dsi_write32(0x0800,0x00000001); // IOB_CTRL1
	// Boot Settings
	i2c1_edp2dsi_write32(0x1000,0x00006978); // BootWaitCount
	i2c1_edp2dsi_write32(0x1004,0x00040901); // Boot Set0
	i2c1_edp2dsi_write32(0x1008,0x03B8000A); // Boot Set1
	i2c1_edp2dsi_write32(0x100C,0x03000321); // Boot Set2
	i2c1_edp2dsi_write32(0x1010,0x01040020); // Boot Set3
	i2c1_edp2dsi_write32(0x1014,0x00000001); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000002){;} // Check if 0x1018<bit2:0> is expected value
	// Internal PCLK Setting for Non Preset or REFCLK=26MHz
	i2c1_edp2dsi_write8(0xB005,0x0A); // SET CG_VIDPLL_CTRL1
	i2c1_edp2dsi_write8(0xB006,0x01); // SET CG_VIDPLL_CTRL2
	i2c1_edp2dsi_write8(0xB007,0x0A); // SET CG_VIDPLL_CTRL3
	i2c1_edp2dsi_write8(0xB008,0x00); // SET CG_VIDPLL_CTRL4
	i2c1_edp2dsi_write8(0xB009,0x21); // SET CG_VIDPLL_CTRL5
	i2c1_edp2dsi_write8(0xB00A,0x07); // SET CG_VIDPLL_CTRL6
	// DSI Clock setting for Non Preset or REFCLK=26MHz
	i2c1_edp2dsi_write32(0x41B0,0x00002624); // MIPI_PLL_CNF
	i2c1_edp2dsi_write32(0x41BC,0x00000A01); // MIPI_PLL_PARA
	i2c1_edp2dsi_write32(0x41C0,0x00000030); // MIPI_PLL_FIX
	i2c1_edp2dsi_write32(0x1014,0x00000003); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000006){;} // Check if 0x1018<bit2:0> is expected value
	// Additional Setting for eDP
	i2c1_edp2dsi_write8(0x8200,0x01); // Sink Count
	i2c1_edp2dsi_write8(0x8003,0x41); // Max Downspread
	i2c1_edp2dsi_write8(0xB400,0x0D); // AL Mode Control Link
	// DPRX CAD Register Setting
	i2c1_edp2dsi_write8(0xB88E,0xFF); // Set CR_OPT_WCNT0
	i2c1_edp2dsi_write8(0xB88F,0xFF); // Set CR_OPT_WCNT1
	i2c1_edp2dsi_write8(0xB89A,0xFF); // Set CR_OPT_WCNT2
	i2c1_edp2dsi_write8(0xB89B,0xFF); // Set CR_OPT_WCNT3
	i2c1_edp2dsi_write8(0xB800,0x0E); // Set CDR_PHASE_LP_EN
	i2c1_edp2dsi_write8(0xBB26,0x02); // RX_VREG_VALUE
	i2c1_edp2dsi_write8(0xBB01,0x20); // RX_VREG_ENABLE
	i2c1_edp2dsi_write8(0xB8C0,0xF1); // RX_CDR_LUT1
	i2c1_edp2dsi_write8(0xB8C1,0xF1); // RX_CDR_LUT2
	i2c1_edp2dsi_write8(0xB8C2,0xF0); // RX_CDR_LUT3
	i2c1_edp2dsi_write8(0xB8C3,0xF0); // RX_CDR_LUT4
	i2c1_edp2dsi_write8(0xB8C4,0xF0); // RX_CDR_LUT5
	i2c1_edp2dsi_write8(0xB8C5,0xF0); // RX_CDR_LUT6
	i2c1_edp2dsi_write8(0xB8C6,0xF0); // RX_CDR_LUT7
	i2c1_edp2dsi_write8(0xB8C7,0xF0); // RX_CDR_LUT8
	i2c1_edp2dsi_write8(0xB80B,0x00); // PLL_CP1P1
	i2c1_edp2dsi_write8(0xB833,0x00); // PLL_CP1P2
	i2c1_edp2dsi_write8(0xB85B,0x00); // PLL_CP1P3
	i2c1_edp2dsi_write8(0xB810,0x00); // PLL_CP2P1
	i2c1_edp2dsi_write8(0xB838,0x00); // PLL_CP2P2
	i2c1_edp2dsi_write8(0xB860,0x00); // PLL_CP2P3
	i2c1_edp2dsi_write8(0xB815,0x00); // PLL_CP2P4
	i2c1_edp2dsi_write8(0xB83D,0x00); // PLL_CP2P5
	i2c1_edp2dsi_write8(0xB865,0x00); // PLL_CP2P6
	i2c1_edp2dsi_write8(0xB81A,0x00); // PLL_CP2P7
	i2c1_edp2dsi_write8(0xB842,0x00); // PLL_CP2P8
	i2c1_edp2dsi_write8(0xB86A,0x00); // PLL_CP2P9
	i2c1_edp2dsi_write8(0xB81F,0x00); // PLL_CP3P1
	i2c1_edp2dsi_write8(0xB847,0x00); // PLL_CP3P2
	i2c1_edp2dsi_write8(0xB86F,0x00); // PLL_CP3P3
	i2c1_edp2dsi_write8(0xB824,0x00); // PLL_CP4P1
	i2c1_edp2dsi_write8(0xB84C,0x00); // PLL_CP4P2
	i2c1_edp2dsi_write8(0xB874,0x00); // PLL_CP4P3
	i2c1_edp2dsi_write8(0xB829,0x00); // PLL_CP4P4
	i2c1_edp2dsi_write8(0xB851,0x00); // PLL_CP4P5
	i2c1_edp2dsi_write8(0xB879,0x00); // PLL_CP4P6
	i2c1_edp2dsi_write8(0xB82E,0x00); // PLL_CP5P7
	i2c1_edp2dsi_write8(0xB856,0x00); // PLL_CP5P2
	i2c1_edp2dsi_write8(0xB87E,0x00); // PLL_CP5P3
	i2c1_edp2dsi_write8(0xBB90,0x10); // ctle_em_data_rate_control_0[7:0]
	i2c1_edp2dsi_write8(0xBB91,0x0F); // ctle_em_data_rate_control_1[7:0]
	i2c1_edp2dsi_write8(0xBB92,0xF6); // ctle_em_data_rate_control_2[7:0]
	i2c1_edp2dsi_write8(0xBB93,0x10); // ctle_em_data_rate_control_3[7:0]
	i2c1_edp2dsi_write8(0xBB94,0x0F); // ctle_em_data_rate_control_4[7:0]
	i2c1_edp2dsi_write8(0xBB95,0xF6); // ctle_em_data_rate_control_5[7:0]
	i2c1_edp2dsi_write8(0xBB96,0x10); // ctle_em_data_rate_control_6[7:0]
	i2c1_edp2dsi_write8(0xBB97,0x0F); // ctle_em_data_rate_control_7[7:0]
	i2c1_edp2dsi_write8(0xBB98,0xF6); // ctle_em_data_rate_control_8[7:0]
	i2c1_edp2dsi_write8(0xBB99,0x10); // ctle_em_data_rate_control_A[7:0]
	i2c1_edp2dsi_write8(0xBB9A,0x0F); // ctle_em_data_rate_control_B[7:0]
	i2c1_edp2dsi_write8(0xBB9B,0xF6); // ctle_em_data_rate_control_0[7:0]
	i2c1_edp2dsi_write8(0xB88A,0x03); // CR_OPT_CTRL
	i2c1_edp2dsi_write8(0xB896,0x03); // EQ_OPT_CTRL
	i2c1_edp2dsi_write8(0xBBD1,0x07); // ctle_em_contro_1
	i2c1_edp2dsi_write8(0xBBB0,0x07); // eye_configuration_0
	i2c1_edp2dsi_write8(0xB88B,0x04); // CR_OPT_MIN_EYE_VALID
	i2c1_edp2dsi_write8(0xB88C,0x45); // CR_OPT_WCNT0_EYE
	i2c1_edp2dsi_write8(0xB88D,0x05); // CT_OPT_WCNT1_EYE
	i2c1_edp2dsi_write8(0xB897,0x04); // EQ_OPT_MIN_EYE_VALID
	i2c1_edp2dsi_write8(0xB898,0xE0); // EQ_OPT_WCNT0_FEQ
	i2c1_edp2dsi_write8(0xB899,0x2E); // EQ_OPT_WCNT1_FEQ
	i2c1_edp2dsi_write8(0x800E,0x00); // TRAINING_AUX_RD_INTERVAL
	i2c1_edp2dsi_write32(0x1014,0x00000007); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000007){;} // Check if 0x1018<bit2:0> is expected value
	// Video Size Related Setting for Non Preset
	i2c1_edp2dsi_write32(0x0110,0x000002D0); // HTIM2_LEFT
	i2c1_edp2dsi_write32(0x0124,0x000002D0); // HTIM2_RIGHT
	i2c1_edp2dsi_write32(0x0148,0x000005A0); // DPRX_HTIM2
	i2c1_edp2dsi_write32(0x2920,0x3E0B0870); // DSI0_PIC_SYN_PKT_A
	i2c1_edp2dsi_write32(0x3920,0x3E0B0870); // DSI1_PIC_SYN_PKT_A
	// eDP Settings for Link Training
	while((rdata8=i2c1_edp2dsi_read8(0xB631, &data8))!=0x01){;} // Check if 0xB631<bit1:0>=01b.

	i2c1_edp2dsi_write8(0x8000,0x11); // DPCD Rev
	i2c1_edp2dsi_write8(0x8001,0x0A); // Max Link Rate
	i2c1_edp2dsi_write8(0x8002,0x02); // Max Lane Count
	i2c1_edp2dsi_write8(0xB608,0x0B); // Set AUXTXHSEN
	i2c1_edp2dsi_write8(0xB800,0x1E); // Set CDR_PHASE_LP_EN
	i2c1_edp2dsi_write8(0x8700,0x00); // DPCD 0700h

	// For DP EDID Read
	i2c1_edp2dsi_write8(0xB201,0x30); //
	i2c1_edp2dsi_write16(0xB202,0x0000); //
#ifdef _EEPROM_EDID_
	i2c1_edp2dsi_write32(0xB204,0x00000004); // 	
#else
	i2c1_edp2dsi_write32(0xB204,0x00000011); // 
#endif
	// VSYNC monitor output setting for DP connection
	i2c1_edp2dsi_write32(0x5010,0x016A0000); // Monitor Signal Selection
	i2c1_edp2dsi_write32(0x008C,0x00000040); // GPIOOUTMODE
	i2c1_edp2dsi_write32(0x0080,0x00000002); // GPIOC
	// I2C direction control for DP connection
	i2c1_edp2dsi_write32(0x00B0,0x00000001); // I2CMD_SL

	//EDID Write--Save the screen information
#if defined _GPIO_EDID_
	printf ("AUO2.1 60fps -- start read edid!\n");
	edid_send_init();
	DelayMs(1000);
	edid_send_deinit();
#elif defined _EEPROM_EDID_
	edid_eeprom_send_init();
	DelayMs(1000);
	edid_send_deinit();
#endif

#if !defined _GPIO_EDID_ && !defined _EEPROM_EDID_
	DelayMs(1000);
#endif
	
	printf ("2LANE start traning!\n");

	// Start Link Training
	// By detecting VSYNC monitor output (INT rising edge)
	// I2C direction control for DP connection
	// Check if VSYNC is detected on monitor signal
	i2c1_edp2dsi_write32(0x00B0,0x00000000); // I2CMD_SL
	// After Link Training finishes
	// Check Link Training Status
	while((rdata8=i2c1_edp2dsi_read8(0x8202, &data8))!=0x77){;} // Check if 0x8203 is expected value.
	printf ("training---0x07 rdata8=0x%x\n",rdata8);
	while(((rdata8=i2c1_edp2dsi_read8(0x8204, &data8)) & 0x01)!=0x01){;} // Check if 0x8204 is expected value.
	printf ("training---rdata8=0x%x\n",rdata8);
	// DSI Transition Time Setting for Non Preset
	i2c1_edp2dsi_write32(0x4154,0x00000006); // PPI_DPHY_LPTXTIMECNT
	i2c1_edp2dsi_write32(0x4158,0x00140003); // PPI_DPHY_TCLK_HEADERCNT
	i2c1_edp2dsi_write32(0x415C,0x00060002); // PPI_DPHY_TCLK_TRAILCNT
	i2c1_edp2dsi_write32(0x4160,0x000A0003); // PPI_DPHY_THS_HEADERCNT
	i2c1_edp2dsi_write32(0x4164,0x00002710); // PPI_DPHY_TWAKEUPCNT
	i2c1_edp2dsi_write32(0x4168,0x0000000A); // PPI_DPHY_TCLK_POSTCNT
	i2c1_edp2dsi_write32(0x416C,0x00070005); // PPI_DPHY_THSTRAILCNT
	i2c1_edp2dsi_write32(0x4178,0x00060006); // PPI_DSI_BTA_COUNT
	// DSI Start
	i2c1_edp2dsi_write32(0x407C,0x00000081); // DSI_DSITX_MODE
	i2c1_edp2dsi_write32(0x4050,0x00000000); // DSI_FUNC_MODE
	i2c1_edp2dsi_write32(0x401C,0x00000001); // DSI_DSITX_START
	DelayUs(100);
	while((rdata32=i2c1_edp2dsi_read32(0x4060, data32))!=0x00000003){;} // Check if 0x2060/4060<bit1:0>=11b.
	// GPIO setting for LCD control.  (Depends on LCD specification and System configuration)
	i2c1_edp2dsi_write32(0x0804,0x00000000); // IOB_CTRL2
	i2c1_edp2dsi_write32(0x0080,0x0000000F); // GPIOC
	i2c1_edp2dsi_write32(0x0084,0x0000000F); // GPIOO
	i2c1_edp2dsi_write32(0x0084,0x00000000); // GPIOO
	i2c1_edp2dsi_write32(0x0084,0x0000000F); // GPIOO
	DelayMs(50);
	// DSI Hs Clock Mode
	i2c1_edp2dsi_write32(0x4050,0x00000020); // DSI_FUNC_MODE
	DelayMs(100);
	// Command Transmission Before Video Start. (Depeds on LCD specification)

	// LCD Initialization
	LCD_init_LPMode();

	i2c1_edp2dsi_write32(0x2A10,0x80040010); // DSI0_CQMODE
	i2c1_edp2dsi_write32(0x3A10,0x80040010); // DSI1_CQMODE
	i2c1_edp2dsi_write32(0x2A04,0x00000001); // DSI0_VideoSTART
	i2c1_edp2dsi_write32(0x3A04,0x00000001); // DSI1_VideoSTART
	// Check if eDP video is coming
	i2c1_edp2dsi_write32(0x0154,0x00000001); // Set_DPVideoEn
	// Command Transmission After Video Start. (Depends on LCD specification)
	LCD_BL_On();
#endif
	
	
#ifdef _DP_MIPI_COLORBAR_ //DP Color bar
	// IO Voltahge Setting
	i2c1_edp2dsi_write32(0x0800,0x00000001); // IOB_CTRL1
	// Boot Settings
	i2c1_edp2dsi_write32(0x1000,0x00006978); // BootWaitCount
	i2c1_edp2dsi_write32(0x1004,0x00040906); // Boot Set0
	i2c1_edp2dsi_write32(0x1008,0x038C000A); // Boot Set1
	i2c1_edp2dsi_write32(0x100C,0x03000324); // Boot Set2
	i2c1_edp2dsi_write32(0x1010,0x01040020); // Boot Set3
	i2c1_edp2dsi_write32(0x1014,0x00000005); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000002){;} // Check if 0x1018<bit2:0> is expected value

	// Internal PCLK Setting for Non Preset or REFCLK=26MHz
	i2c1_edp2dsi_write8(0xB005,0x0A); // SET CG_VIDPLL_CTRL1
	i2c1_edp2dsi_write8(0xB006,0x08); // SET CG_VIDPLL_CTRL2
	i2c1_edp2dsi_write8(0xB007,0x2E); // SET CG_VIDPLL_CTRL3
	i2c1_edp2dsi_write8(0xB008,0x00); // SET CG_VIDPLL_CTRL4
	i2c1_edp2dsi_write8(0xB009,0x22); // SET CG_VIDPLL_CTRL5
	i2c1_edp2dsi_write8(0xB00A,0x03); // SET CG_VIDPLL_CTRL6
	// DSI Clock setting for Non Preset or REFCLK=26MHz
	i2c1_edp2dsi_write32(0x41B0,0x00002624); // MIPI_PLL_CNF
	i2c1_edp2dsi_write32(0x41BC,0x00000A01); // MIPI_PLL_PARA
	i2c1_edp2dsi_write32(0x41C0,0x00000030); // MIPI_PLL_FIX
	i2c1_edp2dsi_write32(0x1014,0x00000007); // Boot Ctrl
	DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000007){;} // Check if 0x1018<bit2:0> is expected value
	// Video Size Related Setting for Non Preset
	i2c1_edp2dsi_write32(0x0110,0x000002D0); // HTIM2_LEFT
	i2c1_edp2dsi_write32(0x0124,0x000002D0); // HTIM2_RIGHT
	i2c1_edp2dsi_write32(0x0148,0x000005A0); // DPRX_HTIM2
	i2c1_edp2dsi_write32(0x2920,0x3E0B0870); // DSI0_PIC_SYN_PKT_A
	i2c1_edp2dsi_write32(0x3920,0x3E0B0870); // DSI1_PIC_SYN_PKT_A
	// DSI Transition Time Setting for Non Preset
	i2c1_edp2dsi_write32(0x4154,0x00000006); // PPI_DPHY_LPTXTIMECNT
	i2c1_edp2dsi_write32(0x4158,0x00140003); // PPI_DPHY_TCLK_HEADERCNT
	i2c1_edp2dsi_write32(0x415C,0x00060002); // PPI_DPHY_TCLK_TRAILCNT
	i2c1_edp2dsi_write32(0x4160,0x000A0003); // PPI_DPHY_THS_HEADERCNT
	i2c1_edp2dsi_write32(0x4164,0x00002710); // PPI_DPHY_TWAKEUPCNT
	i2c1_edp2dsi_write32(0x4168,0x0000000A); // PPI_DPHY_TCLK_POSTCNT
	i2c1_edp2dsi_write32(0x416C,0x00070005); // PPI_DPHY_THSTRAILCNT
	i2c1_edp2dsi_write32(0x4178,0x00060006); // PPI_DSI_BTA_COUNT
	// DSI Start
	i2c1_edp2dsi_write32(0x407C,0x00000081); // DSI_DSITX_MODE
	i2c1_edp2dsi_write32(0x4050,0x00000000); // DSI_FUNC_MODE
	i2c1_edp2dsi_write32(0x401C,0x00000001); // DSI_DSITX_START
	DelayMs(100);
	while((rdata32=i2c1_edp2dsi_read32(0x4060, data32))!=0x00000003){;} // Check if 0x2060/4060<bit1:0>=11b.
	// GPIO setting for LCD.  (Depends on LCD specification and System configuration)
	i2c1_edp2dsi_write32(0x0804,0x00000000); // IOB_CTRL2
	i2c1_edp2dsi_write32(0x0080,0x0000000F); // GPIOC
	i2c1_edp2dsi_write32(0x0084,0x0000000F); // GPIOO
	i2c1_edp2dsi_write32(0x0084,0x00000000); // GPIOO
	i2c1_edp2dsi_write32(0x0084,0x0000000F); // GPIOO
	DelayMs(50);
	// DSI Hs Clock Mode
	i2c1_edp2dsi_write32(0x4050,0x00000020); // DSI_FUNC_MODE
	DelayMs(100);
	// Command Transmission Before Video Start. (Depeds on LCD specification)
	// LCD Initialization
	rdata32=i2c1_edp2dsi_read32(0x4050, data32);
	printf (":rdatal=%x\n",rdata32);
	LCD_init_LPMode();
	i2c1_edp2dsi_write32(0x2A10,0x80040010); // DSI0_CQMODE
	i2c1_edp2dsi_write32(0x3A10,0x80040010); // DSI1_CQMODE
	i2c1_edp2dsi_write32(0x2A04,0x00000001); // DSI0_VideoSTART
	i2c1_edp2dsi_write32(0x3A04,0x00000001); // DSI1_VideoSTART
	// Color Bar Setting
	i2c1_edp2dsi_write32(0x0300,0x00080008); // CBR00_HTIM1
	i2c1_edp2dsi_write32(0x0304,0x016805A0); // CBR00_HTIM2
	i2c1_edp2dsi_write32(0x0308,0x00080002); // CBR00_VTIM1
	i2c1_edp2dsi_write32(0x030C,0x000A0500); // CBR00_VTIM2
	i2c1_edp2dsi_write32(0x0310,0x00000001); // CBR00_MODE
	i2c1_edp2dsi_write32(0x0314,0x00FF0000); // CBR00_COLOR
	i2c1_edp2dsi_write32(0x0318,0x00000001); // CBR00_ENABLE
	i2c1_edp2dsi_write32(0x031C,0x00000001); // CBR00_START
	// Command Transmission After Video Start. (Depends on LCD specification)
	LCD_BL_On();
#endif
    return 1;
}

#endif

