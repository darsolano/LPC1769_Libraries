/*
 * max7219.c
 *
 *  Created on: Apr 10, 2019
 *      Author: dsolano
 */


#include <max7219.h>
#include <hardware_delay.h>
#include <define_pins.h>
MAX7219_s max;

DEFINE_PIN(shift_CS,3,25)
DEFINE_PIN(shift_CLK,3,26)
DEFINE_PIN(shift_DOUT,4,28)

//Number definition
const uint8_t display7s[] =
{
//        pABCDEFG
		0b01111110,				// 0
		0b00110000,             // 1
		0b01101101,             // 2
		0b01111001,             // 3
		0b00110011,             // 4
		0b01011011,             // 5
		0b01011111,             // 6
		0b01110000,             // 7
		0b01111111,             // 8
		0b01110011,             // 9
		0b01110111,             // A
		0b00011111,             // B
		0b01001110,             // C
		0b00111101,             // D
		0b01001111,             // E
		0b01000111,             // F
		0b10000000				// Dot
//        pABCDEFG
		};

/*
 *        a
        ------
 *      |    |b
 *     f| g  |
 *      ------
 *      |    |c
 *    e |    |
 *      ------ o h
 *         d
 */
const uint8_t display7sLetters[] =
{
//        pABCDEFG
		0b00000000,				// space or blank
		0b01110111,             // A
		0b00011111,             // B
		0b01001110,             // C
		0b00111101,             // D
		0b01001111,             // E
		0b01000111,             // F
		0b01111011,				// G
		0b00110111,				// H
		0b00000110,				// I
		0b00111100,				// J
		0b00000001,				// K
		0b00001110,				// L
		0b00000001,				// M
		0b00010101,				// N
		0b00011101,				// O
		0b01100111,				// P
		0b11111110,				// Q
		0b00000101,				// R
		0b01011011,				// S
		0b00001111,				// T
		0b00111110,				// U
		0b00011100,				// V
		0b00000001,				// W
		0b00000001,				// X
		0b00111011,				// Y
		0b00000001				// Z
//		  pABCDEFG
		};

// SPI BitBang to send data 16 bits
static void max7219_serial3wire_write_short(uint16_t data)
{
	uint16_t temp , count;

	temp=data;
	shift_CLK_LOW();
	for( count=0; count<16; count++)
	{
		if(temp & 0x8000)
			shift_DOUT_ASSERT();
		else
			shift_DOUT_DEASSERT();
		temp <<=  1;
		// pseudo clock
		shift_CLK_HIGH();
		__NOP();
		shift_CLK_LOW();
		__NOP();
	}
}


static uint8_t max7219_convert_digits(uint8_t character, TYPEOFCHAR_t type)
{
	uint8_t data;
	if (type == LETTER)
	{
		if (character == 0x20)
			character = 0x41;
		if (character > 0x40 && character < 0x5a)
		{
			data = (character - 0x40);
			return display7sLetters[data];
		}
	}
	if (type == NUMBER)
	{
		data = 0x0f & character;
		return display7s[data];
	}
	return -1;
}


/*
 * Send data to the specific device in the chain
 * the device data must be preceded and followed by NOP´s
 * depending on the location of the specific device in the chain
 * Devices must start from 0 to xx, max devices is from 1 to max
 */
static void max7219_Send_Data(uint8_t reg, uint8_t data, uint8_t device){
	int preced_nop, follow_nop = 0;

	if (device > max.max_dev_number) {
		return;
	}

	// Determine how many NOP´s before and after should be sent
	preced_nop = max.max_dev_number - (device + 1);
	follow_nop = max.max_dev_number - (preced_nop + 1);

	shift_CS_DEASSERT();

	for (int i = 0; i < preced_nop; i++) {
		MAX7219(NOP,0);
	}

	MAX7219(reg,data);

	for (int i = 0; i < follow_nop; i++) {
		MAX7219(NOP,0);
	}
	shift_CS_ASSERT();
}

/*	Initial configuration for MAX7219, go for no config to use 7Segment array
 * 	Each display declare in the init max devices will be tested and programmed
 * 	in the same way, with the same parameters
 */
void max7219_init(SCAN_LIMIT_e digits , INTENSISTY_e intensity , DECODE_e decode, uint8_t max_dev)
{
	Chip_IOCON_Init(LPC_IOCON);
	Chip_GPIO_Init(LPC_GPIO);
	// Set DIRECTION for Pins
	shift_CS_OUTPUT();
	shift_CLK_OUTPUT();
	shift_DOUT_OUTPUT();
	// Initial state
	shift_CS_ASSERT();
	shift_CLK_DEASSERT();
	shift_DOUT_DEASSERT();

	max.scan_limit = digits;
	max.intensity = intensity;
	max.decode_mode = decode;
	max.max_dev_number = max_dev;
	max.disp_test = DISP_TEST_NORMAL_OP;
	max.shutdown_mode = NORMAL_OPERATION;

	for (int i = 0; i < max.max_dev_number; i++) {
		max7219_Send_Data(DISPLAY_TEST, max.disp_test, i);
		max7219_Send_Data(SHUTDOWN, max.shutdown_mode, i);
		max7219_Send_Data(SCAN_LIMIT, max.scan_limit, i);
		max7219_Send_Data(INTENSITY, max.intensity, i);
		max7219_Send_Data(DECODE_MODE, max.decode_mode, i);

		max7219_test_cycle(i);

		_delay_ms(500);

		max7219_clear_all_display(i);
	}
}


void max7219_test_cycle(uint8_t dev)
{
	max7219_Send_Data(DISPLAY_TEST, DISP_TEST_TEST_MODE, dev);
	_delay_ms(250);
	max7219_Send_Data(DISPLAY_TEST , DISP_TEST_NORMAL_OP, dev);

	// circle around display
	int i = 0;
	uint8_t digit = 0;

	// round display from a to dot
	for (i=0;i<9;i++)
	{
		max7219_serial3wire_write_short((digit<<8)|digit);
		max7219_Send_Data(DIG0,digit, dev);
		max7219_Send_Data(DIG1,digit, dev);
		max7219_Send_Data(DIG2,digit, dev);
		max7219_Send_Data(DIG3,digit, dev);
		max7219_Send_Data(DIG4,digit, dev);
		max7219_Send_Data(DIG5,digit, dev);
		max7219_Send_Data(DIG6,digit, dev);
		max7219_Send_Data(DIG7,digit, dev);
		digit = (1<<i);
		_delay_ms(250);
	}

	max7219_blink(dev, 250,3);

	//Count to 0-9 for each display from display 0 to 7

	// count from 0-9 every display together
}

void max7219_blink(uint8_t dev, int timeinmilli, int blink_times)
{
	max7219_Send_Data(DIG0,0xff, dev);
	max7219_Send_Data(DIG1,0xff, dev);
	max7219_Send_Data(DIG2,0xff, dev);
	max7219_Send_Data(DIG3,0xff, dev);
	max7219_Send_Data(DIG4,0xff, dev);
	max7219_Send_Data(DIG5,0xff, dev);
	max7219_Send_Data(DIG6,0xff, dev);
	max7219_Send_Data(DIG7,0xff, dev);
	while (blink_times--)
	{
		max7219_Send_Data(SHUTDOWN , SHUTDOWN_MODE, dev);
		_delay_ms(timeinmilli);
		max7219_Send_Data(SHUTDOWN , NORMAL_OPERATION, dev);
		_delay_ms(timeinmilli);
	}
}

/*
 * Clear all digits in an specific device in the array
 */
void max7219_clear_all_display(uint8_t dev)
{
	int i = 9;
	while (i--)	// i = digit register. BUT 0 IS NOT A DIGIT REGISTER, Start from 1 thru 8
	{
		max7219_Send_Data(i, (CLEAR_MASK&DATA_MASK), dev);
	}

}


/*
 * Enters shutdown mode as per request in order to
 * drop down current consumption in the circuit
 * mode true = shutdown mode
 * mode false = Normal mode
 */
void max7219_shutdown(uint8_t dev, Bool mode){
	if (mode){
		max7219_Send_Data(SHUTDOWN , SHUTDOWN_MODE, dev);
	}else{
		max7219_Send_Data(SHUTDOWN , NORMAL_OPERATION, dev);
	}
}
/*
 * DIgits are counted from right to left in the array
 * device are counted from left to right in the array
 */
void max7219_clear_digit(uint8_t dev, uint8_t digit){

}

void max7219_blink_display(uint8_t dev, int timeinmilli, int blink_times){

	while (blink_times--)
	{
		max7219_Send_Data(SHUTDOWN , SHUTDOWN_MODE, dev);
		_delay_ms(timeinmilli);
		max7219_Send_Data(SHUTDOWN , NORMAL_OPERATION, dev);
		_delay_ms(timeinmilli);
	}
}

void max7219_scroll_display(uint8_t dev, SCROLL_DIR_e direction)
{

}

void max7219_display_print_string(uint8_t dev, SCROLL_DIR_e dir, uint8_t* text, int len){
	if (dir == NO_SCROLL){

	}
	else if(dir == SCROLL_LEFT){

	}
	else if(dir == SCROLL_RIGHT){

	}
}

/*
 * Print a digit character in a digit position within a device in the array to displays
 * from digit 8 to digit 1, from left to right5
 */
void max7219_display_print_char(uint8_t character, uint8_t pos){
	uint8_t dev = pos/8;
	uint8_t digit = (pos%8)+1;
	if (character >= 0x30 && character <= 0x39) {
		max7219_Send_Data(digit, max7219_convert_digits(character, NUMBER), dev);
	} else if (character >= 0x40 && character <= 0x5A){
		max7219_Send_Data(digit, max7219_convert_digits(character, LETTER), dev);
	}
}

void max7219_print_32(uint8_t dev, uint32_t number)
{
	// Digit 0
	uint8_t c1 = number % 10;
	max7219_Send_Data(DIG0 , max7219_convert_digits(c1,NUMBER), dev);

	// Digit 1
	uint8_t c2 = (number / 10) % 10;
	max7219_Send_Data(DIG1 , max7219_convert_digits(c2,NUMBER), dev);

	// Digit 2
	uint8_t c3 = (number / 100) % 10;
	max7219_Send_Data(DIG2 , max7219_convert_digits(c3,NUMBER), dev);

	// Digit 3
	uint8_t c4 = (number / 1000) % 10;
	max7219_Send_Data(DIG3 , max7219_convert_digits(c4,NUMBER), dev);

	// Digit 4
	uint8_t c5 = (number / 10000) % 10;
	max7219_Send_Data(DIG4 , max7219_convert_digits(c5,NUMBER), dev);

	// Digit 5
	uint8_t c6 = (number / 100000) % 10;
	max7219_Send_Data(DIG5 , max7219_convert_digits(c6,NUMBER), dev);

	// Digit 6
	uint8_t c7 = (number / 1000000) % 10;
	max7219_Send_Data(DIG6 , max7219_convert_digits(c7,NUMBER), dev);

	// Digit 7
	uint8_t c8 = (number / 10000000) % 10;
	max7219_Send_Data(DIG7 , max7219_convert_digits(c8,NUMBER), dev);
}


void max7219_print_16(uint8_t dev, uint16_t number)
{
	// Digit 0
	uint8_t c1 = number % 10;
	max7219_Send_Data(DIG0 , max7219_convert_digits(c1,NUMBER), dev);

	// Digit 1
	uint8_t c2 = (number / 10) % 10;
	max7219_Send_Data(DIG1 , max7219_convert_digits(c2,NUMBER), dev);

	// Digit 2
	uint8_t c3 = (number / 100) % 10;
	max7219_Send_Data(DIG2 , max7219_convert_digits(c3,NUMBER), dev);

	// Digit 3
	uint8_t c4 = (number / 1000) % 10;
	max7219_Send_Data(DIG3 , max7219_convert_digits(c4,NUMBER), dev);

	// Digit 4
	uint8_t c5 = (number / 10000) % 10;
	max7219_Send_Data(DIG4 , max7219_convert_digits(c5,NUMBER), dev);

}

void max7219_print_8(uint8_t dev, uint8_t number)
{
	// Digit 0
	uint8_t c1 = number % 10;
	max7219_Send_Data(DIG0 , max7219_convert_digits(c1,NUMBER), dev);

	// Digit 1
	uint8_t c2 = (number / 10) % 10;
	max7219_Send_Data(DIG1 , max7219_convert_digits(c2,NUMBER), dev);

	// Digit 2
	uint8_t c3 = (number / 100) % 10;
	max7219_Send_Data(DIG2 , max7219_convert_digits(c3,NUMBER), dev);
}


/*********************************************************************//**
 * @brief		Puts a hex number to LCD-1602
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void max7219_print_hex_8(uint8_t dev, uint8_t hexnum)
{
	uint8_t nibble, i;

	i = 1;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		//LCD1602_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
		max7219_Send_Data(i+1, max7219_convert_digits(nibble, NUMBER), dev);
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-1602
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void max7219_print_hex_16(uint8_t dev, uint16_t hexnum)
{
	uint8_t nibble, i;

	i = 3;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		//LCD1602_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
		max7219_Send_Data(i+1, max7219_convert_digits(nibble, NUMBER), dev);
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-1602
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void max7219_print_hex_32(uint8_t dev, uint32_t hexnum)
{
	uint8_t nibble, i;

	i = 7;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		//LCD1602_Putc((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
		max7219_Send_Data(i+1, max7219_convert_digits(nibble, NUMBER), dev);
	} while (i--);
}

