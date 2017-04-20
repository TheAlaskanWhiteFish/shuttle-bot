/*
 *  i2c.c
 *  Functions for using I2C on the MSP430. USCI_A is reserved for UART so we
 *  are using USCI_B, which supports I2C and SPI.
 *
 *  MSP430F22x2/4 Datasheet: http://www.ti.com/lit/ds/symlink/msp430f2274.pdf
 *  MSP430x2xx USer Guide: http://www.ti.com/lit/ug/slau144j/slau144j.pdf
 *
 *  Version 1: 04/01/17 - Nathan Duprey
 *                      - added initialization and send cmd
 */

#include "i2c.h"
#include "../msp430x22x4.h"
#include "stdint.h"

void I2CInitMaster(void)
//-------------------------------------------------------------------------
// Func:  Configure I2C for master mode, SMCLK source
// Args:  None
// Retn:  None
//-------------------------------------------------------------------------
{
    // note: cc2500 is connected to USCB0 on ez430 card, needs to be disabled
    P3DIR |= 0x0F;                          // P3.0-3 as outputs
    P3OUT |= 0x09;                          // disable cc2500
    P3SEL |= 0x06;                          // P3.1 and P3.2 as I2C
    UCB0CTL1 = UCSWRST;                     // stop/reset USCB0
    UCB0CTL0 = UCMODE_3 | UCMST | UCSYNC;   // I2C mode, master, synchronous
    UCB0CTL1 = UCSSEL_2;                    // SMCLK source
    UCB0BR1 = 0;                            // msb of divisor
    UCB0BR0 = 10;                           // lsb of divisor, SMCLK/10 = ~100kHz
    UCB0CTL1 &= ~UCSWRST;                   // start USCB0
    while(UCB0STAT & UCBBUSY);              // Check if Bus Busy (carrier sense)
}

void I2CSetSlaveAddr(uint16_t addr)
//-------------------------------------------------------------------------
// Func:  Set I2C slave address
// Args:  addr - slave address
// Retn:  None
//-------------------------------------------------------------------------
{
    UCB0CTL1 |= UCSWRST;    // stop i2c
    UCB0I2CSA = addr;       // set slave address
    UCB0CTL1 &= ~UCSWRST;   // start i2c
}

void I2CSendByte(uint8_t data)
//-------------------------------------------------------------------------
// Func:  Send a single byte over I2C. Note: this is blocking until the bus
//        is free and the transaction is complete
// Args:  data - 8 bit data to send
// Retn:  None
//-------------------------------------------------------------------------
{
    while(UCB0STAT & UCBBUSY);      // wait for bus to be free
    UCB0CTL1 |= UCTR | UCTXSTT;     // send start bit, slave addr, write bit
    while(!(IFG2 & UCB0TXIFG));     // wait until tx buffer is empty

    UCB0TXBUF = data;               // send data
    while(!(IFG2 & UCB0TXIFG));     // wait until tx buffer is empty

    UCB0CTL1 |= UCTXSTP;            // send stop bit
    while(UCB0CTL1 & UCTXSTP);      // wait until stop bit is sent
}

void I2CSend(uint8_t * data, uint8_t length)
//-------------------------------------------------------------------------
// Func:  Send multiple bytes over I2C. Note: this is blocking until the bus
//        is free and the transaction is complete. Also sends MSB first
// Args:  data - pointer to data byte array
//        length - length of data in bytes
// Retn:  None
//-------------------------------------------------------------------------
{
    while(UCB0STAT & UCBBUSY);      // wait for bus to be free
    UCB0CTL1 |= UCTR | UCTXSTT;     // send start bit, slave addr, write bit

    int8_t i;
    for(i = length - 1; i >= 0; i--)
    {
        while(!(IFG2 & UCB0TXIFG)); // wait until tx buffer is empty
        UCB0TXBUF = data[i];        // send data
    }
    while(!(IFG2 & UCB0TXIFG));     // wait until tx buffer is empty
    UCB0CTL1 |= UCTXSTP;            // send stop bit
    while(UCB0CTL1 & UCTXSTP);      // wait until stop bit is sent
}

void I2CSendRegister(uint8_t reg, uint8_t data)
//-------------------------------------------------------------------------
// Func:  Send a register and data byte over I2C. Note: this is blocking until
//        the bus is free and the transaction is complete. Also sends MSB first
// Args:  reg - the device register to modify
//        data - the value to set the register to
// Retn:  None
//-------------------------------------------------------------------------
{
    while(UCB0STAT & UCBBUSY);      // wait for bus to be free
    UCB0CTL1 |= UCTXSTT + UCTR;     // send start bit, slave addr, write bit

    UCB0TXBUF = reg;                // send address of register
    while(!(UCB0TXIFG & IFG2));     // wait for buffer empty

    UCB0TXBUF = data;               // send data
    while(!(UCB0TXIFG & IFG2));     // wait for buffer empty

    UCB0CTL1 |= UCTXSTP;            // send stop bit
    while(UCB0CTL1 & UCTXSTP);      // wait until stop bit is sent
}

uint8_t I2CReadRegister(uint8_t addr)
//-------------------------------------------------------------------------
// Func:  Read a single register
// Args:  addr - the register address to read
// Retn:  the data contained in the requested register
//-------------------------------------------------------------------------
{
    while(UCB0STAT & UCBBUSY);  // wait until bus is free
    UCB0CTL1 |= UCTR | UCTXSTT; // send start, address, write bit
    UCB0TXBUF = addr;           // send first register to read from
    while(!(UCB0TXIFG & IFG2)); // wait for buffer empty

    UCB0CTL1 &= ~UCTR;              // read mode
    UCB0CTL1 |= UCTXSTT;            // send start, address, read bit
    while(UCB0CTL1 & UCTXSTT);      // wait for start to be sent
    UCB0CTL1 |= UCTXNACK | UCTXSTP; // send NACK and stop
    while(!(IFG2 & UCB0RXIFG));     // wait for byte to be read
    uint8_t data = UCB0RXBUF;       // get data

    return data;
}

void I2CReadMultRegisters(uint8_t firstAddr, uint8_t numRegs, int16_t * retData)
//-------------------------------------------------------------------------
// Func:  Read a specified number of registers starting from firstAddr
// Args:  firstAddr - the addredd of the first register to read
//        numRegs - the number of registers to read
//        retData - a pointer to an array to store the read values
// Retn:  none
//-------------------------------------------------------------------------
{
    uint8_t i;                  // loop counter

    while(UCB0STAT & UCBBUSY);  // wait until bus is free
    UCB0CTL1 |= UCTR | UCTXSTT; // send start, address, write bit
    UCB0TXBUF = firstAddr;      // send first register to read from
    while(!(UCB0TXIFG & IFG2)); // wait for buffer empty

    UCB0CTL1 &= ~UCTR;          // read mode
    UCB0CTL1 |= UCTXSTT;        // send start, address, read bit

    for(i = 0; i <= (numRegs - 1); i++)
    {
        while(!(IFG2 & UCB0RXIFG)); // wait for byte to be read
        retData[i] = UCB0RXBUF;     // store byte

        if(i == numRegs - 2)                 // if final byte enroute
        {
            UCB0CTL1 |= UCTXNACK | UCTXSTP; // send NACK and stop
        }
    }
}
