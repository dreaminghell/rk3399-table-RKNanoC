#include "RkvrInterface.h"

/**
*   rk3399 will use dpcd@0x8030~0x8034 to check dp sink device is nanoc or not.
*/
void tc358860xgb_sync_init()
{
	uint8 data8 = 0;

	printf("tc358860xgb_sync_init...\n");
	data8 = 'n';
	i2c1_edp2dsi_write8(0x8030,data8);
	data8 = 'a';
	i2c1_edp2dsi_write8(0x8031,data8);
	data8 = 'n';
	i2c1_edp2dsi_write8(0x8032,data8);
	data8 = 'o';
	i2c1_edp2dsi_write8(0x8033,data8);
	data8 = 'c';
	i2c1_edp2dsi_write8(0x8034,data8);
}

/**
*	sync with rk3399, we use dpcd@0x8035 to sync with rk3399, if dpcd@0x8035 == aa ,means sync operation is done
*/
void tc358860xgb_sync(void)
{
	uint8 data8 = 0;
	uint8 rdata8 = 0;

	printf("tc358860xgb_sync...\n");
	rdata8=i2c1_edp2dsi_read8(0x8035, &data8);
	while ((rdata8=i2c1_edp2dsi_read8(0x8035, &data8))!=0xaa)
	{
	    DelayMs(50);
	}
}

/**
*	fill sensor data to union
*/
void sensor_data_fill(struct rkvr_sensor_data *sensor_data, const int16 *accel_data, const int16 *gyro_data, uint8 *temperature)
{
	uint16 accel_tmp[3] ={0};
	uint16 gyro_tmp[3]  ={0};

	accel_tmp[0] = (uint16)(accel_data[X_ORI] * X_SIGN);
    accel_tmp[1] = (uint16)(accel_data[Y_ORI] * Y_SIGN);
    accel_tmp[2] = (uint16)(accel_data[Z_ORI] * Z_SIGN);

    gyro_tmp[0] = (uint16)(gyro_data[X_ORI] * X_SIGN);
    gyro_tmp[1] = (uint16)(gyro_data[Y_ORI] * Y_SIGN);
    gyro_tmp[2] = (uint16)(gyro_data[Z_ORI] * Z_SIGN);

    //Message_Sensors
    sensor_data->buf_head[0] = 0x01; //No.ReportID is Sensor
    //SampleCount
    sensor_data->buf_head[1] = 0x02;
    //Timestamp
    sensor_data->buf_head[2] = 0x00; //Time Low bit
    sensor_data->buf_head[3] = 0x00; //Time High bit

    //Temperature
    sensor_data->buf_sensortemperature[0] = temperature[0];//:Temperature µÍ×Ö½Ú
    sensor_data->buf_sensortemperature[1] = temperature[1];//:Temperature ¸ß×Ö½Ú

    sensor_data->buf_sensor[0]  = (accel_tmp[0]&0x00ff); //accel_x_l
    sensor_data->buf_sensor[1]  = (accel_tmp[0]>>8);     //accel_x_h
    sensor_data->buf_sensor[2] = (accel_tmp[1]&0x00ff); //accel_y_l
    sensor_data->buf_sensor[3] = (accel_tmp[1]>>8);     //accel_y_h
    sensor_data->buf_sensor[4] = (accel_tmp[2]&0x00ff); //accel_z_l
    sensor_data->buf_sensor[5] = (accel_tmp[2]>>8);     //accel_z_h

    sensor_data->buf_sensor[6] = (gyro_tmp[0]&0x00ff);  //gyro_x_l
    sensor_data->buf_sensor[7] = (gyro_tmp[0]>>8);      //gyro_x_h
    sensor_data->buf_sensor[8] = (gyro_tmp[1]&0x00ff);  //gyro_y_l
    sensor_data->buf_sensor[9] = (gyro_tmp[1]>>8);      //gyro_y_h
    sensor_data->buf_sensor[10] = (gyro_tmp[2]&0x00ff);  //gyro_z_l
    sensor_data->buf_sensor[11] = (gyro_tmp[2]>>8);      //gyro_z_
}

