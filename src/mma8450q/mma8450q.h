/*
 *  MMA8450Q.h
 *  Register definitions and function prototypes for interfacing with
 *  the Freescale (now NXP) MMA8450Q 3 axis accelerometer
 *
 *  Datasheet: http://www.nxp.com/assets/documents/data/en/data-sheets/MMA8450Q.pdf
 *
 *  Version 1: 04/01/17 - Nathan Duprey
 *                      - Added register addresses
 *             04/03/17 - added CTRL_REG1 bit definitions
 */

#ifndef MMA8450Q_H_
#define MMA8450Q_H_

#include "stdint.h"

// Register definitions from Table 11 in datasheet
#define MMA_STATUS  0x00
#define OUT_X_LSB   0x05
#define OUT_X_MSB   0x06
#define OUT_Y_LSB   0x07
#define OUT_Y_MSB   0x08
#define OUT_Z_LSB   0x09
#define OUT_Z_MSB   0x0A
#define OUT_X_DELTA 0x0C
#define OUT_Y_DELTA 0x0D
#define OUT_Z_DELTA 0x0E
#define WHO_AM_I    0x0F
#define F_STATUS    0x10
#define F_8DATA     0x11
#define F_12DATA    0x12
#define F_SETUP     0x13
#define SYSMOD      0x14
#define INT_SOURCE  0x15
#define XYZ_DATA_CFG        0x16
#define HP_FILTER_CUTOFF    0x17
#define PL_STATUS           0x18
#define PL_PRE_STATUS       0x19
#define PL_CFG              0x1A
#define PL_COUNT            0x1B
#define PL_BF_ZCOMP         0x1C
#define PL_P_L_THS_REG1     0x1D
#define PL_P_L_THS_REG2     0x1E
#define PL_P_L_THS_REG3     0x1F
#define PL_L_P_THS_REG1     0x20
#define PL_L_P_THS_REG2     0x21
#define PL_L_P_THS_REG3     0x22
#define FF_MT_CGF_1         0x23
#define FF_MT_SRC_1         0x24
#define FF_MT_THS_1         0x25
#define FF_MT_COUNT_1       0x26
#define FF_MT_CGF_2         0x27
#define FF_MT_SRC_2         0x28
#define FF_MT_THS_2         0x29
#define FF_MT_COUNT_2       0x2A
#define TRANSIENT_CFG       0x2B
#define TRANSIENT_SRC       0x2C
#define TRANSIENT_THS       0x2D
#define TRANSIENT_COUNT     0x2E
#define PULSE_CFG   0x2F
#define PULSE_SRC   0x30
#define PULSE_THSX  0x31
#define PULSE_THSY  0x32
#define PULSE_THSZ  0x33
#define PULSE_TMLT  0x34
#define PULSE_LTCY  0x35
#define PULSE_WIND  0x36
#define ASLP_COUNT  0x37
#define CTRL_REG1   0x38
#define CTRL_REG2   0x39
#define CTRL_REG3   0x3A
#define CTRL_REG4   0x3B
#define CTRL_REG5   0x3C
#define OFF_X       0x3D
#define OFF_Y       0x3E
#define OFF_Z       0x3F


// STATUS register bit definitions
#define ZYXOW   0x80
#define ZOW     0x40
#define YOW     0x20
#define XOW     0x10
#define ZYXDR   0x08
#define ZDR     0x04
#define YDR     0x02
#define XDR     0x01


// XYZ_DATA_CFG bit definitions
#define FDE     0x80
#define ZDEFE   0x04
#define YDEFE   0x02
#define XDEFE   0x01


// CTRL_REG1 bit definitions
#define ASLP_RATE1_BIT  0x80
#define ASLP_RATE0_BIT  0x40
#define DR2_BIT         0x10
#define DR1_BIT         0x08
#define DR0_BIT         0x04
#define FS1_BIT         0x02
#define FS0_BIT         0x01
// sleep mode poll rate selection
#define ASLP_RATE_50    0x00
#define ASLP_RATE_25    ASLP_RATE0_BIT
#define ASLP_RATE_12_5  ASLP_RATE1_BIT
#define ASLP_RATE_1_56  (ASLP_RATE1_BIT | ASLP_RATE0_BIT)
// system output data rate
#define DATA_RATE_400   0x00
#define DATA_RATE_200   DR0_BIT
#define DATA_RATE_100   DR1_BIT
#define DATA_RATE_50    (DR1_BIT | DR0_BIT)
#define DATA_RATE_12_5  DR2_BIT
#define DATA_RATE_1_56  (DR2_BIT | DR0_BIT)
// full scale selection
#define FS_STANDBY      0x00
#define FS_2G           FS0_BIT
#define FS_4G           FS1_BIT
#define FS_8G           (FS1_BIT | FS0_BIT)



// function prototypes
void MMA8450Init(void);
uint8_t MMA8450ReadXYZ(int16_t * retData);
void MMA8450SetZero();

#endif
