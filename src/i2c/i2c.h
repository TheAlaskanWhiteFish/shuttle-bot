/*
 *  i2c.h
 *  Definitions and prototypess for using I2C on the MSP430. USCI_A is reserved
 *  for UART so we are using USCI_B, which supports I2C and SPI.
 *
 *  MSP430F22x2/4 Datasheet: http://www.ti.com/lit/ds/symlink/msp430f2274.pdf
 *  MSP430x2xx USer Guide: http://www.ti.com/lit/ug/slau144j/slau144j.pdf
 *
 *  Version 1: 04/01/17 - Nathan Duprey
 */

#ifndef I2C_H_
#define I2C_H_

#include "stdint.h"

// union data type for storing register values
typedef union
{
    int8_t s8[8];
    int16_t s16[4];
} DataUnion_t;

void I2CInitMaster(void);
void I2CSetSlaveAddr(uint16_t addr);
void I2CSendByte(uint8_t data);
void I2CSend(uint8_t * data, uint8_t length);
void I2CSendRegister(uint8_t reg, uint8_t data);
DataUnion_t I2CReadRegisters(uint8_t firstAddr, uint8_t numRegs);

#endif
