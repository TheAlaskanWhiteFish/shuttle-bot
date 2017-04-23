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

int16_t NewVel(int16_t accelmm, int16_t vInitmm, uint8_t tmsec)
//-------------------------------------------------------------------------
// Func:  Calculate total distance travelled given initial velocity
// Args:  accelmm - acceleration in mm/sec^2
//        vInitmm - initial velocity in mm/sec (previously returned newVelmm)
//        tmsec - time period in miliseconds
// Retn:  newVelmm - new velocity in mm/sec
//-------------------------------------------------------------------------
{
    int16_t dVel1k = accelmm * tmsec;            // change in velocity in mm/sec * 1000
    int16_t newVelmm = dVel1k / 1000 + vInitmm;  // new velocity after acceleration
    return(newVelmm);
}

int16_t NewDist(int16_t velmm, int16_t currDistmm, uint8_t tmsec)
//-------------------------------------------------------------------------
// Func:  Calculate total distance travelled given velocity and time
// Args:  velmm - velocity in mm/sec (previously returned newVelmm)
//        currDistmm - current distance travelled in milimeters
//        tmsec - time period in miliseconds
// Retn:  newDistmm - new distance in mm
//-------------------------------------------------------------------------
{
    int16_t dDistmm1k = velmm * tmsec;        // change in distance in mm/sec * 1000
    int16_t newDistmm = dDistmm1k / 1000 + currDistmm;
    return(newDistmm);
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // disable watchdog
    DCOCTL = CALDCO_1MHZ;       // 1MHz DCO
    BCSCTL1 = CALBC1_1MHZ;
    P1DIR |= 0x03;              // set led outputs
    P1OUT &= ~0x03;             // clear led outputs

    UARTInit();         // initialize uart
    P1OUT |= 0x01;      // turn on red led while setting up accelerometer
    MMA8450Init();      // initialize accelerometer
    MMA8450SetZero();   // zero out accelerometer, dont move robot while happening
    P1OUT &= ~0x01;     // turn off led after finished

    TACCR0 = 5000;                          // 1 MHz / 5000 = 200Hz
    TACTL = TASSEL_2 | ID_0 | MC_1 | TAIE;  // SMCLK, div 1, Up mode


    int16_t accelData[3];
    while(1)
    {
        P1OUT |= 0x02;

        int16_t data[3];            // array for storing acceleration data
        int16_t xaccel;             //
        static int16_t vel = 0;
        static int16_t dist = 0;
        int8_t t = 100;
        static int8_t step = 0;

        uint8_t i;                  // loop counter
        for(i = 0; i < 4; i++)
        {
            MMA8450ReadXYZ(data);   // read accelerometer
            // convert to 16 bit signed and sum samples
            xaccel += (data[0] > 0x07FF) ? (data[0] - 4096) : data[0];
        }
        xaccel >>= 2;                       // divide by 4 to get average
        //xaccel = data[0] * 10;              // Convert x to mm/s^2 and store
        vel = NewVel(xaccel, vel, t);       // Find velocity and distance
        dist = NewDist(vel, dist, t);

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
