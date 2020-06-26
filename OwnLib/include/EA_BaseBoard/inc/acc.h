/*****************************************************************************
 *   light.h:  Header file for ISL29003 Light-to-Digital Output Sensor
 *
 *   Copyright(C) 2009, Embedded Artists AB
 *   All rights reserved.
 *
******************************************************************************/
#ifndef __ACC_H
#define __ACC_H

#include <chip.h>

typedef enum
{
    ACC_MODE_STANDBY,
    ACC_MODE_MEASURE,
    ACC_MODE_LEVEL, /* level detection */
    ACC_MODE_PULSE, /* pulse detection */
} acc_mode_t;

typedef enum
{
    ACC_RANGE_8G,
    ACC_RANGE_2G,
    ACC_RANGE_4G,
} acc_range_t;

typedef struct acc
{
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t x_off;
	uint8_t y_off;
	uint8_t z_off;
	double x_g;
	double y_g;
	double z_g;
	acc_range_t G_range;
}ACC_READ_t;

void acc_init (ACC_READ_t *acc);

void 	acc_read (ACC_READ_t *acc);
void 	acc_setRange(acc_range_t range);
void 	acc_setMode(acc_mode_t mode);
uint8_t acc_getRange(void);



#endif /* end __LIGHT_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
