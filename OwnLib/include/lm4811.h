/*
 * lm4811.h
 *
 *  Created on: Dec 31, 2018
 *      Author: dsolano
 */

#ifndef INCLUDE_LM4811_H_
#define INCLUDE_LM4811_H_

#include <chip.h>

// states must be ored for use
typedef enum{
	NORMAL   	= 0x01,
	POWER_UP 	= 0x02,
	SHUTDOWN 	= 0x04,
	WAKEUP   	= 0x08,
	VOLUME_UP 	= 0x10,
	VOLUME_DOWN = 0x20
}LM4811_STATE_t;

typedef struct{
	LM4811_STATE_t state;
	uint8_t volume_power;
	int current_volume;
	int steps_up_from_zero;
	int setps_down_from_zero;
	void* stream_sound;
}LM4811_t;


#endif /* INCLUDE_LM4811_H_ */
