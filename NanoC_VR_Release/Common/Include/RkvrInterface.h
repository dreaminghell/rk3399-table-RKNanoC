#ifndef _RKVR_INTERFACE_H_
#define _RKVR_INTERFACE_H_

#define __u16 	unsigned short
#define __u8	unsigned char

#include "SysInclude.h"

#define HID_INPUT_REPORT	0
#define HID_OUTPUT_REPORT	1
#define HID_FEATURE_REPORT	2
#define HID_REPORT_TYPES	3

#define HID_REPORT_ID_RKVR	3
#define RKVR_ID_IDLE	1
#define RKVR_ID_SYNC	2
#define RKVR_ID_CAPS	3

#define HID_REPORT_ID_R		4
#define HID_REPORT_ID_W		5
#define HID_REPORT_ID_CRYP	6

#define HID_REGR_REPORT	4
#define HID_REGW_REPORT	5
#define HID_MISC_REPORT	6


#define HID_ENCRYPTW_REPORT	7
#define HID_ENCRYPTR_REPORT	8



/*
   XYZ  010_001_000 Identity Matrix
   XZY  001_010_000
   YXZ  010_000_001
   YZX  000_010_001
   ZXY  001_000_010
   ZYX  000_001_010
 */

//#define ORIENTATION 0B010101000
//#define ORIENTATION 0B010001000
//#define ORIENTATION 0B010100001
//#define ORIENTATION 0B010101000
//#define ORIENTATION 0x1A8

//#define ORIENTATION 0B110101000
//#define ORIENTATION 0x1AC
//sharp 2.89
//#define ORIENTATION 0x1A8
#define ORIENTATION 0x1A5

#define SIGNSET(x) ((x) ? -1 : +1)
#define X_ORI (ORIENTATION&3)
#define Y_ORI ((ORIENTATION>>3)&3)
#define Z_ORI ((ORIENTATION>>6)&3)

#define X_SIGN SIGNSET(ORIENTATION&0x04)
#define Y_SIGN SIGNSET(ORIENTATION&0x020)
#define Z_SIGN SIGNSET(ORIENTATION&0x0100)

struct rkvr_sensor_data{
	__u8 buf_head[6];
	__u8 buf_sensortemperature[2];
	__u8 buf_sensor[40];
};
extern int sync_check(const unsigned char *buf, int count);
extern void tc358860xgb_sync_init();
extern void tc358860xgb_sync(void);
extern void sensor_data_fill(struct rkvr_sensor_data *sensor_data, const int16 *accel_data, const int16 *gyro_data, uint8 *temperature);

#endif
