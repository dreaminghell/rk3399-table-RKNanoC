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
#ifdef _AUO_3P81_75FPS_LCD_

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

/*0*/   //dcs_write_1P_mode(0x11, 200); // dcs_write_1P_mode(cmd, delayms)

/*1*/   dcs_write_2P_mode(0xfe, 0x07, 0);

/*2*/   dcs_write_2P_mode(0x00, 0xec, 0);

/*3*/   dcs_write_2P_mode(0x0b, 0xec, 0);

/*4*/   dcs_write_2P_mode(0x16, 0xec, 0);

/*5*/   dcs_write_2P_mode(0x21, 0xec, 0);


/*6*/   dcs_write_2P_mode(0x2d, 0xec, 0);
/*7*/   dcs_write_2P_mode(0xa9, 0xba, 0);
        //i2c1_uh2d_read16(0x0270, test_data1);
/*8*/   dcs_write_2P_mode(0xab, 0x06, 0);

/*9*/   dcs_write_2P_mode(0xbb, 0x84, 0);


/*10*/  dcs_write_2P_mode(0xbc, 0x1c, 0);

/*11*/  dcs_write_2P_mode(0xfe, 0x08, 0);

/*12*/  dcs_write_2P_mode(0x07, 0x1A, 0);


/*13*/  dcs_write_2P_mode(0xfe, 0x0a, 0);

        dcs_write_2P_mode(0x2a, 0x1b, 0);     //14


        dcs_write_2P_mode( 0xfe, 0x0d, 0);    //15
        //i2c1_uh2d_read16(0x0270, test_data1);

        dcs_write_2P_mode( 0x02, 0x65, 0);    //16



        dcs_write_2P_mode( 0x4d, 0x41, 0);   //17


        dcs_write_2P_mode( 0x4b, 0x0f, 0);    //18


        dcs_write_2P_mode( 0x53, 0xfe, 0);    //19


        dcs_write_2P_mode( 0xfe, 0x00, 0);   //20


        dcs_write_2P_mode( 0xc2, 0x03, 0);    //21


        dcs_write_2P_mode( 0x51, 0xff, 0);   //22

#endif
        dcs_write_1P_mode(0x11, 0);
        dcs_write_1P_mode(0x29, 10);

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
    if ((rdata_0x01==0xFF)&&(rdata_0x50==0xA4)
		&&(rdata_0xA0==0x0C)&&(rdata_0xFF==0xBA))
    {
        printf ("EDID already been written \n");
		return;
    }
    else
    {
        printf ("\n Write EDID\n");
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
	i2c1_edidrom_write8(0x0A,0x32); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x0B,0x05); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x0C,0x03); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x0D,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x0E,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x0F,0x00); // EDID_Data
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
	i2c1_edidrom_write8(0x36,0x7C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x37,0x60); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x38,0x70); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x39,0x28); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x3A,0x82); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x3B,0xB0); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x3C,0x0C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x3D,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x3E,0xA4); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x3F,0x20); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x40,0x63); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x41,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x42,0xD8); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x43,0x78); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x44,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x45,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x46,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x47,0x18); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x48,0x7C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x49,0x60); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x4A,0x70); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x4B,0x28); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x4C,0x82); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x4D,0xB0); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x4E,0x0C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x4F,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x50,0xA4); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x51,0x20); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x52,0x63); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x53,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x54,0xD8); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x55,0x78); // EDID_Data
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
	i2c1_edidrom_write8(0x7F,0x2B); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x80,0x02); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x81,0x03); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x82,0x1A); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x83,0x07); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x84,0x47); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x85,0x84); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x86,0x93); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x87,0x83); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x88,0x82); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x89,0x87); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x8A,0x86); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x8B,0x81); // EDID_Data
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
	i2c1_edidrom_write8(0x9A,0x7C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x9B,0x60); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x9C,0x70); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x9D,0x28); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x9E,0x82); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0x9F,0xB0); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA0,0x0C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA1,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA2,0xA4); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA3,0x20); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA4,0x63); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA5,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA6,0xD8); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA7,0x78); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA8,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xA9,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xAA,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xAB,0x18); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xAC,0x7C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xAD,0x60); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xAE,0x70); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xAF,0x28); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB0,0x82); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB1,0xB0); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB2,0x0C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB3,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB4,0xA4); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB5,0x20); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB6,0x63); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB7,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB8,0xD8); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xB9,0x78); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xBA,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xBB,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xBC,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xBD,0x18); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xBE,0x7C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xBF,0x60); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC0,0x70); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC1,0x28); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC2,0x82); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC3,0xB0); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC4,0x0C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC5,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC6,0xA4); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC7,0x20); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC8,0x63); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xC9,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xCA,0xD8); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xCB,0x78); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xCC,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xCD,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xCE,0x00); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xCF,0x18); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD0,0x7C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD1,0x60); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD2,0x70); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD3,0x28); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD4,0x82); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD5,0xB0); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD6,0x0C); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD7,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD8,0xA4); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xD9,0x20); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xDA,0x63); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xDB,0x40); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xDC,0xD8); // EDID_Data
	DelayMs(10);
	i2c1_edidrom_write8(0xDD,0x78); // EDID_Data
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
	i2c1_edidrom_write8(0xFF,0xBA); // EDID_Data
	DelayMs(10);    

}

const uint8 edid_data[] = 
{
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0x52,0x62,0x32,0x05,0x03,0x00,0x00,0x00,
	0x01,0x19,0x01,0x03,0x80,0x00,0x00,0x78,
	0x0A,0x0D,0xC9,0xA0,0x57,0x47,0x98,0x27,
	0x12,0x48,0x4C,0x00,0x00,0x00,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x7C,0x60,
	0x70,0x28,0x82,0xB0,0x0C,0x40,0xA4,0x20,
	0x63,0x40,0xD8,0x78,0x00,0x00,0x00,0x18,
	0x7C,0x60,0x70,0x28,0x82,0xB0,0x0C,0x40,
	0xA4,0x20,0x63,0x40,0xD8,0x78,0x00,0x00,
	0x00,0x18,0x00,0x00,0x00,0xFC,0x00,0x54,
	0x38,0x36,0x30,0x2D,0x57,0x51,0x51,0x48,
	0x44,0x34,0x6B,0x0A,0x00,0x00,0x00,0xFD,
	0x00,0x14,0x64,0x05,0xFF,0x35,0x00,0x0A,
	0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x2B,

	0x02,0x03,0x1A,0x07,0x47,0x84,0x93,0x83,
	0x82,0x87,0x86,0x81,0x23,0x00,0x00,0x00,
	0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,
	0x10,0x00,0x7C,0x60,0x70,0x28,0x82,0xB0,
	0x0C,0x40,0xA4,0x20,0x63,0x40,0xD8,0x78,
	0x00,0x00,0x00,0x18,0x7C,0x60,0x70,0x28,
	0x82,0xB0,0x0C,0x40,0xA4,0x20,0x63,0x40,
	0xD8,0x78,0x00,0x00,0x00,0x18,0x7C,0x60,
	0x70,0x28,0x82,0xB0,0x0C,0x40,0xA4,0x20,
	0x63,0x40,0xD8,0x78,0x00,0x00,0x00,0x18,
	0x7C,0x60,0x70,0x28,0x82,0xB0,0x0C,0x40,
	0xA4,0x20,0x63,0x40,0xD8,0x78,0x00,0x00,
	0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xBA
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
    //À­µÍINT ÅäÖÃSlaveÎª0xD1
    GpioMuxSet(GPIOPortC_Pin5,IOMUX_GPIOC5_IO);
    Gpio_SetPinDirection(GPIOPortC_Pin5,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin5, GPIO_LOW);
    DelayMs(10);
    //À­µÍResetN
    GpioMuxSet(GPIOPortC_Pin6,IOMUX_GPIOC6_IO);
    Gpio_SetPinDirection(GPIOPortC_Pin6,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortC_Pin6, GPIO_LOW);
    DelayMs(100);
    Gpio_SetPinLevel(GPIOPortC_Pin6, GPIO_HIGH);
#endif

#ifdef _EEPROM_EDID_
	Edidrom_write();
#endif

#ifdef _DP_MIPI_2LANE_ //DP->MIPI 2lane
    // IO Voltahge Setting							
	i2c1_edp2dsi_write32(0x0800,0x00000001); // IOB_CTRL1							
	// Boot Settings							
	i2c1_edp2dsi_write32(0x1000,0x00006978); // BootWaitCount							
	i2c1_edp2dsi_write32(0x1004,0x00040905); // Boot Set0							
	i2c1_edp2dsi_write32(0x1008,0x054C0006); // Boot Set1							
	i2c1_edp2dsi_write32(0x100C,0x03000523); // Boot Set2							
	i2c1_edp2dsi_write32(0x1010,0x01900020); // Boot Set3							
	i2c1_edp2dsi_write32(0x1014,0x00000001); // Boot Ctrl					
    DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000002){;} // Check if 0x1018<bit2:0> is expected value 						
	// Internal PCLK Setting for Non Preset or REFCLK=26MHz							
	i2c1_edp2dsi_write8(0xB005,0x06); // SET CG_VIDPLL_CTRL1							
	i2c1_edp2dsi_write8(0xB006,0x04); // SET CG_VIDPLL_CTRL2							
	i2c1_edp2dsi_write8(0xB007,0x3A); // SET CG_VIDPLL_CTRL3							
	i2c1_edp2dsi_write8(0xB008,0x00); // SET CG_VIDPLL_CTRL4							
	i2c1_edp2dsi_write8(0xB009,0x21); // SET CG_VIDPLL_CTRL5							
	i2c1_edp2dsi_write8(0xB00A,0x08); // SET CG_VIDPLL_CTRL6							
	// DSI Clock setting for Non Preset or REFCLK=26MHz							
	i2c1_edp2dsi_write32(0x41B0,0x000422AF); // MIPI_PLL_CNF							
	i2c1_edp2dsi_write32(0x41BC,0x00000D01); // MIPI_PLL_PARA							
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
	i2c1_edp2dsi_write32(0x0110,0x00000438); // HTIM2_LEFT							
	i2c1_edp2dsi_write32(0x0124,0x00000438); // HTIM2_RIGHT							
	i2c1_edp2dsi_write32(0x0148,0x00000870); // DPRX_HTIM2							
	i2c1_edp2dsi_write32(0x2920,0x3E0B0CA8); // DSI0_PIC_SYN_PKT_A							
	i2c1_edp2dsi_write32(0x3920,0x3E0B0CA8); // DSI1_PIC_SYN_PKT_A							
	// eDP Settings for Link Training														
	while((rdata8=i2c1_edp2dsi_read8(0xB631, &data8))!=0x01){;} // Check if 0xB631<bit1:0>=01b. 						
	i2c1_edp2dsi_write8(0x8001,0x14); // Max Link Rate							
	i2c1_edp2dsi_write8(0x8002,0x02); // Max Lane Count							
	i2c1_edp2dsi_write8(0xB608,0x0B); // Set AUXTXHSEN							
	i2c1_edp2dsi_write8(0xB800,0x1E); // Set CDR_PHASE_LP_EN							
	i2c1_edp2dsi_write8(0x8700,0x00); // DPCD 0700h							
	// For DP EDID Read							
	i2c1_edp2dsi_write8(0xB201,0x30); // 							
	i2c1_edp2dsi_write16(0xB202,0x0000); // 							
#ifdef _GPIO_EDID_
	i2c1_edp2dsi_write32(0xB204,0x00000022); // 	
#else
	i2c1_edp2dsi_write32(0xB204,0x00000004); // 
#endif					
	// VSYNC monitor output setting for DP connection							
	i2c1_edp2dsi_write32(0x5014,0x016A0000); // Monitor Signal Selection							
	i2c1_edp2dsi_write32(0x008C,0x00004000); // GPIOOUTMODE							
	i2c1_edp2dsi_write32(0x0080,0x0000000C); // GPIOC							
	i2c1_edp2dsi_write32(0x0084,0x00000004); // GPIOC							
	// I2C direction control for DP connection							
	i2c1_edp2dsi_write32(0x00B0,0x00000001); // I2CMD_SL													

	//EDID Write--Save the screen information
#ifdef _GPIO_EDID_
	printf ("AUO3.81 75fps -- start read edid!\n");
	edid_send_init();
	DelayMs(1200);
	edid_send_deinit();
#endif
#ifdef _EEPROM_EDID_
	edid_eeprom_send_init();
	DelayMs(1000);
	edid_send_deinit();
#endif

#if !defined _GPIO_EDID_ && !defined _EEPROM_EDID_
	DelayMs(600);
#endif

    printf ("AUO3.81 75fps -- 2LANE start traning!\n");

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
	i2c1_edp2dsi_write32(0x4154,0x0000000B); // PPI_DPHY_LPTXTIMECNT							
	i2c1_edp2dsi_write32(0x4158,0x00260005); // PPI_DPHY_TCLK_HEADERCNT							
	i2c1_edp2dsi_write32(0x415C,0x000D0006); // PPI_DPHY_TCLK_TRAILCNT							
	i2c1_edp2dsi_write32(0x4160,0x000F0007); // PPI_DPHY_THS_HEADERCNT							
	i2c1_edp2dsi_write32(0x4164,0x0000283C); // PPI_DPHY_TWAKEUPCNT							
	i2c1_edp2dsi_write32(0x4168,0x0000000D); // PPI_DPHY_TCLK_POSTCNT							
	i2c1_edp2dsi_write32(0x416C,0x000E0009); // PPI_DPHY_THSTRAILCNT							
	i2c1_edp2dsi_write32(0x4178,0x000B000C); // PPI_DSI_BTA_COUNT							
	// DSI Start							
	i2c1_edp2dsi_write32(0x407C,0x00000081); // DSI_DSITX_MODE							
	i2c1_edp2dsi_write32(0x4050,0x00000000); // DSI_FUNC_MODE							
	i2c1_edp2dsi_write32(0x401C,0x00000001); // DSI_DSITX_START

    DelayUs(100);
	while((rdata32=i2c1_edp2dsi_read32(0x4060, data32))!=0x00000003){;} // Check if 0x2060/4060<bit1:0>=11b. 						
	// GPIO setting for LCD control.  (Depends on LCD specification and System configuration)							
	// DSI Hs Clock Mode							
	i2c1_edp2dsi_write32(0x4050,0x00000020); // DSI_FUNC_MODE							
	DelayMs(100);							
	// Command Transmission Before Video Start. (Depeds on LCD specification)							
	i2c1_edp2dsi_write32(0x42FC,0x8107FE15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC0015); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC0B15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC1615); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC2115); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC2D15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81BAA915); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8106AB15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8184BB15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x811CBC15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8108FE15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x811A0715); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x810AFE15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x811B2A15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x810DFE15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81650215); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81414D15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x810F4B15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81FE5315); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8100FE15); // DSIG_CQ_HEADER							
	DelayMs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8103C215); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81FF5115); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81001105); // DSIG_CQ_HEADER							
	DelayMs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81002905); // DSIG_CQ_HEADER							
	DelayMs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1

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
    //LCD_BL_On();
#endif

#ifdef _DP_MIPI_COLORBAR_ //DP Color bar
	// IO Voltahge Setting							
	i2c1_edp2dsi_write32(0x0800,0x00000001); // IOB_CTRL1							
	// Boot Settings							
	i2c1_edp2dsi_write32(0x1000,0x00006978); // BootWaitCount							
	i2c1_edp2dsi_write32(0x1004,0x00040905); // Boot Set0							
	i2c1_edp2dsi_write32(0x1008,0x054C0006); // Boot Set1							
	i2c1_edp2dsi_write32(0x100C,0x0300043D); // Boot Set2							
	i2c1_edp2dsi_write32(0x1010,0x01900020); // Boot Set3							
	i2c1_edp2dsi_write32(0x1014,0x00000005); // Boot Ctrl					    
    DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000002){;} // Check if 0x1018<bit2:0> is expected value							
	// Internal PCLK Setting for Non Preset or REFCLK=26MHz							
	i2c1_edp2dsi_write8(0xB005,0x06); // SET CG_VIDPLL_CTRL1							
	i2c1_edp2dsi_write8(0xB006,0x04); // SET CG_VIDPLL_CTRL2							
	i2c1_edp2dsi_write8(0xB007,0x3A); // SET CG_VIDPLL_CTRL3							
	i2c1_edp2dsi_write8(0xB008,0x00); // SET CG_VIDPLL_CTRL4							
	i2c1_edp2dsi_write8(0xB009,0x21); // SET CG_VIDPLL_CTRL5							
	i2c1_edp2dsi_write8(0xB00A,0x08); // SET CG_VIDPLL_CTRL6							
	// DSI Clock setting for Non Preset or REFCLK=26MHz							
	i2c1_edp2dsi_write32(0x41B0,0x000422AF); // MIPI_PLL_CNF							
	i2c1_edp2dsi_write32(0x41BC,0x00000D01); // MIPI_PLL_PARA							
	i2c1_edp2dsi_write32(0x41C0,0x00000030); // MIPI_PLL_FIX							
	i2c1_edp2dsi_write32(0x1014,0x00000007); // Boot Ctrl						    
    DelayMs(1);
	while((rdata32=i2c1_edp2dsi_read32(0x1018, data32))!=0x00000007){;} // Check if 0x1018<bit2:0> is expected value							
	// Video Size Related Setting for Non Preset							
	i2c1_edp2dsi_write32(0x0110,0x00000438); // HTIM2_LEFT							
	i2c1_edp2dsi_write32(0x0124,0x00000438); // HTIM2_RIGHT							
	i2c1_edp2dsi_write32(0x0148,0x00000870); // DPRX_HTIM2							
	i2c1_edp2dsi_write32(0x2920,0x3E0B0CA8); // DSI0_PIC_SYN_PKT_A							
	i2c1_edp2dsi_write32(0x3920,0x3E0B0CA8); // DSI1_PIC_SYN_PKT_A							
	// DSI Transition Time Setting for Non Preset							
	i2c1_edp2dsi_write32(0x4154,0x0000000B); // PPI_DPHY_LPTXTIMECNT							
	i2c1_edp2dsi_write32(0x4158,0x00260005); // PPI_DPHY_TCLK_HEADERCNT							
	i2c1_edp2dsi_write32(0x415C,0x000D0006); // PPI_DPHY_TCLK_TRAILCNT							
	i2c1_edp2dsi_write32(0x4160,0x000F0007); // PPI_DPHY_THS_HEADERCNT							
	i2c1_edp2dsi_write32(0x4164,0x0000283C); // PPI_DPHY_TWAKEUPCNT							
	i2c1_edp2dsi_write32(0x4168,0x0000000D); // PPI_DPHY_TCLK_POSTCNT							
	i2c1_edp2dsi_write32(0x416C,0x000E0009); // PPI_DPHY_THSTRAILCNT							
	i2c1_edp2dsi_write32(0x4178,0x000B000C); // PPI_DSI_BTA_COUNT							
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
	i2c1_edp2dsi_write32(0x42FC,0x8107FE15); // DSIG_CQ_HEADER	
	DelayUs(200);
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC0015); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC0B15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC1615); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC2115); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81EC2D15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81BAA915); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8106AB15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8184BB15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x811CBC15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8108FE15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x811A0715); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x810AFE15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x811B2A15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x810DFE15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81650215); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81414D15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x810F4B15); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81FE5315); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8100FE15); // DSIG_CQ_HEADER							
	DelayMs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x8103C215); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81FF5115); // DSIG_CQ_HEADER							
	DelayUs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81001105); // DSIG_CQ_HEADER							
	DelayMs(200);							
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	i2c1_edp2dsi_write32(0x42FC,0x81002905); // DSIG_CQ_HEADER							
	DelayMs(200);	
	
    // LCD Initialization
	while((rdata32=i2c1_edp2dsi_read32(0x4200, data32))!=0x00000001){;} // Check if <bit0>=1							
	LCD_init_LPMode();

	i2c1_edp2dsi_write32(0x2A10,0x80040010); // DSI0_CQMODE							
	i2c1_edp2dsi_write32(0x3A10,0x80040010); // DSI1_CQMODE							
	i2c1_edp2dsi_write32(0x2A04,0x00000001); // DSI0_VideoSTART							
	i2c1_edp2dsi_write32(0x3A04,0x00000001); // DSI1_VideoSTART							
	// Color Bar Setting							
	i2c1_edp2dsi_write32(0x0300,0x00640020); // CBR00_HTIM1							
	i2c1_edp2dsi_write32(0x0304,0x01A40870); // CBR00_HTIM2							
	i2c1_edp2dsi_write32(0x0308,0x00030003); // CBR00_VTIM1							
	i2c1_edp2dsi_write32(0x030C,0x000604B0); // CBR00_VTIM2							
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

