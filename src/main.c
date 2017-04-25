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

uint8_t forward[] = {105, 234};      // preset motor commands
uint8_t stop[] = {0, 0};
uint8_t reverse[] = {23, 149};

#pragma vector=TIMERA1_VECTOR
#pragma type_attribute=__interrupt
void TimerA1Interrupt(void)
{
    switch(__even_in_range(TAIV, 10))
    {
        case TAIV_TAIFG:
            // return to active mode
            __bic_SR_register_on_exit(CPUOFF);
            break;
        case TAIV_TACCR1:
            break;
        case TAIV_TACCR2:
            break;
        default:
            break;
    }
    volatile uint16_t flags = TAIV;     // read flags to clear them
}

int32_t NewVel(int32_t accel, int32_t vInit, uint8_t tmsec)
//-------------------------------------------------------------------------
// Func:  Calculate total distance travelled given initial velocity
// Args:  accel  - acceleration from accelerometer
//        vInit  - initial velocity in units/msec (previously returned newVel)
//        tmsec  - time period in miliseconds
// Retn:  newVel - new velocity in units/dsec
//-------------------------------------------------------------------------
{
    int32_t dVel = (accel * tmsec) / 10;    // change in velocity in units/dsec
    int32_t newVel = dVel + vInit;          // new velocity after acceleration
    return(newVel);
}

int32_t NewDist(int32_t vel, int32_t currDist, uint8_t tmsec)
//-------------------------------------------------------------------------
// Func:  Calculate total distance travelled given velocity and time
// Args:  vel      - velocity in units/msec (previously returned newVel)
//        currDist - current distance travelled in units
//        tmsec    - time period in miliseconds
// Retn:  newDist  - new distance in units
//-------------------------------------------------------------------------
{
    int32_t dDist = (vel * tmsec) / 10;      // change in distance
    int32_t newDist = dDist + currDist;      // new distance
    return(newDist);
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // disable watchdog
    DCOCTL = CALDCO_1MHZ;       // 1MHz DCO
    BCSCTL1 = CALBC1_1MHZ;
    P1DIR |= 0x03;              // set led outputs
    P1OUT &= ~0x03;             // clear led outputs
    P2DIR &= ~0x08;             // P2.3 as input
    P2SEL |= 0x08;              // P2.3 as CCI1B

    UARTInit();         // initialize uart
    UARTSend(stop, 2);  // send stop command to robot
    P1OUT |= 0x01;      // turn on red led while setting up accelerometer
    MMA8450Init();      // initialize accelerometer
    MMA8450SetZero();   // zero out accelerometer, dont move robot while happening
    P1OUT &= ~0x01;     // turn off led after finished

    TACCR0 = 3333;                          // 1 MHz / 5000 = 200Hz
    TACTL = TASSEL_2 | ID_0 | MC_1 | TAIE;  // SMCLK, div 1, Up mode

    int16_t data[3];        // array for storing acceleration data
    int16_t xAccel = 0;     // x component of acceleration
    int32_t vel = 0;        // current velocity
    int32_t dist = 0;       // distance travelled
    uint8_t timeStep = 27;  // time step
    int8_t step = 0;        // flag for what action is happening
    uint8_t i = 0;          // sample counter

    while(1)
    {
        if(step == 0)   // drive forward, slowly increasing speed
        {
            P1OUT |= 0x02;
            MMA8450ReadXYZ(data);   // read accelerometer
            // convert to 16 bit signed and sum samples
            xAccel += (data[0] > 0x07FF) ? (data[0] - 4096) : data[0];
            P1OUT &= ~0x02;
            i += 1;                 // increment sample counter

            if(i == 8)
            {
                i = 0;                                  // reset sample counter
                xAccel >>= 3;                           // divide by 8 to get average
                xAccel &= ~0x0003;                      // get rid of 2 LSBs for noise
                vel = NewVel(xAccel, vel, timeStep);    // Find velocity and distance
                dist = NewDist(vel, dist, timeStep);
                xAccel = 0;

                static uint8_t fwdSpeed[] = {64, 192};
                UARTSend(fwdSpeed, 2);
                fwdSpeed[0] += 1;
                fwdSpeed[1] += 1;

                if(fwdSpeed[0] == 116)
                {
                    step = 1;
                }
            }
        }
        else if(step == 1)    // Stop at 12 meters
        {
            dist = NewDist(vel, dist, timeStep);    // calculate distance
            if(dist >= 78500000)
            {
                UARTSend(stop, 2);
                P1OUT |= 0x01;
                MMA8450SetZero();               // recalibrate at opposite end
                P1OUT &= ~0x01;
                step = 2;                       // move to next step
                vel = 0;                        // reset velocity
            }
        }
        else if(step == 2)
        {
            P1OUT |= 0x02;
            MMA8450ReadXYZ(data);   // read accelerometer
            // convert to 16 bit signed and sum samples
            xAccel += (data[0] > 0x07FF) ? (data[0] - 4096) : data[0];
            P1OUT &= ~0x02;
            i += 1;                 // increment sample counter

            if(i == 8)
            {
                i = 0;                                  // reset sample counter
                xAccel >>= 3;                           // divide by 8 to get average
                xAccel &= ~0x0003;                      // get rid of 2 LSBs for noise
                vel = NewVel(xAccel, vel, timeStep);    // Find velocity and distance
                dist = NewDist(vel, dist, timeStep);
                xAccel = 0;

                static uint8_t revSpeed[] = {64, 186};
                UARTSend(revSpeed, 2);
                revSpeed[0] -= 1;
                revSpeed[1] -= 1;

                if(revSpeed[0] == 13)
                {
                    step = 3;
                }
            }
        }
        else if(step == 3)     // Stop at starting line
        {
            dist = NewDist(vel, dist, timeStep);
            if(dist <= -1500000)
            {
                UARTSend(stop, 2);  // send stop command
                P1OUT |= 0x01;
                while(1)
                {
                  P1OUT ^= 0x03;
                  volatile uint32_t j = 0;
                  for(j = 0; j < 10000; j++);
                }
            }
        }

        __bis_SR_register(LPM1_bits | GIE);
    }
}
