/*
 * tft_Geometry.c
 *
 *  Created on: Apr 13, 2016
 *      Author: dsolano
 */


/*
  UTFT_Geometry.cpp - Add-on Library for UTFT: Geometric functions
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved

  This library adds some geometric functions to UTFT which are not
  included in the main library.

  You can find the latest version of the library at
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the
  examples and tools supplied with the library.
*/

#include <tft_Geometry.h>
#include <ssd1289.h>
#include <math.h>

#define swap(a, b) { int16_t t = a; a = b; b = t; }


void tft_geom_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	tft32_drawLine(x1, y1, x2, y2);
	tft32_drawLine(x2, y2, x3, y3);
	tft32_drawLine(x3, y3, x1, y1);
}

void tft32_geom_fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	int32_t xs, xe;
	int16_t y, ly;

	if (y1 > y2)
	{
		swap(y1, y2);
		swap(x1, x2);
	}
	if (y2 > y3)
	{
		swap(y3, y2);
		swap(x3, x2);
	}
	if (y1 > y2)
	{
		swap(y1, y2);
		swap(x1, x2);
	}

	if(y1 == y3)	// Single line triangles
	{
		xs = xe = x1;
		if(x2 < xs)			xs = x2;
		else if(x2 > xe)	xe = x2;
		if(x3 < xs)			xs = x3;
		else if(x3 > xe)	xe = x3;
		tft32_drawHLine(xs, y1, xe-xs);
		return;
	}

	// Upper part
	if (y2 == y3) ly = y2;
	else          ly = y2-1;

	for(y=y1; y<=ly; y++)
	{
		xs = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		xe = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
		tft32_drawHLine(xs, y, xe-xs);
	}

	// Lower part
	for(; y<=y3; y++)
	{
		xs = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
		xe = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
		tft32_drawHLine(xs, y, xe-xs);
	}
}

void tft32_geom_drawArc(int x, int y, int r, int startAngle, int endAngle, int thickness)
{
	int rDelta = -(thickness/2);
	int px, py, cx, cy;
	int i;
	int d;

	startAngle -= 90;
	endAngle   -= 90;

	if (startAngle!=endAngle)
	{
		for ( i=0; i<thickness; i++)
		{
			px = x + cos((startAngle*3.14)/180) * (r+rDelta+i);
			py = y + sin((startAngle*3.14)/180) * (r+rDelta+i);
			for ( d=startAngle+1; d<endAngle+1; d++)
			{
				cx = x + cos((d*3.14)/180) * (r+rDelta+i);
				cy = y + sin((d*3.14)/180) * (r+rDelta+i);
				tft32_drawLine(px, py, cx, cy);
				px = cx;
				py = cy;
			}
		}
	}
	else
	{
		px = x + cos((startAngle*3.14)/180) * (r+rDelta);
		py = y + sin((startAngle*3.14)/180) * (r+rDelta);
		cx = x + cos((startAngle*3.14)/180) * (r-rDelta);
		cy = y + sin((startAngle*3.14)/180) * (r-rDelta);
		tft32_drawLine(px, py, cx, cy);
	}
}

void tft32_geom_drawPie(int x, int y, int r, int startAngle, int endAngle)
{
	int px, py, cx, cy;
	int d;

	startAngle -= 90;
	endAngle   -= 90;
	if (startAngle>endAngle)
		startAngle -= 360;

	px = x + cos((startAngle*3.14)/180) * r;
	py = y + sin((startAngle*3.14)/180) * r;
	tft32_drawLine(x, y, px, py);
	for ( d=startAngle+1; d<endAngle+1; d++)
	{
		cx = x + cos((d*3.14)/180) * r;
		cy = y + sin((d*3.14)/180) * r;
		tft32_drawLine(px, py, cx, cy);
		px = cx;
		py = cy;
	}
	tft32_drawLine(x, y, px, py);
}
