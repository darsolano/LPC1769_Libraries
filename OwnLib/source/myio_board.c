/*
 * myio_board.c
 *
 *  Created on: 30/1/2015
 *      Author: dsolano
 */

#include <myio_board.h>


__attribute__ ( ( naked ) ) void myio_board_init (void)
{
	// Start PushButtons configuration
	// No need to specify the Function or mode as they already are by reset
	// So PINCON bits are not touch here for the Push buttons, just for directions
	// and Pullup, pulldown or OpenDrain modes and functions as EINTx
	// Pins 2.10 , 2.11 , 2.12 , 2.13
	// Those PINS will have pullup resistors, so no config to be done
	LPC_IOCON->PINSEL[4] |= (uint32_t)(_BIT(20) | _BIT(22) | _BIT(24) | _BIT(26)); // Pins functions as EINTx all 4
	LPC_GPIO2->DIR &=  ~(uint32_t)(_BIT(20) | _BIT(22) | _BIT(24) | _BIT(26)); // DIR as inputs

	// RGB Config Pins Port0:2,3,21
	// AS they are GPIO nothing to do
	// Let set as output and pulldown
	LPC_IOCON->PINMODE[0] |= (uint32_t)(3<<4);	//Pin 0.2 as PullDown
	LPC_IOCON->PINMODE[0] |= (uint32_t)(3<<6);   //Pin 0.3 as Pulldown
	LPC_IOCON->PINMODE[1] |= (uint32_t)(3<<10);  //Pin0.21 as Pulldown
	LPC_GPIO->DIR |= (uint32_t)(_BIT(2) | _BIT(3) | _BIT(21)); // Bits as Output

	// Led indicators 4 pins as Port 1 pins:26,27,28,29
	// As output, pullDown
	LPC_IOCON->PINMODE[3] |= (uint32_t)(3<<20 | 3<<22 | 3<<24 | 3<<26);	// Pins as PullDown resistors
	LPC_GPIO1->DIR |= (uint32_t)(_BIT(26) | _BIT(27) | _BIT(28) | _BIT(29)); // As output

	// Led Bar config as output, , pulldown
	// Port1 pins:18 .... 25
	LPC_IOCON->PINMODE[3] |= (uint32_t)(3<<4 | 3<<6 | 3<<8 | 3<<10 | 3<<12 | 3<<14 | 3<<16 | 3<<18);	// Pins as PullDown resistors
	LPC_GPIO1->DIR |= (uint32_t)(_BIT(18) | _BIT(19) | _BIT(20) | _BIT(21) | _BIT(22) | _BIT(23) | _BIT(24) | _BIT(25)); // As output
}

void pushbuttons_SetInt(uint8_t button)
{

}

void pushbutton_Read(uint8_t button)
{

}

void rgb_ON(uint8_t color)
{

}

void rgb_OFF(uint8_t color)
{

}

void rgb_Toggle(uint8_t color)
{

}

__attribute__ ( ( naked ) ) void rgb_RollColors(void)
{

}

void ledindicator_ON(uint8_t led)
{

}

void ledindicator_OFF(uint8_t led)
{

}

void ledindicator_Roll(void)
{

}

void ledbar_LedON(uint8_t ledbit)
{

}

void ledbar_LedOFF(uint8_t ledbit)
{

}

/*
 * Function accepts value for an 8 bit variable
 * and set them in the pins directly
 */
void ledbar_SetValue(uint8_t value)
{
	LPC_GPIO1->PIN |= (uint32_t)(0x03FC0000 & (uint32_t)(value<<18));
}
