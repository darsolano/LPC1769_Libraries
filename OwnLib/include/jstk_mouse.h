/*
 * jstk_mouse.h
 * Create a library to use EABAseBoard joystick to create a mouse pointer and selector
 * with functions as hoover, click-on, click-off
 *  Created on: Apr 17, 2016
 *      Author: dsolano
 */

#ifndef INCLUDE_JSTK_MOUSE_H_
#define INCLUDE_JSTK_MOUSE_H_

#include <lpc_types.h>

#define TFT32
//#define OLED
typedef enum
{
	JS_NONE 	= 0x00,
	JS_UP 		= 0x01,
	JS_DOWN 	= 0x02,
	JS_RIGHT	= 0x04,
	JS_LEFT	= 0x08,
	JS_CENTER	= 0x10
}JOYSTICK_STATE_e;


/*
 * Prototypes functions
 */



#endif /* INCLUDE_JSTK_MOUSE_H_ */
