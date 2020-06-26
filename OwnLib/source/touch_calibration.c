/*
 * touch_calibration.c
 *
 *  Created on: Apr 14, 2016
 *      Author: dsolano
 */

// UTouch_Calibration
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program can be used to calibrate the touchscreen
// of the display modules.
// This program requires the UTFT library and a touch
// screen module that is compatible with UTFT.
//
// It is assumed that the display module is connected to an
// appropriate shield or that you know how to change the pin
// numbers in the setup.
//
// Instructions will be given on the display.
//
#include <ssd1289.h>
#include <tftTouch.h>
#include <stdlib.h>
#include <hardware_delay.h>

// Define the orientation of the touch screen. Further
// information can be found in the instructions.
#define TOUCH_ORIENTATION  LANDSCAPE//PORTRAIT LANDSCAPE

// Initialize display
// ------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Uno/2009 Shield            : <display model>,19,18,17,16
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Due       : <display model>,25,26,27,28
// Teensy 3.x TFT Test Board                   : <display model>,23,22, 3, 4
// ElecHouse TFT LCD/SD Shield for Arduino Due : <display model>,22,23,31,33
//
// Remember to change the model parameter to suit your display module!
//UTFT    myGLCD(ITDB32S,38,39,40,41);

// Initialize touchscreen
// ----------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Uno/2009 Shield            : 15,10,14, 9, 8
// Standard Arduino Mega/Due shield            :  6, 5, 4, 3, 2
// CTE TFT LCD/SD Shield for Arduino Due       :  6, 5, 4, 3, 2
// Teensy 3.x TFT Test Board                   : 26,31,27,28,29
// ElecHouse TFT LCD/SD Shield for Arduino Due : 25,26,27,29,30
//
//UTouch  myTouch( 6, 5, 4, 3, 2);

// ************************************
// DO NOT EDIT ANYTHING BELOW THIS LINE
// ************************************

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t SmallFont_Param[];
void fail(void);

uint32_t cx, cy;
uint32_t rx[8], ry[8];
uint32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center;
uint32_t calx, caly, cals;
char buf[13];

void setup(void)
{
	tft32_PINS();
	tft32_hdw_reset();
	tft32_InitLCD();
	tft32_backlight_on();
	tft32_setFont(SmallFont, SmallFont_Param);
	tft32_set_orientation(TOUCH_ORIENTATION);

	touch_InitTouch(TOUCH_ORIENTATION);
	dispx = tft32_getDisplayXSize();
	dispy = tft32_getDisplayYSize();
	text_y_center = (dispy / 2) - 6;

}

void drawCrossHair(int x, int y)
{
	tft32_drawRect(x - 10, y - 10, x + 10, y + 10);
	tft32_drawLine(x - 5, y, x + 5, y);
	tft32_drawLine(x, y - 5, x, y + 5);
}

void readCoordinates()
{
	int iter = 5000;
	int failcount = 0;
	int cnt = 0;
	uint32_t tx = 0;
	uint32_t ty = 0;
	Bool OK = false;

	while (OK == false)
	{
		tft32_setColor(WHITE);
		tft32_print("*  PRESS  *", CENTER, text_y_center, 0);
		while (touch_dataAvailable() == false)
		{
		}
		_delay_uS(100);
		tft32_print("*  HOLD!  *", CENTER, text_y_center, 0);
		while ((touch_dataAvailable() == true) && (cnt < iter)
				&& (failcount < 10000))
		{
			_delay_uS(100);
			touch_calibrateRead();
			if (!((touch.TP_X == 65535) || (touch.TP_Y == 65535)))
			{
				tx += touch.TP_X;
				ty += touch.TP_Y;
				cnt++;
			}
			else
				failcount++;
		}
		if (cnt >= iter)
		{
			OK = true;
		}
		else
		{
			tx = 0;
			ty = 0;
			cnt = 0;
		}
		if (failcount >= 10000)
			fail();
	}

	cx = tx / iter;
	cy = ty / iter;
}

void calibrate(int x, int y, int i)
{
	tft32_setColor(WHITE);
	drawCrossHair(x, y);
	tft32_setBackColor(RED);
	readCoordinates();
	tft32_setColor(WHITE);
	tft32_print("* RELEASE *", CENTER, text_y_center, 0);
	tft32_setColor(VERY_DARK_GRAY);
	drawCrossHair(x, y);
	rx[i] = cx;
	ry[i] = cy;
	while (touch_dataAvailable() == true)
	{
	}
	_delay_uS(100);
}

void waitForTouch()
{
	while (touch_dataAvailable() == true)
	{
	}
	while (touch_dataAvailable() == false)
	{
	}
	while (touch_dataAvailable() == true)
	{
	}
	_delay_uS(100);
}

void toHex(uint32_t num)
{
	int zz;
	buf[0] = '0';
	buf[1] = 'x';
	buf[10] = 'U';
	buf[11] = 'L';
	buf[12] = 0;
	for (zz = 9; zz > 1; zz--)
	{
		if ((num & 0xF) > 9)
			buf[zz] = (num & 0xF) + 55;
		else
			buf[zz] = (num & 0xF) + 48;
		num = num >> 4;
	}
}

void startup(void)
{
	tft32_clrScr(BLACK);
	tft32_setColor(VGA_RED);
	tft32_fillRect(0, 0, dispx - 1, 13);
	tft32_setColor(VGA_WHITE);
	tft32_setBackColor(VGA_RED);
	tft32_drawLine(0, 14, dispx - 1, 14);
	tft32_print("UTouch Calibration", CENTER, 1, 0);
	tft32_setBackColor(VGA_BLACK);

	if (dispx == T_X_SIZE)
	{
		tft32_print("Use a stylus or something", LEFT, 30, 0);
		tft32_print("similar to touch as close", LEFT, 42, 0);
		tft32_print("to the center of the", LEFT, 54, 0);
		tft32_print("highlighted crosshair as", LEFT, 66, 0);
		tft32_print("possible. Keep as still as", LEFT, 78, 0);
		tft32_print("possible and keep holding", LEFT, 90, 0);
		tft32_print("until the highlight is", LEFT, 102, 0);
		tft32_print("removed. Repeat for all", LEFT, 114, 0);
		tft32_print("crosshairs in sequence.", LEFT, 126, 0);
		tft32_print("Touch screen to continue", CENTER, 162, 0);
	}
	else
	{
		tft32_print("INSTRUCTIONS", CENTER, 30, 0);
		tft32_print("Use a stylus or something similar to", LEFT, 50, 0);
		tft32_print("touch as close to the center of the", LEFT, 62, 0);
		tft32_print("highlighted crosshair as possible. Keep", LEFT, 74, 0);
		tft32_print("as still as possible and keep holding", LEFT, 86, 0);
		tft32_print("until the highlight is removed. Repeat", LEFT, 98, 0);
		tft32_print("for all crosshairs in sequence.", LEFT, 110, 0);

		tft32_print("Further instructions will be displayed", LEFT, 134, 0);
		tft32_print("when the calibration is complete.", LEFT, 146, 0);

		tft32_print("Do NOT use your finger as a calibration", LEFT, 170, 0);
		tft32_print("stylus or the result WILL BE imprecise.", LEFT, 182, 0);

		tft32_print("Touch screen to continue", CENTER, 226, 0);
	}

	waitForTouch();
	tft32_clrScr(VGA_BLACK);
}

void done()
{
	tft32_clrScr(VGA_BLACK);
	tft32_setColor(VGA_RED);
	tft32_fillRect(0, 0, dispx - 1, 13);
	tft32_setColor(VGA_WHITE);
	tft32_setBackColor(VGA_RED);
	tft32_drawLine(0, 14, dispx - 1, 14);
	tft32_print("UTouch Calibration", CENTER, 1, 0);
	tft32_setBackColor(VGA_BLACK);

	if (dispx == T_X_SIZE)
	{
		tft32_print("To use the new calibration", LEFT, 30, 0);
		tft32_print("settings you must edit the", LEFT, 42, 0);
		tft32_setColor(KIND_OF_BLUE);
		tft32_print("tftTouch.h", LEFT, 54, 0);
		tft32_setColor(VGA_WHITE);
		tft32_print("file and change", 88, 54, 0);
		tft32_print("the following values. The", LEFT, 66, 0);
		tft32_print("values are located right", LEFT, 78, 0);
		tft32_print("below the opening comment.", LEFT, 90, 0);
		tft32_print("CAL_X", LEFT, 110, 0);
		tft32_print("CAL_Y", LEFT, 122, 0);
		tft32_print("CAL_S", LEFT, 134, 0);
		toHex(calx);
		tft32_print(buf, 75, 110, 0);
		toHex(caly);
		tft32_print(buf, 75, 122, 0);
		toHex(cals);
		tft32_print(buf, 75, 134, 0);
	}
	else
	{
		tft32_print("CALIBRATION COMPLETE", CENTER, 30, 0);
		tft32_print("To use the new calibration", LEFT, 50, 0);
		tft32_print("settings you must edit the", LEFT, 62, 0);
		tft32_setColor(KIND_OF_BLUE);
		tft32_print("tftTouch.h", LEFT, 74, 0);
		tft32_setColor(VGA_WHITE);
		tft32_print("file and change", 88, 74, 0);
		tft32_print("the following values.", LEFT, 86, 0);
		tft32_print("The values are located right", LEFT, 98, 0);
		tft32_print("below the opening comment in", LEFT, 110, 0);
		tft32_print("the file.", LEFT, 122, 0);
		tft32_print("CAL_X", LEFT, 150, 0);
		tft32_print("CAL_Y", LEFT, 162, 0);
		tft32_print("CAL_S", LEFT, 174, 0);

		toHex(calx);
		tft32_print(buf, 75, 150, 0);
		toHex(caly);
		tft32_print(buf, 75, 162, 0);
		toHex(cals);
		tft32_print(buf, 75, 174, 0);
	}
}

void fail(void)
{
	tft32_clrScr(BLACK);
	tft32_setColor(RED);
	tft32_fillRect(0, 0, dispx - 1, 13);
	tft32_setColor(WHITE);
	tft32_setBackColor(RED);
	tft32_drawLine(0, 14, dispx - 1, 14);
	tft32_print("UTouch Calibration FAILED", CENTER, 1, 0);
	tft32_setBackColor(BLACK);

	tft32_print("Unable to read the position", LEFT, 30, 0);
	tft32_print("of the press. This is a", LEFT, 42, 0);
	tft32_print("hardware issue and can", 88, 54, 0);
	tft32_print("not be corrected in", LEFT, 66, 0);
	tft32_print("software.", LEFT, 78, 0);

	while (true)
	{
	};
}

void touch_calibrate(void)
{
	startup();

	tft32_setColor(VERY_DARK_GRAY);
	drawCrossHair(dispx - 11, 10);
	drawCrossHair(dispx / 2, 10);
	drawCrossHair(10, 10);
	drawCrossHair(dispx - 11, dispy / 2);
	drawCrossHair(10, dispy / 2);
	drawCrossHair(dispx - 11, dispy - 11);
	drawCrossHair(dispx / 2, dispy - 11);
	drawCrossHair(10, dispy - 11);
	tft32_setColor(WHITE);
	tft32_setBackColor(RED);
	tft32_print("***********", CENTER, text_y_center - 12, 0);
	tft32_print("***********", CENTER, text_y_center + 12, 0);

	calibrate(10, 10, 0);
	calibrate(10, dispy / 2, 1);
	calibrate(10, dispy - 11, 2);
	calibrate(dispx / 2, 10, 3);
	calibrate(dispx / 2, dispy - 11, 4);
	calibrate(dispx - 11, 10, 5);
	calibrate(dispx - 11, dispy / 2, 6);
	calibrate(dispx - 11, dispy - 11, 7);

	if (TOUCH_ORIENTATION == LANDSCAPE)
		cals = ((long) (dispx - 1) << 12) + (dispy - 1);
	else
		cals = ((long) (dispy - 1) << 12) + (dispx - 1);

	if (TOUCH_ORIENTATION == PORTRAIT)
		px = abs(
				(((float) (rx[2] + rx[4] + rx[7]) / 3)
						- ((float) (rx[0] + rx[3] + rx[5]) / 3))
						/ (dispy - 20));  // PORTRAIT
	else
		px = abs(
				(((float) (rx[5] + rx[6] + rx[7]) / 3)
						- ((float) (rx[0] + rx[1] + rx[2]) / 3))
						/ (dispy - 20));  // LANDSCAPE

	if (TOUCH_ORIENTATION == PORTRAIT)
	{
		clx = (((rx[0] + rx[3] + rx[5]) / 3));  // PORTRAIT
		crx = (((rx[2] + rx[4] + rx[7]) / 3));  // PORTRAIT
	}
	else
	{
		clx = (((rx[0] + rx[1] + rx[2]) / 3));  // LANDSCAPE
		crx = (((rx[5] + rx[6] + rx[7]) / 3));  // LANDSCAPE
	}
	if (clx < crx)
	{
		clx = clx - (px * 10);
		crx = crx + (px * 10);
	}
	else
	{
		clx = clx + (px * 10);
		crx = crx - (px * 10);
	}

	if (TOUCH_ORIENTATION == PORTRAIT)
		py = abs(
				(((float) (ry[5] + ry[6] + ry[7]) / 3)
						- ((float) (ry[0] + ry[1] + ry[2]) / 3))
						/ (dispx - 20));  // PORTRAIT
	else
		py = abs(
				(((float) (ry[0] + ry[3] + ry[5]) / 3)
						- ((float) (ry[2] + ry[4] + ry[7]) / 3))
						/ (dispx - 20));  // LANDSCAPE

	if (TOUCH_ORIENTATION == PORTRAIT)
	{
		cty = (((ry[5] + ry[6] + ry[7]) / 3));  // PORTRAIT
		cby = (((ry[0] + ry[1] + ry[2]) / 3));  // PORTRAIT
	}
	else
	{
		cty = (((ry[0] + ry[3] + ry[5]) / 3));  // LANDSCAPE
		cby = (((ry[2] + ry[4] + ry[7]) / 3));  // LANDSCAPE
	}
	if (cty < cby)
	{
		cty = cty - (py * 10);
		cby = cby + (py * 10);
	}
	else
	{
		cty = cty + (py * 10);
		cby = cby - (py * 10);
	}

	calx = ((long) (clx) << 14) + (long) (crx);
	caly = ((long) (cty) << 14) + (long) (cby);
	if (TOUCH_ORIENTATION == LANDSCAPE)
		cals = cals + (1L << 31);

	done();
	while (true)
	{
	}
}
