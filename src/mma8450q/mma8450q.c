/*
 *  MMA8450Q.c
 *  Functions for interfacing with the Freescale (now NXP) MMA8450Q 3 axis
 *  accelerometer
 *
 *  Datasheet: http://www.nxp.com/assets/documents/data/en/data-sheets/MMA8450Q.pdf
 *
 *  Version 1: 04/01/17 - Nathan Duprey
 */

 #include "mma8450q.h"
 #include "../i2c/i2c.h"
 #include "../msp430x22x4.h"
 #include "stdint.h"

void MMA8450Init(void)
//-------------------------------------------------------------------------
// Func:  Start I2C and initialize the accelerometer
// Args:  none
// Retn:  none
//-------------------------------------------------------------------------
{
    I2CInitMaster();                    // initialize I2C in master mode
    I2CSetSlaveAddr(0x1C);              // set slave address for accel
    I2CSendRegister(CTRL_REG1, 0x0D);   // set active mode, +/-2g, 50Hz sample
}
