/*
 *  uart.h
 *  Definitions and prototypess for using UART on the MSP430. USCI_A is reserved
 *  for UART.
 *
 *  MSP430F22x2/4 Datasheet: http://www.ti.com/lit/ds/symlink/msp430f2274.pdf
 *  MSP430x2xx USer Guide: http://www.ti.com/lit/ug/slau144j/slau144j.pdf
 *
 *  Version 1: 04/01/17 - Nathan Duprey
 *                      - added init and send functions
 */

#ifndef I2C_H_
#define I2C_H_

void UARTInit(void);
void UARTSendByte(uint8_t data);
void UARTSend(uint8_t * data, uint8_t length);

#endif
