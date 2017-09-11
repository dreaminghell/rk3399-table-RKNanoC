/*************************************************************
 *Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved.
 *
 *File: edid_config.c
 *Desc:

 *Author: wjh
 *Date: 16-11-11
 *Notes:
 *$Log: $
*************************************************************/

#include "DriverInclude.h"
#include "Gpio.h"
#include "OsInclude.h"
#include "edid_send.h"

#ifdef _AUO_3P81_75FPS_LCD_
#include "tc358860xbgAUO3P81-75fps.c"
#endif

#ifdef _AUO_2P1_60FPS_LCD_
#include "tc358860xbgAUO2P1-60fps.c"
#endif

#ifdef _SHARP_2P89_75FPS_LCD_
#include "tc358860xbgSHARP2P89-75fps.c"
#endif

#ifdef _RAYKEN_5P46_60FPS_LCD_
#include "tc358860xbgRAYKEN5P46-60fps.c"
#endif


#define ADDR_LENGHT								8
#define DATA_LENGHT								8
#define EDID_BLOCK_SIZE							128
#define WAIT_SCL_LOW_TIME_OUT					900000
#define WAIT_SCL_HIGH_TIME_OUT					800000

#define GPIO_SCL						GPIOPortC_Pin7
#define GPIO_SDA						GPIOPortD_Pin0

#define GPIO_SCL_LEVEL()				Gpio_GetPinLevel(GPIO_SCL)
#define GPIO_SDA_LEVEL()				Gpio_GetPinLevel(GPIO_SDA)

static uint8 edid_data_start = 0;
static uint64 wait_timer = 0;

#define WAIT_SCL_TO_LOW();				wait_timer = 0;															\
										while(GPIO_SCL_LEVEL()) {												\
											if(++wait_timer >= WAIT_SCL_LOW_TIME_OUT) 							\
											{																	\
												printf("Wait scl to low level time out line %d\n", __LINE__);	\
												edid_data_start = 0;											\
												Gpio_DisableInt(GPIO_SDA);										\
												return;															\
											}																	\
										}																		\

#define WAIT_SCL_TO_HIGH();				wait_timer = 0;															\
										while(!GPIO_SCL_LEVEL()) {												\
											if(++wait_timer >= WAIT_SCL_HIGH_TIME_OUT) 							\
											{																	\
												printf("Wait scl to high level time out line %d\n", __LINE__);	\
												edid_data_start = 0;											\
												Gpio_DisableInt(GPIO_SDA);										\
												return; 														\
											}																	\
										}																		\

void ACK(void)
{
	Gpio_SetPinDirection(GPIO_SDA, GPIO_OUT);
	Gpio_SetPinLevel(GPIO_SDA, GPIO_LOW);
	WAIT_SCL_TO_HIGH();
	WAIT_SCL_TO_LOW();
	Gpio_SetPinDirection(GPIO_SDA, GPIO_IN);
}
 
void NOTACK(void)
{
	Gpio_SetPinDirection(GPIO_SDA, GPIO_OUT);
	Gpio_SetPinLevel(GPIO_SDA, GPIO_HIGH);
	WAIT_SCL_TO_HIGH();
	WAIT_SCL_TO_LOW();
	Gpio_SetPinDirection(GPIO_SDA, GPIO_IN);
}
 
void edid_send_working(void)
{
	uint8 i,j,retry = 100;
	uint8 start_read_flag = 0;
	uint8 device_addr = 0;
	uint8 word_addr = 0;
	uint8 send_ptr;

	WAIT_SCL_TO_LOW();

	//get device addr
	for (i=0; i<ADDR_LENGHT; i++) {
		WAIT_SCL_TO_HIGH();
		device_addr <<= 1;
		if (GPIO_SDA_LEVEL())
			device_addr |= 0x01;
		WAIT_SCL_TO_LOW();
	}
	ACK();

	if ((device_addr&0xfe) == 0xa0) {//is EEPROM slave addr
		if (device_addr&0x01) {//read
			for (i=edid_data_start; i<EDID_BLOCK_SIZE+edid_data_start; i++) {
				Gpio_SetPinDirection(GPIO_SDA, GPIO_OUT);
				send_ptr = edid_data[i];
				for (j=0; j<DATA_LENGHT; j++) {
					if (send_ptr&0x80) {
						Gpio_SetPinLevel(GPIO_SDA, 1);
					} else {
						Gpio_SetPinLevel(GPIO_SDA, 0);
					}
					WAIT_SCL_TO_HIGH();
					WAIT_SCL_TO_LOW();
					send_ptr <<=1;
				}
				Gpio_SetPinDirection(GPIO_SDA, GPIO_IN);
				WAIT_SCL_TO_HIGH();
				WAIT_SCL_TO_LOW();
			}

			//printf("sizeof(edid_data) = %d, i=%d\n", sizeof(edid_data), i);
			if (sizeof(edid_data) == i) {
				edid_data_start = 0;
				DelayUs(100);
				Gpio_EnableInt(GPIO_SDA);
				return;
			} else {
				edid_data_start = EDID_BLOCK_SIZE;
				DelayUs(100);
				Gpio_EnableInt(GPIO_SDA);
			}
		} else {//write
			//get word addr
			for (i=0; i<ADDR_LENGHT; i++) {
				WAIT_SCL_TO_HIGH();
				word_addr <<= 1;
				if (GPIO_SDA_LEVEL()) {
					word_addr |= 0x01;
				}
				WAIT_SCL_TO_LOW();
			}
			ACK();

			DelayUs(100);
			Gpio_EnableInt(GPIO_SDA);
		}
	} else {
		DelayUs(100);
		Gpio_EnableInt(GPIO_SDA);
	}

}
 
void sda_low_interrupt_handler()
{
	Gpio_DisableInt(GPIO_SDA);
	if (GPIO_SCL_LEVEL()) {	 //I2C start
		edid_send_working();
	} else {
		DelayUs(100);
		Gpio_EnableInt(GPIO_SDA);
	}
}
 
void edid_send_init(void)
{
	GpioMuxSet(GPIO_SCL,IOMUX_GPIOC7_IO);
	Gpio_SetPinDirection(GPIO_SCL,GPIO_IN);
	GPIO_SetPinPull(GPIO_SCL,ENABLE);

	GpioMuxSet(GPIO_SDA,IOMUX_GPIOD0_IO);
	Gpio_SetPinDirection(GPIO_SDA,GPIO_IN);
	Gpio_SetIntMode(GPIO_SDA, IntrTypeLowLevel);//IntrTypeLowLevel
	GpioIsrRegister(GPIO_SDA, sda_low_interrupt_handler);
	Gpio_EnableInt(GPIO_SDA);
}

void edid_eeprom_send_init(void)
{
	GpioMuxSet(GPIO_SCL,IOMUX_GPIOC7_IO);
	Gpio_SetPinDirection(GPIO_SCL,GPIO_IN);
	GPIO_SetPinPull(GPIO_SCL,ENABLE);

	GpioMuxSet(GPIO_SDA,IOMUX_GPIOD0_IO);
	Gpio_SetPinDirection(GPIO_SDA,GPIO_IN);
	GPIO_SetPinPull(GPIO_SCL,ENABLE);
}
 
void edid_send_deinit(void)
{
	GpioMuxSet(GPIO_SCL,IOMUX_GPIOC7_IO);
    Gpio_SetPinDirection(GPIO_SCL,GPIO_IN);
    GPIO_SetPinPull(GPIO_SCL,ENABLE);

	Gpio_DisableInt(GPIO_SDA);
    GpioMuxSet(GPIO_SDA,IOMUX_GPIOD0_IO);
    Gpio_SetPinDirection(GPIO_SDA,GPIO_IN);
    GPIO_SetPinPull(GPIO_SDA,ENABLE);

	GpioMuxSet(GPIO_SCL,IOMUX_GPIOC7_I2C_SCL);
    GpioMuxSet(GPIO_SDA,IOMUX_GPIOD0_I2C_SDA);
}
