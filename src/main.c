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
#include "stdint.h"

#pragma vector=TIMERA1_VECTOR
#pragma type_attribute=__interrupt
void TimerA1Interrupt(void)
{
    static uint8_t cmdIndex = 0;
    uint8_t drvCmds[4][2] = {
        {0x60, 0xE0},           // drive forward, half speed
        {0x40, 0xC0},           // coast to stop
        {0x60, 0xA0},           // spin in place
        {0x00, 0x00}            // stop
    };

    switch(__even_in_range(TAIV, 10))
    {
        case TAIV_TAIFG:
            if(cmdIndex < 4)            // only send 4 commands
            {
                uint8_t i = 0;
                for(i = 0; i < 2; i++)
                {
                    UARTSendByte(drvCmds[cmdIndex][i]);
                }
                cmdIndex++;
            }
            break;
        case TAIV_TACCR1:
            break;
        case TAIV_TACCR2:
            break;
        default:
            break;
    }
}

void newDist(int8_t accelmm, int8_t vInitmm, uint8_t tmsec, int16_t currDist)
//-------------------------------------------------------------------------
// Func:  Calculate total distance travelled given initial velocity and acceleration
// Args:  accelmm - acceleration in mm/sec^2
//        vInitmm - initial velocity in mm/sec
//        tmsec - time period in miliseconds
//        currDistmm - current distance travelled in milimeters
// Retn:  newDistmm - new distance in mm
//-------------------------------------------------------------------------
    int16_t dVel1k = accelmm * tmsec;            // change in velocity in mm/sec * 1000
    int16_t newVelmm = dVel1k / 1000 + vInitmm;  // new velocity after acceleration
    int16_t dDistmm1k = newVelmm * tmsec;        // change in distance in mm/sec * 1000
    int16_t newDistmm = dDistmm1k/1000 + currDistmm;
    return(newDistmm);
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // disable watchdog
    DCOCTL = CALDCO_1MHZ;       // 1MHz DCO
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL3 |= LFXT1S_2;        // ACLK run by VLO

    UARTInit();     // initialize uart
    MMA8450Init();  // initialize accelerometer
    MMA8450SetZero();
    TACCR0 = 0x5DC0;
    TACTL = TASSEL_1 | ID_0 | MC_1 | TAIE;

    __bis_SR_register(LPM0_bits | GIE);
}
