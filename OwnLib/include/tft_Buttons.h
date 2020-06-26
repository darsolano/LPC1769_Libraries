/*
  UTFT_Buttons.h - Add-on Library for UTFT: Buttons
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

#ifndef tft_Buttons_h
#define tft_Buttons_h

#define UTFT_BUTTONS_VERSION	102

#include <tftTouch.h>


#define MAX_BUTTONS	20	// Maximum number of buttons available at one time

// Define presets for button status
#define BUTTON_DISABLED			0x0001
#define BUTTON_SYMBOL			0x0002
#define BUTTON_SYMBOL_REP_3X	0x0004
#define BUTTON_BITMAP			0x0008	
#define BUTTON_NO_BORDER		0x0010
#define BUTTON_UNUSED			0x8000

#define bitmapdatatype unsigned short*

typedef struct
{
  uint16_t			pos_x, pos_y, width, height;
  uint16_t			flags;
  char				*label;
  bitmapdatatype	data;
} button_type;

void 	tft32_Buttons_init(void);
int		tft32_addButton_txt(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *label, uint16_t flags);
int		tft32_addButton_bmp(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bitmapdatatype data, uint16_t flags);
void	tft32_drawButtons(void);
void	tft32_drawButton(int buttonID);
void	tft32_enableButton(int buttonID, Bool redraw);
void	tft32_disableButton(int buttonID, Bool redraw);
void	tft32_relabelButton(int buttonID, char *label, Bool redraw);
Bool	tft32_buttonEnabled(int buttonID);
void	tft32_deleteButton(int buttonID);
void	tft32_deleteAllButtons(void);
int		tft32_checkButtons(void);
void 	tft32_setTextFont(uint8_t* font,uint8_t* font_p);
void 	tft32_setSymbolFont(uint8_t* font, uint8_t* font_p);
void	tft32_setButtonColors(uint16_t atxt, uint16_t iatxt, uint16_t brd, uint16_t brdhi, uint16_t back);

typedef int	Buttontype;
button_type	buttons[MAX_BUTTONS];
uint16_t	_color_text, _color_text_inactive, _color_background, _color_border, _color_hilite;
uint8_t		*_font_text,*_font_param_text, *_font_symbol, *_font_param_symbol;

#endif
