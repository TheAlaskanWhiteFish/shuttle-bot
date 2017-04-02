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
