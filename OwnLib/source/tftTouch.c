/*
 * tftTouch.c
 *
 *  Created on: Oct 13, 2015
 *      Author: dsolano
 */

#include <tftTouch.h>
#include <define_pins.h>
#include <hardware_delay.h>

#define TOUCHSCREEN			LPC_GPIO1
DEFINE_PIN(T_CLK,1,24)
DEFINE_PIN(T_CS,1,25)
DEFINE_PIN(T_DOUT,1,26)
DEFINE_PIN(T_BUSY,1,27)
DEFINE_PIN(T_DIN,1,28)
DEFINE_PIN(T_PENIRQ,1,29)

struct touch_panel_s
{
	long	_default_orientation;
	byte	orient;
	byte	prec;
	byte	display_model;
	long	disp_x_size, disp_y_size, default_orientation;
	long	touch_x_left, touch_x_right, touch_y_top, touch_y_bottom;
};
struct touch_panel_s tp;


void touch_InitTouch(uint8_t orient)
{
	Chip_GPIO_Init(TOUCHSCREEN);
	T_CS_OUTPUT();				// OUT		Chip Select Out
	T_CLK_OUTPUT();				// OUT		Clock Out
	T_DOUT_OUTPUT();			// OUT		Data Out
	T_DIN_INPUT();				// IN		Serial Data In
	T_PENIRQ_INPUT();			// IN		Touch IRQ
	T_BUSY_INPUT();				// IN
	T_CS_ASSERT();
	T_CLK_DEASSERT();
	T_DOUT_DEASSERT();

	tp.orient					= orient;
	tp._default_orientation		= 0;	//CAL_S>>31;
	tp.touch_x_left				= 4096;	//(CAL_X>>14) & 0x3FFF;
	tp.touch_x_right			= 0;		//CAL_X & 0x3FFF;
	tp.touch_y_top				= 4096;		//(CAL_Y>>14) & 0x3FFF;
	tp.touch_y_bottom			= 0;		//CAL_Y & 0x3FFF;
	tp.disp_x_size				= 320;	//(CAL_S>>12) & 0x0FFF;
	tp.disp_y_size				= 240;	//CAL_S & 0x0FFF;
	tp.prec						= 10;
}

// SPI BitBang to send data
static void touch_WriteData(uint8_t data)
{
	uint8_t temp , count;

	temp=data;
	for( count=0; count<8; count++)
	{
		if(temp & 0x80)
			T_DOUT_HIGH();
		else
			T_DOUT_LOW();
		temp = temp << 1;
		T_CLK_LOW();
		__NOP();
		__NOP();
		T_CLK_HIGH();
		__NOP();
		__NOP();
	}
}

// SPI BitBang to get data
static uint16_t touch_ReadData(void)
{
	uint8_t  count;
	uint16_t data = 0;
	for( count=0; count<12; count++)
	{
		data <<= 1;
		T_CLK_LOW();
		__NOP();
		__NOP();
		T_CLK_HIGH();
		__NOP();
		__NOP();
		if (T_DIN_READ()) data++;
	}
	return(data);
}

void touch_read(void)
{
	unsigned long tx=0, temp_x=0;
	unsigned long ty=0, temp_y=0;
	unsigned long minx=99999, maxx=0;
	unsigned long miny=99999, maxy=0;
	int datacount=0;
	int i;

	T_CS_DEASSERT();
	for ( i=0; i<tp.prec; i++)
	{
		if (T_PENIRQ_READ() == 0)
		{
			_delay_uS(100);
			touch_WriteData(0x90);
			T_CLK_HIGH();
			temp_x=touch_ReadData();

			if (T_PENIRQ_READ() == 0)
			{
				_delay_uS(100);
				touch_WriteData(0xD0);
				T_CLK_HIGH();
				temp_y=touch_ReadData();

				if ((temp_x>0) && (temp_x<4096) && (temp_y>0) && (temp_y<4096))
				{
					tx+=temp_x;
					ty+=temp_y;
//					if (tp.prec>5)
//					{
//						if (temp_x<minx)
//							minx=temp_x;
//						if (temp_x>maxx)
//							maxx=temp_x;
//						if (temp_y<miny)
//							miny=temp_y;
//						if (temp_y>maxy)
//							maxy=temp_y;
//					}
					datacount++;
				}
			}
		}
	}

//	if (tp.prec>5)
//	{
//		tx = tx-(minx+maxx);
//		ty = ty-(miny+maxy);
//		datacount -= 2;
//	}
	T_CS_ASSERT();
	//if ((datacount==(tp.prec-2)) || (datacount==PREC_LOW))
	if (datacount== tp.prec)
	{
		if (tp.orient == tp._default_orientation)
		{
			touch.TP_X=ty/datacount;
			touch.TP_Y=tx/datacount;
		}
		else
		{
			touch.TP_X=tx/datacount;
			touch.TP_Y=ty/datacount;
		}
	}
	else
	{
		touch.TP_X=-1;
		touch.TP_Y=-1;
	}
}

Bool touch_dataAvailable(void)
{
	return (T_PENIRQ_READ() == 0) ? true:false;
}

int16_t touch_getTouchX(void)
{
	long c;

	if ((touch.TP_X==-1) || (touch.TP_Y==-1))
		return -1;

	c = touch.TP_X;

	/*if (c<0)
		c = 0;
	if (c>disp_x_size)
		c = disp_x_size;
	*/
	return c;
}


int16_t touch_getTouchY(void)
{
	long c;

	if ((touch.TP_X==-1) || (touch.TP_Y==-1))
		return -1;

	c = touch.TP_Y;

	/*if (c<0)
		c = 0;
	if (c>disp_y_size)
		c = disp_y_size;
	*/
	return c;
}



int touch_getX(void)
{
	long c;

	if ((touch.TP_X==-1) || (touch.TP_Y==-1))
		return -1;
	c = (long)((long)(touch.TP_X-tp.touch_x_left) * tp.disp_x_size) / (long)(tp.touch_x_right - tp.touch_x_left);	// inserted by recomendation
//	if (tp.orient == tp._default_orientation)
//	{
//		c = (long)((long)(touch.TP_X - tp.touch_x_left) * (tp.disp_x_size)) / (long)(tp.touch_x_right - tp.touch_x_left);
//		if (c<0) c = 0;
//		if (c>tp.disp_x_size) c = tp.disp_x_size;
//	}
//	else
//	{
//		if (tp._default_orientation == PORTRAIT)
//			c = (long)((long)(touch.TP_X - tp.touch_y_top) * (-tp.disp_y_size)) / (long)(tp.touch_y_bottom - tp.touch_y_top) + (long)(tp.disp_y_size);
//		else
//			c = (long)((long)(touch.TP_X - tp.touch_y_top) * (tp.disp_y_size)) / (long)(tp.touch_y_bottom - tp.touch_y_top);
//		if (c<0)
//			c = 0;
//		if (c>tp.disp_y_size)
//			c = tp.disp_y_size;
//	}
	return c;
}

int touch_getY(void)
{
	int c;

	if ((touch.TP_X==-1) || (touch.TP_Y==-1))
		return -1;

	c = (long)((long)(touch.TP_Y-tp.touch_y_bottom)*tp.disp_y_size)/(long)(tp.touch_y_top - tp.touch_y_bottom); // inserted by recomendation
//	if (tp.orient == tp._default_orientation)
//	{
//		c = (long)((long)(touch.TP_Y - tp.touch_y_top) * (tp.disp_y_size)) / (long)(tp.touch_y_bottom - tp.touch_y_top);
//		if (c<0)
//			c = 0;
//		if (c>tp.disp_y_size)
//			c = tp.disp_y_size;
//	}
//	else
//	{
//		if (tp._default_orientation == PORTRAIT)
//			c = (long)((long)(touch.TP_Y - tp.touch_x_left) * (tp.disp_x_size)) / (long)(tp.touch_x_right - tp.touch_x_left);
//		else
//			c = (long)((long)(touch.TP_Y - tp.touch_x_left) * (-tp.disp_x_size)) / (long)(tp.touch_x_right - tp.touch_x_left) + (long)(tp.disp_x_size);
//		if (c<0)
//			c = 0;
//		if (c>tp.disp_x_size)
//			c = tp.disp_x_size;
//	}
	return c;
}

void touch_setPrecision(uint8_t prec)
{
	switch (prec)
	{
		case PREC_LOW:
			tp.prec=1;
			break;
		case PREC_MEDIUM:
			tp.prec=12;
			break;
		case PREC_HI:
			tp.prec=27;
			break;
		case PREC_EXTREME:
			tp.prec=102;
			break;
		default:
			tp.prec=12;
			break;
	}
}



void touch_calibrateRead()
{
	unsigned long tx=0;
	unsigned long ty=0;

	T_CS_DEASSERT();
	touch_WriteData(0x90);
	T_CLK_HIGH();
	tx=touch_ReadData();

	touch_WriteData(0xD0);
	T_CLK_HIGH();
	ty=touch_ReadData();

	T_CS_ASSERT();
	touch.TP_X=ty;
	touch.TP_Y=tx;
}
