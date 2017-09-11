#ifndef _RKVR_DATA_H_
#define _RKVR_DATA_H_

#include "RkvrInterface.h"

struct keymap_t{
	__u16 key_menu_up:1;
	__u16 key_menu_down:1;
	__u16 key_home_up:1;
	__u16 key_home_down:1;
	__u16 key_power_up:1;
	__u16 key_power_down:1;
	__u16 key_volup_up:1;
	__u16 key_volup_down:1;
	__u16 key_voldn_up:1;
	__u16 key_voldn_down:1;
	__u16 key_esc_up:1;
	__u16 key_esc_down:1;
	/*for touch screen **/
	__u16 key_up_pressed:1;
	__u16 key_up_released:1;
	__u16 key_down_pressed:1;
	__u16 key_down_released:1;
	__u16 key_left_pressed:1;
	__u16 key_left_released:1;
	__u16 key_right_pressed:1;
	__u16 key_right_released:1;
	__u16 key_enter_pressed:1;
	__u16 key_enter_released:1;
	__u16 key_pressed:1;
	__u16 psensor_on:1;
	__u16 psensor_off:1;
};

typedef union rkvr_data_t{
	struct rkvr_data{
		struct rkvr_sensor_data sensor_data;
		__u8 buf_reserve[10];
		struct keymap_t key_map;
	}rkvr_data;
	__u8 buf[62];
}RKVR_DATA_UN;

#endif
