/*
 *  uart.c
 *  Functions for using UART on the MSP430. USCI_A is reserved for UART.
 *
 *  MSP430F22x2/4 Datasheet: http://www.ti.com/lit/ds/symlink/msp430f2274.pdf
 *  MSP430x2xx USer Guide: http://www.ti.com/lit/ug/slau144j/slau144j.pdf
 *
 *  Version 1: 04/01/17 - Nathan Duprey
 *                      - added init and send functions
 */

 #include "stdint.h"
 #include "uart.h"
 #include "msp430x22x4.h"

void UARTInit(void)
//-------------------------------------------------------------------------
// Func:  Configure UART, 9600 baud @ 1 MHz DCO
// Args:  None
// Retn:  None
//-------------------------------------------------------------------------
{
    P3SEL |= 0x30;          // P3.4 and P3.5 as uart
    UCA0CTL1 |= UCSSEL_2;   // assuming we're running at SMCLK = 1MHz
    UCA0MCTL = UCBRS0;      // set bit 0 of UCBRSx field
    UCA0BR0 = 104;          // 1MHz / 104 = ~9600 baud
    UCA0BR1 = 0;
    UCA0CTL1 &= ~UCSWRST;   // enable usci
}

void UARTSendByte(uint8_t data)
//-------------------------------------------------------------------------
// Func:  Send a single byte over UART. Note: this is blocking until the
//        buffer is free
// Args:  data - 8 bit data to send
// Retn:  None
//-------------------------------------------------------------------------
{
    while(!(IFG2 & UCA0TXIFG));     // wait for tx buffer to empty
    UCA0TXBUF = data;               // put data in tx buffer
}

void UARTSend(uint8_t * data, uint8_t length)
//-------------------------------------------------------------------------
// Func:  Send a multiple bytes over UART. Note: this is blocking until the
//        buffer is free
// Args:  data - pointer to byte array to send
//        length - length of data in number of bytes
// Retn:  None
//-------------------------------------------------------------------------
{
    uint8_t i;
    for(i = 0; i < length; i++)
    {
        UARTSendByte(data[i]);
    }
}
