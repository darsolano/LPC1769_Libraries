
#include <hardware_delay.h>
#include "LCDDRV_HD44780.h"


uint8_t buff[12];
uint8_t lcdbuffer; // Buffer to hold data to tranfer to LCD
uint32_t actualClock;
uint8_t lcd2004_line;
Status status;

PRIVATE Status lcd_WriteData(uint8_t* buffer, uint8_t len, uint8_t addr)
{
	/* Sets data to be send to MCP23008 to init*/
	I2C_XFER_T i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

	// Fill Data Structure with proper data
	i2ctx.rxBuff = 0;
	i2ctx.rxSz = 0;
	i2ctx.slaveAddr = addr;
	i2ctx.txBuff = buffer;
	i2ctx.txSz = len;
	// Send data to I2C
	status = Chip_I2C_MasterTransfer( I2C1, &i2ctx);

	return status;
}

void LCDI2C_Init(void)
{

	//first init string
	_delay_ms( 10);
	buff[0] = (0x3C);
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	_delay_ms( 5);
	buff[0] = (0x38);
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	_delay_ms( 5);

	//second init string
	buff[0] = (0x3C);
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	_delay_ms( 5);
	buff[0] = (0x38);
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	_delay_ms( 5);

	//third init string
	buff[0] = (0x3C);
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	_delay_ms( 5);
	buff[0] = (0x38);
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	_delay_ms( 5);

	//fourth init string
	// 4 bits mode, only per indications of initializaton
	buff[0] = (0x2C);
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	_delay_ms( 5);
	buff[0] = (0x28);
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	_delay_ms( 5);

	// 4 bits mode, only per indications of initializaton
	LCDI2C_Write( LCD_FUNCTIONSET | LCD_5x8DOTS | LCD_4BITMODE | LCD_2LINE,
			CMD);

	// Display ON
	LCDI2C_Write( LCD_DISPLAYCONTROL | LCD_DISPLAYON, CMD);

	// Clear Display,
	LCDI2C_Write( LCD_CLEARDISPLAY, CMD);

	// Entry Mode Set
	LCDI2C_Write( LCD_ENTRYMODESET | LCD_ENTRYLEFT, CMD);
}

void LCDI2C_SetDisplayOnOff(Bool status)
{
	if (status)
	{
		LCDI2C_Write( LCD_DISPLAYCONTROL | LCD_DISPLAYON, CMD);
	}
	else
	{
		LCDI2C_Write( LCD_DISPLAYCONTROL | LCD_DISPLAYOFF, CMD);
	}
}

void LCDI2C_SetCursorOnOff(Bool status)
{
	if (status)
	{
		LCDI2C_Write( LCD_DISPLAYCONTROL | LCD_CURSORON, CMD);
	}
	else
	{
		LCDI2C_Write( LCD_DISPLAYCONTROL | LCD_CURSOROFF, CMD);
	}
}

void LCDI2C_Write(uint8_t data, uint8_t regsel)
{
	//****************************Data upper bits-Nibble***************************
	lcdbuffer = ((data & 0xf0) | regsel | LCDI2C_BACKLIGHT); //upper nibble
	lcdbuffer &= ~ENB;
	//***************************
	buff[0] = lcdbuffer;
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	//***********
	lcdbuffer |= ENB;
	buff[0] = lcdbuffer;
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	//***********
	lcdbuffer &= ~ENB;
	buff[0] = lcdbuffer;
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	//_delay( dly_5ms );
	//********************************** Lower nibble*******************************
	lcdbuffer = (data & 0x0f);
	lcdbuffer <<= 4;
	lcdbuffer |= regsel | LCDI2C_BACKLIGHT; // Lower nibble
	lcdbuffer &= ~ENB;
	//***************************
	buff[0] = lcdbuffer;
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	//***********
	lcdbuffer |= ENB;
	buff[0] = lcdbuffer;
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
	//***********
	lcdbuffer &= ~ENB;
	buff[0] = lcdbuffer;
	lcd_WriteData(buff, 1, LCD_I2C_ADDR);
}

void LCDI2C_SetCursorPos(uint8_t x, uint8_t y)
{
	uint8_t address;

	switch (y)
	{
	case 1:
		address = LCD2004_LINE_1_ADDRESS;
		break;

	case 2:
		address = LCD2004_LINE_2_ADDRESS;
		break;

	case 3:
		address = LCD2004_LINE_3_ADDRESS;
		break;

	case 4:
		address = LCD2004_LINE_4_ADDRESS;
		break;

	default:
		address = LCD2004_LINE_1_ADDRESS;
		break;
	}
	address += x - 1;
	LCDI2C_Write( LCD_SETDDRAMADDR | address, CMD);
}

void LCDI2C_SetCursorBlink(Bool status)
{
	if (status)
		LCDI2C_Write( LCD_DISPLAYCONTROL | LCD_BLINKON, CMD);
	else
		LCDI2C_Write( LCD_DISPLAYCONTROL | LCD_BLINKOFF, CMD);
}

void LCDI2C_Putc(uint8_t c)
{
	switch (c)
	{
	case '\f':
		LCDI2C_Write( LCD_CLEARDISPLAY, CMD);
		lcd2004_line = 1;
		_delay_ms(5);
		break;

	case '\n':
		LCDI2C_Write(++lcd2004_line, DATA);
		break;

	case '\b':
		LCDI2C_Write( LCD_CURSOR_DISPLAY_SHIFT | LCD_CURSOR_MOVELEFT, CMD);
		break;

	default:
		LCDI2C_Write(c, DATA);
		break;
	}
}

void LCDI2C_SetCursorHome(void)
{ // works OK
	LCDI2C_Write( LCD_RETURNHOME, CMD);
}

void LCDI2C_SetDisplayClear(void)
{ // works OK
	LCDI2C_Write( LCD_CLEARDISPLAY, CMD);
	LCDI2C_SetCursorHome();
}

void LCDI2C_Printrs(const char * buffer)
{
	while (*buffer)
	{
		LCDI2C_Putc(*buffer++);
	}
}

void LCDI2C_Prints(char * buffer)
{
	while (*buffer)
		LCDI2C_Putc(*buffer++);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-I2C
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void LCDI2C_PutDec99(uint8_t decnum)
{
	uint8_t c1 = decnum % 10;
	uint8_t c2 = (decnum / 10) % 10;
	//uint8_t c3 = (decnum / 100) % 10;
	//LCDI2C_Putc('0' + c3);
	LCDI2C_Putc('0' + c2);
	LCDI2C_Putc('0' + c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-I2C
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void LCDI2C_PutDec(uint8_t decnum)
{
	uint8_t c1 = decnum % 10;
	uint8_t c2 = (decnum / 10) % 10;
	uint8_t c3 = (decnum / 100) % 10;
	LCDI2C_Putc('0' + c3);
	LCDI2C_Putc('0' + c2);
	LCDI2C_Putc('0' + c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-I2C
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void LCDI2C_PutDec16(uint16_t decnum)
{
	uint8_t c1 = decnum % 10;
	uint8_t c2 = (decnum / 10) % 10;
	uint8_t c3 = (decnum / 100) % 10;
	uint8_t c4 = (decnum / 1000) % 10;
	uint8_t c5 = (decnum / 10000) % 10;
	LCDI2C_Putc('0' + c5);
	LCDI2C_Putc('0' + c4);
	LCDI2C_Putc('0' + c3);
	LCDI2C_Putc('0' + c2);
	LCDI2C_Putc('0' + c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-I2C
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void LCDI2C_PutDec32(uint32_t decnum)
{
	uint8_t c1 = decnum % 10;
	uint8_t c2 = (decnum / 10) % 10;
	uint8_t c3 = (decnum / 100) % 10;
	uint8_t c4 = (decnum / 1000) % 10;
	uint8_t c5 = (decnum / 10000) % 10;
	uint8_t c6 = (decnum / 100000) % 10;
	uint8_t c7 = (decnum / 1000000) % 10;
	uint8_t c8 = (decnum / 10000000) % 10;
	uint8_t c9 = (decnum / 100000000) % 10;
	uint8_t c10 = (decnum / 1000000000) % 10;
	LCDI2C_Putc('0' + c10);
	LCDI2C_Putc('0' + c9);
	LCDI2C_Putc('0' + c8);
	LCDI2C_Putc('0' + c7);
	LCDI2C_Putc('0' + c6);
	LCDI2C_Putc('0' + c5);
	LCDI2C_Putc('0' + c4);
	LCDI2C_Putc('0' + c3);
	LCDI2C_Putc('0' + c2);
	LCDI2C_Putc('0' + c1);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C port without the 0x chars
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void LCDI2C_PutHex_(uint8_t hexnum)
{
	uint8_t nibble, i;

	//UARTPuts(UARTx, "0x");
	i = 1;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		LCDI2C_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void LCDI2C_PutHex(uint8_t hexnum)
{
	uint8_t nibble, i;

	LCDI2C_Prints("0x");
	i = 1;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		LCDI2C_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void LCDI2C_PutHex16(uint16_t hexnum)
{
	uint8_t nibble, i;

	LCDI2C_Prints("0x");
	i = 3;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		LCDI2C_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void LCDI2C_PutHex32(uint32_t hexnum)
{
	uint8_t nibble, i;

	LCDI2C_Prints("0x");
	i = 7;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		LCDI2C_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}
