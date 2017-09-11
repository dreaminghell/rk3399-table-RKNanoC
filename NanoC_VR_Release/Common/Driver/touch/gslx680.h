/*************************************************************
 *Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved.
 *
 *File: gslx680.h
 *Desc:

 *Author: lsh
 *Date: 16-10-08
 *Notes:
 *$Log: $
 *************************************************************/
#include "DriverInclude.h"
#ifndef _GSLX680_H_

#define GSL_DATA_REG		0x80
#define GSL_STATUS_REG		0xe0
#define GSL_PAGE_REG		0xf0
#define DMA_TRANS_LEN		0x20
#define MAX_FINGERS 		10
#define MAX_CONTACTS 		10

/*touch keys **/
#define KEY_UP_MASK_BIT		1
#define KEY_DOWN_MASK_BIT	2
#define KEY_LEFT_MASK_BIT	4
#define KEY_RIGHT_MASK_BIT	8
#define KEY_ENTER_MASK_BIT	16

#define KEY_UP			103
#define KEY_PAGEUP		104
#define KEY_LEFT		105
#define KEY_RIGHT		106
#define KEY_END			107
#define KEY_DOWN		108
#define KEY_ENTER		28
#define RK_GEAR_TOUCH

#define _DIRECT_IIC_
#define _ELEMENT_SIZE_  	(DMA_TRANS_LEN + 1)

void gslx680_init_chip();
void check_mem_data();

struct gsl_ts_data {
	uint8 x_index;
	uint8 y_index;
	uint8 z_index;
	uint8 id_index;
	uint8 touch_index;
	uint8 data_reg;
	uint8 status_reg;
	uint8 data_size;
	uint8 touch_bytes;
	uint8 update_data;
	uint8 touch_meta_data;
	uint8 finger_size;
	/*
	---------------------------------------
	| 4 bytes |   4 bytes   |   4 bytes   |
	| touches | 1'st touche | 1'st touche |
	---------------------------------------
	**/
	uint8 touch_data[ 4 + MAX_FINGERS*4 ]; 
};

#define	GSL_NOID_VERSION

#ifndef GSL_NOID_VERSION
#pragma pack(push)
#pragma pack(1)
struct gsl_touch_info_raw
{
	uint8 touches;
	uint8 reserved[3];
	int y[10];
	int id[10];
	int finger_num;	
};
#pragma pack(pop)

#else

struct gsl_touch_info
{
	int x[10];
	int y[10];
	int id[10];
	int finger_num;
};

/*
   unsigned int gsl_config_versions[]=
   {
   0xa55a0003,//版本
   0x168045,//芯片型号
   0,//空
   0,0,0,0,//方案公司0-3
   0,0,0,0,//项目名称4-7
   0,0,0,0,//平台8-11
   0,0,0,0,//TP厂12-15
   0,//TP尺寸16

   0,0,0,0,//版本，姓名1，姓名2，日期17-20
   0,0,0,0,
   0,0,0,0,
   0,0,0,0,
   0,0,0,0,

   0,0,0,0,
   0,0,0,0,
   0,0,0,0,
   0,0,0,0,
   0,0,0,0,
   }
   */
#endif

struct fw_data
{
    uint8 offset;
    uint32 val;
};

extern unsigned int gsl_mask_tiaoping(void);
extern unsigned int gsl_version_id(void);
extern void gsl_alg_id_main(struct gsl_touch_info *cinfo);
extern void gsl_DataInit(int *ret);

extern void GSLX680_Read(void);
#endif

