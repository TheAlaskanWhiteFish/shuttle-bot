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

const uint16_t forward[] = {96, 234};      // preset motor commands
const uint16_t stop[] = {0, 0};
const uint16_t reverse[] = {32, 160};

#pragma vector=TIMERA1_VECTOR
#pragma type_attribute=__interrupt
void TimerA1Interrupt(void)
{
  int32_t data[3];
  volatile int8_t xaccel;
  int16_t vel = 0;
  int16_t dist = 0;
  int8_t t = 100;
  int8_t step = 0;
    switch(__even_in_range(TAIV, 10))
    {
        case TAIV_TAIFG:
            MMA8450ReadXYZ(int16_t *data);      // Read accelerometer
            xaccel = data[0] * 10;              // Convert x to mm/s^2 and store
            vel = NewVel(accel, vel, t);        // Find velocity and distance
            dist = NewDist(vel, t, dist);
            
            if(dist > 1000 && step == 0)        // Stop at 1 meter
            {
              UARTSend(*stop, 2);
              step = 1;
              for (i = 0; i < 12000; i++){};    // Wait 1 sec before reversing
              UARTSend(*reverse, 2);
            }
            else if(dist < 0 && step == 1)      // Stop at starting line
            {
              UARTSend(*stop, 2);
            }
            
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
        MMA8450ReadXYZ(accelData);
        uint8_t whoAmI = I2CReadRegister(WHO_AM_I);

        uint8_t charMap[] = "0123456789ABCDEF";

        UARTSend("\fX: 0x", 6);
        UARTSendByte(charMap[(accelData[0] & 0x0F00) >> 8]);
        UARTSendByte(charMap[(accelData[0] & 0x00F0) >> 4]);
        UARTSendByte(charMap[accelData[0] & 0x000F]);

        UARTSend("\r\nY: 0x", 7);
        UARTSendByte(charMap[(accelData[1] & 0x0F00) >> 8]);
        UARTSendByte(charMap[(accelData[1] & 0x00F0) >> 4]);
        UARTSendByte(charMap[accelData[1] & 0x000F]);

        UARTSend("\r\nZ: 0x", 7);
        UARTSendByte(charMap[(accelData[2] & 0x0F00) >> 8]);
        UARTSendByte(charMap[(accelData[2] & 0x00F0) >> 4]);
        UARTSendByte(charMap[accelData[2] & 0x000F]);

        UARTSend("\r\nwho: ", 7);
        UARTSendByte(charMap[(whoAmI & 0xF0) >> 4]);
        UARTSendByte(charMap[whoAmI & 0x0F]);


        P1OUT &= ~0x02;
        __bis_SR_register(CPUOFF | GIE);
    }
}
