/*
 * tftTouch.h
 *
 *  Created on: Oct 13, 2015
 *      Author: dsolano
 */

#ifndef INCLUDE_TFTTOUCH_H_
#define INCLUDE_TFTTOUCH_H_

#include <chip.h>

/* Display coordinates:
 *  [x,y]
 *     [0,319]                                 [0,0]
 *       -----------------------------------------
 *       |                                       |
 *       |                                       |
 *       |                                       |
 *       |                                       |
 *       |             TOUCH DISPLAY             |
 *       |                                       |
 *       |                                       |
 *       |                                       |
 *       |                                       |
 *       -----------------------------------------
 *   [239,319]                               [239,0]
 */

// Since there are slight deviations in all touch screens you should run a
// calibration on your display module. Run the UTouch_Calibration sketch
// that came with this library and follow the on-screen instructions to
// update this file.
//
// Remember that is you have multiple display modules they will probably
// require different calibration data so you should run the calibration
// every time you switch to another module.
// You can, of course, store calibration data for all your modules here
// and comment out the ones you dont need at the moment.
//

// These calibration settings works with my ITDB02-3.2S.
// They MIGHT work on your 320x240 display module, but you should run the
// calibration sketch anyway. If you are using a display with any other
// resolution you MUST calibrate it as these settings WILL NOT work.
//portrait
#define CAL_X 0x00FC83E5UL
#define CAL_Y 0x011E03EEUL
#define CAL_S 0x8013F0EFUL



#define PORTRAIT	0
#define LANDSCAPE	1

#define PREC_LOW		1
#define PREC_MEDIUM		2
#define PREC_HI			3
#define PREC_EXTREME	4
#define T_X_SIZE		240
#define T_Y_SIZE		320
typedef uint8_t byte;

typedef struct touch_data_s
{
	uint16_t	TP_X;
	uint16_t    TP_Y;
}TDATA_t;
TDATA_t touch;

void		touch_InitTouch(uint8_t orient);
void		touch_read(void);
Bool		touch_dataAvailable(void);
int			touch_getX(void);
int			touch_getY(void);
void		touch_setPrecision(uint8_t prec);
void 		touch_calibrateRead(void);
int16_t 	touch_getTouchY(void);	//inserted new
int16_t 	touch_getTouchX(void);	// inserted new
#endif /* INCLUDE_TFTTOUCH_H_ */
