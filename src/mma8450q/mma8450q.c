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
    I2CSendRegister(MMA_CTRL_REG1, 0x0D);   // set active mode, +/-2g, 50Hz sample
}

int16_t * MMA8450ReadXYZStatus()
//-------------------------------------------------------------------------
// Func:  Read the X, Y, Z, and status registers from the accelerometer
// Args:  none
// Retn:  An array of data formatted correctly for 12 bits
//-------------------------------------------------------------------------
{
    DataUnion_t data = I2CReadRegisters(OUT_X_LSB, 7);  // read X,Y,Z and status
    uint8_t i;                                          // loop counter
    int16_t retData[4];                                 // array for returning
                                                        // formatted data

    // convert the received data into a valid 12 bit value
    // left shift the LSBs and then right shift the whole thing
    for(i = 0; i < 4; i++)
    {
        retData[i] = (data.s8[i*2+1] | data.s8[i*2] << 4) >> 4;
    }

    return retData;
}

void MMA8450SetZero()
//-------------------------------------------------------------------------
// Func:  function to determine zero offsets to minimize error
// Args:  none
// Retn:  none
//-------------------------------------------------------------------------
{
    // todo: - take several samples while device isnt moving
    //       - send offset values to OFF_X, OFF_Y, OFF_Z registers
}
