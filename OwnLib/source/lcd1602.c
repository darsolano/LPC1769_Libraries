
#include <hardware_delay.h>
#include <lcd1602.h>
#include <mcp23008.h>


uint8_t buff[128];
uint8_t lcdbuffer;    // Buffer to hold data to transfer to LCD
uint32_t actualClock;
uint8_t lcd2004_line;



static void Init_LCD1602(void)
{
	MCP23008_IODIR_REG_sType iodir;
	iodir.IODir_reg = 0;
	mcp23008_SetGPIODir(&iodir,LCD1602_I2C_ADDR);

	mcp23008_SetIPOL(0, LCD1602_I2C_ADDR);

	MCP23008_IOCON_REG_sType iocon;
	iocon.DISSLW = 0;
	iocon.HAEN = 1;
	iocon.INTPOL = 1;
	iocon.ODR = 1;
	iocon.SEQOP = 1;
	mcp23008_ConfigIOCON(&iocon, LCD1602_I2C_ADDR);
}


void LCD1602_Init(void)
{
	Init_LCD1602();

	//first init string
	_delay_ms( 10);
	mcp23008_WriteGPIO(0x93, LCD1602_I2C_ADDR);
	_delay_ms(10);
	mcp23008_WriteGPIO(0x83, LCD1602_I2C_ADDR);
	_delay_ms(5);

	//second init string
	mcp23008_WriteGPIO(0x93, LCD1602_I2C_ADDR);
	_delay_ms(5);
	mcp23008_WriteGPIO(0x83, LCD1602_I2C_ADDR);
	_delay_ms(5);

	//third init string
	mcp23008_WriteGPIO(0x93, LCD1602_I2C_ADDR);
	_delay_ms(5);
	mcp23008_WriteGPIO(0x83, LCD1602_I2C_ADDR);
	_delay_ms(5);

	//fourth init string
	// 4 bits mode, only per indications of initializaton
	mcp23008_WriteGPIO(0x92, LCD1602_I2C_ADDR);
	_delay_ms(5);
	mcp23008_WriteGPIO(0x82, LCD1602_I2C_ADDR);
	_delay_ms(5);

	// 4 bits mode, only per indications of initializaton
	LCD1602_Write(LCD_FUNCTIONSET | LCD_5x8DOTS| LCD_4BITMODE | LCD_2LINE, RS_CMD);

	// Display ON
	LCD1602_Write(LCD_DISPLAYCONTROL | LCD_DISPLAYON, RS_CMD);

	// Clear Display,
	LCD1602_Write(LCD_CLEARDISPLAY, RS_CMD);

	// Entry Mode Set
	LCD1602_Write(LCD_ENTRYMODESET | LCD_ENTRYLEFT, RS_CMD);
}

void LCD1602_SetDisplayOnOff(Bool status)
{
	if (status)
	{
		LCD1602_Write(LCD_DISPLAYCONTROL | LCD_DISPLAYON, RS_CMD);
	}
	else
	{
		LCD1602_Write(LCD_DISPLAYCONTROL | LCD_DISPLAYOFF, RS_CMD);
	}
}

void LCD1602_SetCursorOnOff(Bool status)
{
	if (status)
	{
		LCD1602_Write(LCD_DISPLAYCONTROL | LCD_CURSORON, RS_CMD);
	}
	else
	{
		LCD1602_Write(LCD_DISPLAYCONTROL | LCD_CURSOROFF, RS_CMD);
	}
}

void LCD1602_Write(uint8_t data, uint8_t RegSel)
{
//****************************Data upper bits-Nibble***************************
	lcdbuffer = ((data & 0xf0));	//upper nibble
	lcdbuffer >>= 4;
	lcdbuffer |= (RegSel | BL | WRITE_LCD);
	lcdbuffer &= ~EN;
	//***************************
	mcp23008_WriteGPIO(lcdbuffer, LCD1602_I2C_ADDR);
	_delay_ms(1 );
//***********
	lcdbuffer |= EN;
	mcp23008_WriteGPIO(lcdbuffer, LCD1602_I2C_ADDR);
	_delay_ms(1);
//***********
	lcdbuffer &= ~EN;
	mcp23008_WriteGPIO(lcdbuffer, LCD1602_I2C_ADDR);
	_delay_ms(1);

//********************************** Lower nibble*******************************
	lcdbuffer = (data & 0x0f);
	lcdbuffer |= RegSel | BL | WRITE_LCD;				// Lower nibble
	lcdbuffer &= ~EN;
	//***************************
	//printf("Buffer is: %x\n",lcdbuffer);
	mcp23008_WriteGPIO(lcdbuffer, LCD1602_I2C_ADDR);
	_delay_ms(1 );
//***********
	lcdbuffer |= EN;
	mcp23008_WriteGPIO(lcdbuffer, LCD1602_I2C_ADDR);
	_delay_ms(1 );
//***********
	lcdbuffer &= ~EN;
	mcp23008_WriteGPIO(lcdbuffer, LCD1602_I2C_ADDR);
	_delay_ms(1 );
}

void LCD1602_SetCursorPos(uint8_t x, uint8_t y)
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

	default:
		address = LCD2004_LINE_1_ADDRESS;
		break;
	}
	address += x - 1;
	LCD1602_Write(0x80 | address, RS_CMD);
}

void LCD1602_SetCursorBlink(Bool status)
{
	if (status)
		LCD1602_Write(LCD_DISPLAYCONTROL | LCD_BLINKON, RS_CMD);
	else
		LCD1602_Write(LCD_DISPLAYCONTROL | LCD_BLINKOFF, RS_CMD);
}

void LCD1602_Putc(uint8_t c)
{
	switch (c)
	{
	case '\f':
		LCD1602_Write(LCD_CLEARDISPLAY, RS_CMD);
		lcd2004_line = 1;
		_delay_uS(5);
		break;

	case '\n':
		LCD1602_Write(++lcd2004_line, RS_DATA);
		break;

	case '\b':
		LCD1602_Write(LCD_CURSOR_DISPLAY_SHIFT | LCD_CURSOR_MOVELEFT, RS_CMD);
		break;

	default:
		LCD1602_Write(c, RS_DATA);
		break;
	}
}

void LCD1602_SetCursorHome()
{				// works OK
	LCD1602_Write(LCD_RETURNHOME, RS_CMD);
}

void LCD1602_SetDisplayClear()
{				// works OK
	LCD1602_Write(LCD_CLEARDISPLAY, RS_CMD);
	LCD1602_SetCursorHome();
}

void LCD1602_Printrs(const char * buffer)
{
	while (*buffer)
	{
		LCD1602_Putc(*buffer++);
	}
}

void LCD1602_Prints(char * buffer)
{
	while (*buffer)
		LCD1602_Putc(*buffer++);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-1602 until 99
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void LCD1602PutDec99(uint8_t decnum)
{
	uint8_t c1 = decnum % 10;
	uint8_t c2 = (decnum / 10) % 10;
	//uint8_t c3 = (decnum / 100) % 10;
	//LCD1602_Putc('0' + c3);
	LCD1602_Putc('0' + c2);
	LCD1602_Putc('0' + c1);
}


/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-1602
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void LCD1602PutDec(uint8_t decnum)
{
	uint8_t c1 = decnum % 10;
	uint8_t c2 = (decnum / 10) % 10;
	uint8_t c3 = (decnum / 100) % 10;
	LCD1602_Putc('0' + c3);
	LCD1602_Putc('0' + c2);
	LCD1602_Putc('0' + c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-1602
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void LCD1602PutDec16(uint16_t decnum)
{
	uint8_t c1 = decnum % 10;
	uint8_t c2 = (decnum / 10) % 10;
	uint8_t c3 = (decnum / 100) % 10;
	uint8_t c4 = (decnum / 1000) % 10;
	uint8_t c5 = (decnum / 10000) % 10;
	LCD1602_Putc('0' + c5);
	LCD1602_Putc('0' + c4);
	LCD1602_Putc('0' + c3);
	LCD1602_Putc('0' + c2);
	LCD1602_Putc('0' + c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-1602
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void LCD1602PutDec32(uint32_t decnum)
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
	LCD1602_Putc('0' + c10);
	LCD1602_Putc('0' + c9);
	LCD1602_Putc('0' + c8);
	LCD1602_Putc('0' + c7);
	LCD1602_Putc('0' + c6);
	LCD1602_Putc('0' + c5);
	LCD1602_Putc('0' + c4);
	LCD1602_Putc('0' + c3);
	LCD1602_Putc('0' + c2);
	LCD1602_Putc('0' + c1);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-1602 port without the 0x chars
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void LCD1602PutHex_(uint8_t hexnum)
{
	uint8_t nibble, i;

	//UARTPuts(UARTx, "0x");
	i = 1;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		LCD1602_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-1602
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void LCD1602PutHex(uint8_t hexnum)
{
	uint8_t nibble, i;

	LCD1602_Prints("0x");
	i = 1;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		LCD1602_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-1602
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void LCD1602PutHex16(uint16_t hexnum)
{
	uint8_t nibble, i;

	LCD1602_Prints("0x");
	i = 3;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		LCD1602_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-1602
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void LCD1602PutHex32( uint32_t hexnum)
{
	uint8_t nibble, i;

	LCD1602_Prints("0x");
	i = 7;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		LCD1602_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}

