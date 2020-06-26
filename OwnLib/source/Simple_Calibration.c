/*
 * Simple_Calibration.c
 *
 *  Created on: Apr 18, 2016
 *      Author: dsolano
 */


// UTouch_QuickDraw (C)2010-2012 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics

//Change "selector" to the # of the program you want to run:
//1: Map out the display coordinates
//2: Map out the touch screen coordinates

#define selector 0

#include <ssd1289.h>
#include <tftTouch.h>
#include <hardware_delay.h>
extern uint8_t SmallFont[];
extern uint8_t SmallFont_Param[];


long x, y;

void setup_simple_calib(void)
{
	tft32_PINS();
	tft32_hdw_reset();
	tft32_InitLCD();
	tft32_backlight_on();
	tft32_setFont(SmallFont, SmallFont_Param);
	tft32_set_orientation(LANDSCAPE);

	touch_InitTouch(LANDSCAPE);
	touch_setPrecision(PREC_LOW);
}

void touch_simple_calib(void)
{
	while(1)
	{
		tft32_setColor(WHITE);
		y=0;
		for (x=0; x <= 320; x=x+5){ // change the 320 to your x resolution
			 tft32_fillScr(BLACK); //clear the screen
			 tft32_printNumI(x,x,100);
			// _delay(dly_10ms);
		}
		for (y=0; y <= 240; y=y+5){ //change the 240 to your y resolution
			 tft32_fillScr(BLACK); //clear the screen
			 tft32_printNumI(y,100,y);
			 //_delay(dly_10ms);
		}

		while (touch_dataAvailable() != true);
		 _delay_ms(50);  //This delay is crucial!!!
		 touch_read();
		 x = touch_getTouchX();
		 y = touch_getTouchY();

		 if (x!=-1 && y!=-1){
			 tft32_fillScr(BLACK); //clear the screen
			 tft32_printNumI((int)x,100,120); //print the X location of the touch at 100,120
			 tft32_printNumI((int)y,100,150); //print the Y location of the touch at 100,150
		 }
		 while (touch_dataAvailable() != true);


//		while (touch_dataAvailable() == true){
//		 _delay(dly_10ms*5);  //This delay is crucial!!!
//		 touch_read();
//		 x = touch_getX(); //scales and offsets in the .cpp file
//		 y = touch_getY(); //scales and offsets in the .cpp file
//
//		 if (x!=-1 && y!=-1){
//			 tft32_fillScr(BLACK); //clear the screen
//			 tft32_printNumI((int)x,x,120); //print the X location of the touch at x,120
//			 tft32_printNumI((int)y,100,y); //print the Y location of the touch at 100,y
//		 }
//		}
//
//		while (touch_dataAvailable() == true){
//		   _delay(dly_10ms*5);  //This delay is crucial!!!
//		 touch_read();
//		 x = touch_getX(); //scales and offsets in the .cpp file
//		 y = touch_getY(); //scales and offsets in the .cpp file
//
//		 if (x!=-1 && y!=-1){
//		   tft32_drawPixel (x, y);
//		 }
//		}
	}
while(1);
}
