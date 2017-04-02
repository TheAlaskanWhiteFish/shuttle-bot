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

 UCB0I2CSA = addr;				  // Slave Dev addr w/ SA0=0
 
 while (UCB0STST & UCBBUSY){};    // Carrier Sense
 UCB0CTL1 |= UCTXSTT + UCTR;      // Tx (ST, Dev addr) + w-bit
 
 UCB0TXBUF = 0x38;               // Tx reg addr of CTRL_REG1
 while (!(UCB0TXIFG & IFG2)){};   // Wait for Tx_Buf empty

 UCB0TXBUF = 0x0D;                // Tx Data to CTRL_REG1
 while (!(UCB0TXIFG & IFG2)){};   // Wait for Tx_Buf empty

 UCB0CTL1 |= UCTXSTP;             // Tx Sp after curr byte
 while (UCB0CTL1 & UCTXSTP){};     // Wait for SP to be sent
