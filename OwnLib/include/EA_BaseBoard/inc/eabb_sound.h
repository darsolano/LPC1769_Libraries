/*
 * eabb_sound.h
 *
 *  Created on: Oct 22, 2017
 *      Author: dsolano
 */

#ifndef INCLUDE_EA_BASEBOARD_INC_EABB_SOUND_H_
#define INCLUDE_EA_BASEBOARD_INC_EABB_SOUND_H_

#include <chip.h>
#include <lpc_types.h>

extern const char* songs[];
void playSong(const char* song);
void sound_init(void);

#endif /* INCLUDE_EA_BASEBOARD_INC_EABB_SOUND_H_ */
