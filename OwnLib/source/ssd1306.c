/*
 * File:   ssd1306.c
 * Must include ssd1306.h
 * Author: dsolano
 * I2C Driven SSD1306 or SH1106 SaintSmart OLED LCD 128x64
 * Created on July 5, 2014, 1:06 AM
 * NXP LPCXpresso 1769
 */
#include "ssd1306.h"
#include <string.h>
#include <hardware_delay.h>
#include <board.h>
#include <EA_BaseBoard/inc/font5x7.h>

//#define I2C_PORT

#ifndef I2C_PORT
#define GLCD_SSPx		LPC_SSP1
#define GLCD_CS_OFF() 	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 0, 6 )
#define GLCD_CS_ON()  	Chip_GPIO_SetPinOutLow(LPC_GPIO, 0, 6 )
#define GLCD_DATA()   	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 7 )
#define GLCD_CMD()    	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 7 )
#endif

#define GLCD_PWR_OFF()  Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 1 )
#define GLCD_PWR_ON()   Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 1 )

Status status;
uint8_t buff[16];
PRIVATE uint8_t shadowFB[SHADOW_FB_SIZE];
PRIVATE uint8_t const  font_mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

PRIVATE void GLCDResetSSD1306(void);
PRIVATE void GLCDWriteCommand(uint8_t data);
PRIVATE void GLCDWriteData(uint8_t data);
PRIVATE void GLCD_vLine(uint8_t x0, uint8_t y0, uint8_t y1, oled_color_t color);
PRIVATE void GLCD_hLine(uint8_t x0, uint8_t y0, uint8_t x1, oled_color_t color);
PRIVATE void GLCD_WriteDataLen(unsigned char data, unsigned int len);

#ifdef I2C_PORT
PRIVATE Status GLCD_com_WriteData(uint8_t* buffer , uint8_t len);
/*Communication to port Write Data*/
PRIVATE Status GLCD_com_WriteData(uint8_t* buffer , uint8_t len){
	/* Sets data to be send to GLCD to init*/
	I2C_XFER_T i2ctx;			//Data structure to be used to send byte thru I2C Master Data Transfer

	// Fill Data Structure with proper data
	i2ctx.rxBuff = 0;
	i2ctx.rxSz = 0;
	i2ctx.slaveAddr = SSD1306_I2C_ADDR;
	i2ctx.txBuff = buffer;
	i2ctx.txSz = len;

	// Send data to I2C
	status = Chip_I2C_MasterTransfer (SSD1306_I2C, &i2ctx );
	return status;
}
#endif

/** Set up everything so that the display is ready to start having text
  * rendered on it. By default, this will not turn on the display; use
  * displayOn() to do that. */
void GLCD_SSD1306Init(void){
#ifdef I2C_PORT
	/* Init I2C */
	Board_I2C_Init(SSD1306_I2C);
	//Board_I2C_EnableFastPlus(SSD1306_I2C);
	Chip_I2C_Init(SSD1306_I2C);
	Chip_I2C_SetClockRate(SSD1306_I2C , 400000);
	Chip_I2C_SetMasterEventHandler(I2C1, Chip_I2C_EventHandlerPolling);
#else
	GLCD_PWR_OFF();
	Board_SSP_Init(GLCD_SSPx, 5000000);
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, (1 << 1));
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, (1 << 7));
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 0, (1 << 6));

#endif
	/* A little delay */
	_delay_ms(100);


	GLCDResetSSD1306();
#ifndef I2C_PORT
	GLCD_PWR_ON();
#endif
	_delay_ms(100);
}

/** Turn display on. This must be called in order to have anything appear
  * on the screen. */
void GLCDisplayOn(void)
{
	GLCDWriteCommand( GLCD_DISP_ON); // display on
}

/** Turn display off. This will cause the SSD1306 controller to enter a
  * low power state. */
void GLCDisplayOff(void)
{
	GLCDWriteCommand( GLCD_DISP_OFF); // display off
}

/******************************************************************************
 *
 * Description:
 *    Write len number of same data to the display
 *
 * Params:
 *   [in] data - data (color) to write to the display
 *   [in] len  - number of bytes to write
 *
 *****************************************************************************/
PRIVATE void GLCD_WriteDataLen(unsigned char data, unsigned int len)
{

#ifdef I2C_PORT
    int i;
    uint8_t buf[140];

    buf[0] = 0x40; // write Co & D/C bits

    for (i = 1; i < len+1; i++) {
        buf[i] = data;
    }
	status = GLCD_com_WriteData(buf,len+1);

#else
    GLCD_DATA();
    GLCD_CS_ON();

    while (len--)
    	SpiIO(GLCD_SSPx, data);

    GLCD_CS_OFF();
#endif
}

/** Reset and initialise the SSD1306 display controller. This mostly follows
  * Figure 2 ("Software Initialization Flow Chart") on page 64 of the
  * SSD1306 datasheet.
  * This does not turn the display on (displayOn() does that). This is so
  * that display GDDRAM can be cleared. If the display is turned on
  * immediately, undefined junk will appear on the display.
  */
PRIVATE void GLCDResetSSD1306(void)
{
    /*
     * Recommended Initial code according to manufacturer
     */

    GLCDWriteCommand(0x02);//set low column address
    GLCDWriteCommand(0x12);//set high column address
    GLCDWriteCommand(0x40);//(display start set)
    GLCDWriteCommand(0x2e);//(stop horizontal scroll)
    GLCDWriteCommand(0x81);//(set contrast control register)
    GLCDWriteCommand(0x7f);
    GLCDWriteCommand(0x82);//(brightness for color banks)
    GLCDWriteCommand(0x80);//(display on)
    GLCDWriteCommand(0xa1);//(set segment re-map)
    GLCDWriteCommand(0xa6);//(set normal/inverse display)black
    //GLCDWriteCommand(0xa7);//(set inverse display)white
    GLCDWriteCommand(0xa8);//(set multiplex ratio)
    GLCDWriteCommand(0x3F);
    GLCDWriteCommand(0xd3);//(set display offset)
    GLCDWriteCommand(0x40);
    GLCDWriteCommand(0xad);//(set dc-dc on/off)
    GLCDWriteCommand(0x8E);//
    GLCDWriteCommand(0xc8);//(set com output scan direction)
    GLCDWriteCommand(0xd5);//(set display clock divide ratio/oscillator/frequency)
    GLCDWriteCommand(0xf0);//
    GLCDWriteCommand(0xd8);//(set area color mode on/off & low power display mode )
    GLCDWriteCommand(0x05);//
    GLCDWriteCommand(0xd9);//(set pre-charge period)
    GLCDWriteCommand(0xF1);
    GLCDWriteCommand(0xda);//(set com pins hardware configuration)
    GLCDWriteCommand(0x12);
    GLCDWriteCommand(0xdb);//(set vcom deselect level)
    GLCDWriteCommand(0x34);
    GLCDWriteCommand(0x91);//(set look up table for area color)
    GLCDWriteCommand(0x3f);
    GLCDWriteCommand(0x3f);
    GLCDWriteCommand(0x3f);
    GLCDWriteCommand(0x3f);
    GLCDWriteCommand(0xaf);//(display on)
    GLCDWriteCommand(0xa4);//(display on)
	GLCD_ClearScreen(OLED_COLOR_BLACK);
	//systick_delay(dly_100us); //100us
}

/******************************************************************************
 *
 * Description:
 *    Clear the entire screen
 *
 * Params:
 *   [in] color - color to fill the screen with
 *
 *****************************************************************************/
void GLCD_ClearScreen(oled_color_t color)
{
    uint8_t i;
    uint8_t c = 0;

    if (color == OLED_COLOR_WHITE)
        c = 0xff;

    for(i=0xB0;i<0xB8;i++) {            // Go through all 8 pages
        mSetAddress(i,0x00,0x10);
        GLCD_WriteDataLen(c, 132);
    }
    memset(shadowFB, c, SHADOW_FB_SIZE);
}

PRIVATE void GLCDWriteCommand(uint8_t cmd)
{
#ifdef I2C_PORT
	buff[0] = 0x00;	// write Co & D/C bits
	buff[1] = cmd;	// Command
	status = GLCD_com_WriteData(buff,2);
#else
    GLCD_CMD();
    GLCD_CS_ON();
    SpiIO(GLCD_SSPx, cmd);
    GLCD_CS_OFF();
#endif

}

PRIVATE void GLCDWriteData(uint8_t data){
#ifdef I2C_PORT
	buff[0] = 0x40;	// write Co & D/C bits
	buff[1] = data;	// data
	status = GLCD_com_WriteData(buff,2);
#else
    GLCD_DATA();
    GLCD_CS_ON();
    SpiIO(GLCD_SSPx, data);
    GLCD_CS_OFF();
#endif

}


/******************************************************************************
 *
 * Description:
 *    Draw a horizontal line
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - y position
 *   [in] x1 - end y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
PRIVATE void GLCD_hLine(uint8_t x0, uint8_t y0, uint8_t x1, oled_color_t color)
{
    // TODO: optimize

	uint8_t bak;

    if (x0 > x1)
    {
        bak = x1;
        x1 = x0;
        x0 = bak;
    }

    while(x1 >= x0)
    {
        GLCD_putPixel(x0, y0, color);
        x0++;
    }
}

/******************************************************************************
 *
 * Description:
 *    Draw a vertical line
 *
 * Params:
 *   [in] x0 - x position
 *   [in] y0 - start y position
 *   [in] y1 - end y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
PRIVATE void GLCD_vLine(uint8_t x0, uint8_t y0, uint8_t y1, oled_color_t color)
{
	uint8_t bak;

    if(y0 > y1)
    {
        bak = y1;
        y1 = y0;
        y0 = bak;
    }

    while(y1 >= y0)
    {
        GLCD_putPixel(x0, y0, color);
        y0++;
    }
    return;
}


/******************************************************************************
 *
 * Description:
 *    Draw a line on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the line
 *
 *****************************************************************************/
void GLCD_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color)
{
    int16_t   dx = 0, dy = 0;
    int8_t    dx_sym = 0, dy_sym = 0;
    int16_t   dx_x2 = 0, dy_x2 = 0;
    int16_t   di = 0;

    dx = x1-x0;
    dy = y1-y0;

    if(dx == 0)           /* vertical line */
    {
        GLCD_vLine(x0, y0, y1, color);
        return;
    }

    if(dx > 0)
    {
        dx_sym = 1;
    }
    else
    {
        dx_sym = -1;
    }


    if(dy == 0)           /* horizontal line */
    {
        GLCD_hLine(x0, y0, x1, color);
        return;
    }


    if(dy > 0)
    {
        dy_sym = 1;
    }
    else
    {
        dy_sym = -1;
    }

    dx = dx_sym*dx;
    dy = dy_sym*dy;

    dx_x2 = dx*2;
    dy_x2 = dy*2;

    if(dx >= dy)
    {
        di = dy_x2 - dx;
        while(x0 != x1)
        {

            GLCD_putPixel(x0, y0, color);
            x0 += dx_sym;
            if(di<0)
            {
                di += dy_x2;
            }
            else
            {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }
        GLCD_putPixel(x0, y0, color);
    }
    else
    {
        di = dx_x2 - dy;
        while(y0 != y1)
        {
            GLCD_putPixel(x0, y0, color);
            y0 += dy_sym;
            if(di < 0)
            {
                di += dx_x2;
            }
            else
            {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }
        GLCD_putPixel(x0, y0, color);
    }
    return;
}

/******************************************************************************
 *
 * Description:
 *    Draw a circle on the display starting at x0,y0 with radius r
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] r - radius
 *   [in] color - color of the circle
 *
 *****************************************************************************/
void GLCD_circle(uint8_t x0, uint8_t y0, uint8_t r, oled_color_t color)
{
    int16_t draw_x0, draw_y0;
    int16_t draw_x1, draw_y1;
    int16_t draw_x2, draw_y2;
    int16_t draw_x3, draw_y3;
    int16_t draw_x4, draw_y4;
    int16_t draw_x5, draw_y5;
    int16_t draw_x6, draw_y6;
    int16_t draw_x7, draw_y7;
    int16_t xx, yy;
    int16_t  di;

    if(r == 0)          /* no radius */
    {
        return;
    }

    draw_x0 = draw_x1 = x0;
    draw_y0 = draw_y1 = y0 + r;
    if(draw_y0 < OLED_DISPLAY_HEIGHT)
    {
        GLCD_putPixel(draw_x0, draw_y0, color);     /* 90 degree */
    }

    draw_x2 = draw_x3 = x0;
    draw_y2 = draw_y3 = y0 - r;
    if(draw_y2 >= 0)
    {
        GLCD_putPixel(draw_x2, draw_y2, color);    /* 270 degree */
    }

    draw_x4 = draw_x6 = x0 + r;
    draw_y4 = draw_y6 = y0;
    if(draw_x4 < OLED_DISPLAY_WIDTH)
    {
        GLCD_putPixel(draw_x4, draw_y4, color);     /* 0 degree */
    }

    draw_x5 = draw_x7 = x0 - r;
    draw_y5 = draw_y7 = y0;
    if(draw_x5>=0)
    {
        GLCD_putPixel(draw_x5, draw_y5, color);     /* 180 degree */
    }

    if(r == 1)
    {
        return;
    }

    di = 3 - 2*r;
    xx = 0;
    yy = r;
    while(xx < yy)
    {

        if(di < 0)
        {
            di += 4*xx + 6;
        }
        else
        {
            di += 4*(xx - yy) + 10;
            yy--;
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;
            draw_x4--;
            draw_x5++;
            draw_x6--;
            draw_x7++;
        }
        xx++;
        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;
        draw_y4++;
        draw_y5++;
        draw_y6--;
        draw_y7--;

        if( (draw_x0 <= OLED_DISPLAY_WIDTH) && (draw_y0>=0) )
        {
            GLCD_putPixel(draw_x0, draw_y0, color);
        }

        if( (draw_x1 >= 0) && (draw_y1 >= 0) )
        {
            GLCD_putPixel(draw_x1, draw_y1, color);
        }

        if( (draw_x2 <= OLED_DISPLAY_WIDTH) && (draw_y2 <= OLED_DISPLAY_HEIGHT) )
        {
            GLCD_putPixel(draw_x2, draw_y2, color);
        }

        if( (draw_x3 >=0 ) && (draw_y3 <= OLED_DISPLAY_HEIGHT) )
        {
            GLCD_putPixel(draw_x3, draw_y3, color);
        }

        if( (draw_x4 <= /*OLED_DISPLAY_HEIGHT*/OLED_DISPLAY_WIDTH) && (draw_y4 >= 0) )
        {
            GLCD_putPixel(draw_x4, draw_y4, color);
        }

        if( (draw_x5 >= 0) && (draw_y5 >= 0) )
        {
            GLCD_putPixel(draw_x5, draw_y5, color);
        }
        if( (draw_x6 <= OLED_DISPLAY_WIDTH) && (draw_y6 <= OLED_DISPLAY_HEIGHT) )
        {
            GLCD_putPixel(draw_x6, draw_y6, color);
        }
        if( (draw_x7 >= 0) && (draw_y7 <= OLED_DISPLAY_HEIGHT) )
        {
            GLCD_putPixel(draw_x7, draw_y7, color);
        }
    }
    return;
}

/******************************************************************************
 *
 * Description:
 *    Draw a rectangle on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the rectangle
 *
 *****************************************************************************/
void GLCD_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color)
{
    GLCD_hLine(x0, y0, x1, color);
    GLCD_hLine(x0, y1, x1, color);
    GLCD_vLine(x0, y0, y1, color);
    GLCD_vLine(x1, y0, y1, color);
}

/******************************************************************************
 *
 * Description:
 *    Fill a rectangle on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the rectangle
 *
 *****************************************************************************/
void GLCD_fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color)
{
    uint8_t i = 0;

    if(x0 > x1)
    {
        i  = x0;
        x0 = x1;
        x1 = i;
    }

    if(y0 > y1)
    {
        i  = y0;
        y0 = y1;
        y1 = i;
    }

    if(y0 == y1)
    {
        GLCD_hLine(x0, y0, x1, color);
        return;
    }

    if(x0 == x1)
    {
        GLCD_vLine(x0, y0, y1, color);
        return;
    }

    while(y0 <= y1)
    {
        GLCD_hLine(x0, y0, x1, color);
        y0++;
    }
    return;
}

uint8_t GLCD_putChar(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb,
		oled_color_t bg, FONTCFG_Typedef* pfont)
{
	uint8_t data = 0;
	uint8_t i = 0, j = 0, n = 0;
	//http://www.rinkydinkelectronics.com/index.php
	// To locate the exact byte to start with a char
	uint8_t* bmptr = (pfont->font + ((ch - pfont->firstchar) * pfont->height * pfont->fontHrows));

	oled_color_t color = OLED_COLOR_BLACK;

	if ((x >= (OLED_DISPLAY_WIDTH - pfont->width)) || (y >= (OLED_DISPLAY_HEIGHT - pfont->height)))
	{
		return 0;
	}

	for (i = 0; i < pfont->height; i++)
	{
		for(n=0;n<pfont->fontHrows;n++)
		{
			data = *(bmptr + i*pfont->fontHrows + n);
			for (j = 0; j < 8; j++)
			{
				if ((data & font_mask[j]) == 0)
				{
					color = bg;
				}
				else
				{
					color = fb;
				}
				GLCD_putPixel(x, y, color);
				x++;
			}
		}
		y++;
		x -= pfont->width;
	}
	return (1);
}

uint8_t GLCD_putChar_5x7(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb, oled_color_t bg)
{
    unsigned char data = 0;
    unsigned char i = 0, j = 0;

    oled_color_t color = OLED_COLOR_BLACK;

    if((x >= (OLED_DISPLAY_WIDTH - 8)) || (y >= (OLED_DISPLAY_HEIGHT - 8)) )
    {
        return 0;
    }

    if( (ch < 0x20) || (ch > 0x7f) )
    {
        ch = 0x20;      /* unknown character will be set to blank */
    }

    ch -= 0x20;
    for(i=0; i<8; i++)
    {
        data = font5x7[ch][i];
        for(j=0; j<6; j++)
        {
            if( (data&font_mask[j])==0 )
            {
                color = bg;
            }
            else
            {
                color = fb;
            }
            GLCD_putPixel(x, y, color);
            x++;
        }
        y++;
        x -= 6;
    }
    return( 1 );
}
void GLCD_putString(uint8_t x, uint8_t y, uint8_t *pStr, oled_color_t fb,
		oled_color_t bg, FONTCFG_Typedef *pfont)
{
//	if (align == OLED_ALIGN_CENTER) x = (OLED_DISPLAY_WIDTH - strlen((char*)pStr)) / 2;
//	if (align == OLED_ALIGN_LEFT) x = x;
//	if (align == OLED_ALIGN_RIGHT) x += (OLED_DISPLAY_WIDTH - strlen((char*)pStr));

	while (1)
	{
		if ((*pStr) == '\0')
		{
			break;
		}

		if (pfont->font != 0)	// Weather 5x7 or other font is used, just equals font pointer to 0
		{
			if (GLCD_putChar(x, y, *pStr++, fb, bg , pfont) == 0) break;
			x += pfont->width;
		}
		else
		{
			if (GLCD_putChar_5x7(x, y, *pStr++, fb, bg) == 0)  break;
			x += 6;
		}
	}
	return;
}

//void GLCD_putString(uint8_t x, uint8_t y, uint8_t *pStr, oled_color_t fb,
//		oled_color_t bg, FONTCFG_Typedef* pfont)
//{
//	while (1)
//	{
//		if ((*pStr) == '\0')
//		{
//			break;
//		}
//		if (GLCD_putChar(x, y, *pStr++, fb, bg, pfont) == 0)
//		{
//			break;
//		}
//		x += pfont->width;
//	}
//	return;
//}

/******************************************************************************
 *
 * Description:
 *    Draw one pixel on the display
 *
 * Params:
 *   [in] x - x position
 *   [in] y - y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
void GLCD_putPixel(uint8_t x, uint8_t y, oled_color_t color) {
	uint8_t page;
    uint16_t add;
    uint8_t lAddr;
    uint8_t hAddr;
    uint8_t mask;
    uint32_t shadowPos = 0;

    if (x > OLED_DISPLAY_WIDTH) {
        return;
    }
    if (y > OLED_DISPLAY_HEIGHT) {
        return;
    }

    /* page address */
         if(y < 8)  page = 0xB0;
    else if(y < 16) page = 0xB1;
    else if(y < 24) page = 0xB2;
    else if(y < 32) page = 0xB3;
    else if(y < 40) page = 0xB4;
    else if(y < 48) page = 0xB5;
    else if(y < 56) page = 0xB6;
    else            page = 0xB7;

    add = x + X_OFFSET;
    lAddr = 0x0F & add;             // Low address
    hAddr = 0x10 | (add >> 4);      // High address

    // Calculate mask from rows basically do a y%8 and remainder is bit position
    add = y>>3;                     // Divide by 8
    add <<= 3;                      // Multiply by 8
    add = y - add;                  // Calculate bit position
    mask = 1 << add;                // Left shift 1 by bit position

    mSetAddress(page, lAddr, hAddr) // Set the address (sets the page,
                                    // lower and higher column address pointers)

    shadowPos = (page-0xB0)*OLED_DISPLAY_WIDTH+x;

    if(color > 0)
        shadowFB[shadowPos] |= mask;
    else
        shadowFB[shadowPos] &= ~mask;

    GLCDWriteData(shadowFB[shadowPos]);
}

Status GLCD_FontSetup( const uint8_t* fontarray, FONTCFG_Typedef* pfont, const uint8_t* font_param)
{
	if (fontarray == 0) return ERROR;
	else{
		pfont->font = (uint8_t*)fontarray;
		pfont->width = font_param[0];
		pfont->height = font_param[1];
		pfont->charamount = font_param[3];
		pfont->firstchar = font_param[2];
		pfont->fontHrows = pfont->width / 8;
		if (pfont->width  % 8) pfont->fontHrows++;
	}
	return SUCCESS;
}

