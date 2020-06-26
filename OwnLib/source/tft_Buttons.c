/*
  UTFT_Buttons.cpp - Add-on Library for UTFT: Buttons
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
  This library adds simple but easy to use buttons to extend the use
  of the UTFT and UTouch libraries.

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

#include <ssd1289.h>
#include <tftTouch.h>
#include <string.h>
#include <tft_Buttons.h>


void tft32_Buttons_init(void)
{
	tft32_deleteAllButtons();
	_color_text				= WHITE;
	_color_text_inactive	= GRAY;
	_color_background		= BLUE;
	_color_border			= WHITE;
	_color_hilite			= RED;
	_font_text				= NULL;
	_font_symbol			= NULL;
}

int tft32_addButton_txt(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *label, uint16_t flags)
{
	int btcnt = 0;
  
	while (((buttons[btcnt].flags & BUTTON_UNUSED) == 0) && (btcnt<MAX_BUTTONS))
		btcnt++;
  
	if (btcnt == MAX_BUTTONS)
		return -1;
	else
	{
		buttons[btcnt].pos_x  = x;
		buttons[btcnt].pos_y  = y;
		buttons[btcnt].width  = width;
		buttons[btcnt].height = height;
		buttons[btcnt].flags  = flags;
		buttons[btcnt].label  = label;
		buttons[btcnt].data   = 0;
		return btcnt;
	}
}

int tft32_addButton_bmp(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bitmapdatatype data, uint16_t flags)
{
	int btcnt = 0;
  
	while (((buttons[btcnt].flags & BUTTON_UNUSED) == 0) && (btcnt<MAX_BUTTONS))
		btcnt++;
  
	if (btcnt == MAX_BUTTONS)
		return -1;
	else
	{
		buttons[btcnt].pos_x  = x;
		buttons[btcnt].pos_y  = y;
		buttons[btcnt].width  = width;
		buttons[btcnt].height = height;
		buttons[btcnt].flags  = flags | BUTTON_BITMAP;
		buttons[btcnt].label  = NULL;
		buttons[btcnt].data   = data;
		return btcnt;
	}
}


void tft32_drawButtons(void)
{
	int i;
	for ( i=0;i<MAX_BUTTONS;i++)
	{
		if ((buttons[i].flags & BUTTON_UNUSED) == 0)
			tft32_drawButton(i);
	}
}

void tft32_drawButton(int buttonID)
{
	int		text_x, text_y;
	uint8_t	*_font_current = tft32_getFont();
	uint8_t *_font_current_param = tft32_getFont_param();
	uint16_t	_current_color = tft32_getColor();
	uint16_t	_current_back  = tft32_getBackColor();

	if (buttons[buttonID].flags & BUTTON_BITMAP)
	{
		tft32_drawBitmap_(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].width, buttons[buttonID].height, (uint16_t*)buttons[buttonID].data,1);
		if (!(buttons[buttonID].flags & BUTTON_NO_BORDER))
		{
			if ((buttons[buttonID].flags & BUTTON_DISABLED))
				tft32_setColor(_color_text_inactive);
			else
				tft32_setColor(_color_border);
			tft32_drawRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].pos_x+buttons[buttonID].width, buttons[buttonID].pos_y+buttons[buttonID].height);
		}
	}
	else
	{
		tft32_setColor(_color_background);
		tft32_fillRoundRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].pos_x+buttons[buttonID].width, buttons[buttonID].pos_y+buttons[buttonID].height);
		tft32_setColor(_color_border);
		tft32_drawRoundRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].pos_x+buttons[buttonID].width, buttons[buttonID].pos_y+buttons[buttonID].height);
		if (buttons[buttonID].flags & BUTTON_DISABLED)
			tft32_setColor(_color_text_inactive);
		else
			tft32_setColor(_color_text);
		if (buttons[buttonID].flags & BUTTON_SYMBOL)
		{
			tft32_setFont(_font_symbol,_font_param_symbol);
			text_x = (buttons[buttonID].width/2) - (tft32_getFontXsize()/2) + buttons[buttonID].pos_x;
			text_y = (buttons[buttonID].height/2) - (tft32_getFontYsize()/2) + buttons[buttonID].pos_y;
		}
		else
		{
			tft32_setFont(_font_text, _font_param_text);
			text_x = ((buttons[buttonID].width/2) - ((strlen(buttons[buttonID].label) * tft32_getFontXsize())/2)) + buttons[buttonID].pos_x;
			text_y = (buttons[buttonID].height/2) - (tft32_getFontYsize()/2) + buttons[buttonID].pos_y;
		}
		tft32_setBackColor(_color_background);
		tft32_print(buttons[buttonID].label, text_x, text_y,0);
		if ((buttons[buttonID].flags & BUTTON_SYMBOL) && (buttons[buttonID].flags & BUTTON_SYMBOL_REP_3X))
		{
			tft32_print(buttons[buttonID].label, text_x-tft32_getFontXsize(), text_y,0);
			tft32_print(buttons[buttonID].label, text_x+tft32_getFontXsize(), text_y,0);
		}
	}
	tft32_setFont(_font_current,_font_current_param);
	tft32_setColor(_current_color);
	tft32_setBackColor(_current_back);
}

void tft32_enableButton(int buttonID, Bool redraw)
{
	if (!(buttons[buttonID].flags & BUTTON_UNUSED))
	{
		buttons[buttonID].flags = buttons[buttonID].flags & ~BUTTON_DISABLED;
		if (redraw)
			tft32_drawButton(buttonID);
	}
}

void tft32_disableButton(int buttonID, Bool redraw)
{
	if (!(buttons[buttonID].flags & BUTTON_UNUSED))
	{
		buttons[buttonID].flags = buttons[buttonID].flags | BUTTON_DISABLED;
		if (redraw)
			tft32_drawButton(buttonID);
	}
}

void tft32_relabelButton(int buttonID, char *label, Bool redraw)
{
	if (!(buttons[buttonID].flags & BUTTON_UNUSED))
	{
		buttons[buttonID].label = label;
		if (redraw)
			tft32_drawButton(buttonID);
	}
}

Bool tft32_buttonEnabled(int buttonID)
{
	return !(buttons[buttonID].flags & BUTTON_DISABLED);
}
    
void tft32_deleteButton(int buttonID)
{
	if (!(buttons[buttonID].flags & BUTTON_UNUSED))
		buttons[buttonID].flags = BUTTON_UNUSED;
}

void tft32_deleteAllButtons(void)
{
	int i;
	for ( i=0;i<MAX_BUTTONS;i++)
	{
		buttons[i].pos_x=0;
		buttons[i].pos_y=0;
		buttons[i].width=0;
		buttons[i].height=0;
		buttons[i].flags=BUTTON_UNUSED;
		buttons[i].label="";
	}
}

int tft32_checkButtons(void)
{
	int		touch_x;
	int		result;
	int		touch_y;
	int i;
	uint16_t	_current_color;
    if (touch_dataAvailable() == true)
    {
		touch_read();
		result = -1;
		touch_x = touch_getX();
		touch_y = touch_getY();
		_current_color = tft32_getColor();
		for ( i=0;i<MAX_BUTTONS;i++)
		{
			if (((buttons[i].flags & BUTTON_UNUSED) == 0) && ((buttons[i].flags & BUTTON_DISABLED) == 0) && (result == -1))
			{
				if ((touch_x >= buttons[i].pos_x) && (touch_x <= (buttons[i].pos_x + buttons[i].width)) && (touch_y >= buttons[i].pos_y) && (touch_y <= (buttons[i].pos_y + buttons[i].height)))
				{
					result = i;
				}
			}
		}
		if (result != -1)	// Hilite the button
		{
			if (!(buttons[result].flags & BUTTON_NO_BORDER))
			{
				tft32_setColor(_color_hilite);
				if (buttons[result].flags & BUTTON_BITMAP)
					tft32_drawRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].pos_x+buttons[result].width, buttons[result].pos_y+buttons[result].height);
				else
					tft32_drawRoundRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].pos_x+buttons[result].width, buttons[result].pos_y+buttons[result].height);
			}
		}

		while (touch_dataAvailable() == true) {};
		if (result != -1)	// return the border to normal
		{
			if (!(buttons[result].flags & BUTTON_NO_BORDER))
			{
				tft32_setColor(_color_border);
				if (buttons[result].flags & BUTTON_BITMAP)
					tft32_drawRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].pos_x+buttons[result].width, buttons[result].pos_y+buttons[result].height);
				else
					tft32_drawRoundRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].pos_x+buttons[result].width, buttons[result].pos_y+buttons[result].height);
			}
		}

		tft32_setColor(_current_color);
		return result;
	}
	else
		return -1;
}

void tft32_setTextFont(uint8_t* font,uint8_t* font_p)
{
	_font_text = font;
	_font_param_text = font_p;
}

void tft32_setSymbolFont(uint8_t* font, uint8_t* font_p)
{
	_font_symbol = font;
	_font_param_symbol = font_p;
}

void tft32_setButtonColors(uint16_t atxt, uint16_t iatxt, uint16_t brd, uint16_t brdhi, uint16_t back)
{
	_color_text				= atxt;
	_color_text_inactive	= iatxt;
	_color_background		= back;
	_color_border			= brd;
	_color_hilite			= brdhi;
}
