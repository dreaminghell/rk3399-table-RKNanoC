/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name：   I2c.c
*
* Description:  C program template
*
* History:      <author>          <time>        <version>
*             yangwenjie      2008-11-20         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_I2C_

#include "DriverInclude.h"

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : I2CStart(void)
  Author        : yangwenjie
  Description   : I2C start

  Input         : null

  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void I2CStart(void)
{
    // Set start to LCMR
    I2cReg->I2C_LCMR |= (I2C_LCMR_RESUME | I2C_LCMR_START);
}

/*
--------------------------------------------------------------------------------
  Function name : I2CStop(void)
  Author        : yangwenjie
  Description   : I2C stop

  Input         : null

  Return        : null

  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void I2CStop(void)
{
    int timeout;

    timeout = 200000;   // timeout must long enogh,that will suit some slow suitation.

    I2cReg->I2C_LCMR |=( I2C_LCMR_RESUME | I2C_LCMR_STOP);

    while (((I2cReg->I2C_LCMR & I2C_LCMR_STOP) != 0) && (timeout > 0))
    {
        Delay10cyc(1);
        timeout--;
    }

    I2cReg->I2C_IER = 0x0;

    I2cReg->I2C_ISR = 0x0;

}

/*
--------------------------------------------------------------------------------
  Function name : I2CSendData(UINT8 Data, BOOL StartBit)
  Author        : yangwenjie
  Description   :

  Input         : Data -- data will send(UINT8)

  Return        : 0 -- success
                  non 0 -- failure

  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
int32 I2CSendData(UINT8 Data, BOOL StartBit)
{
    int intstatus;
    int ackcorestatus;
    int timeout;  // 如果I2C设备没有应答也需要超时退出

    timeout = 200000;   // timeout要足够长,以适应个别I2C应答较慢的情况

    I2cReg->I2C_CONR &= ~(I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MTX;
    // Set MTXR

    I2cReg->I2C_MTXR = Data;
    if (StartBit)

        I2cReg->I2C_LCMR = (I2C_LCMR_RESUME | I2C_LCMR_START);
    else

        I2cReg->I2C_LCMR = I2C_LCMR_RESUME ;

    // ACK 释放SDA线,等待从设备应答
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    // waiting ACK
    do
    {
        intstatus = I2cReg->I2C_ISR;
        // If  Arbitration Lost, will stop and return
        if ((intstatus & I2C_INT_AL) != 0)
        {
            // Clear INT_AL status
           I2cReg->I2C_ISR &= ~( I2C_INT_AL);
            // stop
            I2CStop();

            return ERROR;
        }
        Delay10cyc(1);

        ackcorestatus = I2cReg->I2C_LSR;
        timeout--;
    }
    while ((((ackcorestatus & I2C_LSR_NACK) != 0) || ((intstatus& I2C_INT_MACK) == 0)) && (timeout > 0));

    // Clear INT_MACK status

    I2cReg->I2C_ISR &= ~(I2C_INT_MACK);

    if (timeout)
    {
        return OK;
    }
    else
    {
        return TIMEOUT;
    }
}


/*
--------------------------------------------------------------------------------
  Function name : I2CReadData(UINT8 *Data)
  Author        : yangwenjie
  Description   :

  Input         : Data -- to put the read data.

  Return        :

  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
int32 I2CReadData(UINT8 *Data)
{
    int intstatus;
    int timeout;  // 如果I2C设备没有应答也需要超时退出

    timeout = 200000;   // timeout要足够长,以适应个别I2C应答较慢的情况

    I2cReg->I2C_LCMR |= I2C_LCMR_RESUME;
    // waiting ACK
    do
    {

        intstatus = I2cReg->I2C_ISR;
        // Clear INT_MACK status

        if ((intstatus & I2C_INT_AL) != 0)
        {
            // Clear INT_AL status

            I2cReg->I2C_ISR &= ~(I2C_INT_AL);
            // stop
            I2CStop();

            return ERROR;
        }
        Delay10cyc(1);
        timeout--;
    }
    while (((intstatus & I2C_INT_MACKP) == 0) && (timeout > 0));


    *Data = (UINT8)(I2cReg->I2C_MRXR);

    // Clear INT_MACKP status

    I2cReg->I2C_ISR &= ~(I2C_INT_MACKP);

    return OK;
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : I2CSetSpeed(UINT16 speed)
  Author        : yangwenjie
  Description   :

  Input         : speed -- unit is k,for example 400 is mean 400k

  Return        :TRUE -- success
                 FALSE -- failure

  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2CSetSpeed(UINT16 speed)
{
    UINT32 exp;
    UINT32 rem;
    UINT32 scalespeed;
    UINT32 freqbase;

    /*
    SCL Divisor = (I2CCDVR[5:3] + 1) x 2^((I2CCDVR[2:0] + 1)
    SCL = PCLK/ 5*SCLK Divisor
    rem = I2CCDVR[5:3]
    exp = I2CCDVR[2:0]
    */

	freqbase   = (GetPclkFreq()/**1000*/) / 5;//by YWJ 04.19

    scalespeed = speed << 3;      // speed以K为单位   (rem+1)<=8 ,先以8来确定exp

    for (exp = 0;exp < 7;exp++)
    {
        scalespeed = scalespeed << 1;
        if (scalespeed > freqbase)
        {
            break;
        }
    }

    rem = (freqbase) / speed / (1 << (exp + 1)) - 1;

    I2cReg->I2C_OPR &= ~(I2C_OPR_DIV_SPEED_MASK);
    I2cReg->I2C_OPR |= ((rem << 3) | exp);
    return TRUE;
}

/*
--------------------------------------------------------------------------------
  Function name : I2C_Init(UINT8 SlaveAddress, UINT16 nKHz)
  Author        : yangwenjie
  Description   :

  Input         : SlaveAddress :I2C device address
                  nKHz         :I2C max speed

  Return        :TRUE -- ok
                 FALSE -- fail

  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2C_Init(UINT8 SlaveAddress, UINT16 nKHz)
{
    UserIsrDisable();

    ScuClockGateCtr(CLOCK_GATE_I2C, 1);

    GpioMuxSet(GPIOPortC_Pin7,IOMUX_GPIOC7_I2C_SCL);
    GpioMuxSet(GPIOPortD_Pin0,IOMUX_GPIOD0_I2C_SDA);

    I2cReg->I2C_OPR &= ~(I2C_OPR_RESET);
    I2cReg->I2C_IER = 0x0;

    I2C_slaveaddr = SlaveAddress & 0xfe;

    if (I2C_slaveaddr == (0x55 << 1))
    {
        //IoMuxSetI2C(I2C_Internal);
    }
    else
    {
        //IoMuxSetI2C(I2C_External);
    }

    if (I2C_oldspeed != nKHz)
    {
        I2cReg->I2C_OPR &= ~( I2C_OPR_ENABLE);
        I2CSetSpeed(nKHz);
        I2C_oldspeed = nKHz;
    }
    else
    {
        //printf ("\n--2:I2C_oldspeed=%d nKHz=%d\n",I2C_oldspeed,nKHz);
    }

    I2cReg->I2C_OPR |= I2C_OPR_ENABLE;

    return TRUE;

}

/*
--------------------------------------------------------------------------------
  Function name : I2C_Write(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode)
  Author        : yangwenjie
  Description   :

  Input         : RegAddr -- I2C device address.
                  *pData -- data pointer.
                  size -- 一write size
                  mode -- NormalMode/DirectMode

  Return        :TRUE --ok
                 FALSE -- fail

  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2C_Write(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode)
{
    BOOL ret = TRUE;

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x0;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
    {
        //printf("I2C_Write: Send slaveaddr(%d) Error!\n",I2C_slaveaddr);
        ret = FALSE;
        goto error;
    }

    if (mode == NormalMode)
    {
        if (I2CSendData(RegAddr, FALSE) != OK)
        {
            //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
            ret = FALSE;
            goto error;
        }
    }
    // Write data
    while (size)
    {
        if (I2CSendData(*pData, FALSE) != OK)
        {
            //printf("I2C_Write: Send Data(%d) Error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        pData++;
        size --;
    }

error:
    // Set CONR ACK    释放SDA总线
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    I2CStop();

    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name : I2C_Read(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode)
  Author        : yangwenjie
  Description   :

  Input         : RegAddr
                  size -- read size
                  mode -- NormalMode/DirectMode

  Return        :TRUE -- ok
                 FALSE --fail

  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2C_Read(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode)
{
    BOOL ret = TRUE;

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x00;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if ((mode == NormalMode)||(mode ==RDA_5820Mode)) // 为了以后扩展
    {
        if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
        {
            //printf("I2C_Read: Send slaveaddr(%d) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }

        if (I2CSendData(RegAddr, FALSE) != OK)
        {
            //printf("I2C_Read: Send cmd(%d) error!\n",RegAddr);
            ret = FALSE;
            goto error;
        }

        if(mode != RDA_5820Mode)
        {
            I2CStop(); // I2C_CONR = 0x1C , I2C_CONR = 0xC
        }

        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            //printf("I2C_Read: Send read slaveaddr(%d) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }
    else if (mode == DirectMode) // FM5767
    {
        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            //printf("I2C_Read: Send read slaveaddr(%d) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }

    // 设置为主设备接收
    // MaskRegBits32(&pI2CReg->I2C_CONR, I2C_CONR_MASTER_MASK, I2C_CONR_MRX);
    I2cReg->I2C_CONR &= ~( I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MRX;
    while (size)
    {
        if (OK != I2CReadData(pData))
        {
            //printf("I2C_Read: read data(%d) error!\n",*pData);
            ret = FALSE;
            goto error;
        }

        //  the last read (size = 1)is NACK , other is ACK
        if (1 == size)
            //SetRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR |= I2C_CON_NACK;
        else
            //ClrRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
        pData++;

        size --;
    }

error:

    I2CStop();

    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name : I2C_Deinit(void)
  Author        : yangwenjie
  Description   :
  History:     <author>         <time>         <version>
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2C_Deinit(void)
{
    //IoMuxSetI2C(I2C_CE);

    I2cReg->I2C_OPR &= ~(I2C_OPR_ENABLE);
    I2cReg->I2C_OPR |= I2C_OPR_RESET;

    UserIsrEnable(0);

    GpioMuxSet(GPIOPortC_Pin7,IOMUX_GPIOC7_FLASH_CSN2);
    GpioMuxSet(GPIOPortD_Pin0,IOMUX_GPIOD0_FLASH_CSN3);

    ScuClockGateCtr(CLOCK_GATE_I2C, 0);

    return TRUE;
}
/*
_ATTR_DRIVERLIB_CODE_
int32 I2CReadData_Tc(UINT8 *Data)
{
    int intstatus;
    int timeout;  // 如果I2C设备没有应答也需要超时退出

    timeout = 200000;   // timeout要足够长,以适应个别I2C应答较慢的情况

    I2cReg->I2C_LCMR |= I2C_LCMR_RESUME;
    // waiting ACK
    do
    {

        intstatus = I2cReg->I2C_ISR;
        // Clear INT_MACK status

        if ((intstatus & I2C_INT_AL) != 0)
        {
            // Clear INT_AL status

            I2cReg->I2C_ISR &= ~(I2C_INT_AL);
            // stop
            I2CStop();

            return ERROR;
        }
        Delay10cyc(1);
        timeout--;
    }
    while (((intstatus & I2C_INT_MACKP) == 0) && (timeout > 0));


    *Data = (UINT8)(I2cReg->I2C_MRXR);

    // Clear INT_MACKP status

    I2cReg->I2C_ISR &= ~(I2C_INT_MACKP);

    return OK;
}


_ATTR_DRIVER_CODE_
int32 I2CSendData_Tc(UINT8 Data, BOOL StartBit)
{
    int intstatus;
    int ackcorestatus;
    int timeout;  // 如果I2C设备没有应答也需要超时退出

    timeout = 200000;   // timeout要足够长,以适应个别I2C应答较慢的情况

    I2cReg->I2C_CONR &= ~(I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MTX;
    // Set MTXR

    I2cReg->I2C_MTXR = Data;
    if (StartBit)

        I2cReg->I2C_LCMR = (I2C_LCMR_RESUME | I2C_LCMR_START);
    else

        I2cReg->I2C_LCMR = I2C_LCMR_RESUME ;

    // ACK 释放SDA线,等待从设备应答
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    // waiting ACK
    do
    {
        intstatus = I2cReg->I2C_ISR;
        // If  Arbitration Lost, will stop and return
        if ((intstatus & I2C_INT_AL) != 0)
        {
            // Clear INT_AL status
            I2cReg->I2C_ISR &= ~( I2C_INT_AL);
            // stop
            I2CStop();

            return ERROR;
        }
        Delay10cyc(1);

        ackcorestatus = I2cReg->I2C_LSR;
        timeout--;
    }
    while ((((ackcorestatus & I2C_LSR_NACK) != 0) || ((intstatus& I2C_INT_MACK) == 0)) && (timeout > 0));

    // Clear INT_MACK status

    I2cReg->I2C_ISR &= ~(I2C_INT_MACK);

    if (timeout)
    {
        return OK;
    }
    else
    {
        return TIMEOUT;
    }
}
*/

_ATTR_DRIVER_CODE_
BOOL i2c1_slaveaddress_init(uint8 address)
{
    I2C_slaveaddr_raw = address;

    return 1;
}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg16_read8(uint8 slave_addr, uint16 address, uint8 *data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 1;
    uint8 addr_size = 2;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;
#if 1
    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x00;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (mode == NormalMode) // 为了以后扩展
    {
        if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
        {
            printf("I2C_Write: Send slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }

        //Rigister address
        buf = (address >> 8); //地址高8位
        //buf = (address&0x00ff ); //寄存器地址低八位
        while(addr_size)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                printf ("Read error 2\n");
                goto error;
            }
            addr_size--;
            buf = (address&0x00ff ); //寄存器地址低八位
            //buf = (address >> 8); //地址高8位
        }

        if(mode != RDA_5820Mode)
        {
            I2CStop(); // I2C_CONR = 0x1C , I2C_CONR = 0xC
            //I2cReg->I2C_CONR |= I2C_CON_NACK;

            I2cReg->I2C_OPR |= I2C_OPR_ENABLE;
        }

        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            printf("I2C_Read: Send read slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }

    // 设置为主设备接收
    // MaskRegBits32(&pI2CReg->I2C_CONR, I2C_CONR_MASTER_MASK, I2C_CONR_MRX);
    I2cReg->I2C_CONR &= ~( I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MRX;
    while (data_size)
    {
        if (OK != I2CReadData(data))
        {
            //printf("I2C_Read: read data(%d) error!\n",*pData);
            ret = FALSE;
            printf ("Read error 3\n");
            goto error;
        }

        //  the last read (size = 1)is NACK , other is ACK
        if (1 == data_size)
            //SetRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR |= I2C_CON_NACK;
        else
            //ClrRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
        data++;

        data_size --;
    }

error:

    I2CStop();

    I2C_Deinit();

    return ret;
#endif
}


_ATTR_DRIVER_CODE_
BOOL i2c1_reg16_read16(uint8 slave_addr, uint16 address, uint8 *data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 2;
    uint8 addr_size = 2;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;
#if 1
    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x00;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (mode == NormalMode) // 为了以后扩展
    {
        if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
        {
            printf("I2C_Write: Send slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }

        //Rigister address
        buf = (address >> 8); //地址高8位
        //buf = (address&0x00ff ); //寄存器地址低八位
        while(addr_size)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                printf ("Read error 2\n");
                goto error;
            }
            addr_size--;
            buf = (address&0x00ff ); //寄存器地址低八位
            //buf = (address >> 8); //地址高8位
        }

        if(mode != RDA_5820Mode)
        {
            I2CStop(); // I2C_CONR = 0x1C , I2C_CONR = 0xC
            //I2cReg->I2C_CONR |= I2C_CON_NACK;

            I2cReg->I2C_OPR |= I2C_OPR_ENABLE;
        }

        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            printf("I2C_Read: Send read slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }

    // 设置为主设备接收
    // MaskRegBits32(&pI2CReg->I2C_CONR, I2C_CONR_MASTER_MASK, I2C_CONR_MRX);
    I2cReg->I2C_CONR &= ~( I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MRX;
    while (data_size)
    {
        if (OK != I2CReadData(data))
        {
            //printf("I2C_Read: read data(%d) error!\n",*pData);
            ret = FALSE;
            printf ("Read error 3\n");
            goto error;
        }

        //  the last read (size = 1)is NACK , other is ACK
        if (1 == data_size)
            //SetRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR |= I2C_CON_NACK;
        else
            //ClrRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
        data++;

        data_size --;
    }

error:

    I2CStop();

    I2C_Deinit();

    return ret;
#endif
}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg16_read32(uint8 slave_addr, uint16 address, uint8 *data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 4;
    uint8 addr_size = 2;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;

    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x00;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (mode == NormalMode) // 为了以后扩展
    {
        if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
        {
            printf("I2C_Write: Send slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }

        //Rigister address
        buf = (address >> 8); //地址高8位
        //buf = (address&0x00ff ); //寄存器地址低八位
        while(addr_size)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                printf ("Read error 2\n");
                goto error;
            }
            addr_size--;
            buf = (address&0x00ff ); //寄存器地址低八位
            //buf = (address >> 8); //地址高8位
        }

        if(mode != RDA_5820Mode)
        {
            I2CStop(); // I2C_CONR = 0x1C , I2C_CONR = 0xC
            //I2cReg->I2C_CONR |= I2C_CON_NACK;

            I2cReg->I2C_OPR |= I2C_OPR_ENABLE;
        }

        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            printf("I2C_Read: Send read slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }

    // 设置为主设备接收
    // MaskRegBits32(&pI2CReg->I2C_CONR, I2C_CONR_MASTER_MASK, I2C_CONR_MRX);
    I2cReg->I2C_CONR &= ~( I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MRX;
    while (data_size)
    {
        if (OK != I2CReadData(data))
        {
            //printf("I2C_Read: read data(%d) error!\n",*pData);
            ret = FALSE;
            printf ("Read error 3\n");
            goto error;
        }

        //  the last read (size = 1)is NACK , other is ACK
        if (1 == data_size)
            //SetRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR |= I2C_CON_NACK;
        else
            //ClrRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
        data++;

        data_size --;
    }

error:

    I2CStop();

    I2C_Deinit();

    return ret;

}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg16_write8(uint8 slave_addr, uint16 address, uint8 data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 1;
    uint8 addr_size = 2;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;

    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x0;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
    {
        printf("I2C_Write: Send slaveaddr(%x) Error!\n",I2C_slaveaddr);
        ret = FALSE;
        goto error;
    }
    //Rigister address
    buf = (address >> 8); //地址高8位
    //buf = (address&0x00ff ); //寄存器地址低八位
    while(addr_size)
    {
        if (mode == NormalMode)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                goto error;
            }
        }
        addr_size--;
        buf = (address&0x00ff ); //寄存器地址低八位
        //buf = (address >> 8); //地址高8位
    }

    // Write data
    //buf = (data >> 8); //传输数据高8位
    buf = (data&0x00ff); //传输数据低8位
    while(data_size)
    {
        if (I2CSendData(buf, FALSE) != OK)
        {
            //printf("I2C_Write: Send Data(%d) Error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        data_size--;
        //buf = (data&0x00ff); //传输数据低8位
        buf = (data >> 8); //传输数据高8位
    }

error:
    // Set CONR ACK    释放SDA总线
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    I2CStop();

    I2C_Deinit();

    return ret;
}


_ATTR_DRIVER_CODE_
BOOL i2c1_reg16_write16(uint8 slave_addr, uint16 address, uint16 data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 2;
    uint8 addr_size = 2;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;

    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x0;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
    {
        printf("I2C_Write: Send slaveaddr(%x) Error!\n",I2C_slaveaddr);
        ret = FALSE;
        goto error;
    }
    //Rigister address
    buf = (address >> 8); //地址高8位
    //buf = (address&0x00ff ); //寄存器地址低八位
    while(addr_size)
    {
        if (mode == NormalMode)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                goto error;
            }
        }
        addr_size--;
        buf = (address&0x00ff ); //寄存器地址低八位
        //buf = (address >> 8); //地址高8位
    }

    // Write data
    //buf = (data >> 8); //传输数据高8位
    buf = (data&0x00ff); //传输数据低8位
    while(data_size)
    {
        if (I2CSendData(buf, FALSE) != OK)
        {
            //printf("I2C_Write: Send Data(%d) Error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        data_size--;
        //buf = (data&0x00ff); //传输数据低8位
        buf = (data >> 8); //传输数据高8位
    }

error:
    // Set CONR ACK    释放SDA总线
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    I2CStop();

    I2C_Deinit();

    return ret;
}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg16_write32(uint8 slave_addr, uint16 address, uint32 data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 4;
    uint8 addr_size = 2;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;

    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x0;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
    {
        printf("I2C_Write: Send slaveaddr(%x) Error!\n",I2C_slaveaddr);
        ret = FALSE;
        goto error;
    }

    //Rigister address
    buf = (address >> 8); //地址高8位
    //buf = (address&0x00ff); //寄存器地址低八位
    while(addr_size)
    {
        if (mode == NormalMode)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                goto error;
            }
        }
        addr_size--;
        buf = (address&0x00ff); //寄存器地址低八位
        //buf = (address >> 8); //地址高8位
    }

    // Write data
    //buf = (data >> 24); //传输数据高8位
    buf = (data&0x000000ff ); //传输数据低8位
    while(data_size)
    {
        if (I2CSendData(buf, FALSE) != OK)
        {
            //printf("I2C_Write: Send Data(%d) Error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        data_size--;
        if (data_size == 3)
        {
            //buf = ((data>>16)&0x000000ff ); //传输数据次高8位
            buf = ((data>>8)&0x000000ff ); //传输数据次低8位
        }
        if (data_size == 2)
        {
            //buf = ((data>>8)&0x000000ff ); //传输数据次低8位
            buf = ((data>>16)&0x000000ff ); //传输数据次高8位
        }
        if (data_size == 1)
        {
            //buf = (data&0x000000ff ); //传输数据低8位
            buf = (data >> 24); //传输数据高8位
        }
    }

error:
    // Set CONR ACK    释放SDA总线
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    I2CStop();

    I2C_Deinit();

    return ret;
}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg8_read8(uint8 slave_addr, uint8 address, uint8 *data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 1;
    uint8 addr_size = 1;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;
#if 1
    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x00;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (mode == NormalMode) // 为了以后扩展
    {
        if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
        {
            printf("I2C_Write: Send slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }

        //Rigister address
        buf = address;
        while(addr_size)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                printf ("Read error 2\n");
                goto error;
            }
            addr_size--;
            //buf = (address >> 8); //地址高8位
        }

        if(mode != RDA_5820Mode)
        {
            I2CStop(); // I2C_CONR = 0x1C , I2C_CONR = 0xC
            //I2cReg->I2C_CONR |= I2C_CON_NACK;

            I2cReg->I2C_OPR |= I2C_OPR_ENABLE;
        }

        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            printf("I2C_Read: Send read slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }

    // 设置为主设备接收
    // MaskRegBits32(&pI2CReg->I2C_CONR, I2C_CONR_MASTER_MASK, I2C_CONR_MRX);
    I2cReg->I2C_CONR &= ~( I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MRX;
    while (data_size)
    {
        if (OK != I2CReadData(data))
        {
            //printf("I2C_Read: read data(%d) error!\n",*pData);
            ret = FALSE;
            printf ("Read error 3\n");
            goto error;
        }

        //  the last read (size = 1)is NACK , other is ACK
        if (1 == data_size)
            //SetRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR |= I2C_CON_NACK;
        else
            //ClrRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
        data++;

        data_size --;
    }

error:

    I2CStop();

    I2C_Deinit();

    return ret;
#endif
}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg8_read(uint8 slave_addr, uint8 address, uint8 *data, uint8 size)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = size;
    uint8 addr_size = 1;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;
#if 1
    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x00;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (mode == NormalMode) // 为了以后扩展
    {
        if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
        {
            printf("I2C_Write: Send slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }

        //Rigister address
        buf = address;
        while(addr_size)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                printf ("Read error 2\n");
                goto error;
            }
            addr_size--;
            //buf = (address >> 8); //地址高8位
        }

        if(mode != RDA_5820Mode)
        {
            I2CStop(); // I2C_CONR = 0x1C , I2C_CONR = 0xC
            //I2cReg->I2C_CONR |= I2C_CON_NACK;

            I2cReg->I2C_OPR |= I2C_OPR_ENABLE;
        }

        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            printf("I2C_Read: Send read slaveaddr(%x) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }

    // 设置为主设备接收
    // MaskRegBits32(&pI2CReg->I2C_CONR, I2C_CONR_MASTER_MASK, I2C_CONR_MRX);
    I2cReg->I2C_CONR &= ~( I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MRX;
    while (data_size)
    {
        if (OK != I2CReadData(data))
        {
            //printf("I2C_Read: read data(%d) error!\n",*pData);
            ret = FALSE;
            printf ("Read error 3\n");
            goto error;
        }

        //  the last read (size = 1)is NACK , other is ACK
        if (1 == data_size)
            //SetRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR |= I2C_CON_NACK;
        else
            //ClrRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
        data++;

        data_size --;
    }

error:

    I2CStop();

    I2C_Deinit();

    return ret;
#endif
}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg8_write(uint8 slave_addr, uint8 address,uint8 *data, uint8 size)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = size;
    uint8 addr_size = 1;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;

    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x0;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
    {
        printf("I2C_Write: Send slaveaddr(%x) Error!\n",I2C_slaveaddr);
        ret = FALSE;
        goto error;
    }

    while(addr_size)
    {
        if (mode == NormalMode)
        {
            if (I2CSendData(address, FALSE) != OK)
            {
                ret = FALSE;
                goto error;
            }
        }
        addr_size--;
    }

    while(data_size)
    {
        if (I2CSendData(*data, FALSE) != OK)
        {
            //printf("I2C_Write: Send Data(%d) Error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        data_size--;
		data++;
    }

error:
    // Set CONR ACK    释放SDA总线
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    I2CStop();

    I2C_Deinit();

    return ret;
}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg8_write8(uint8 slave_addr, uint8 address, uint8 data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 1;
    uint8 addr_size = 1;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;

    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x0;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
    {
        printf("I2C_Write: Send slaveaddr(%x) Error!\n",I2C_slaveaddr);
        ret = FALSE;
        goto error;
    }
    //Rigister address
    buf = address; //地址8位
    //buf = (address&0x00ff ); //寄存器地址低八位
    while(addr_size)
    {
        if (mode == NormalMode)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                goto error;
            }
        }
        addr_size--;
        buf = (address&0x00ff ); //寄存器地址低八位
        //buf = (address >> 8); //地址高8位
    }

    // Write data
    //buf = (data >> 8); //传输数据高8位
    buf = (data&0x00ff); //传输数据低8位
    while(data_size)
    {
        if (I2CSendData(buf, FALSE) != OK)
        {
            //printf("I2C_Write: Send Data(%d) Error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        data_size--;
        //buf = (data&0x00ff); //传输数据低8位
        buf = (data >> 8); //传输数据高8位
    }

error:
    // Set CONR ACK    释放SDA总线
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    I2CStop();

    I2C_Deinit();

    return ret;
}

_ATTR_DRIVER_CODE_
BOOL i2c1_reg8_write32(uint8 slave_addr, uint8 address, uint32 data)
{
    BOOL ret = TRUE;
    eI2C_mode_t mode;

    uint8 data_size = 4;
    uint8 addr_size = 1;
    uint8 buf = 0;
    uint8 SLave_Address = slave_addr;

    mode = NormalMode;

    I2C_Init(SLave_Address, 400);

    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x0;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;

    if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
    {
        printf("I2C_Write: Send slaveaddr(%x) Error!\n",I2C_slaveaddr);
        ret = FALSE;
        goto error;
    }

    //Rigister address
    buf = address >> 8; //地址高8位
    //buf = (address&0x00ff); //寄存器地址低八位
    while(addr_size)
    {
        if (mode == NormalMode)
        {
            if (I2CSendData(buf, FALSE) != OK)
            {
                //printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
                ret = FALSE;
                goto error;
            }
        }
        addr_size--;
        //buf = (address >> 8); //地址高8位
    }

    // Write data
    //buf = (data >> 24); //传输数据高8位
    buf = (data&0x000000ff ); //传输数据低8位
    while(data_size)
    {
        if (I2CSendData(buf, FALSE) != OK)
        {
            //printf("I2C_Write: Send Data(%d) Error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        data_size--;
        if (data_size == 3)
        {
            //buf = ((data>>16)&0x000000ff ); //传输数据次高8位
            buf = ((data>>8)&0x000000ff ); //传输数据次低8位
        }
        if (data_size == 2)
        {
            //buf = ((data>>8)&0x000000ff ); //传输数据次低8位
            buf = ((data>>16)&0x000000ff ); //传输数据次高8位
        }
        if (data_size == 1)
        {
            //buf = (data&0x000000ff ); //传输数据低8位
            buf = (data >> 24); //传输数据高8位
        }
    }

error:
    // Set CONR ACK    释放SDA总线
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    I2CStop();

    I2C_Deinit();

    return ret;
}

_ATTR_DRIVER_CODE_
uint8 i2c_read_data(uint8 slaveaddr, uint8 reg_address, uint8 *data, uint8 size)
{
    I2C_Init(slaveaddr, 400);//210
    I2C_Read(reg_address, data, size, NormalMode);
    I2C_Deinit();

    return *data;
}

_ATTR_DRIVER_CODE_
uint8 i2c_write_data(uint8 slaveaddr, uint8 reg_address, uint8 data, uint8 size)
{
    uint8 wdata = data;
    I2C_Init(slaveaddr, 400);
    I2C_Write(reg_address, &wdata, size, NormalMode);
    I2C_Deinit();

    return 0;
}



/*
********************************************************************************
*
*                         End of I2S.c
*
********************************************************************************
*/
