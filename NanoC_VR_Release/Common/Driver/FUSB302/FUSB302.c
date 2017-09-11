/*
 * Copyright (c) 2016, Fuzhou Rockchip Electronics Co., Ltd
 * Author: Zain Wang <zain.wang@rock-chips.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * Some ideas are from chrome ec and fairchild GPL fusb302 driver.
 */

#include "DriverInclude.h"
#include "Gpio.h"
#include "OsInclude.h"

#include "FUSB302.h"
#include "SysConfig.h"

#define FUSB302ID           0x81
#define BIT(x)              (1<<x)

#define FUSB302_MAX_REG		(FUSB_REG_FIFO + 50)
//#define FUSB_MS_TO_NS(x)	((s64)x * 1000 * 1000)

#define FUSB_MODE_DRP		0
#define FUSB_MODE_UFP		1
#define FUSB_MODE_DFP		2
#define FUSB_MODE_ASS		3

#define TYPEC_CC_VOLT_OPEN	0
#define TYPEC_CC_VOLT_RA	1
#define TYPEC_CC_VOLT_RD	2
#define TYPEC_CC_VOLT_RP	3

#define EVENT_CC		    BIT(0)
#define EVENT_RX		    BIT(1)
#define EVENT_TX		    BIT(2)
#define EVENT_REC_RESET		BIT(3)
#define EVENT_WORK_CONTINUE	BIT(5)
#define EVENT_TIMER_MUX		BIT(6)
#define EVENT_TIMER_STATE	BIT(7)
#define FLAG_EVENT		    (EVENT_RX)

#define PIN_MAP_A		    BIT(0)
#define PIN_MAP_B		    BIT(1)
#define PIN_MAP_C		    BIT(2)
#define PIN_MAP_D		    BIT(3)
#define PIN_MAP_E		    BIT(4)
#define PIN_MAP_F		    BIT(5)

#define FUSB302_SLAVEADDR   0x45
#define EINPROGRESS         1

struct fusb30x_chip gchip;
uint8 fusb_do_workfunc = 0;
uint8 wrm_test = 0;
volatile int gevt = 0;
volatile int work_continue = 0;


void state_machine_typec(struct fusb30x_chip *chip);
void set_mesg(struct fusb30x_chip *chip, int cmd, int is_DMT);



uint8 regmap_read(uint8 reg_address, uint8* rdata, uint8 size)
{
    i2c_read_data(FUSB302_SLAVEADDR, reg_address, rdata, size);

    return *rdata;
}

uint8 regmap_write(uint8 reg_address, uint8 wdata, uint8 size)
{
    i2c_write_data(FUSB302_SLAVEADDR, reg_address, wdata, size);

    return wdata;
}

uint8 regmap_raw_write(uint8 reg_address, uint8* wdata, uint8 size)
{
    BOOL ret = 0;
    I2C_Init(FUSB302_SLAVEADDR, 400);
    ret = I2C_Write(reg_address, wdata, size, NormalMode);
    I2C_Deinit();

    return ret;
}



uint8 regmap_update_bits(uint8 reg_address, uint8 mask, uint8 value)
{
    uint8 val;
    uint8 rdata8;

    i2c_read_data(FUSB302_SLAVEADDR, reg_address, &rdata8, 1);
    val = ((rdata8 & (~mask)) | (value & mask));
    i2c_write_data(FUSB302_SLAVEADDR, reg_address, val, 1);

    return 0;
}
void platform_fusb_notify(struct fusb30x_chip *chip)
{
    printf ("\n platform_fusb_notify \n");

#if 0
    BOOL plugged = 0, flip = 0, dfp = 0, ufp = 0, dp = 0;
    union extcon_property_value property;

    if (chip->notify.is_cc_connected)
        chip->notify.orientation = chip->cc_polarity + 1;

    /* avoid notify repeated */
    if (memcmp(&chip->notify, &chip->notify_cmp,
           sizeof(struct notify_info))) {
        dump_notify_info(chip);
        chip->notify.attention = 0;
        memcpy(&chip->notify_cmp, &chip->notify,
               sizeof(struct notify_info));

        plugged = chip->notify.is_cc_connected ||
              chip->notify.is_pd_connected;
        flip = chip->notify.orientation ?
               (chip->notify.orientation - 1) : 0;
        dp = chip->notify.is_enter_mode;

        if (dp)
            dfp = (chip->notify.pin_assignment_def &
                (PIN_MAP_B | PIN_MAP_D | PIN_MAP_F)) ? 1 : 0;
        else if (chip->notify.data_role)
            dfp = 1;
        else if (plugged)
            ufp = 1;

        property.intval = flip;
        extcon_set_property(chip->extcon, EXTCON_USB,
                    EXTCON_PROP_USB_TYPEC_POLARITY, property);
        extcon_set_property(chip->extcon, EXTCON_USB_HOST,
                    EXTCON_PROP_USB_TYPEC_POLARITY, property);
        extcon_set_property(chip->extcon, EXTCON_DISP_DP,
                    EXTCON_PROP_USB_TYPEC_POLARITY, property);
        extcon_set_state(chip->extcon, EXTCON_USB, ufp);
        extcon_set_state(chip->extcon, EXTCON_USB_HOST, dfp);
        extcon_set_state(chip->extcon, EXTCON_DISP_DP, dp);
        extcon_sync(chip->extcon, EXTCON_USB);
        extcon_sync(chip->extcon, EXTCON_USB_HOST);
        extcon_sync(chip->extcon, EXTCON_DISP_DP);
    }
#endif
}



void set_state(struct fusb30x_chip *chip, enum connection_state state)
{
    printf("port %d, state %d\n", chip->port_num, state);
    if (!state)
        printf("PD disabled\n");
    chip->conn_state = state;
    chip->sub_state = 0;
    chip->val_tmp = 0;
    chip->work_continue = 1;
    work_continue++ ;
}
int tcpm_get_message(struct fusb30x_chip *chip)
{
    uint8 buf[32];
    int len;
	int i;
    regmap_read(FUSB_REG_FIFO, buf, 3);
//   if(!((buf[1] & 0xff) | ((buf[2] << 8) & 0xff00))){
//   	printf("%s,recieve bad packets, len is 0\n",__FUNCTION__);
//   	return -1;
//   }
    chip->rec_head = (buf[1] & 0xff) | ((buf[2] << 8) & 0xff00);
    len = PD_HEADER_CNT(chip->rec_head) << 2;

    if(len > 32){
	    printf("%s,[warning] stack may overflow!\n",__FUNCTION__);
    }
    regmap_read(FUSB_REG_FIFO, buf, len + 4);
    memcpy(chip->rec_load, buf, len);

    return 0;
}

void fusb302_flush_rx_fifo(struct fusb30x_chip *chip)
{
//    tcpm_get_message(chip);
	regmap_update_bits(FUSB_REG_CONTROL1, CONTROL1_RX_FLUSH, CONTROL1_RX_FLUSH);
}

void fusb302_flush_tx_fifo(struct fusb30x_chip *chip)
{
//    tcpm_get_message(chip);
	regmap_update_bits(FUSB_REG_CONTROL0, CONTROL0_TX_FLUSH, CONTROL0_TX_FLUSH);
}

int tcpm_get_cc(struct fusb30x_chip *chip, int *CC1, int *CC2)
{
    uint8 val;
    int *CC_MEASURE;
    uint8 store;
    uint32 control0_val;
    uint32 meas_high, meas_low;

    *CC1 = TYPEC_CC_VOLT_OPEN;
    *CC2 = TYPEC_CC_VOLT_OPEN;

    if (chip->cc_state & 0x01) //0x05
        CC_MEASURE = CC1;
    else
        CC_MEASURE = CC2;

    if (chip->cc_state & 0x04) {
        regmap_read(FUSB_REG_SWITCHES0, (&store), 1);

        /* measure cc1 first */
        regmap_update_bits( FUSB_REG_SWITCHES0,
                            SWITCHES0_MEAS_CC1 | SWITCHES0_MEAS_CC2 |
                            SWITCHES0_PU_EN1 | SWITCHES0_PU_EN2 |
                            SWITCHES0_PDWN1 | SWITCHES0_PDWN2,
                            SWITCHES0_PDWN1 | SWITCHES0_PDWN2 |
                            SWITCHES0_MEAS_CC1);
        //usleep_range(250, 300);
        DelayUs(300);

        regmap_read(FUSB_REG_STATUS0, (&val), 1);
        val &= STATUS0_BC_LVL;
        if (val)
            *CC1 = val;

        /* measure cc2 first */
        regmap_update_bits( FUSB_REG_SWITCHES0,
                            SWITCHES0_MEAS_CC1 | SWITCHES0_MEAS_CC2 |
                            SWITCHES0_PU_EN1 | SWITCHES0_PU_EN2 |
                            SWITCHES0_PDWN1 | SWITCHES0_PDWN2,
                            SWITCHES0_PDWN1 | SWITCHES0_PDWN2 |
                            SWITCHES0_MEAS_CC2);
        //usleep_range(250, 300);
        DelayUs(300);

        regmap_read(FUSB_REG_STATUS0, (&val), 1);
        val &= STATUS0_BC_LVL;
        if (val)
            *CC2 = val;

        regmap_update_bits( FUSB_REG_SWITCHES0,
                            SWITCHES0_MEAS_CC1 | SWITCHES0_MEAS_CC2,
                            store);
    } else {
        if (chip->cc_state & 0x01) {
            regmap_update_bits( FUSB_REG_SWITCHES0,
                                SWITCHES0_MEAS_CC1 |
                                SWITCHES0_MEAS_CC2 |
                                SWITCHES0_PU_EN1 |
                                SWITCHES0_PU_EN2 |
                                SWITCHES0_PDWN1 |
                                SWITCHES0_PDWN2,
                                SWITCHES0_MEAS_CC1 | SWITCHES0_PU_EN1);
        } else {
            regmap_update_bits( FUSB_REG_SWITCHES0,
                                SWITCHES0_MEAS_CC1 |
                                SWITCHES0_MEAS_CC2 |
                                SWITCHES0_PU_EN1 |
                                SWITCHES0_PU_EN2 |
                                SWITCHES0_PDWN1 |
                                SWITCHES0_PDWN2,
                                SWITCHES0_MEAS_CC2 | SWITCHES0_PU_EN2);
        }

	/*
	 * according to the host current, the compare value is different
	*/
	switch ((control0_val & CONTROL0_HOST_CUR) >> 2) {
		/* host pull up current is 80ua , high voltage is 1.596v, low is 0.21v */
		case 0x1:
			meas_high = 0x25;
			meas_low = 0x4;
			break;
		/* host pull up current is 180ua , high voltage is 1.596v, low is 0.42v */
		case 0x2:
			meas_high = 0x25;
			meas_low = 0x9;
			break;
		/* host pull up current is 80ua , high voltage is 2.604v, low is 0.798v*/
		case 0x3:
			meas_high = 0x25;
			meas_low = 0x12;
			break;
		default:
			meas_high = 0x25;
			meas_low = 0x25;
			break;

	}
	regmap_write(FUSB_REG_MEASURE, meas_high,1);
	//regmap_write(FUSB_REG_MEASURE, 0x26 << 2, 1);
        //usleep_range(250, 300);
        DelayUs(300);

        regmap_read(FUSB_REG_STATUS0, (&val), 1);

        if (val & STATUS0_COMP) {
		int retry = 3;
		int comp_times = 0;
		
		while(retry--){
			regmap_write(FUSB_REG_MEASURE, meas_high,1);
			DelayUs(300);
			regmap_read( FUSB_REG_STATUS0, &val,1);
			if (val & STATUS0_COMP){
				comp_times ++;
				if(comp_times == 3) {
					printf("%s,status0=%x,meas_high=%x\n",__func__,val,meas_high);
					*CC_MEASURE = TYPEC_CC_VOLT_OPEN;
					return 0;
				}
			}

		}
	} 
	
	regmap_write(FUSB_REG_MEASURE, meas_low, 1);
	//usleep_range(250, 300);
	DelayUs(300);

	regmap_read(FUSB_REG_STATUS0, (&val), 1);

	if (val & STATUS0_COMP)
	    *CC_MEASURE = TYPEC_CC_VOLT_RA;
	else
	    *CC_MEASURE = TYPEC_CC_VOLT_RD;
	}

    return 0;
}

static int tcpm_set_cc(struct fusb30x_chip *chip, int mode)
{
    uint8 val = 0, mask;

    val &= ~(SWITCHES0_PU_EN1 | SWITCHES0_PU_EN2 | SWITCHES0_PDWN1 | SWITCHES0_PDWN2);

    mask = ~val;

    switch (mode) {
        case FUSB_MODE_DFP:
            if (chip->togdone_pullup)
                val |= SWITCHES0_PU_EN2;
            else
                val |= SWITCHES0_PU_EN1;
            break;
        case FUSB_MODE_UFP:
            //?
            val |= SWITCHES0_PDWN1 | SWITCHES0_PDWN2;
            break;
        case FUSB_MODE_DRP:
            val |= SWITCHES0_PDWN1 | SWITCHES0_PDWN2;
            break;
        case FUSB_MODE_ASS:
            break;
    }

    regmap_update_bits(FUSB_REG_SWITCHES0, mask, val);
    return 0;
}


int tcpm_set_rx_enable(struct fusb30x_chip *chip, int enable)
{
    uint8 val = 0;

    if (enable) {
        if (chip->cc_polarity)
            val |= SWITCHES0_MEAS_CC2;
        else
            val |= SWITCHES0_MEAS_CC1;
        regmap_update_bits( FUSB_REG_SWITCHES0,
                            SWITCHES0_MEAS_CC1 | SWITCHES0_MEAS_CC2,
                            val);
//	regmap_update_bits(FUSB_REG_CONTROL0, CONTROL0_INT_MASK | CONTROL0_HOST_CUR, ~CONTROL0_INT_MASK | CONTROL0_HOST_CUR);
        regmap_update_bits(FUSB_REG_SWITCHES1, SWITCHES1_AUTO_CRC, SWITCHES1_AUTO_CRC);
    } else {
        /*
        * bit of a hack here.
        * when this function is called to disable rx (enable=0)
        * using it as an indication of detach (gulp!)
        * to reset our knowledge of where
        * the toggle state machine landed.
        */
        chip->togdone_pullup = 0;

#ifdef	FUSB_HAVE_DRP
        tcpm_set_cc(chip, FUSB_MODE_DRP);
        regmap_update_bits(FUSB_REG_CONTROL2, CONTROL2_TOG_RD_ONLY, CONTROL2_TOG_RD_ONLY);
#endif

#ifdef	FUSB_HAVE_UFP
        tcpm_set_cc(chip, FUSB_MODE_UFP);
        regmap_update_bits(FUSB_REG_CONTROL2, CONTROL2_TOG_RD_ONLY, CONTROL2_TOG_RD_ONLY);
#endif
        regmap_update_bits(FUSB_REG_SWITCHES0, SWITCHES0_MEAS_CC1 | SWITCHES0_MEAS_CC2, 0);
        regmap_update_bits(FUSB_REG_SWITCHES1, SWITCHES1_AUTO_CRC, 0);

    }

    return 0;
}

int tcpm_set_msg_header(struct fusb30x_chip *chip)
{
    regmap_update_bits( FUSB_REG_SWITCHES1,
                        SWITCHES1_POWERROLE | SWITCHES1_DATAROLE,
                       (chip->notify.power_role << 7) |
                       (chip->notify.data_role << 4));
    regmap_update_bits(FUSB_REG_SWITCHES1, SWITCHES1_SPECREV, 2 << 5); //Do Not Use
    return 0;
}

int tcpm_set_polarity(struct fusb30x_chip *chip, bool polarity)
{
    uint8 val = 0;

#ifdef FUSB_VCONN_SUPPORT
    if (chip->vconn_enabled) {
        if (polarity)
            val |= SWITCHES0_VCONN_CC1;
        else
            val |= SWITCHES0_VCONN_CC2;
    }
#endif

    if (polarity)
        val |= SWITCHES0_MEAS_CC2;
    else
        val |= SWITCHES0_MEAS_CC1;

    regmap_update_bits(FUSB_REG_SWITCHES0,
                       SWITCHES0_VCONN_CC1 | SWITCHES0_VCONN_CC2 |
                       SWITCHES0_MEAS_CC1 | SWITCHES0_MEAS_CC2,
                       val);

    val = 0;
    if (polarity)
        val |= SWITCHES1_TXCC2;
    else
        val |= SWITCHES1_TXCC1;

    //Enable BMC transmit driver on CCX pin.
    regmap_update_bits(FUSB_REG_SWITCHES1, SWITCHES1_TXCC1 | SWITCHES1_TXCC2, val);

    chip->cc_polarity = polarity;
    return 0;
}

static int tcpm_set_vconn(struct fusb30x_chip *chip, int enable)
{
    uint8 val = 0;

    if (enable) {
        tcpm_set_polarity(chip, chip->cc_polarity);
    } else {
        val &= ~(SWITCHES0_VCONN_CC1 | SWITCHES0_VCONN_CC2);
        //CC1 CC2 VCONN Disable
        regmap_update_bits(FUSB_REG_SWITCHES0,
                           SWITCHES0_VCONN_CC1 | SWITCHES0_VCONN_CC2,
                           val);
    }
    chip->vconn_enabled = enable;
    return 0;
}

void fusb302_pd_reset(struct fusb30x_chip *chip)
{
    regmap_write(FUSB_REG_RESET, RESET_PD_RESET, 1);
}
void tcpm_init(struct fusb30x_chip *chip)
{
    uint8 val;
    uint8 readID = 0;
    readID = regmap_read(FUSB_REG_DEVICEID, &chip->chip_id, 1);
    if (readID == 0)
    {
        printf ("Fusb302 read ID failure readID=0x%x,read again\n",readID);
        readID = regmap_read(FUSB_REG_DEVICEID, &chip->chip_id, 1);
	if(readID == 0){
		printf ("Fusb302 read ID failure readID=0x%x,reboot system\n",readID);
		SysReboot(0x00000000, 1);
	}
    }
    else
    {
        printf ("Fusb302 read ID success readID=0x%x\n",readID);
    }
    chip->notify.is_cc_connected = 0;
    chip->cc_state = 0;

    /* restore default settings */
    regmap_update_bits(FUSB_REG_RESET, RESET_SW_RESET, RESET_SW_RESET);
    fusb302_pd_reset(chip);

    /* set auto_retry and number of retries */
    regmap_update_bits( FUSB_REG_CONTROL3,
                        CONTROL3_AUTO_RETRY | CONTROL3_N_RETRIES,
                        CONTROL3_AUTO_RETRY | CONTROL3_N_RETRIES),

    /* set interrupts */
    val = 0xff;
    val &= ~(MASK_M_BC_LVL | MASK_M_COLLISION | MASK_M_ALERT | MASK_M_VBUSOK); //这几个中断不屏蔽
    regmap_write(FUSB_REG_MASK, val, 1);

    val = 0xff;
    val &= ~(MASKA_M_TOGDONE | MASKA_M_RETRYFAIL | MASKA_M_HARDSENT | MASKA_M_TXSENT | MASKA_M_HARDRST);
    regmap_write(FUSB_REG_MASKA, val, 1);

    val = 0xff;
	val = ~MASKB_M_GCRCSEND;
    regmap_write(FUSB_REG_MASKB, val, 1);

#ifdef	FUSB_HAVE_DRP
    regmap_update_bits( FUSB_REG_CONTROL2,
                        CONTROL2_MODE | CONTROL2_TOGGLE,
                        (1 << 1) | CONTROL2_TOGGLE);

    regmap_update_bits( FUSB_REG_CONTROL2,
                        CONTROL2_TOG_RD_ONLY,
                        CONTROL2_TOG_RD_ONLY);
#endif
#ifdef	FUSB_HAVE_UFP
    regmap_update_bits( FUSB_REG_CONTROL2,
                        CONTROL2_MODE | CONTROL2_TOGGLE,
                        (1 << 2) | CONTROL2_TOGGLE);

    regmap_update_bits( FUSB_REG_CONTROL2,
                        CONTROL2_TOG_RD_ONLY,
                        CONTROL2_TOG_RD_ONLY);
#endif

    /* Interrupts Enable */
    regmap_update_bits(FUSB_REG_CONTROL0, CONTROL0_INT_MASK, ~CONTROL0_INT_MASK);

    tcpm_set_polarity(chip, 0);
    tcpm_set_vconn(chip, 0);

    regmap_write(FUSB_REG_POWER, 0xf, 1);
}




void pd_execute_hard_reset(struct fusb30x_chip *chip)
{
    chip->msg_id = 0;
    chip->vdm_state = 0;
    if (chip->notify.power_role)
        set_state(chip, policy_src_transition_default);
    else
        set_state(chip, policy_snk_transition_default);
}


void tcpc_alert(struct fusb30x_chip *chip, int *evt)
{
    uint8 interrupt, interrupta, interruptb;
    uint8 val;
    uint8 mdac = 0;

    regmap_read(FUSB_REG_INTERRUPT, &interrupt, 1);
    regmap_read(FUSB_REG_INTERRUPTA, &interrupta, 1);
    regmap_read(FUSB_REG_INTERRUPTB, &interruptb, 1);
#if 0
    printf ("**************************************\n");
    printf ("\n interrupt=0x%x\n",interrupt);
    printf ("\n interrupta=0x%x\n",interrupta);
    printf ("\n interruptb=0x%x\n",interruptb);
    printf ("**************************************end\n");
#endif
	if (interrupta & INTERRUPTA_TOGDONE) {
		//printf ("interrupta: I_TOGDONE\n");
		*evt |= EVENT_CC;
		regmap_read(FUSB_REG_STATUS1A, &val, 1);
		chip->cc_state = (val >> 3) & 0x07; //判断连接CC1还是CC2(101 CC1 110 CC2)
		if (chip->cc_state == 0x05)
		printf ("fusb connect cc1\n");
		if (chip->cc_state == 0x06)
		printf ("fusb connect cc2\n");

		regmap_update_bits(FUSB_REG_CONTROL2, CONTROL2_TOGGLE, 0);
		val = 0;
		if (chip->cc_state & 0x01)
		{
			val |= SWITCHES0_PDWN1;
		}
		else
		{
			val |= SWITCHES0_PDWN2;
		}
		regmap_update_bits( FUSB_REG_SWITCHES0,
		                SWITCHES0_PU_EN1 | SWITCHES0_PU_EN2 |
		                SWITCHES0_PDWN1 | SWITCHES0_PDWN2|
		                SWITCHES0_MEAS_CC1 | SWITCHES0_MEAS_CC2,
		                val);
		chip->int_toggle = 1;
	}
	if (interrupt & INTERRUPT_VBUSOK) {
		//printf ("interrupt: I_VBUSOK\n");
		val = 0;
		regmap_read(FUSB_REG_MEASURE, &val, 1);
		mdac = val & 0x3f;
		//printf ("Measure Support %d mV\n", mdac*42+42);
		if (chip->notify.is_cc_connected)
		*evt |= EVENT_CC;
		chip->int_vbusok = 1;
	}

    if (interrupt & INTERRUPT_BC_LVL) {
        //printf ("interrupt: I_BC_LVL\n");
        //val = 0;
        //regmap_read(FUSB_REG_STATUS0, &val, 1);
        //printf ("Status0 val=0x%x\n", val);
        if (chip->notify.is_cc_connected)
            *evt |= EVENT_CC;
    }

    if (interrupt & INTERRUPT_CRC_CHK)
    {
        //printf ("interrupt: I_CRC_CHK\n");
    }

    if (interrupt & INTERRUPT_ACTIVITY)
    {
       //printf ("interrupt: I_ACTIVITY\n");
    }

    if (interrupt & INTERRUPT_COMP_CHNG)
    {
        //printf ("interrupt: I_COMP_CHNG\n");
    }

    if (interrupt & INTERRUPT_ALERT) {
        printf ("interrupt: I_ALERT\n");
        val = 0;
        regmap_read(FUSB_REG_STATUS1, &val, 1);
        //printf ("\n tcpc_alert STATUS1 val =0x%x\n",val);
    }


    if (interrupta & INTERRUPTA_TXSENT) {
        printf ("interrupta: TXSENT\n");
        *evt |= EVENT_TX;
//        fusb302_flush_rx_fifo(chip);
        chip->tx_state = tx_success;
    }

    if (interruptb & INTERRUPTB_GCRCSENT)
    {
        printf ("interruptb: GCRCSENT\n");
        *evt |= EVENT_RX;
    }


    if (interrupta & INTERRUPTA_HARDRST) {
        printf ("interrupta: HARDRST\n");
        fusb302_pd_reset(chip);
        pd_execute_hard_reset(chip);
        *evt |= EVENT_REC_RESET;
    }

    if (interrupta & INTERRUPTA_RETRYFAIL) {
        printf ("interrupta: RETRYFAIL\n");
        *evt |= EVENT_TX;
        chip->tx_state = tx_failed;
    }

    if (interrupta & INTERRUPTA_HARDSENT) {
        printf ("interrupta: HARDSENT\n");
        chip->tx_state = tx_success;
        *evt |= EVENT_TX;
    }
}

void mux_alert(struct fusb30x_chip *chip, int *evt)
{
	if (chip->work_continue) {
		*evt |= EVENT_WORK_CONTINUE;
		chip->work_continue = 0;
	}
}


void set_state_unattached(struct fusb30x_chip *chip)
 {
     printf("connection has disconnected\n");
     tcpm_init(chip);
     tcpm_set_rx_enable(chip, 0);
     chip->conn_state = unattached;
     tcpm_set_cc(chip, FUSB_MODE_UFP);
 }

int tcpm_check_vbus(struct fusb30x_chip *chip)
{
    uint8 val;

    /* Read status register */
    regmap_read(FUSB_REG_STATUS0, &val, 1);

    return (val & STATUS0_VBUSOK) ? 1 : 0;
}





void set_mesg(struct fusb30x_chip *chip, int cmd, int is_DMT)
{
    int i;
    struct PD_CAP_INFO *pd_cap_info = &chip->pd_cap_info;

    chip->send_head = ((chip->msg_id & 0x7) << 9) |
                       ((chip->notify.power_role & 0x1) << 8) |
                       (1 << 6) |
                       ((chip->notify.data_role & 0x1) << 5);

    if (is_DMT) {
        switch (cmd) {
        case DMT_SOURCECAPABILITIES:
            chip->send_head |= ((chip->n_caps_used & 0x3) << 12) | (cmd & 0xf);

            for (i = 0; i < chip->n_caps_used; i++) {
                chip->send_load[i] = (pd_cap_info->supply_type << 30) |
                            (pd_cap_info->dual_role_power << 29) |
                            (pd_cap_info->usb_suspend_support << 28) |
                            (pd_cap_info->externally_powered << 27) |
                            (pd_cap_info->usb_communications_cap << 26) |
                            (pd_cap_info->data_role_swap << 25) |
                            (pd_cap_info->peak_current << 20) |
                            (chip->source_power_supply[i] << 10) |
                            (chip->source_max_current[i]);
            }
            break;
        case DMT_REQUEST:
            chip->send_head |= ((1 << 12) | (cmd & 0xf));
            /* send request with FVRDO */
            chip->send_load[0] = (chip->pos_power << 28) |
                        (0 << 27) |
                        (1 << 26) |
                        (0 << 25) |
                        (0 << 24);
            switch (CAP_POWER_TYPE(chip->rec_load[chip->pos_power - 1])) {
                case 0:
                    /* Fixed Supply */
                    //printf ("set_mesg: Fixed Supply\n");
                    chip->send_load[0] |= ((CAP_FPDO_VOLTAGE(chip->rec_load[chip->pos_power - 1]) << 10) & 0x3ff);
                    chip->send_load[0] |= (CAP_FPDO_CURRENT(chip->rec_load[chip->pos_power - 1]) & 0x3ff);
                    break;
                case 1:
                    /* Battery */
                    //printf ("set_mesg: Battery\n");
                    chip->send_load[0] |= ((CAP_VPDO_VOLTAGE(chip->rec_load[chip->pos_power - 1]) << 10) & 0x3ff);
                    chip->send_load[0] |= (CAP_VPDO_CURRENT(chip->rec_load[chip->pos_power - 1]) & 0x3ff);
                    break;
                default:
                    /* not meet battery caps */
                    break;
            }
            break;
        case DMT_SINKCAPABILITIES:
            chip->send_head |= ((1 << 12) | (cmd & 0xf));
            chip->send_load[0] = 0x00019096; //5.0V 1.5A
            break;
        case DMT_VENDERDEFINED:
            break;
        default:
            break;
        }
    } else {
        chip->send_head |= (cmd & 0xf);
    }
}

void set_vdm_mesg(struct fusb30x_chip *chip, int cmd, int type, int mode)
{
    chip->send_head = (chip->msg_id & 0x7) << 9;
    chip->send_head |= (chip->notify.power_role & 0x1) << 8;

    chip->send_head = ((chip->msg_id & 0x7) << 9) |
    ((chip->notify.power_role & 0x1) << 8) |
    (1 << 6) |
    ((chip->notify.data_role & 0x1) << 5) |
    (DMT_VENDERDEFINED & 0xf);

    chip->send_load[0] = (1 << 15) | (0 << 13) | (type << 6) | (cmd);

    switch (cmd) {
        case VDM_DISCOVERY_ID:
        case VDM_DISCOVERY_SVIDS:
        case VDM_ATTENTION:
            chip->send_load[0] |= (0xff00 << 16);
            chip->send_head |= (1 << 12);
            break;
        case VDM_DISCOVERY_MODES:
            chip->send_load[0] |=
            (chip->vdm_svid[chip->val_tmp >> 1] << 16);
            chip->send_head |= (1 << 12);
            break;
        case VDM_ENTER_MODE:
            chip->send_head |= (1 << 12);
            chip->send_load[0] |= (mode << 8) | (0xff01 << 16);
            break;
        case VDM_EXIT_MODE:
            chip->send_head |= (1 << 12);
            chip->send_load[0] |= (0x0f << 8) | (0xff01 << 16);
            break;
        case VDM_DP_STATUS_UPDATE:
            chip->send_head |= (2 << 12);
            chip->send_load[0] |= (1 << 8) | (0xff01 << 16);
            chip->send_load[1] = 5;
            break;
        case VDM_DP_CONFIG:
            chip->send_head |= (2 << 12);
            chip->send_load[0] |= (1 << 8) | (0xff01 << 16);
            chip->send_load[1] = (chip->notify.pin_assignment_def << 8) |
            (1 << 2) | 2;
            break;
        default:
            break;
    }
}

enum tx_state policy_send_hardrst(struct fusb30x_chip *chip, int evt)
{
    switch (chip->tx_state) {
    case 0:
        regmap_update_bits( FUSB_REG_CONTROL3,
                            CONTROL3_SEND_HARDRESET,
                            CONTROL3_SEND_HARDRESET);
        chip->tx_state = tx_busy;
        break;
    default:
        break;
    }
    return chip->tx_state;
}

enum tx_state policy_send_data(struct fusb30x_chip *chip)
{
    uint8 senddata[40];
    int pos = 0;
    uint8 len;

//    fusb302_flush_tx_fifo(chip);
    switch (chip->tx_state) {
        case 0:
            //printf ("policy_send_data\n");
            senddata[pos++] = FUSB_TKN_SYNC1;
            senddata[pos++] = FUSB_TKN_SYNC1;
            senddata[pos++] = FUSB_TKN_SYNC1;
            senddata[pos++] = FUSB_TKN_SYNC2;

            len = PD_HEADER_CNT(chip->send_head) << 2;
            senddata[pos++] = FUSB_TKN_PACKSYM | ((len + 2) & 0x1f);

            senddata[pos++] = chip->send_head & 0xff;
            senddata[pos++] = (chip->send_head >> 8) & 0xff;

            memcpy(&senddata[pos], chip->send_load, len);
            pos += len;

            senddata[pos++] = FUSB_TKN_JAMCRC;
            senddata[pos++] = FUSB_TKN_EOP;
            senddata[pos++] = FUSB_TKN_TXOFF;
            senddata[pos++] = FUSB_TKN_TXON;
            regmap_raw_write(FUSB_REG_FIFO, senddata, pos);
            chip->tx_state = tx_busy;

            break;

        default:
            /* wait Tx result */
            break;
    }

    return chip->tx_state;
}
void process_vdm_msg(struct fusb30x_chip *chip)
{
    uint32 vdm_header = chip->rec_load[0];
    int i;
    uint32 tmp;

    /* can't procee unstructed vdm msg */
    if (!GET_VDMHEAD_STRUCT_TYPE(vdm_header))
    {
        printf ("can't procee unstructed vdm msg\n");
        return;
    }
    switch (GET_VDMHEAD_CMD_TYPE(vdm_header)) {
    case VDM_TYPE_INIT:
        switch (GET_VDMHEAD_CMD(vdm_header)) {
        case VDM_ATTENTION:
            printf("attention, dp_status %x\n", chip->rec_load[1]);
            chip->notify.attention = 1;
            chip->vdm_state = 6;
            break;
        default:
            printf("rec unknown init vdm msg\n");
            break;
        }
        break;
    case VDM_TYPE_ACK:
        switch (GET_VDMHEAD_CMD(vdm_header)) {
        case VDM_DISCOVERY_ID:
            chip->vdm_id = chip->rec_load[1];
            break;
        case VDM_DISCOVERY_SVIDS:
            for (i = 0; i < 6; i++) {
                tmp = (chip->rec_load[i + 1] >> 16) &
                      0x0000ffff;
                if (tmp) {
                    chip->vdm_svid[i * 2] = tmp;
                    chip->vdm_svid_num++;
                } else {
                    break;
                }

                tmp = (chip->rec_load[i + 1] & 0x0000ffff);
                if (tmp) {
                    chip->vdm_svid[i * 2 + 1] = tmp;
                    chip->vdm_svid_num++;
                } else {
                    break;
                }
            }
            break;
        case VDM_DISCOVERY_MODES:
            /* indicate there are some vdo modes */
            if (PD_HEADER_CNT(chip->rec_head) > 1) {
                /*
                 * store mode config,
                 * enter first mode default
                 */
                if (!((chip->rec_load[1] >> 8) & 0x3f)) {
                    chip->val_tmp |= 1;
                    break;
                }
                chip->notify.pin_assignment_support = 0;
                chip->notify.pin_assignment_def = 0;
                chip->notify.pin_assignment_support =
                    (chip->rec_load[1] >> 8) & 0x3f;
                tmp = chip->notify.pin_assignment_support;
                for (i = 0; i < 6; i++) {
                    if (!(tmp & 0x20))
                        tmp = tmp << 1;
                    else
                        break;
                }
                chip->notify.pin_assignment_def = 0x20 >> i;
                chip->val_tmp |= 1;
            }
            break;
        case VDM_ENTER_MODE:
            chip->val_tmp = 1;
            break;
        case VDM_DP_STATUS_UPDATE:
            printf("dp_status 0x%x\n", chip->rec_load[1]);
            chip->val_tmp = 1;
            break;
        case VDM_DP_CONFIG:
            chip->val_tmp = 1;
            printf("DP config successful, pin_assignment 0x%x\n",
                    chip->notify.pin_assignment_def);
            chip->notify.is_enter_mode = 1;
            break;
        default:
            break;
        }
        break;
    case VDM_TYPE_NACK:
            printf("REC NACK for 0x%x\n", GET_VDMHEAD_CMD(vdm_header));
            /* disable vdm */
            chip->vdm_state = 0xff;
        break;
    }
}

int vdm_send_discoveryid(struct fusb30x_chip *chip, int evt)
{
    int tmp;

    switch (chip->vdm_send_state) {
        case 0:
            set_vdm_mesg(chip, VDM_DISCOVERY_ID, VDM_TYPE_INIT, 0);
            chip->vdm_id = 0;
            chip->tx_state = 0;
            chip->vdm_send_state++;
        case 1:
            tmp = policy_send_data(chip);
            if (tmp == tx_success) {
                chip->vdm_send_state++;
            } else if (tmp == tx_failed) {
                printf("VDM_DISCOVERY_ID send failed\n");
                /* disable auto_vdm_machine */
                chip->vdm_state = 0xff;
            }

            if (chip->vdm_send_state != 2)
                break;
        default:
            if (!chip->vdm_id)
                break;
            chip->vdm_send_state = 0;
            return 0;
    }
    return -EINPROGRESS;
}

int vdm_send_discoverysvid(struct fusb30x_chip *chip, int evt)
{
    int tmp;

    switch (chip->vdm_send_state) {
        case 0:
            set_vdm_mesg(chip, VDM_DISCOVERY_SVIDS, VDM_TYPE_INIT, 0);
            memset(chip->vdm_svid, 0, 12);
            chip->vdm_svid_num = 0;
            chip->tx_state = 0;
            chip->vdm_send_state++;
        case 1:
            tmp = policy_send_data(chip);
            if (tmp == tx_success) {
                chip->vdm_send_state++;
            } else if (tmp == tx_failed) {
                printf("VDM_DISCOVERY_SVIDS send failed\n");
                /* disable auto_vdm_machine */
                chip->vdm_state = 0xff;
            }

            if (chip->vdm_send_state != 2)
            break;
        default:
            if (!chip->vdm_svid_num)
                break;
            chip->vdm_send_state = 0;
            return 0;
    }
    return -EINPROGRESS;
}

int vdm_send_discoverymodes(struct fusb30x_chip *chip, int evt)
{
    int tmp;

    if ((chip->val_tmp >> 1) != chip->vdm_svid_num) {
        switch (chip->vdm_send_state) {
            case 0:
                set_vdm_mesg(chip, VDM_DISCOVERY_MODES,
                VDM_TYPE_INIT, 0);
                chip->tx_state = 0;
                chip->vdm_send_state++;
            case 1:
                tmp = policy_send_data(chip);
                if (tmp == tx_success) {
                    chip->vdm_send_state++;
                } else if (tmp == tx_failed) {
                    printf("VDM_DISCOVERY_MODES send failed\n");
                    chip->vdm_state = 0xff;
                }

                if (chip->vdm_send_state != 2)
                    break;
            default:
                if (!(chip->val_tmp & 1))
                    break;
                chip->val_tmp &= 0xfe;
                chip->val_tmp += 2;
                chip->vdm_send_state = 0;
                chip->work_continue = 1;
                break;
        }
    } else {
        chip->val_tmp = 0;
        return 0;
    }

    return -EINPROGRESS;
}

int vdm_send_entermode(struct fusb30x_chip *chip, int evt)
{
    int tmp;

    switch (chip->vdm_send_state) {
        case 0:
            set_vdm_mesg(chip, VDM_ENTER_MODE, VDM_TYPE_INIT, 1);
            chip->tx_state = 0;
            chip->vdm_send_state++;
            chip->notify.is_enter_mode = 0;
        case 1:
            tmp = policy_send_data(chip);
            if (tmp == tx_success) {
                chip->vdm_send_state++;
            } else if (tmp == tx_failed) {
                printf("VDM_ENTER_MODE send failed\n");
                /* disable auto_vdm_machine */
                chip->vdm_state = 0xff;
            }

            if (chip->vdm_send_state != 2)
            break;
        default:
            if (!chip->val_tmp)
                break;
            chip->val_tmp = 0;
            chip->vdm_send_state = 0;
            return 0;
    }
    return -EINPROGRESS;
}

int vdm_send_getdpstatus(struct fusb30x_chip *chip, int evt)
{
    int tmp;

    switch (chip->vdm_send_state) {
        case 0:
            set_vdm_mesg(chip, VDM_DP_STATUS_UPDATE, VDM_TYPE_INIT, 1);
            chip->tx_state = 0;
            chip->vdm_send_state++;
        case 1:
            tmp = policy_send_data(chip);
            if (tmp == tx_success) {
                chip->vdm_send_state++;
            } else if (tmp == tx_failed) {
                printf("VDM_DP_STATUS_UPDATE send failed\n");
                /* disable auto_vdm_machine */
                chip->vdm_state = 0xff;
            }

            if (chip->vdm_send_state != 2)
            break;
        default:
            if (!chip->val_tmp)
                break;
            chip->val_tmp = 0;
            chip->vdm_send_state = 0;
            return 0;
    }
    return -EINPROGRESS;
}

int vdm_send_dpconfig(struct fusb30x_chip *chip, int evt)
{
    int tmp;

    switch (chip->vdm_send_state) {
        case 0:
            set_vdm_mesg(chip, VDM_DP_CONFIG, VDM_TYPE_INIT, 0);
            chip->tx_state = 0;
            chip->vdm_send_state++;
        case 1:
            tmp = policy_send_data(chip);
            if (tmp == tx_success) {
                chip->vdm_send_state++;
            } else if (tmp == tx_failed) {
                printf("vdm_send_dpconfig send failed\n");
                /* disable auto_vdm_machine */
                chip->vdm_state = 0xff;
            }

            if (chip->vdm_send_state != 2)
                break;
        default:
            if (!chip->val_tmp)
                break;
            chip->val_tmp = 0;
            chip->vdm_send_state = 0;
            return 0;
    }
    return -EINPROGRESS;
}
void auto_vdm_machine(struct fusb30x_chip *chip, int evt)
{
    switch (chip->vdm_state) {
        case 0:
            if (vdm_send_discoveryid(chip, evt))
                break;
            chip->vdm_state++;
        /* without break */
        case 1:
            if (vdm_send_discoverysvid(chip, evt))
                break;
            chip->vdm_state++;
        /* without break */
        case 2:
            if (vdm_send_discoverymodes(chip, evt))
                break;
            chip->vdm_state++;
        /* without break */
        case 3:
            if (vdm_send_entermode(chip, evt))
                break;
            chip->vdm_state++;
        /* without break */
        case 4:
            if (vdm_send_dpconfig(chip, evt))
                break;
            chip->vdm_state = 6;
        /* without break */
        case 5:
            if (vdm_send_getdpstatus(chip, evt))
                break;
            chip->vdm_state++;
        /* without break */
        default:
            platform_fusb_notify(chip);
        break;
    }
}



void fusb_state_disabled(struct fusb30x_chip *chip, int evt)
{
    platform_fusb_notify(chip);
}

void fusb_state_unattached(struct fusb30x_chip *chip, int evt)
{
    chip->notify.is_cc_connected = 0;
    if ((evt & EVENT_CC) && chip->cc_state) {
        if (chip->cc_state & 0x04)
            set_state(chip, attach_wait_sink);
        else
            set_state(chip, attach_wait_source);

        tcpm_get_cc(chip, &chip->cc1, &chip->cc2);
        chip->debounce_cnt = 0;
    }
    gevt = 0;
}

void fusb_state_attach_wait_sink(struct fusb30x_chip *chip, int evt)
{
	int cc1, cc2;
	while(1) {
		printf ("chip->debounce_cnt=%d\n",chip->debounce_cnt);
		tcpm_get_cc(chip, &cc1, &cc2);

		if ((chip->cc1 == cc1) && (chip->cc2 == cc2)) {
			chip->debounce_cnt++;
		} else {
			chip->cc1 = cc1;
			chip->cc2 = cc2;
			chip->debounce_cnt = 0;
		}

		if (chip->debounce_cnt > N_DEBOUNCE_CNT) {
			if ((chip->cc1 != chip->cc2) && ((!chip->cc1) || (!chip->cc2))){
				set_state(chip, attached_sink);
			} else {
				set_state_unattached(chip);
			}
			return;
		}
	}
}

void fusb_state_attached_sink(struct fusb30x_chip *chip, int evt)
{
    chip->notify.is_cc_connected = 1;
    if (chip->cc_state & 0x01)
        chip->cc_polarity = 0;
    else
        chip->cc_polarity = 1;

    chip->notify.power_role = 0;
    chip->notify.data_role = 0;
    chip->hardrst_count = 0;
    set_state(chip, policy_snk_startup);
    printf("CC connected in %d as UFP\n", chip->cc_polarity);

}

void fusb_state_snk_startup(struct fusb30x_chip *chip, int evt)
{
    chip->notify.is_pd_connected = 0;
    chip->msg_id = 0;
    chip->vdm_state = 0;
    chip->vdm_substate = 0;
    chip->vdm_send_state = 0;
    chip->val_tmp = 0;
    chip->pos_power = 0;

    memset(chip->partner_cap, 0, sizeof(chip->partner_cap));

    tcpm_set_msg_header(chip);
    tcpm_set_polarity(chip, chip->cc_polarity);
    tcpm_set_rx_enable(chip, 1);
    set_state(chip, policy_snk_discovery);
}

void fusb_state_snk_discovery(struct fusb30x_chip *chip, int evt)
{
    set_state(chip, policy_snk_wait_caps);
}

static int sink_wait_cap_times = 5;
int fusb_state_snk_wait_caps(struct fusb30x_chip *chip, int evt)
{
    if (evt & EVENT_RX) {
	printf("%s,head=%x\n",__FUNCTION__,chip->rec_head);
       if (PD_HEADER_CNT(chip->rec_head) &&
            PD_HEADER_TYPE(chip->rec_head) == DMT_SOURCECAPABILITIES) {
            set_state(chip, policy_snk_evaluate_caps);
	     return 0;
        }
    }
    printf("%s,tims=%d\n",__FUNCTION__,sink_wait_cap_times);
	return -1;
#if 0
    //if source capabilty packets has been recieved, we will fall through to policy_snk_evaluate_caps state
    //to deal with these packets, if not , we don't care about the source capabilities, so we just fall though to
    //state policy_snk_select_cap to send our packets to source device.
    //if we don't do this , the state machine will stall on the state policy_snk_wait_caps forever because we didn't
    //recieve source capability packets.
    if(sink_wait_cap_times-- <= 0){
		sink_wait_cap_times = 5;
		set_state(chip, policy_snk_evaluate_caps);
    }
#endif
}

void fusb_state_snk_evaluate_caps(struct fusb30x_chip *chip, int evt)
{
    uint32 tmp;

    chip->hardrst_count = 0;
    chip->pos_power = 0;

    for (tmp = 0; tmp < PD_HEADER_CNT(chip->rec_head); tmp++) {
//	printf("%s,chip->rec_head=%x,chip->rec_load[%d]=%x\n",__func__,chip->rec_head,tmp,chip->rec_load[tmp]);
        switch (CAP_POWER_TYPE(chip->rec_load[tmp])) {
            case 0:
                /* Fixed Supply */
                if (CAP_FPDO_VOLTAGE(chip->rec_load[tmp]) <= 100)
                    chip->pos_power = tmp + 1;
                break;
            case 1:
                /* Battery */
                if (CAP_VPDO_VOLTAGE(chip->rec_load[tmp]) <= 100)
                    chip->pos_power = tmp + 1;
                break;
            default:
                /* not meet battery caps */
                break;
        }
    }

    if ((!chip->pos_power) || (chip->pos_power > 7)) {
        chip->pos_power = 0;
        set_state(chip, policy_snk_wait_caps);
	 gevt &= ~EVENT_RX;
    } else {
        set_state(chip, policy_snk_select_cap);
	 gevt &= ~EVENT_RX;
    }
}

void fusb_state_snk_select_cap(struct fusb30x_chip *chip, int evt)
{
    uint32 tmp;
    switch (chip->sub_state) {
        case 0:
            set_mesg(chip, DMT_REQUEST, DATAMESSAGE);
            chip->sub_state = 1;
            chip->tx_state = tx_idle;
            /* without break */
        case 1:
            tmp = policy_send_data(chip);
            if (tmp == tx_success) {
                chip->sub_state++;
            } else if (tmp == tx_failed) {
                set_state(chip, policy_snk_discovery);
                break;
            }
            if (!(evt & FLAG_EVENT))
            {
                break;
            }
        default:
            if (evt & EVENT_RX) {
                if (!PD_HEADER_CNT(chip->rec_head)) {
			gevt &= ~EVENT_RX;
                    switch (PD_HEADER_TYPE(chip->rec_head)) {
                        case CMT_ACCEPT:
                            set_state(chip, policy_snk_transition_sink);
                            break;
                        case CMT_WAIT:
                        case CMT_REJECT:
                            if (chip->notify.is_pd_connected) {
                                printf("PD connected as UFP, fetching 5V 1111\n");
                                set_state(chip, policy_snk_ready);
                            } else {
                                set_state(chip, policy_snk_wait_caps);
                                /*
                                * make sure don't send
                                * hard reset to prevent
                                * infinite loop
                                */
                                chip->hardrst_count = N_HARDRESET_COUNT + 1;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
    }
}

void fusb_state_snk_transition_sink(struct fusb30x_chip *chip, int evt)
{
    if (evt & EVENT_RX) {
        if ((!PD_HEADER_CNT(chip->rec_head)) &&
            (PD_HEADER_TYPE(chip->rec_head) == CMT_PS_RDY)) {
            chip->notify.is_pd_connected = 1;
            printf("PD connected as UFP, fetching 5V\n");
            set_state(chip, policy_snk_ready);
	     evt &= ~EVENT_RX;
        } else if ((PD_HEADER_CNT(chip->rec_head)) &&
                    (PD_HEADER_TYPE(chip->rec_head) == DMT_SOURCECAPABILITIES)) {
            evt &= ~EVENT_RX;
            set_state(chip, policy_snk_evaluate_caps);
        }
    }
}

void fusb_state_snk_transition_default(struct fusb30x_chip *chip, int evt)
{
    switch (chip->sub_state) {
        case 0:
            chip->notify.is_pd_connected = 0;
            if (chip->notify.data_role)
                tcpm_set_msg_header(chip);

            chip->sub_state++;
        case 1:
            if (!tcpm_check_vbus(chip)) {
                chip->sub_state++;
            }
            break;
        default:
            if (tcpm_check_vbus(chip)) {
                set_state(chip, policy_snk_startup);
            }
            break;
    }
}

void sink_set_vdm_mesg(struct fusb30x_chip *chip,uint8 cmd)
{
    int i;
    struct PD_CAP_INFO *pd_cap_info = &chip->pd_cap_info;

    chip->send_head = ((chip->msg_id & 0x7) << 9) |
                       ((chip->notify.power_role & 0x1) << 8) |
                       (1 << 6) |
                       ((chip->notify.data_role & 0x1) << 5);

    switch (cmd)
    {
        case VDM_DISCOVERY_ID:
            chip->send_head &= 0x0fff;
            chip->send_head |= ((5 << 12)|(0xf));
            //chip->send_head = 0x544f;
            chip->send_load[0] = 0xff008041;
            chip->send_load[1] = 0x6c002109;
            chip->send_load[2] = 0x0000037c;
            chip->send_load[3] = 0x01000001;
            chip->send_load[4] = 0x00000039;
            break;
        case VDM_DISCOVERY_SVIDS:
            chip->send_head &= 0x0fff;
            chip->send_head |= ((2 << 12)|(0xf));
            //chip->send_head = 0x264f;
            chip->send_load[0] = 0xff008042;
            chip->send_load[1] = 0xff010000;
            break;
        case VDM_DISCOVERY_MODES:
            chip->send_head &= 0x0fff;
            chip->send_head |= ((2 << 12)|(0xf));
            chip->send_load[0] = 0xff018043;
#ifdef	_DP_MIPI_1LANE_
            chip->send_load[1] = 0x00000185;
#endif

#ifdef _DP_MIPI_2LANE_
			chip->send_load[1] = 0x00000285;
#endif

#ifdef _DP_MIPI_4LANE_
			chip->send_load[1] = 0x00000485;
#endif
            break;
        case VDM_ENTER_MODE:
            chip->send_head &= 0x0fff;
            chip->send_head |= ((1 << 12)|(0xf));
            chip->send_load[0] = 0xff018144;
            break;
        case VDM_ATTENTION:
            chip->send_head &= 0x0fff;
            chip->send_head |= ((2 << 12)|(0xf));
            chip->send_load[0] = 0xff018106;
            chip->send_load[1] = 0x0000001a;
            break;
        case VDM_DP_STATUS_UPDATE:
            chip->send_head &= 0x0fff;
            chip->send_head |= ((2 << 12)|(0xf));
            chip->send_load[0] = 0xff018150;
            chip->send_load[1] = 0x0000001a;
            break;
        case VDM_DP_CONFIG:
            chip->send_head &= 0x0fff;
            chip->send_head |= ((1 << 12)|(0xf));
            chip->send_load[0] = 0xff018151;
            break;
        default:
            break;
    }
}


void sink_process_vdm_msg(struct fusb30x_chip *chip)
{
    uint32 vdm_cmd = (chip->rec_load[0] & 0x0000001f);
//	printf("-------sink_process_vdm_msg,rec_load=%x,cmd=%d\n",chip->rec_load[0],vdm_cmd);
    switch (vdm_cmd)
    {
        case VDM_DISCOVERY_ID:
            printf ("VDM_DISCOVERY_ID\n");
            sink_set_vdm_mesg(chip,VDM_DISCOVERY_ID);
            chip->tx_state = tx_idle;
            policy_send_data(chip);

            break;
        case VDM_DISCOVERY_SVIDS:
            printf ("VDM_DISCOVERY_SVIDS\n");
            sink_set_vdm_mesg(chip,VDM_DISCOVERY_SVIDS);
            chip->tx_state = tx_idle;
            policy_send_data(chip);
            break;
        case VDM_DISCOVERY_MODES:
            printf ("VDM_DISCOVERY_MODES\n");
            sink_set_vdm_mesg(chip,VDM_DISCOVERY_MODES);
            chip->tx_state = tx_idle;
            policy_send_data(chip);
            break;
        case VDM_ENTER_MODE:
            printf ("VDM_ENTER_MODE\n");
            sink_set_vdm_mesg(chip,VDM_ENTER_MODE);
            chip->tx_state = tx_idle;
            policy_send_data(chip);
            break;
        case VDM_ATTENTION:
            printf ("VDM_ATTENTION\n");
            sink_set_vdm_mesg(chip,VDM_ATTENTION);
            chip->tx_state = tx_idle;
            policy_send_data(chip);
            break;
        case VDM_DP_STATUS_UPDATE:
            printf ("VDM_DP_STATUS_UPDATE\n");
            sink_set_vdm_mesg(chip,VDM_DP_STATUS_UPDATE);
            chip->tx_state = tx_idle;
            policy_send_data(chip);
            chip->PDok = 1;
            break;
        case VDM_DP_CONFIG:
            printf ("VDM_DP_CONFIG\n");
            sink_set_vdm_mesg(chip,VDM_DP_CONFIG);
            chip->tx_state = tx_idle;
            policy_send_data(chip);
            break;
        default:
            printf ("unknow vmd cmd\n");
            break;
    }
}


void fusb_state_snk_ready(struct fusb30x_chip *chip, int evt)
{
    /* TODO: snk_ready_function would be added later on*/
    //platform_fusb_notify(chip);
    if (evt & EVENT_RX) {
        if ((PD_HEADER_CNT(chip->rec_head)) &&
            (PD_HEADER_TYPE(chip->rec_head) == DMT_VENDERDEFINED)) {
            printf("start process vdm message,gevt=%x\n",gevt);
#ifdef SUPPORT_VDM
	    gevt &= ~EVENT_RX;
	    chip->work_continue = 1;
	fusb302_flush_rx_fifo(chip);
	fusb302_flush_tx_fifo(chip);
            sink_process_vdm_msg(chip);
#else
            chip->PDok = 1;
            return ;
#endif
        }
        else if (PD_HEADER_TYPE(chip->rec_head) == GET_SINK_CAP) //GET_SINK_CAP
        {
            printf ("GET_SINK_CAP \n");
           set_mesg(chip, DMT_SINKCAPABILITIES, DATAMESSAGE);
            chip->tx_state = tx_idle;
            policy_send_data(chip);
        }
    }
}

void fusb_state_snk_send_hardreset(struct fusb30x_chip *chip, int evt)
{
    uint32 tmp;

    switch (chip->sub_state) {
        case 0:
            chip->tx_state = tx_idle;
            chip->sub_state++;
        default:
            tmp = policy_send_hardrst(chip, evt);
            if (tmp == tx_success) {
                chip->hardrst_count++;
                set_state(chip, policy_snk_transition_default);
            } else if (tmp == tx_failed) {
                set_state(chip, error_recovery);
            }
        break;
    }
}

void fusb_state_snk_send_softreset(struct fusb30x_chip *chip, int evt)
{
    uint32 tmp;
    switch (chip->sub_state) {
        case 0:
            set_mesg(chip, CMT_SOFTRESET, CONTROLMESSAGE);
            chip->tx_state = tx_idle;
            chip->sub_state++;
        case 1:
            tmp = policy_send_data(chip);
            if (tmp == tx_success) {
                chip->sub_state++;
            } else if (tmp == tx_failed) {
                /* can't reach here */
                set_state(chip, policy_snk_send_hardrst);
            }

            if (!(evt & FLAG_EVENT))
                break;
        default:
            if (evt & EVENT_RX) {
                if ((!PD_HEADER_CNT(chip->rec_head)) &&
                    (PD_HEADER_TYPE(chip->rec_head) == CMT_ACCEPT))
                    set_state(chip, policy_snk_wait_caps);
            }
            break;
    }
}

void read_int_status(struct fusb30x_chip *chip,int *evt)
{
	int cc1, cc2;
	uint8 val = 0;
	
	tcpc_alert(chip, evt); //interrupt report
	mux_alert(chip, evt);
	if (!*evt){
		return;
	}
	if (chip->notify.is_cc_connected) {
		if (*evt & EVENT_CC) {
			if ((chip->cc_state & 0x04) &&
				(chip->conn_state != policy_snk_transition_default)) {
				if (!tcpm_check_vbus(chip))
				{
				    set_state_unattached(chip);
				}
			} else if (chip->conn_state != policy_src_transition_default) {
				tcpm_get_cc(chip, &cc1, &cc2);
				if (!(chip->cc_state & 0x01))
					cc1 = cc2;
				if (cc1 == TYPEC_CC_VOLT_OPEN)
					set_state_unattached(chip);
			}
			*evt &= ~EVENT_CC;
		}
	}
	if (*evt & EVENT_RX) {
		int ret = 0;
#if 1
		/*
		* if tx state is busy , need to wait for TXSENT interrupt ,which means data is sent successfullly,
		* and we clear the EVENT_RX flag, because TXSENT interrupt will clear the rx fifo, so we may
		* recieve wrong data or data which is send to source device.
		*/			
		if(chip->tx_state == tx_busy){
			printf("%s,tx state is busy,so don't read fifo\n",__FUNCTION__);
			*evt &= ~EVENT_RX;
			chip->work_continue = 1;
			return;
		}
#endif
		ret = tcpm_get_message(chip);
		if ((!PD_HEADER_CNT(chip->rec_head)) &&
						(PD_HEADER_TYPE(chip->rec_head) == CMT_SOFTRESET)) {
			if (chip->notify.power_role)
				set_state(chip, policy_src_send_softrst);
			else
				set_state(chip, policy_snk_send_softrst);
		}
		/*because the state machine will judge the condition if(evt & EVENT_RX),so we can't clear this flag in it,
		*  intead of it, we will clear EVENT_RX flag when message has been processed.
		*/

//		if(ret)
//			*evt &= ~EVENT_RX;
	}
	if (*evt & EVENT_TX) {
		if (chip->tx_state == tx_success){
			*evt &= ~EVENT_TX;
			chip->msg_id++;
		}
	}
}

void state_machine_typec(struct fusb30x_chip *chip)
{
	int ret =0;
	
	read_int_status(chip,&gevt);
	if(!gevt)
		return;
	switch (chip->conn_state) {
		case disabled:
			fusb_state_disabled(chip, gevt);
			break;
		case error_recovery:
			set_state_unattached(chip);
			break;
		case unattached:
			//printf ("--------------------------------------------1\n");
			fusb_state_unattached(chip, gevt);
			//printf ("--------------------------------------------2\n");
			break;
		case attach_wait_sink:
			fusb_state_attach_wait_sink(chip, gevt);
			break;
		case attached_sink:
			fusb_state_attached_sink(chip, gevt);
			break;

		/* UFP */
		case policy_snk_startup:
			fusb_state_snk_startup(chip, gevt);
			break;
		case policy_snk_discovery:
			fusb_state_snk_discovery(chip, gevt);
			break;
		case policy_snk_wait_caps:
			ret = fusb_state_snk_wait_caps(chip, gevt);
			//if wait caps successfully, fall through to policy_snk_evaluate_caps.
			if(ret < 0)
				break;
		case policy_snk_evaluate_caps:
			fusb_state_snk_evaluate_caps(chip, gevt);
		/* without break */
		case policy_snk_select_cap:
			fusb_state_snk_select_cap(chip, gevt);
			break;
		case policy_snk_transition_sink:
			fusb_state_snk_transition_sink(chip, gevt);
			break;
		case policy_snk_transition_default:
			fusb_state_snk_transition_default(chip, gevt);
			break;
		case policy_snk_ready:
			fusb_state_snk_ready(chip, gevt);
			break;
		case policy_snk_send_hardrst:
			fusb_state_snk_send_hardreset(chip, gevt);
			break;
		case policy_snk_send_softrst:
			fusb_state_snk_send_softreset(chip, gevt);
			break;

		default:
			break;
	}
	if(chip->work_continue == 0)
		gevt &= ~ EVENT_WORK_CONTINUE;
}

/*
	interrupt only enabled before toogle and vbus is ok,
	so we only process toogole and vbus.
*/
void cc_interrupt_handler_1()
{
	struct fusb30x_chip *chip = &gchip;

	Gpio_DisableInt(GPIOPortB_Pin4);
	work_continue++;
		
}

void cc_interrupt_handler()
{
	struct fusb30x_chip *chip = &gchip;
	uint8 interrupt, interrupta, interruptb;
	uint8 val;
	uint8 mdac = 0;

	Gpio_DisableInt(GPIOPortB_Pin4);
	regmap_read(FUSB_REG_INTERRUPT, &interrupt, 1);
	regmap_read(FUSB_REG_INTERRUPTA, &interrupta, 1);
	regmap_read(FUSB_REG_INTERRUPTB, &interruptb, 1);
	
	if (interrupta & INTERRUPTA_TOGDONE) {
		printf ("interrupta: I_TOGDONE\n");
		gevt |= EVENT_CC;
		regmap_read(FUSB_REG_STATUS1A, &val, 1);
		chip->cc_state = (val >> 3) & 0x07; //判断连接CC1还是CC2(101 CC1 110 CC2)
		if (chip->cc_state == 0x05)
		printf ("fusb connect cc1\n");
		if (chip->cc_state == 0x06)
		printf ("fusb connect cc2\n");

		regmap_update_bits(FUSB_REG_CONTROL2, CONTROL2_TOGGLE, 0);
		val = 0;
		if (chip->cc_state & 0x01)
		{
			val |= SWITCHES0_PDWN1;
		}
		else
		{
			val |= SWITCHES0_PDWN2;
		}
		regmap_update_bits( FUSB_REG_SWITCHES0,
		                SWITCHES0_PU_EN1 | SWITCHES0_PU_EN2 |
		                SWITCHES0_PDWN1 | SWITCHES0_PDWN2|
		                SWITCHES0_MEAS_CC1 | SWITCHES0_MEAS_CC2,
		                val);
		chip->int_toggle = 1;
	}
	if (interrupt & INTERRUPT_VBUSOK) {
		printf ("interrupt: I_VBUSOK\n");
		val = 0;
		regmap_read(FUSB_REG_MEASURE, &val, 1);
		mdac = val & 0x3f;
		//printf ("Measure Support %d mV\n", mdac*42+42);
		if (chip->notify.is_cc_connected)
		gevt |= EVENT_CC;
		chip->int_vbusok = 1;
	}

	if (interrupt & INTERRUPT_BC_LVL) {
		//printf ("interrupt: I_BC_LVL\n");
		val = 0;
		regmap_read(FUSB_REG_STATUS0, &val, 1);
		//printf ("Status0 val=0x%x\n", val);
		if (chip->notify.is_cc_connected)
		gevt |= EVENT_CC;
	}
	if (interruptb & INTERRUPTB_GCRCSENT)
	{
		printf ("WARNING!! interruptb GCRCSENT is happened duaring hardware connecting\n");
	}
}
/***********************************************************
*Function: fusb302_init_gpio
*
*
*
***********************************************************/
int fusb302_init_gpio ()
{
    //CHG_CC_INT_L
    GpioMuxSet(GPIOPortB_Pin4,IOMUX_GPIOB4_IO);
    Gpio_SetPinDirection(GPIOPortB_Pin4,GPIO_IN);
}

int fusb302_init()
{
	struct fusb30x_chip* chip = &gchip;
	struct PD_CAP_INFO *pd_cap_info;
	uint32 sysclk_start_counts = 0;
	uint32 sysclk_check_counts = 0;
	static int wait_caps_times = 0;

fusb302_init_start:
	memset(chip,0,sizeof(struct fusb30x_chip));
	chip->int_toggle = 0;
	chip->int_vbusok = 0;
	chip->PDok = 0;
	chip->work_continue = 0;
	printf ("Fusb302 Init\n");
	fusb302_init_gpio();

	tcpm_init(chip);
        fusb302_flush_rx_fifo(chip);
	fusb302_flush_tx_fifo(chip);
	tcpm_set_rx_enable(chip, 0);
	regmap_update_bits(FUSB_REG_CONTROL0, CONTROL0_INT_MASK | CONTROL0_HOST_CUR, ~CONTROL0_INT_MASK | CONTROL0_HOST_CUR);
	tcpm_set_cc(chip, FUSB_MODE_UFP);

fusb302_init_start2:
	/*
	* these two variable should be 1 if support DRP,
	* but now we do not support swap,
	* it will be blanked in future
	*/
	pd_cap_info = &chip->pd_cap_info;
	pd_cap_info->dual_role_power = 0;
	pd_cap_info->data_role_swap = 0;

	pd_cap_info->externally_powered = 1;
	pd_cap_info->usb_suspend_support = 0;
	pd_cap_info->usb_communications_cap = 0;
	pd_cap_info->supply_type = 0;
	pd_cap_info->peak_current = 0;
	wait_caps_times = 0;

	chip->conn_state = unattached;
	chip->n_caps_used = 1;
	chip->source_power_supply[0] = 0x64; //PD 协议 5V
	chip->source_max_current[0] = 0x96;  //1.5A
	
	/* Interrupts Enable */
#ifdef _FUSB302_
	Gpio_SetIntMode(GPIOPortB_Pin4, IntrTypeLowLevel);//IntrTypeLowLevel
	GpioIsrRegister(GPIOPortB_Pin4, cc_interrupt_handler);
	Gpio_EnableInt(GPIOPortB_Pin4);
#endif

	printf ("Fusb302 start up sink\n");
	sysclk_start_counts = GetSysTick();
	while(1) {
		if(chip->int_toggle != 1 || chip->int_toggle != 1){
			sysclk_check_counts = GetSysTick();
			if (((sysclk_check_counts - sysclk_start_counts) > 900) && (chip->int_toggle == 0 || chip->int_vbusok == 0))    //3s延时
			{
				printf ("Fusb302 check retry..\n");
				Gpio_DisableInt(GPIOPortB_Pin4);
				goto fusb302_init_start;
			}

			Gpio_EnableInt(GPIOPortB_Pin4);
			continue;
		}

//		read_int_status(chip,&gevt);
		switch (chip->conn_state) {
			case unattached:
				fusb_state_unattached(chip,gevt);
				//break;
			case attach_wait_sink:
				fusb_state_attach_wait_sink(chip,gevt);
				//break;
			case attached_sink:
				fusb_state_attached_sink(chip,gevt);
				//break;
			case policy_snk_startup:
				fusb_state_snk_startup(chip,gevt);
				//break;
			case policy_snk_discovery:
				fusb_state_snk_discovery(chip, gevt);
				printf("Fusb302 starting to wait capabilities\n");
				break;
			default:
				printf("illegle state!!\n");
				break;
		}
		if(chip->conn_state == policy_snk_wait_caps){
			//if sink is started up already, we only care about EVENT_RX and EVENT_TX
			gevt &= EVENT_RX |EVENT_TX;
//			regmap_update_bits( FUSB_REG_MASK,MASK_M_VBUSOK,~MASK_M_VBUSOK);
			break;
		}
	}

	GpioIsrRegister(GPIOPortB_Pin4, cc_interrupt_handler_1);
	Gpio_EnableInt(GPIOPortB_Pin4);

	printf("Fusb302 start state machine step 2\n");
	while (1) {//start UFP statemachine
		if(work_continue){
			state_machine_typec(chip);
			work_continue -- ;
		}
		if(chip->conn_state == policy_snk_wait_caps){
			wait_caps_times ++;
			printf("start wait for capabilities for %d times\n",wait_caps_times);
			if(wait_caps_times > 40){
				gevt = 0;
				work_continue = 0;
				Gpio_DisableInt(GPIOPortB_Pin4);
				regmap_update_bits( FUSB_REG_CONTROL2,
				                    CONTROL2_MODE | CONTROL2_TOGGLE,
				                    (1 << 2) | CONTROL2_TOGGLE);

				regmap_update_bits( FUSB_REG_CONTROL2,
				                    CONTROL2_TOG_RD_ONLY,
				                    CONTROL2_TOG_RD_ONLY);
				DelayMs(3000);
				goto fusb302_init_start2;
			}
		}
			
		if(chip->PDok != 1){
			Gpio_EnableInt(GPIOPortB_Pin4);
		}else {
			Gpio_DisableInt(GPIOPortB_Pin4);
			printf ("fusb302 connect\n");
			break;
		}
		if (chip->notify.is_cc_connected != 1)
		{
			Gpio_DisableInt(GPIOPortB_Pin4);
			printf ("Fusb302 disconnect\n");
			break;
		}
		else
		{
			//printf ("Fusb302 connect\n");
		}
	}
	
	Gpio_DisableInt(GPIOPortB_Pin4);

	return 0;

}



