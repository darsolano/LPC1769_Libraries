/*
 * myio_board.h
 *
 *  Created on: 30/1/2015
 *      Author: dsolano
 */

#ifndef INCLUDE_MYIO_BOARD_H_
#define INCLUDE_MYIO_BOARD_H_

#include <chip.h>
#include <lpc_types.h>

#define PushButtons			LPC_GPIO2

#define PB0					0	//2.10
#define PB1					1	//2.11
#define PB2					2	//2.12
#define PB3					3	//2.13

#define RGB_LEDS			LPC_GPIO0

#define RGB_RED				1	//0.2
#define RGB_BLUE			2	//0.3
#define RGB_GREEN			4	//0.21

#define LED_INDICATOR		LPC_GPIO1

#define LEDI_RED			1		//1.26
#define LEDI_BLUE			2		//1.27
#define LEDI_GREEN			4		//1.28
#define LEDI_YELLOW			8		//1.29

#define LED_BAR				LPC_GPIO1	//1.18 ... 1.25

typedef enum
{
	INPUT = 0, //Controlled pin is input.
	OUTPUT = 1 //Controlled pin is output.
}GPIODIR_Typedef;


__attribute__ ( ( naked ) ) void myio_board_init (void);

#endif /* INCLUDE_MYIO_BOARD_H_ */
