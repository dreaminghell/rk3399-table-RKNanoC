/*************************************************************
 *Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved.
 *
 *File: MPU6500.c
 *Desc:

 *Author: wrm
 *Date: 16-04-24
 *Notes:
 *$Log: $
 *************************************************************/
#include "MDconfig.h"
#include "MPU6500.h"
#include "inv_mpu_iio.h"
static struct inv_mpu_iio_s mpu_st = {0};
#define mpu_printf printf

int32 MPU6500_Read(uint32 reg_addr, uint8* data, uint32 size);
int32 MPU6500_Write(uint32 reg_addr, uint8 data);
static int inv_hid_read(struct inv_mpu_iio_s *st, u8 reg, int len, u8 *data)
{
	MPU6500_Read(reg | 0x80, data, len);
	return 0;
}

static int inv_hid_single_write(struct inv_mpu_iio_s *st, u8 reg, u8 data)
{
	MPU6500_Write(reg,data);
	return 0;
}

static int inv_hid_secondary_read(struct inv_mpu_iio_s *st, u8 reg, int len, u8 *data)
{
	return 0;
}

static int inv_hid_secondary_write(struct inv_mpu_iio_s *st, u8 reg, u8 data)
{
	return 0;
}

static int mpu_hid_memory_write(struct inv_mpu_iio_s *st, u8 mpu_addr, u16 mem_addr,
			u32 len, u8 const *data)
{
	return 0;
}

static int mpu_hid_memory_read(struct inv_mpu_iio_s *st, u8 mpu_addr, u16 mem_addr,
			u32 len, u8 *data)
{
	return 0;
}


/*
 *  inv_recover_setting() recover the old settings after everything is done
 */
void inv_recover_setting(struct inv_mpu_iio_s *st)
{
	struct inv_reg_map_s *reg;
	int data;

	reg = &st->reg;
	inv_plat_single_write(st, reg->gyro_config,
			     st->chip_config.fsr << GYRO_CONFIG_FSR_SHIFT);
	inv_plat_single_write(st, reg->lpf, st->chip_config.lpf);
	data = ONE_K_HZ/st->chip_config.new_fifo_rate - 1;
	inv_plat_single_write(st, reg->sample_rate_div, data);
	if (INV_ITG3500 != st->chip_type) {
		inv_plat_single_write(st, reg->accl_config,
				     (st->chip_config.accl_fs <<
				     ACCL_CONFIG_FSR_SHIFT));
	}
	st->switch_gyro_engine(st, false);
	st->switch_accl_engine(st, false);
	st->set_power_state(st, false);
}

/*
 *  inv_do_test() - do the actual test of self testing
 */
#define DEF_ST_STABLE_TIME          200

int inv_do_test(struct inv_mpu_iio_s *st, int self_test_flag,
		int *gyro_result, int *accl_result)
{
	struct inv_reg_map_s *reg;
	int result, i, j, packet_size;
	u8 data[BYTES_PER_SENSOR * 2], d;
	bool has_accl;
	int fifo_count, packet_count, ind, s;

	reg = &st->reg;
	has_accl = (st->chip_type != INV_ITG3500);
	if (has_accl)
		packet_size = BYTES_PER_SENSOR * 2;
	else
		packet_size = BYTES_PER_SENSOR;

	result = inv_plat_single_write(st, reg->int_enable, 0);
	if (result)
		return result;
	/* disable the sensor output to FIFO */
	result = inv_plat_single_write(st, reg->fifo_en, 0);
	if (result)
		return result;
	/* disable fifo reading */
	result = inv_plat_single_write(st, reg->user_ctrl, st->i2c_dis);
	if (result)
		return result;
	/* clear FIFO */
	result = inv_plat_single_write(st, reg->user_ctrl, BIT_FIFO_RST | st->i2c_dis);
	if (result)
		return result;
	/* setup parameters */
	result = inv_plat_single_write(st, reg->lpf, INV_FILTER_98HZ);
	if (result)
		return result;

	if (INV_MPU6500 == st->chip_type) {
		/* config accel LPF register for MPU6500 */
		result = inv_plat_single_write(st, REG_6500_ACCEL_CONFIG2,
						DEF_ST_MPU6500_ACCEL_LPF |
						BIT_FIFO_SIZE_1K);
		if (result)
			return result;
	}

	result = inv_plat_single_write(st, reg->sample_rate_div,
			DEF_SELFTEST_SAMPLE_RATE);
	if (result)
		return result;
	/* wait for the sampling rate change to stabilize */
	DelayMs(INV_MPU_SAMPLE_RATE_CHANGE_STABLE);
	result = inv_plat_single_write(st, reg->gyro_config,
		self_test_flag | DEF_SELFTEST_GYRO_FS);
	if (result)
		return result;
	if (has_accl) {
		if (INV_MPU6500 == st->chip_type)
			d = DEF_SELFTEST_6500_ACCEL_FS;
		else
			d = DEF_SELFTEST_ACCEL_FS;
		d |= self_test_flag;
		result = inv_plat_single_write(st, reg->accl_config, d);
		if (result)
			return result;
	}
	/* wait for the output to get stable */
	if (self_test_flag) {
		if (INV_MPU6500 == st->chip_type)
			DelayMs(DEF_ST_6500_STABLE_TIME);
		else
			DelayMs(DEF_ST_STABLE_TIME);
	}

	/* enable FIFO reading */
	result = inv_plat_single_write(st, reg->user_ctrl, BIT_FIFO_EN | st->i2c_dis);
	if (result)
		return result;
	/* enable sensor output to FIFO */
	if (has_accl)
		d = BITS_GYRO_OUT | BIT_ACCEL_OUT;
	else
		d = BITS_GYRO_OUT;
	for (i = 0; i < THREE_AXIS; i++) {
		gyro_result[i] = 0;
		accl_result[i] = 0;
	}
	s = 0;
	while (s < st->self_test.samples) {
		result = inv_plat_single_write(st, reg->fifo_en, d);
		if (result)
			return result;
		DelayMs(DEF_GYRO_WAIT_TIME);
		result = inv_plat_single_write(st, reg->fifo_en, 0);
		if (result)
			return result;

		result = inv_plat_read(st, reg->fifo_count_h,FIFO_COUNT_BYTE, data);
		if (result)
			return result;

		mpu_printf("self_test fifo_count %02X %02X\n", data[1], data[0]);
		/*fifo_count = be16_to_cpup((__be16 *)(&data[0]));**/
		fifo_count = (short)(((data[0] << 8)&0xff00) | (data[1]&0xff));
		packet_count = fifo_count / packet_size;
		mpu_printf("%s self_test fifo_count %d pack_count %d\n",st->name, fifo_count, packet_count);

		i = 0;
		while ((i < packet_count) && (s < st->self_test.samples)) {
			short vals[3];
			result = inv_plat_read(st, reg->fifo_r_w, packet_size, data);
			if (result)
				return result;
			ind = 0;
			if (has_accl) {
				for (j = 0; j < THREE_AXIS; j++) {
					vals[j] = (short)(((data[ind + 2 * j] << 8) & 0xff00) | (data[ind + 2 * j + 1] & 0xff));
					/* vals[j] = (short)be16_to_cpup((__be16 *)(&data[ind + 2 * j])); **/
					accl_result[j] += vals[j];
				}
				ind += BYTES_PER_SENSOR;
				//mpu_printf( "%s self_test accel data - %d %+d %+d %+d\n", st->name, s, vals[0], vals[1], vals[2]);
			}

			for (j = 0; j < THREE_AXIS; j++) {
				vals[j] = (short)(((data[ind + 2 * j] << 8) & 0xff00) | data[ind + 2 * j + 1] & 0xff);
				/*vals[j] = (short)be16_to_cpup((__be16 *)(&data[ind + 2 * j])); **/
				gyro_result[j] += vals[j];
			}
			//mpu_printf("%s self_test gyro data - %d %+d %+d %+d\n",st->name, s, vals[0], vals[1], vals[2]);
			s++;
			i++;
		}
	}

	if (has_accl) {
		for (j = 0; j < THREE_AXIS; j++) {
			accl_result[j] = accl_result[j] / s;
			accl_result[j] *= DEF_ST_PRECISION;
		}
	}
	for (j = 0; j < THREE_AXIS; j++) {
		gyro_result[j] = gyro_result[j] / s;
		gyro_result[j] *= DEF_ST_PRECISION;
	}

	return 0;
}

static int set_power_itg(struct inv_mpu_iio_s *st, bool power_on)
{
	struct inv_reg_map_s *reg;
	u8 data;
	int result;

	if ((!power_on) == st->chip_config.is_asleep)
		return 0;
	reg = &st->reg;
	if (power_on)
		data = 0;
	else
		data = BIT_SLEEP;
	result = inv_plat_single_write(st, reg->pwr_mgmt_1, data);
	if (result)
		return result;

	if (power_on) {
		if (INV_MPU6500 == st->chip_type)
			DelayMs(POWER_UP_TIME);
		else
			DelayMs(REG_UP_TIME);
	}

	st->chip_config.is_asleep = !power_on;

	return 0;
}
/**
 *  inv_init_config() - Initialize hardware, disable FIFO.
 *  @indio_dev:	Device driver instance.
 *  Initial configuration:
 *  FSR: +/- 2000DPS
 *  DLPF: 42Hz
 *  FIFO rate: 50Hz
 */
static int inv_init_config(struct inv_mpu_iio_s *st)
{
	struct inv_reg_map_s *reg;
	int result;

	reg = &st->reg;
#if 0
	/* set int latch en */
	result = inv_plat_single_write(st, REG_INT_PIN_CFG, 0x20);
	if (result)
		return result;
#endif
	result = inv_plat_single_write(st, reg->gyro_config,
		INV_FSR_2000DPS << GYRO_CONFIG_FSR_SHIFT);
	if (result)
		return result;

	st->chip_config.fsr = INV_FSR_2000DPS;

	/*reg->lpf = REG_CONFIG**/
	result = inv_plat_single_write(st, reg->lpf, INV_FILTER_42HZ); 
	if (result)
		return result;
	st->chip_config.lpf = INV_FILTER_42HZ;

	result = inv_plat_single_write(st, reg->sample_rate_div,
					ONE_K_HZ / INIT_FIFO_RATE - 1);
	if (result)
		return result;
	st->chip_config.fifo_rate = INIT_FIFO_RATE;
	st->chip_config.new_fifo_rate = INIT_FIFO_RATE;
#if 0
	st->irq_dur_ns            = INIT_DUR_TIME;
	st->chip_config.prog_start_addr = DMP_START_ADDR;
	st->chip_config.dmp_output_rate = INIT_DMP_OUTPUT_RATE;
#endif
	st->self_test.samples = INIT_ST_SAMPLES;
	st->self_test.threshold = INIT_ST_THRESHOLD;
	if (INV_ITG3500 != st->chip_type) {
		st->chip_config.accl_fs = INV_FS_02G;
		result = inv_plat_single_write(st, reg->accl_config,
			(INV_FS_02G << ACCL_CONFIG_FSR_SHIFT));
		if (result)
			return result;
#if 0
		st->tap.time = INIT_TAP_TIME;
		st->tap.thresh = INIT_TAP_THRESHOLD;
		st->tap.min_count = INIT_TAP_MIN_COUNT;
		st->smd.threshold = MPU_INIT_SMD_THLD;
		st->smd.delay     = MPU_INIT_SMD_DELAY_THLD;
		st->smd.delay2    = MPU_INIT_SMD_DELAY2_THLD;

		result = inv_plat_single_write(st, REG_ACCEL_MOT_DUR,
						INIT_MOT_DUR);
		if (result)
			return result;
		st->mot_int.mot_dur = INIT_MOT_DUR;

		result = inv_plat_single_write(st, REG_ACCEL_MOT_THR,
						INIT_MOT_THR);
		if (result)
			return result;
		st->mot_int.mot_thr = INIT_MOT_THR;
#endif
	}
	result = inv_plat_single_write(st, reg->int_enable, 0);
	if (result)
		return result;

	return 0;
}

static void inv_setup_reg(struct inv_reg_map_s *reg)
{
	reg->sample_rate_div	= REG_SAMPLE_RATE_DIV;
	reg->lpf		= REG_CONFIG;
	reg->bank_sel		= REG_BANK_SEL;
	reg->user_ctrl		= REG_USER_CTRL;
	reg->fifo_en		= REG_FIFO_EN;
	reg->gyro_config	= REG_GYRO_CONFIG;
	reg->accl_config	= REG_ACCEL_CONFIG;
	reg->fifo_count_h	= REG_FIFO_COUNT_H;
	reg->fifo_r_w		= REG_FIFO_R_W;
	reg->raw_gyro		= REG_RAW_GYRO;
	reg->raw_accl		= REG_RAW_ACCEL;
	reg->temperature	= REG_TEMPERATURE;
	reg->int_enable		= REG_INT_ENABLE;
	reg->int_status		= REG_INT_STATUS;
	reg->pwr_mgmt_1		= REG_PWR_MGMT_1;
	reg->pwr_mgmt_2		= REG_PWR_MGMT_2;
	reg->mem_start_addr	= REG_MEM_START_ADDR;
	reg->mem_r_w		= REG_MEM_RW;
	reg->prgm_strt_addrh	= REG_PRGM_STRT_ADDRH;
};

static int inv_switch_engine(struct inv_mpu_iio_s *st, bool en, u32 mask)
{
	struct inv_reg_map_s *reg;
	u8 data, mgmt_1;
	int result;
	reg = &st->reg;
	/* switch clock needs to be careful. Only when gyro is on, can
	   clock source be switched to gyro. Otherwise, it must be set to
	   internal clock */
	if (BIT_PWR_GYRO_STBY == mask) {
		result = inv_plat_read(st, reg->pwr_mgmt_1, 1, &mgmt_1);
		if (result)
			return result;

		mgmt_1 &= ~BIT_CLK_MASK;
	}

	if ((BIT_PWR_GYRO_STBY == mask) && (!en)) {
		/* turning off gyro requires switch to internal clock first.
		   Then turn off gyro engine */
		mgmt_1 |= INV_CLK_INTERNAL;
		result = inv_plat_single_write(st, reg->pwr_mgmt_1,
						mgmt_1);
		if (result)
			return result;
	}

	result = inv_plat_read(st, reg->pwr_mgmt_2, 1, &data);
	if (result)
		return result;
	if (en)
		data &= (~mask);
	else
		data |= mask;
	result = inv_plat_single_write(st, reg->pwr_mgmt_2, data);
	if (result)
		return result;

	if ((BIT_PWR_GYRO_STBY == mask) && en) {
		/* only gyro on needs sensor up time */
		DelayMs(SENSOR_UP_TIME);
		/* after gyro is on & stable, switch internal clock to PLL */
		mgmt_1 |= INV_CLK_PLL;
		result = inv_plat_single_write(st, reg->pwr_mgmt_1,
						mgmt_1);
		if (result)
			return result;
	}
	if ((BIT_PWR_ACCL_STBY == mask) && en)
		DelayMs(REG_UP_TIME);

	return 0;
}

/**
 *  inv_lpa_freq() - store current low power frequency setting.
 */
static int inv_lpa_freq(struct inv_mpu_iio_s *st, int lpa_freq)
{
	unsigned long result;
	u8 d;
	const u8 mpu6500_lpa_mapping[] = {2, 4, 6, 7};

	if (lpa_freq > MAX_LPA_FREQ_PARAM)
		return -EINVAL;

	if (INV_MPU6500 == st->chip_type) {
		d = mpu6500_lpa_mapping[lpa_freq];
		result = inv_plat_single_write(st, REG_6500_LP_ACCEL_ODR, d);
		if (result)
			return result;
	}
	st->chip_config.lpa_freq = lpa_freq;

	return 0;
}

/*
Name:       MPU6500_AccelEnable
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
int32 MPU6500_AccelEnable(int enable)
{
    uint8 pwr_mgmt;
    if (enable)
    {
        MPU6500_Read(MPU6500_PWR_MGMT_2|0x80, &pwr_mgmt, 1);
        pwr_mgmt = pwr_mgmt & 0xC7; //使能加速度计
        MPU6500_Write(MPU6500_PWR_MGMT_2,pwr_mgmt);
    }
    else
    {
        MPU6500_Read(MPU6500_PWR_MGMT_2|0x80, &pwr_mgmt, 1);
        pwr_mgmt = pwr_mgmt | 0x38; //使不能加速度计
        MPU6500_Write(MPU6500_PWR_MGMT_2,pwr_mgmt);
    }

    return enable;
}

/*
Name:       MPU6500_GyroEnable
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
int32 MPU6500_GyroEnable(int enable)
{
    uint8 pwr_mgmt;
    if (enable)
    {
        MPU6500_Read(MPU6500_PWR_MGMT_2|0x80, &pwr_mgmt, 1);
        pwr_mgmt = pwr_mgmt & 0xF8; //使能陀螺仪
        MPU6500_Write(MPU6500_PWR_MGMT_2,pwr_mgmt);
    }
    else
    {
        MPU6500_Read(MPU6500_PWR_MGMT_2|0x80, &pwr_mgmt, 1);
        pwr_mgmt = pwr_mgmt | 0x07;//使不能陀螺仪
        MPU6500_Write(MPU6500_PWR_MGMT_2,pwr_mgmt);
    }

    return enable;
}

static int inv_switch_gyro_engine(struct inv_mpu_iio_s *st, bool en)
{
	MPU6500_GyroEnable(en);
	return inv_switch_engine(st, en, BIT_PWR_GYRO_STBY);
}

static int inv_switch_accl_engine(struct inv_mpu_iio_s *st, bool en)
{
	MPU6500_AccelEnable(en);
	return inv_switch_engine(st, en, BIT_PWR_ACCL_STBY);
}

static void inv_setup_func_ptr(struct inv_mpu_iio_s *st)
{
	if (st->chip_type == INV_MPU3050) {
		#if 0
		st->set_power_state    = set_power_mpu3050;
		st->switch_gyro_engine = inv_switch_3050_gyro_engine;
		st->switch_accl_engine = inv_switch_3050_accl_engine;
		st->init_config        = inv_init_config_mpu3050;
		st->setup_reg          = inv_setup_reg_mpu3050;
		#endif
	} else {
		st->set_power_state    = set_power_itg;
		st->switch_gyro_engine = inv_switch_gyro_engine;
		st->switch_accl_engine = inv_switch_accl_engine;
		st->init_config        = inv_init_config;
		st->setup_reg          = inv_setup_reg;
		/*MPU6XXX special functions */
		#if 0
		st->compass_en         = inv_compass_enable;
		st->quaternion_en      = inv_quaternion_on;
		#endif
	}
	/*st->gyro_en            = inv_gyro_enable; **/
	/*st->accl_en            = inv_accl_enable; **/
}

static int inv_detect_6xxx(struct inv_mpu_iio_s *st)
{
	int result;
	u8 d;

	result = inv_plat_read(st, REG_WHOAMI, 1, &d);
	if (result)
		return result;
	if ((d == MPU6500_ID) || (d == MPU6515_ID)) {
		st->chip_type = INV_MPU6500;
		strcpy(st->name, "mpu6500");
	} else {
		strcpy(st->name, "mpu6050");
	}

	return 0;
}

#if defined(CHIP_INFO)
int inv_get_silicon_rev_mpu6500(struct inv_mpu_iio_s *st)
{
	struct inv_chip_info_s *chip_info = &st->chip_info;
	int result;
	u8 whoami, sw_rev;

	if (!st->use_hid) {
		result = inv_plat_read(st, REG_WHOAMI, 1, &whoami);
		if (result)
			return result;
		if (whoami != MPU6500_ID && whoami != MPU9250_ID &&
				whoami != MPU6515_ID)
			return -EINVAL;

		/*memory read need more time after power up */
		DelayMs(POWER_UP_TIME);
		result = mpu_memory_read(st, st->i2c_addr,
				MPU6500_MEM_REV_ADDR, 1, &sw_rev);
		sw_rev &= INV_MPU_REV_MASK;
		if (result)
			return result;
		if (sw_rev != 0)
			return -EINVAL;
	}
	/* these values are place holders and not real values */
	chip_info->product_id = MPU6500_PRODUCT_REVISION;
	chip_info->product_revision = MPU6500_PRODUCT_REVISION;
	chip_info->silicon_revision = MPU6500_PRODUCT_REVISION;
	chip_info->software_revision = sw_rev;
	chip_info->gyro_sens_trim = DEFAULT_GYRO_TRIM;
	chip_info->accl_sens_trim = DEFAULT_ACCL_TRIM;
	chip_info->multi = 1;

	return 0;
}
#endif

int inv_power_up_self_test(struct inv_mpu_iio_s *st)
{
	int result;

	result = st->set_power_state(st, true);
	if (result)
		return result;
	result = st->switch_accl_engine(st, true);
	if (result)
		return result;
	result = st->switch_gyro_engine(st, true);
	if (result)
		return result;

	return 0;
}




/**
 *  inv_set_lpf() - set low pass filer based on fifo rate.
 */
static int inv_set_lpf(struct inv_mpu_iio_s *st, int rate)
{
	const short hz[] = {188, 98, 42, 20, 10, 5};
	const int   d[] = {INV_FILTER_188HZ, INV_FILTER_98HZ,
			INV_FILTER_42HZ, INV_FILTER_20HZ,
			INV_FILTER_10HZ, INV_FILTER_5HZ};
	int i, h, data, result;
	struct inv_reg_map_s *reg;
	reg = &st->reg;
	h = (rate >> 1);
	i = 0;
	while ((h < hz[i]) && (i < ARRAY_SIZE(d) - 1))
		i++;
	data = d[i];
#if 0
	if (INV_MPU3050 == st->chip_type) {
		if (st->mpu_slave != NULL) {
			result = st->mpu_slave->set_lpf(st, rate);
			if (result)
				return result;
		}
		result = inv_plat_single_write(st, reg->lpf, data |
			(st->chip_config.fsr << GYRO_CONFIG_FSR_SHIFT));
	} else 
#endif
	{
		result = inv_plat_single_write(st, reg->lpf, data);
	}
	if (result)
		return result;
	st->chip_config.lpf = data;

	return 0;
}

/**
 *  set_fifo_rate_reg() - Set fifo rate in hardware register
 */
static int set_fifo_rate_reg(struct inv_mpu_iio_s *st)
{
	u8 data;
	u16 fifo_rate;
	int result;
	struct inv_reg_map_s *reg;

	reg = &st->reg;
	fifo_rate = st->chip_config.new_fifo_rate;
	data = ONE_K_HZ / fifo_rate - 1;
	result = inv_plat_single_write(st, reg->sample_rate_div, data);
	if (result)
		return result;
	result = inv_set_lpf(st, fifo_rate);
	if (result)
		return result;
	st->chip_config.fifo_rate = fifo_rate;

	return 0;
}

/**
 *  inv_lpa_mode() - store current low power mode settings
 */
static int inv_lpa_mode(struct inv_mpu_iio_s *st, int lpa_mode)
{
	unsigned long result;
	u8 d;
	struct inv_reg_map_s *reg;

	reg = &st->reg;
	result = inv_plat_read(st, reg->pwr_mgmt_1, 1, &d);
	if (result)
		return result;
	if (lpa_mode)
		d |= BIT_CYCLE;
	else
		d &= ~BIT_CYCLE;

	result = inv_plat_single_write(st, reg->pwr_mgmt_1, d);
	if (result)
		return result;
	if (INV_MPU6500 == st->chip_type) {
		if (lpa_mode)
			d = BIT_ACCEL_FCHOCIE_B;
		else
			d = 0;
		result = inv_plat_single_write(st, REG_6500_ACCEL_CONFIG2, d);
		if (result)
			return result;
	}

	return 0;
}

/**
 *  reset_fifo_itg() - Reset FIFO related registers.
 *  @st:	Device driver instance.
 */
static int reset_fifo_itg(struct inv_mpu_iio_s *st)
{
	struct inv_reg_map_s *reg;
	int result, data;
	u8 val, int_word;
	reg = &st->reg;

	if (st->chip_config.lpa_mode) {
		result = inv_lpa_mode(st, 0);
		if (result) {
			mpu_printf("reset lpa mode failed\n");
			return result;
		}
	}
	/* disable interrupt */
	result = inv_plat_single_write(st, reg->int_enable, 0);
	if (result) {
		mpu_printf("int_enable write failed\n");
		return result;
	}
	/* disable the sensor output to FIFO */
	result = inv_plat_single_write(st, reg->fifo_en, 0);
	if (result)
		goto reset_fifo_fail;
	/* disable fifo reading */
	result = inv_plat_single_write(st, reg->user_ctrl, st->i2c_dis);
	if (result)
		goto reset_fifo_fail;
	int_word = 0;

#if 0
	/* MPU6500's BIT_6500_WOM_EN is the same as BIT_MOT_EN */
	if (st->mot_int.mot_on)
		int_word |= BIT_MOT_EN;
#endif
#if 0
	if (st->chip_config.dmp_on) {
		val = (BIT_FIFO_RST | BIT_DMP_RST);
		result = inv_plat_single_write(st, reg->user_ctrl, val | st->i2c_dis);
		if (result)
			goto reset_fifo_fail;
		st->last_isr_time = get_time_ns();
		if (st->chip_config.dmp_int_on) {
			int_word |= BIT_DMP_INT_EN;
			result = inv_plat_single_write(st, reg->int_enable,
							int_word);
			if (result)
				return result;
		}
		val = (BIT_DMP_EN | BIT_FIFO_EN);
		if (st->chip_config.compass_enable &
			(!st->chip_config.dmp_event_int_on))
			val |= BIT_I2C_MST_EN;
		result = inv_plat_single_write(st, reg->user_ctrl, val | st->i2c_dis);
		if (result)
			goto reset_fifo_fail;

		if (st->chip_config.compass_enable) {
			/* I2C_MST_DLY is set according to sample rate,
			   slow down the power*/
			data = max(COMPASS_RATE_SCALE *
				st->chip_config.new_fifo_rate / ONE_K_HZ,
				st->chip_config.new_fifo_rate /
				st->chip_config.dmp_output_rate);
			if (data > 0)
				data -= 1;
			result = inv_plat_single_write(st, REG_I2C_SLV4_CTRL,
							data);
			if (result)
				return result;
		}
		val = 0;
		if (st->chip_config.accl_fifo_enable)
			val |= INV_ACCL_MASK;
		if (st->chip_config.gyro_fifo_enable)
			val |= INV_GYRO_MASK;
		result = inv_send_sensor_data(st, val);
		if (result)
			return result;
		if (st->chip_config.display_orient_on || st->chip_config.tap_on)
			result = inv_send_interrupt_word(st, true);
		else
			result = inv_send_interrupt_word(st, false);
	} else 
#endif
	{
		/* reset FIFO and possibly reset I2C*/
		val = BIT_FIFO_RST;
		result = inv_plat_single_write(st, reg->user_ctrl, val | st->i2c_dis);
		if (result)
			goto reset_fifo_fail;
		//st->last_isr_time = get_time_ns();
		/* enable interrupt */
		//if (st->chip_config.accl_fifo_enable ||
		//    st->chip_config.gyro_fifo_enable ||
		//    st->chip_config.compass_enable) {
		//	int_word |= BIT_DATA_RDY_EN;
		//}
		//result = inv_plat_single_write(st, reg->int_enable, int_word);
		//if (result)
		//	return result;
		/* enable FIFO reading and I2C master interface*/
		val = BIT_FIFO_EN;
		//if (st->chip_config.compass_enable)
		//	val |= BIT_I2C_MST_EN;
		result = inv_plat_single_write(st, reg->user_ctrl, val | st->i2c_dis);
		if (result)
			goto reset_fifo_fail;
		#if 0
		if (st->chip_config.compass_enable) {
			/* I2C_MST_DLY is set according to sample rate,
			   slow down the power*/
			data = COMPASS_RATE_SCALE *
				st->chip_config.new_fifo_rate / ONE_K_HZ;
			if (data > 0)
				data -= 1;
			result = inv_plat_single_write(st, REG_I2C_SLV4_CTRL,
							data);
			if (result)
				return result;
		}
		#endif
		/* enable sensor output to FIFO */
		val = 0;
		if (st->chip_config.gyro_fifo_enable)
			val |= BITS_GYRO_OUT;
		if (st->chip_config.accl_fifo_enable)
			val |= BIT_ACCEL_OUT;
		result = inv_plat_single_write(st, reg->fifo_en, val);
		if (result)
			goto reset_fifo_fail;
	}
	st->chip_config.normal_compass_measure = 0;
	result = inv_lpa_mode(st, st->chip_config.lpa_mode);
	if (result)
		goto reset_fifo_fail;

	return 0;

reset_fifo_fail:
	if (st->chip_config.dmp_on)
		val = BIT_DMP_INT_EN;
	else
		val = BIT_DATA_RDY_EN;
	inv_plat_single_write(st, reg->int_enable, val);
	mpu_printf("reset fifo failed\n");

	return result;
}

/**
 *  inv_reset_fifo() - Reset FIFO related registers.
 *  @st:	Device driver instance.
 */
static int inv_reset_fifo(struct inv_mpu_iio_s *st)
{

		return reset_fifo_itg(st);
}

/**
 *  set_inv_enable() - Reset FIFO related registers.
 *			This also powers on the chip if needed.
 *  @st:	Device driver instance.
 *  @fifo_enable: enable/disable
 */
static int set_inv_enable(struct inv_mpu_iio_s *st, bool enable)
{
	struct inv_reg_map_s *reg;
	int result;

	reg = &st->reg;
	if (enable) {
		if (st->chip_config.new_fifo_rate !=
				st->chip_config.fifo_rate) {
			result = set_fifo_rate_reg(st);
			if (result)
				return result;
		}

#if 0
		if (st->chip_config.dmp_on) {
			result = inv_set_dmp_sysfs(st);
			if (result)
				return result;
		}
#endif
		if (st->chip_config.gyro_enable) {
			result = st->switch_gyro_engine(st, true);
			if (result)
				return result;
		}
		if (st->chip_config.accl_enable) {
			result = st->switch_accl_engine(st, true);
			if (result)
				return result;
		}

		result = inv_reset_fifo(st);
		if (result)
			return result;
	} else {
#if 0
		if ((INV_MPU3050 != st->chip_type)
			&& st->chip_config.lpa_mode) {
			/* if the chip is in low power mode,
				register write/read could fail */
			result = inv_lpa_mode(st, 0);
			if (result)
				return result;
		}
#endif
		result = inv_plat_single_write(st, reg->fifo_en, 0);
		if (result)
			return result;
		/* disable fifo reading */
		if (INV_MPU3050 != st->chip_type) {
			result = inv_plat_single_write(st, reg->int_enable, 0);
			if (result)
				return result;
			result = inv_plat_single_write(st, reg->user_ctrl, st->i2c_dis);
		} 
#if 0
		else {
			result = inv_plat_single_write(st, reg->int_enable,
				st->plat_data.int_config);
		}
#endif
		if (result)
			return result;
		/* turn off the gyro/accl engine during disable phase */
		result = st->switch_gyro_engine(st, false);
		if (result)
			return result;
		result = st->switch_accl_engine(st, false);
		if (result)
			return result;
		result = st->set_power_state(st, false);
		if (result)
			return result;
	}
	st->chip_config.enable = enable;

	return 0;
}

static int inv_preenable(struct inv_mpu_iio_s *st)
{
	return st->set_power_state(st, true);
}
static int inv_postenable(struct inv_mpu_iio_s *st)
{
	return set_inv_enable(st, true);
}

static int inv_predisable(struct inv_mpu_iio_s *st)
{
	return set_inv_enable(st, false);
}

#ifndef ABS
#define ABS(x)(((x) >= 0) ? (x) : -(x))
#endif
//these 2 value should match driver FSR setting of selftest
#define GYRO_FSR_ST (250.f) //+-250DPS
#define ACCEL_FSR_ST (2.f)//(8.f) //+-8G

#define ACCEL_BIAS_THRESHOLD_SMT	500 //mg

const struct inv_mpu_iio_s * inv_get_sensor_info()
{
    return &mpu_st;
}

int inv_selftest_and_store(struct inv_mpu_iio_s *st)
{
    int axis = 0;
	int axis_sign = 1;
	int result = -1;
    float accel_bias_mg[3]={0.f, 0.f, 0.f};
    float gyro_bias_dps[3]={0.f, 0.f, 0.f};
    float gyro_fsr_multi, accel_fsr_multi;

	inv_preenable(st);
	inv_postenable(st);
	result = inv_power_up_self_test(st);
	if (result)
		goto exit;
	result = inv_do_test(st, 0, st->gyro_bias, st->accel_bias);
	// Find the orientation of the device, by looking for gravity
    if (ABS(st->accel_bias[1] ) > ABS(st->accel_bias[0] )) {
        axis = 1;
    }
    if (ABS(st->accel_bias[2] ) > ABS(st->accel_bias[axis] )) {
        axis = 2;
    }
    if (st->accel_bias[axis] < 0) {
        axis_sign = -1;
    }
    // Remove gravity, gravity in raw units should be 16384 for a 2g setting.
    // We read data scaled by 1000, so
    st->accel_bias[axis] -= axis_sign * 32768L / (int)ACCEL_FSR_ST * 1000L;
	if (result)
		goto exit;

	mpu_printf("gyro_bias %d %d %d\n",st->gyro_bias[0],st->gyro_bias[1],st->gyro_bias[2]);
	mpu_printf("accel_bias %d %d %d\n",st->accel_bias[0],st->accel_bias[1],st->accel_bias[2]);

    accel_bias_mg[0] = (float)st->accel_bias[0]*2/32768.f;
    accel_bias_mg[1] = (float)st->accel_bias[1]*2/32768.f;
    accel_bias_mg[2] = (float)st->accel_bias[2]*2/32768.f;

    mpu_printf("Accel bias =    %13.6f, %13.6f, %13.6f mg, thres=%d mg\n", 
        accel_bias_mg[0], accel_bias_mg[1], accel_bias_mg[2], ACCEL_BIAS_THRESHOLD_SMT);

    gyro_fsr_multi = (float)GYRO_FSR_ST/(float)2000;
    accel_fsr_multi = (float)ACCEL_FSR_ST/(float)2;

	st->accel_bias[0] /= 1000;
	st->accel_bias[1] /= 1000;
	st->accel_bias[2] /= 1000;

	st->gyro_bias[0] = (st->gyro_bias[0] *gyro_fsr_multi)/1000;
	st->gyro_bias[1] = (st->gyro_bias[1] *gyro_fsr_multi)/1000;
	st->gyro_bias[2] = (st->gyro_bias[2] *gyro_fsr_multi)/1000;

	gSysConfig.sensor_calibias = 1;
	gSysConfig.sensor_accel_bias[0] = st->accel_bias[0];
	gSysConfig.sensor_accel_bias[1] = st->accel_bias[1];
	gSysConfig.sensor_accel_bias[2] = st->accel_bias[2];

	gSysConfig.sensor_gyro_bias[0] = st->gyro_bias[0];
	gSysConfig.sensor_gyro_bias[1] = st->gyro_bias[1];
	gSysConfig.sensor_gyro_bias[2] = st->gyro_bias[2];
	mpu_printf("SaveSysInformation\n");
	SaveSysInformation();
	result = 0;
exit:
	return result;

}
int inv_selftest_load(struct inv_mpu_iio_s *st)
{
	st->accel_bias[0] = gSysConfig.sensor_accel_bias[0];
	st->accel_bias[1] = gSysConfig.sensor_accel_bias[1];
	st->accel_bias[2] = gSysConfig.sensor_accel_bias[2];

	st->gyro_bias[0] = gSysConfig.sensor_gyro_bias[0];
	st->gyro_bias[1] = gSysConfig.sensor_gyro_bias[1];
	st->gyro_bias[2] = gSysConfig.sensor_gyro_bias[2];
	return 0;
}

#include "Gpio.h"
#include "Hw_spi.h"
#include "ModuleOverlay.h"
//Normal #define
#define SPI_CTRL_CONFIG             (SPI_MASTER_MODE|MOTOROLA_SPI|RXD_SAMPLE_NO_DELAY|APB_BYTE_WR|MSB_FBIT|LITTLE_ENDIAN_MODE|CS_2_SCLK_OUT_1_CK|CS_KEEP_LOW|SERIAL_CLOCK_POLARITY_HIGH|SERIAL_CLOCK_PHASE_START|DATA_FRAME_8BIT)

#define SPI_PORT0                   0
#define SPI_PORT1                   1
static uint8  MPU6500_SPICsn;

/*
Name:       MPU6500_Write
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
int32 MPU6500_Write(uint32 reg_addr, uint8 data)
{
    uint32 cmd[4]={0};
    uint32 data_tmp = 0;
    int time = 0;


    SPICtl->SPI_CTRLR0 = SPI_CTRL_CONFIG| TRANSMIT_RECEIVE;
    SPICtl->SPI_ENR = 1;
    SPICtl->SPI_SER = MPU6500_SPICsn;
    Gpio_SetPinLevel(GPIOPortA_Pin1, GPIO_LOW);
    //Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_LOW);

    cmd[0] = reg_addr;//写数据的地址
    SPICtl->SPI_TXDR[0] = cmd[0]; //*cmd++;

    cmd[1] = data;//要写的数据

    for(time=0; time<100; time++)  //现在SPI Flash速度是12Mb，发一个数据回来一个数据，因此需要经过8bit的时间，也就是83.34ns*8=666.67ns，timeout比1us大就行
    {
        if ((SPICtl->SPI_SR & RECEIVE_FIFO_EMPTY) != RECEIVE_FIFO_EMPTY)
        {
            data_tmp = SPICtl->SPI_RXDR[0];
            break;
        }
    }
    if (time > 100)
    {
        printf ("timeout\n");
    }

    #if 0
    SPICtl->SPI_ENR = 0;
    SPICtl->SPI_SER = 0;//MPU6500_SPICsn;
    //Gpio_SetPinLevel(GPIOPortA_Pin1, GPIO_HIGH);
    Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_HIGH);
    DelayUs(1);               //cs# high time > 100ns

    SPICtl->SPI_CTRLR0 = SPI_CTRL_CONFIG| TRANSMIT_RECEIVE;
    SPICtl->SPI_ENR = 1;
    SPICtl->SPI_SER = MPU6500_SPICsn;
    //Gpio_SetPinLevel(GPIOPortA_Pin1, GPIO_LOW);
    Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_LOW);
    #endif

    SPICtl->SPI_TXDR[0] = cmd[1]; //*cmd++;
    //printf ("Write 0x%x to 0x%x\n",cmd[1],cmd[0]);
    for(time=0; time<100; time++)  //现在SPI Flash速度是12Mb，发一个数据回来一个数据，因此需要经过8bit的时间，也就是83.34ns*8=666.67ns，timeout比1us大就行
    {
        if ((SPICtl->SPI_SR & RECEIVE_FIFO_EMPTY) != RECEIVE_FIFO_EMPTY)
        {
            data_tmp = SPICtl->SPI_RXDR[0];
            break;
        }
    }
    if (time > 100)
    {
        printf ("timeout\n");
    }

    SPICtl->SPI_ENR = 0;
    SPICtl->SPI_SER = 0;
    Gpio_SetPinLevel(GPIOPortA_Pin1, GPIO_HIGH);
    //Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_HIGH);
    DelayUs(1);               //cs# high time > 100ns

    return OK;
}


/*
Name:       MPU6500_Read
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
int32 MPU6500_Read(uint32 reg_addr, uint8* data, uint32 size)
{
    int32 ret=OK;
    uint32 time;
    uint32 cmd[4]={0};
    uint32 DummySize = 0,CmdLen = 1;
    uint32 DummyLen = 0, DataLen;
    uint32 data_tmp=0;

    SPICtl->SPI_CTRLR0 = SPI_CTRL_CONFIG| TRANSMIT_RECEIVE;
    SPICtl->SPI_ENR = 1;
    SPICtl->SPI_SER = MPU6500_SPICsn;
    Gpio_SetPinLevel(GPIOPortA_Pin1, GPIO_LOW);
    //Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_LOW);

    cmd[0] = reg_addr;//读数据的地址

    /*采用了收发模式, 发送命令的同时会接受数据进来, 所以要过滤掉CmdLen长度的数据
     DummySize 是命令发送完成后, 数据没有马上出来, 需要等待的无效的数据长度,
     一般SPINOR是不需要等待的, 所以DummySize=0*/
    DummyLen = (size > 0)? (DummySize+CmdLen):CmdLen;
    DataLen = size + DummyLen;
    while (DataLen)
    {
        if (CmdLen)
        {
            SPICtl->SPI_TXDR[0] = cmd[0]; //*cmd++;
            CmdLen--;
        }
        else
        {
            SPICtl->SPI_TXDR[0] = 0xFF;     //send clock
        }
        for(time=0; time<100; time++)  //现在SPI Flash速度是12Mb，发一个数据回来一个数据，因此需要经过8bit的时间，也就是83.34ns*8=666.67ns，timeout比1us大就行
        {
            if ((SPICtl->SPI_SR & RECEIVE_FIFO_EMPTY) != RECEIVE_FIFO_EMPTY)
            {
                if (DummyLen > 0)   //发送完命令后, 可能要等几个时钟,数据才出来
                {
                    data_tmp = SPICtl->SPI_RXDR[0];
                    *data = (uint8)(data_tmp & 0xFF);
                    DummyLen--;
                }
                else
                {
                    data_tmp = SPICtl->SPI_RXDR[0];
                    //printf ("--Valid data_tmp=0x%x\n",data_tmp);
                    *data++ = (uint8)(data_tmp & 0xFF);
                }
                break;
            }
            //DelayUs(1);
            //ASMDelay(1);
        }

        if (time>=100)
        {
            printf ("SPI  timeout\n");
            ret = ERROR;
            break;
        }

        DataLen--;
    }

    SPICtl->SPI_ENR = 0;
    SPICtl->SPI_SER = 0;//MPU6500_SPICsn;
    Gpio_SetPinLevel(GPIOPortA_Pin1, GPIO_HIGH);
    //Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_HIGH);
    DelayUs(1);               //cs# high time > 100ns

    return OK;
}

/*
Name:       MPU6500_Spi_Init
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
_ATTR_FLASH_CODE_
static void MPU6500_Spi_Init(uint8 port, uint8 spidev_cs)
{
    if  (port == 0)
    {
        #if 1 //若一开始SPI0没有初始化，就执行注释的步骤
        //SPI CTRL 选择24M 晶振 不分频输入
        CRUReg->CRU_CLKSEL_SPI_CON = 1<<0 | 0<<1 | 1<<16 | 63<<17;
        //init spi
        SPICtl->SPI_BAUDR = 2;     //sclk_out = 24M/2 = 12MHz
        SPICtl->SPI_CTRLR0 = (SPI_MASTER_MODE | TRANSMIT_RECEIVE | MOTOROLA_SPI | RXD_SAMPLE_NO_DELAY | APB_BYTE_WR
                                | MSB_FBIT | LITTLE_ENDIAN_MODE | CS_2_SCLK_OUT_1_CK | CS_KEEP_LOW | SERIAL_CLOCK_POLARITY_HIGH
                                | SERIAL_CLOCK_PHASE_START | DATA_FRAME_8BIT); // 8bit data frame size, CPOL=1,CPHA=1
        //CPOL=1,CPHA=1:空闲为高电平，第二个跳变沿采样

        //先设完spi的再设iomux，否则SPI-CLK会有一个低脉冲
        #endif

        switch (spidev_cs)
        {
            case 1: //Spinor flash
                break;
            case 2: //Sensor MPU6500
                //spi_rxd_p0, spi_txd_p0, spi_clk_p0, spi_csn0
                GRFReg->GPIO0A_IOMUX = 2<<6 | 2<<8 | 2<<10 | 2<<12 | 3<<22| 3<<24| 3<<26 | 3<<28;

                #if 1
                GpioMuxSet(GPIOPortA_Pin6,IOMUX_GPIOA6_IO);
                //拉低SPI Flash0 CS片选有效
                Gpio_SetPinDirection(GPIOPortA_Pin6,GPIO_OUT);
                Gpio_SetPinLevel(GPIOPortA_Pin6, GPIO_HIGH);//SPI0 Flash0_CS

                //GpioMuxSet(GPIOPortC_Pin1,IOMUX_GPIOC1_IO);
                //Gpio_SetPinDirection(GPIOPortC_Pin1,GPIO_OUT);
                //Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_HIGH);//拉高CS，MPU6500无法通信

                //使用GPIOA[1]做MPU6500的CS
                GpioMuxSet(GPIOPortA_Pin1,IOMUX_GPIOA1_IO);
                Gpio_SetPinDirection(GPIOPortA_Pin1,GPIO_OUT);
                Gpio_SetPinLevel(GPIOPortA_Pin1, GPIO_HIGH);//拉高CS，MPU6500无法通信
                #endif

                //spi1 切到gpio避免影响到 spi0 接口
                GRFReg->GPIO0B_IOMUX = 0<<2 | 0<<4 | 0<<6 | 0<<8 | 3<<18| 3<<20| 3<<22 | 3<<24;

                GRFReg->IOMUX_CON1 = 0<<1 | 1<<17; //spi_xxx_p0 available for spi 此设置只影响miso


                break;
            default:
                break;
        }
        MPU6500_SPICsn = 1;
    }

    if  (port == 1)
    {
        //SPI CTRL 选择24M 晶振 不分频输入
        CRUReg->CRU_CLKSEL_SPI_CON = 1<<0 | 0<<1 | 1<<16 | 63<<17;
        //init spi
        SPICtl->SPI_BAUDR = 2;     //sclk_out = 24M/2 = 12MHz
        SPICtl->SPI_CTRLR0 = (SPI_MASTER_MODE | TRANSMIT_RECEIVE | MOTOROLA_SPI | RXD_SAMPLE_NO_DELAY | APB_BYTE_WR
                                | MSB_FBIT | LITTLE_ENDIAN_MODE | CS_2_SCLK_OUT_1_CK | CS_KEEP_LOW | SERIAL_CLOCK_POLARITY_HIGH
                                | SERIAL_CLOCK_PHASE_START | DATA_FRAME_8BIT); // 8bit data frame size, CPOL=1,CPHA=1
        //CPOL=1,CPHA=1:空闲为高电平，第二个跳变沿采样

        //先设完spi的再设iomux，否则SPI-CLK会有一个低脉冲

        #if 1//SPIIomux();
        //spi_rxd_p1, spi_txd_p1, spi_clk_p1, spi_csn1
        GRFReg->GPIO0B_IOMUX = 1<<4 | 1<<6 | 1<<8 | 3<<20| 3<<22 | 3<<24;

        //GRFReg->GPIO0C_IOMUX = 0<<2|3<<18; //使用GPIOC[1]做SPI1的CS
        GpioMuxSet(GPIOPortC_Pin1,IOMUX_GPIOC1_IO);

        //spi0 切到gpio避免影响到 spi1 接口
        GRFReg->GPIO0A_IOMUX = 0<<6 | 0<<8 | 0<<10 | 0<<12 | 3<<22| 3<<24| 3<<26 | 3<<28;

        GRFReg->IOMUX_CON1 = 1<<1 | 1<<17; //spi_xxx_p1 available for spi 此设置只影响miso
        MPU6500_SPICsn = 2;
        //拉低SPI1 CS片选有效
        //GpioMuxSet(GPIOPortC_Pin1,IOMUX_GPIOC1_IO);
        Gpio_SetPinDirection(GPIOPortC_Pin1,GPIO_OUT);
        Gpio_SetPinLevel(GPIOPortC_Pin1, GPIO_LOW);//SPI1_CS
        #endif
    }

}

/*
Name:       MPU6500_Init
Desc:
Param:
Return:
Global:
Note:
Author:
Log:
*/
int MPU6500_Init(int force_calib)
{
	int i = 0;
	int result = -1;
	struct inv_mpu_iio_s *st = &mpu_st;
	struct inv_reg_map_s *reg;

	mpu_printf("mpu init enter...\n");
	MPU6500_Spi_Init(SPI_PORT0, 2);//1:spi flash; 2:MPU6500
	st->plat_read = inv_hid_read;
	st->plat_single_write = inv_hid_single_write;
	st->secondary_read = inv_hid_secondary_read;
	st->secondary_write = inv_hid_secondary_write;
	st->memory_read = mpu_hid_memory_read;
	st->memory_write = mpu_hid_memory_write;
	st->chip_type = INV_MPU6500;
	st->i2c_dis = BIT_I2C_IF_DIS;
	inv_setup_func_ptr(st);
	reg = &st->reg;
	st->setup_reg(reg);

	/* reset to make sure previous state are not there */
	result = inv_plat_single_write(st, reg->pwr_mgmt_1, BIT_H_RESET);
	if (result) {
		mpu_printf("BIT_H_RESET failed!\n");
		goto exit;
	}
	DelayMs(POWER_UP_TIME);
	/* toggle power state */
	result = st->set_power_state(st, false);
	if (result)
		goto exit;
	result = st->set_power_state(st, true);
	if (result)
		goto exit;
	result = inv_plat_single_write(st, reg->user_ctrl, st->i2c_dis);
	if (result)
		goto exit;
	DelayMs(POWER_UP_TIME);
	result = inv_detect_6xxx(st);
	if (result)
		goto exit;

#if defined(CHIP_INFO)
	switch (st->chip_type) {
	case INV_MPU6050:
		/*result = inv_get_silicon_rev_mpu6050(st); **/
		break;
	case INV_MPU6500:
		result = inv_get_silicon_rev_mpu6500(st);
		break;
	default:
		result = 0;
		break;
	}
#endif

	mpu_printf("step1:detected %s\n", st->name);

	/* turn off the gyro engine after OTP reading */
	result = st->switch_gyro_engine(st, false);
	if (result)
		goto exit;
	result = st->switch_accl_engine(st, false);
	if (result)
		goto exit;

	result = st->init_config(st);
	if (result) {
		mpu_printf("Could not initialize device.\n");
		goto exit;
	}

#if USB_CALIBIAS
	if(force_calib || (0 == gSysConfig.sensor_calibias)) {
		mpu_printf("start mpu6500 self test..\n");
		result = inv_selftest_and_store(st);
		if (result) {
			mpu_printf("inv_selftest_and_store fail.\n");
			goto exit;
		}
		inv_recover_setting(st);

		/* turn off the gyro engine after OTP reading */
		result = st->set_power_state(st, false);
		if (result)
			goto exit;
		result = st->switch_gyro_engine(st, false);
		if (result)
			goto exit;

		result = st->switch_accl_engine(st, false);
		if (result)
			goto exit;
	} else {
		result =  inv_selftest_load(st);
		if (result) {
			mpu_printf("inv_selftest_load fail.\n");
			goto exit;
		}
		mpu_printf("inv_selftest_load ok %d\n", gSysConfig.sensor_calibias);
		mpu_printf("gyro_bias %d %d %d\n",st->gyro_bias[0],st->gyro_bias[1],st->gyro_bias[2]);
		mpu_printf("accel_bias %d %d %d\n",st->accel_bias[0],st->accel_bias[1],st->accel_bias[2]);
	}
#endif
	result = st->set_power_state(st, true);
	if (result)
		goto exit;

	result = st->switch_gyro_engine(st, true);
	if (result)
		goto exit;

	result = st->switch_accl_engine(st, true);
	if (result)
		goto exit;
	result = 0;
exit:
	mpu_printf("mpu init exit %d\n", result);
	return result;
}

int32 MPU6500_Read_Seq(int16 *a, int16 *g, int16 * t)
{
	struct inv_mpu_iio_s *st = &mpu_st;
	uint8 reg;
	uint8 buf[6 + 6 + 2];
	uint8 read_count = 0;
	int32 result = ERROR;
	int i;
	uint8 *p;

	if (a)
		read_count += BYTES_PER_SENSOR;
	if (t)
		read_count += 2;
	if (g)
		read_count += BYTES_PER_SENSOR;

	if (a) {
		reg = REG_RAW_ACCEL;
	} else if (t) {
		reg = REG_TEMPERATURE;
	} else if (g) {
		reg = REG_RAW_GYRO;
	}

	result = inv_plat_read(st, reg, read_count, buf) ;
	if (result != OK) {
		return ERROR;
	}
	p = buf;
	if (a) {
		for (i = 0; i < 3; i++) {
			a[i] = (p[0] << 8) | p[1];
#if USB_CALIBIAS
			a[i] -= st->accel_bias[i];
#endif
			p += 2;
		}
	}
	if (t) {
		t[i] = (p[0] << 8) | p[1];
		p += 2;
	}
	if (g) {
		for (i = 0; i < 3; i++) {
			g[i] = (p[0] << 8) | p[1];
#if USB_CALIBIAS
			g[i] -= st->gyro_bias[i];
#endif
			p += 2;
		}
	}

	#if 0
    printf ("accel: %d %d %d  gyro: %d %d %d\n",
             a[0],
             a[1],
             a[2],
             g[0],
             g[1],
             g[2]);
    #endif
	return OK;
}




