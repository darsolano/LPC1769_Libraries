/*
 * hmc5882l.h	Magnetometer / Digital Compass - GY-271
 *
 *  Created on: Nov 7, 2015
 *      Author: dsolano
 */

#ifndef INCLUDE_HMC5883L_H_
#define INCLUDE_HMC5883L_H_

#include <chip.h>

#define HMC5883L_I2C_BUS			I2C0
#define HMC5883L_ADDRESS            0x1E // this device only has one address
#define HMC5883L_DEFAULT_ADDRESS    (HMC5883L_ADDRESS<<1)
#define HMC5883L_I2C_RATE			400000


#define HMC5883L_RA_CONFIG_A        0x00
#define HMC5883L_RA_CONFIG_B        0x01
#define HMC5883L_RA_MODE            0x02
#define HMC5883L_RA_DATAX_H         0x03
#define HMC5883L_RA_DATAX_L         0x04
#define HMC5883L_RA_DATAY_H         0x05
#define HMC5883L_RA_DATAY_L         0x06
#define HMC5883L_RA_DATAZ_H         0x07
#define HMC5883L_RA_DATAZ_L         0x08
#define HMC5883L_RA_STATUS          0x09
#define HMC5883L_RA_ID_A            0x0A
#define HMC5883L_RA_ID_B            0x0B
#define HMC5883L_RA_ID_C            0x0C

#define HMC5883L_CRA_AVERAGE_BIT    6
#define HMC5883L_CRA_AVERAGE_LENGTH 2
#define HMC5883L_CRA_RATE_BIT       4
#define HMC5883L_CRA_RATE_LENGTH    3
#define HMC5883L_CRA_BIAS_BIT       1
#define HMC5883L_CRA_BIAS_LENGTH    2

#define HMC5883L_AVERAGING_1        0x00
#define HMC5883L_AVERAGING_2        0x01
#define HMC5883L_AVERAGING_4        0x02
#define HMC5883L_AVERAGING_8        0x03

#define HMC5883L_RATE_0P75          0x00
#define HMC5883L_RATE_1P5           0x01
#define HMC5883L_RATE_3             0x02
#define HMC5883L_RATE_7P5           0x03
#define HMC5883L_RATE_15            0x04
#define HMC5883L_RATE_30            0x05
#define HMC5883L_RATE_75            0x06

#define HMC5883L_BIAS_NORMAL        0x00
#define HMC5883L_BIAS_POSITIVE      0x01
#define HMC5883L_BIAS_NEGATIVE      0x02

#define HMC5883L_CRB_GAIN_BIT       7
#define HMC5883L_CRB_GAIN_LENGTH    3

#define HMC5883L_GAIN_1370          0x00
#define HMC5883L_GAIN_1090          0x01
#define HMC5883L_GAIN_820           0x02
#define HMC5883L_GAIN_660           0x03
#define HMC5883L_GAIN_440           0x04
#define HMC5883L_GAIN_390           0x05
#define HMC5883L_GAIN_330           0x06
#define HMC5883L_GAIN_220           0x07

#define HMC5883L_MODEREG_BIT        1
#define HMC5883L_MODEREG_LENGTH     2

#define HMC5883L_MODE_CONTINUOUS    0x00
#define HMC5883L_MODE_SINGLE        0x01
#define HMC5883L_MODE_IDLE          0x02

#define HMC5883L_STATUS_LOCK_BIT    1
#define HMC5883L_STATUS_READY_BIT   0

typedef struct{
	uint16_t x_heading;
	uint16_t y_heading;
	uint16_t z_heading;
}AXIS_HEADING_t;

void HMC5883L_Initialize(void);
bool HMC5883L_TestConnection(void);

// CONFIG_A register
uint8_t HMC5883L_GetSampleAveraging(void);
void 	HMC5883L_SetSampleAveraging(uint8_t averaging);
uint8_t HMC5883L_GetDataRate(void);
void 	HMC5883L_SetDataRate(uint8_t rate);
uint8_t HMC5883L_GetMeasurementBias(void);
void 	HMC5883L_SetMeasurementBias(uint8_t bias);

// CONFIG_B register
uint8_t HMC5883L_GetGain();
void 	HMC5883L_SetGain(uint8_t gain);

// MODE register
uint8_t HMC5883L_GetMode();
void 	HMC5883L_SetMode(uint8_t mode);

// DATA* registers
void HMC5883L_GetHeading(AXIS_HEADING_t* Mag);

// STATUS register
Bool 	HMC5883L_GetLockStatus(void);
Bool 	HMC5883L_GetReadyStatus(void);


#endif /* INCLUDE_HMC5883L_H_ */
