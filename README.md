# shuttle-bot
This repository has the code for our final project for EE4735 - Embedded Systems
Engineering. The project is called shuttle-bot and uses only an accelerometer as
input to control a robot. The robot is required to traverse the distance between
two lines in a straight hallway.

Basic operation of the robot is as follows:
1. Starting from the start line, drive to the finish line and stop within 0.5 meters.
2. Wait for a few seconds.
3. Back up to the start line and stop within 1 meter of the start line.

## Hardware
Hardware for this project is fairly minimal. The control board is a Texas
Instruments [EZ430-RF2500 development board][ez430-link] with a MSP430F2274 MCU.
It also includes a CC2500 RF transceiver but that is unused for this project.

The accelerometer is a [MMA8450Q][mma8450-link] 3-axis, 8/12-bit accelerometer
from NXP/Freescale. It has selectable ±2g/±4g/±8g ranges, an output data rate up
to 400 Hz, and two interrupt pins that can be assigned to any of eight different
interrupt sources.

The robot platform started as a [4WD1 Rover Kit][4wd1-link] from Lynxmotion but
has had additional features added by MTU's ECE department. Specifically, a LiPo
battery, rotary encoders, and a [Sabertooth 2x10][sabertooth-link] motor
controller have been added. The MSP430 communicates with the motor controller
using a basic serial protocol.


[ez430-link]: http://www.ti.com/tool/ez430-rf2500
[mma8450-link]: http://www.nxp.com/products/sensors/accelerometers/3-axis-accelerometers/2g-4g-8g-low-g-digital-accelerometer:MMA8450Q
[4wd1-link]: http://www.lynxmotion.com/p-603-aluminum-4wd1-rover-kit.aspx
[sabertooth-link]: https://www.dimensionengineering.com/products/sabertooth2x10
