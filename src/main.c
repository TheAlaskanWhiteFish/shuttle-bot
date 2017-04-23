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
#include "i2c/i2c.h"
#include "stdint.h"

const uint8_t forward[] = {96, 234};      // preset motor commands
const uint8_t stop[] = {0, 0};
const uint8_t reverse[] = {32, 160};

#pragma vector=TIMERA1_VECTOR
#pragma type_attribute=__interrupt
void TimerA1Interrupt(void)
{
    switch(__even_in_range(TAIV, 10))
    {
        case TAIV_TAIFG:
            __bic_SR_register_on_exit(CPUOFF);
            break;
        case TAIV_TACCR1:
            break;
        case TAIV_TACCR2:
            break;
        default:
            break;
    }
}

int16_t NewVel(int16_t accel, int16_t vInit, uint8_t tmsec)
//-------------------------------------------------------------------------
// Func:  Calculate total distance travelled given initial velocity
// Args:  accel  - acceleration from accelerometer
//        vInit  - initial velocity in units/msec (previously returned newVel)
//        tmsec  - time period in miliseconds
// Retn:  newVel - new velocity in units/dsec
//-------------------------------------------------------------------------
{
    int16_t dVel     = (accel * tmsec) / 100;  // change in velocity in units/dsec
    int16_t newVelmm = dVel + vInit;           // new velocity after acceleration
    return(newVel);
}

int16_t NewDist(int16_t vel, int16_t currDist, uint8_t tmsec)
//-------------------------------------------------------------------------
// Func:  Calculate total distance travelled given velocity and time
// Args:  vel      - velocity in units/msec (previously returned newVel)
//        currDist - current distance travelled in units
//        tmsec    - time period in miliseconds
// Retn:  newDist  - new distance in units
//-------------------------------------------------------------------------
{
    int16_t dDist = (vel * tmsec) / 100;     // change in distance
    int16_t newDist = dDist + currDist;      // new distance
    return(newDist);
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // disable watchdog
    DCOCTL = CALDCO_1MHZ;       // 1MHz DCO
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL3 |= LFXT1S_2;        // ACLK run by VLO
    P1DIR |= 0x03;
    P1OUT &= ~0x03;

    UARTInit();         // initialize uart
    P1OUT |= 0x01;      // turn on red led while setting up accelerometer
    MMA8450Init();      // initialize accelerometer
    MMA8450SetZero();   // zero out accelerometer, dont move robot while happening
    P1OUT &= ~0x01;     // turn off led after finished

    TACCR0 = 1200;
    TACTL = TASSEL_1 | ID_0 | MC_1 | TAIE; // ACLK, div 1, Up mode


    int16_t accelData[3];
    while(1)
    {
        P1OUT |= 0x02;

        int16_t data[3];
        int16_t xaccel;
        int16_t vel = 0;
        int16_t dist = 0;
        int8_t t = 100;
        static int8_t step = 0;

        MMA8450ReadXYZ(data);               // Read accelerometer
        xaccel = (data > 0x07FF) ? (data - 4096) : data;    // convert to 16 bit signed
        vel = NewVel(accel, vel, t);        // Find velocity and distance
        dist = NewDist(vel, t, dist);

        if(dist > 1000 && step == 0)        // Stop at 1 meter
        {
          UARTSend(stop, 2);
          step = 1;
          for (i = 0; i < 12000; i++){};    // Wait 1 sec before reversing
          UARTSend(reverse, 2);
        }
        else if(dist < 0 && step == 1)      // Stop at starting line
        {
          UARTSend(stop, 2);
        }

        P1OUT &= ~0x02;
        __bis_SR_register(CPUOFF | GIE);
    }
}
