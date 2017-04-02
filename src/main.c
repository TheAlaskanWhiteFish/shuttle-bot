/*
 *  main.c
 *  Main file for EE4735 shuttle bot project
 *
 *  Team Members:   Nathan Duprey
 *                  Noah Symanzik
 *                  Chad Pollock
 */

#include "msp430x22x4.h"
#include "uart/uart.h"
#include "mma8450q/mma8450q.h"
//#include "i2c/i2c.h"
#include "stdint.h"

#pragma vector=TIMERA1_VECTOR
#pragma type_attribute=__interrupt
void TimerA1Interrupt(void)
{
    switch(__even_in_range(TAIV, 10))
    {
        case TAIV_TAIFG:
            UARTSend("hello world\r\n", 13);
            break;
        case TAIV_TACCR1:
            break;
        case TAIV_TACCR2:
            break;
        default:
            break;
    }
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // disable watchdog
    DCOCTL = CALDCO_1MHZ;       // 1MHz DCO
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL3 |= LFXT1S_2;        // ACLK run by VLO

    UARTInit();     // initialize uart
    MMA8450Init();  // initialize accelerometer
    TACCR0 = 0x5DC0;
    TACTL = TASSEL_1 | ID_0 | MC_1 | TAIE;

    __bis_SR_register(LPM0_bits | GIE);
}
