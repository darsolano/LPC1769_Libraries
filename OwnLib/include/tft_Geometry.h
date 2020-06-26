/*
 * tft_Geometry.h
 *
 *  Created on: Apr 13, 2016
 *      Author: dsolano
 */

#ifndef INCLUDE_TFT_GEOMETRY_H_
#define INCLUDE_TFT_GEOMETRY_H_

void	tft32_geom_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
void	tft32_geom_fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
void	tft32_geom_drawArc(int x, int y, int r, int startAngle, int endAngle, int thickness);
void	tft32_geom_drawPie(int x, int y, int r, int startAngle, int endAngle);

#endif /* INCLUDE_TFT_GEOMETRY_H_ */
