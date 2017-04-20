/*
 *  MMA8450Q.c
 *  Functions for interfacing with the Freescale (now NXP) MMA8450Q 3 axis
 *  accelerometer
 *
 *  Datasheet: http://www.nxp.com/assets/documents/data/en/data-sheets/MMA8450Q.pdf
 *
 *  Version 1: 04/01/17 - Nathan Duprey
 *             04/03/17 - wrote calibration fucntion
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
    volatile uint16_t i;                // generic counter for delay
    for(i = 0; i < 60000; i++);         // wait for power to stabilize
    I2CInitMaster();                    // initialize I2C in master mode
    I2CSetSlaveAddr(0x1C);              // set slave address for accel
    I2CSendRegister(CTRL_REG1,          // set active mode, +/-2g, 200Hz sample
                   (FS_2G | DATA_RATE_200));
}

uint8_t MMA8450ReadXYZ(int16_t * retData)
//-------------------------------------------------------------------------
// Func:  Read the X, Y, and Z registers from the accelerometer
// Args:  a pointer to a 3 byte array for storing the values
// Retn:  the status register
//-------------------------------------------------------------------------
{
    int16_t data[7];
    I2CReadRegisters(OUT_X_LSB, 7, data);  // read X,Y,Z
    uint8_t i;                                          // loop counter

    // convert the received data into a valid 12 bit value
    // left shift the LSBs and then right shift the whole thing
    for(i = 0; i < 3; i++)
    {
        retData[i] = (((data[i*2+1] & 0xFF) << 4) | (data[i*2] & 0x0F)) & 0x0FFF;
        //retData[i] = (data[i*2+1] | data[i*2] << 4) >> 4;
    }

    return data[6];
}

void MMA8450SetZero()
//-------------------------------------------------------------------------
// Func:  function to determine and set zero offsets to minimize error. using
//        the method described in AN3916 from NXP/Freescale.
//        http://www.nxp.com/assets/documents/data/en/application-notes/AN3916.pdf
// Args:  none
// Retn:  none
//-------------------------------------------------------------------------
{
    uint8_t i;                  // generic loop counter
    volatile uint32_t j;        // delay loop counter
    int16_t accelData[3];       // array with accel value
    int16_t xCal, yCal, zCal;   // calibration values

    for(i = 0; i < 2; i++)      // run twice for better accuracy
    {
        I2CSendRegister(CTRL_REG1,      // change to 8g, 1.56 sample rate
                       (FS_8G | DATA_RATE_1_56));

        for(j = 0; j < 166667; j++);    // empty looop to delay for about 2s

        MMA8450ReadXYZ(accelData);      // get readings
        for(j = 0; j < 3; j++)          // calculate calibration values for
        {                               // each axis
            if(accelData[i] > 2047)
            {
                accelData[i] -= 4096;
            }
        }
        xCal += -1 * accelData[0];
        yCal += -1 * accelData[1];
        zCal += (256 - accelData[2]);

        I2CSendRegister(CTRL_REG1, FS_STANDBY); // return to standby mode
        I2CSendRegister(OFF_X, xCal);           // send calibration values
        I2CSendRegister(OFF_Y, yCal);
        I2CSendRegister(OFF_Z, zCal);
    }

    I2CSendRegister(CTRL_REG1,          // set active mode, +/-2g, 200Hz sample
                   (FS_2G | DATA_RATE_200));
}
