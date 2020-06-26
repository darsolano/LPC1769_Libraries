/*
 * u8glib_bsp.c
 *
 *  Created on: Dec 17, 2018
 *      Author: dsolano
 */

#include "u8g2/u8glib_bsp.h"
#include <hardware_delay.h>

#define UG8LIB_I2C_BUS			I2C1
#define U8GLIB_I2C_RATE_HIGH	400000
#define U8GLIB_I2C_RATE_LOW		100000
#define U8GLIB_I2C_ADDR			0x3c

#define OLED_CS_OFF() 	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 0, 6 )
#define OLED_CS_ON()  	Chip_GPIO_SetPinOutLow(LPC_GPIO, 0, 6 )
#define OLED_DATA()   	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 7 )
#define OLED_CMD()    	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 7 )

#define OLED_PWR_OFF()  Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 1 )
#define OLED_PWR_ON()   Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 1 )



uint8_t u8x8_gpio_and_delay_i2c_lpc1769(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC3 | IOCON_MODE_INACT ));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 20, (IOCON_FUNC3 | IOCON_MODE_INACT ));
      break;							// can be used to setup pins
    case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
      break;
    case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
    	_delay_uS(arg_int);
      break;
    case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
    	_delay_uS(10 * arg_int);
      break;
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
    	_delay_ms(arg_int);
      break;
    case U8X8_MSG_DELAY_I2C:			// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz

      break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
//    case U8X8_MSG_GPIO_D0:				// D0 or SPI clock pin: Output level in arg_int
//    //case U8X8_MSG_GPIO_SPI_CLOCK:
//      break;
//    case U8X8_MSG_GPIO_D1:				// D1 or SPI data pin: Output level in arg_int
//    //case U8X8_MSG_GPIO_SPI_DATA:
//      break;
//    case U8X8_MSG_GPIO_D2:				// D2 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D3:				// D3 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D4:				// D4 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D5:				// D5 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D6:				// D6 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D7:				// D7 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_E:				// E/WR pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_CS:				// CS (chip select) pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_DC:				// DC (data/cmd, A0, register select) pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_RESET:			// Reset pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_CS1:				// CS1 (chip select) pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_CS2:				// CS2 (chip select) pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
//      break;							// arg_int=1: Input dir with pullup high for I2C clock pin
//    case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
//      break;							// arg_int=1: Input dir with pullup high for I2C data pin
//    case U8X8_MSG_GPIO_MENU_SELECT:
//      u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
//      break;
//    case U8X8_MSG_GPIO_MENU_NEXT:
//      u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
//      break;
//    case U8X8_MSG_GPIO_MENU_PREV:
//      u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
//      break;
//    case U8X8_MSG_GPIO_MENU_HOME:
//      u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
//      break;
//    default:
//      u8x8_SetGPIOResult(u8x8, 1);			// default return value
//      break;
  }
  return 1;
}
uint8_t u8x8_byte_hw_i2c_lpc1769(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	static uint8_t buffer[32]; /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
	static uint8_t buf_idx;
	uint8_t *data;

	switch(msg)
	{
		case U8X8_MSG_BYTE_SEND:
			data = (uint8_t *)arg_ptr;
			while( arg_int > 0 ){
				buffer[buf_idx++] = *data;	// copy referenced data to send buffer
				data++;
				arg_int--;
			}
			break;
		case U8X8_MSG_BYTE_INIT:
			u8x8_SetI2CAddress(u8x8,0x3c);
			Chip_I2C_Init(UG8LIB_I2C_BUS);
			Chip_I2C_SetMasterEventHandler(UG8LIB_I2C_BUS, Chip_I2C_EventHandlerPolling);
			if ( u8x8->display_info->i2c_bus_clock_100kHz >= 4 )
			{
			  Chip_I2C_SetClockRate(UG8LIB_I2C_BUS, U8GLIB_I2C_RATE_HIGH);
			}else Chip_I2C_SetClockRate(UG8LIB_I2C_BUS, U8GLIB_I2C_RATE_LOW);
			break;
		case U8X8_MSG_BYTE_SET_DC:	// not used in this case
			break;
		case U8X8_MSG_BYTE_START_TRANSFER:
			buf_idx = 0;	// Set index to send to 0
			break;
		case U8X8_MSG_BYTE_END_TRANSFER:
			Chip_I2C_MasterSend(UG8LIB_I2C_BUS, u8x8_GetI2CAddress(u8x8), buffer, (uint8_t)buf_idx);	// send i2c byte
			break;
		default:
			return 0;
	}
	return 1;
}

// In case want to use SPI Bitbang
uint8_t u8x8_gpio_and_delay_spi_lpc1769(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
       	/* Set up clock and muxing for SSP1 interface */
        	/*
        	 * Initialize SSP0 pins connect
        	 * P0.7: SCK
        	 * P0.6: SSEL
        	 * P0.8: MISO
        	 * P0.9: MOSI
        	 */
        	Chip_IOCON_PinMux(LPC_IOCON, 0, 7, IOCON_MODE_INACT, IOCON_FUNC2);
        	Chip_IOCON_PinMux(LPC_IOCON, 0, 6, IOCON_MODE_INACT, IOCON_FUNC2);
        	Chip_IOCON_PinMux(LPC_IOCON, 0, 8, IOCON_MODE_INACT, IOCON_FUNC2);
        	Chip_IOCON_PinMux(LPC_IOCON, 0, 9, IOCON_MODE_INACT, IOCON_FUNC2);

        	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, (1 << 1));
        	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, (1 << 7));
        	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 0, (1 << 6));

      break;							// can be used to setup pins
    case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
      break;
    case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
      break;
    case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
    	_delay_uS(arg_int*10);
      break;
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
    	_delay_ms(arg_int);
      break;
    case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
      break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    case U8X8_MSG_GPIO_D0:				// D0 or SPI clock pin: Output level in arg_int
    //case U8X8_MSG_GPIO_SPI_CLOCK:
      break;
    case U8X8_MSG_GPIO_D1:				// D1 or SPI data pin: Output level in arg_int
    //case U8X8_MSG_GPIO_SPI_DATA:
      break;
    case U8X8_MSG_GPIO_D2:				// D2 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D3:				// D3 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D4:				// D4 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D5:				// D5 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D6:				// D6 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D7:				// D7 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_E:				// E/WR pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_CS:				// CS (chip select) pin: Output level in arg_int
    	if (arg_int) OLED_CS_OFF();
    	else OLED_CS_ON();
      break;
    case U8X8_MSG_GPIO_DC:				// DC (data/cmd, A0, register select) pin: Output level in arg_int
    	if (arg_int) OLED_DATA();
    	else OLED_CMD();
      break;
    case U8X8_MSG_GPIO_RESET:			// Reset pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_CS1:				// CS1 (chip select) pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_CS2:				// CS2 (chip select) pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
      break;							// arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
      break;							// arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
      u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_NEXT:
      u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_PREV:
      u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_HOME:
      u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
      break;
    default:
      u8x8_SetGPIOResult(u8x8, 1);			// default return value
      break;
  }
  return 1;
}

/*
 * Send and receive at the same time
 */
void u8x8_SpiIO(LPC_SSP_T* SSPx, char TX_Data) {
	while (!(SSPx->SR & SSP_STAT_TFE));
	SSPx->DR = TX_Data;	// send data
}

uint8_t u8x8_byte_hw_spi_lpc1769(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  uint8_t *data;

  switch(msg) {
    case U8X8_MSG_BYTE_SEND:
       data = (uint8_t *)arg_ptr;
      while( arg_int > 0 ) {
    	u8x8_SpiIO(LPC_SSP1, (uint8_t)*data);
        data++;
        arg_int--;
      }
      break;
    case U8X8_MSG_BYTE_INIT:

    	LPC_SYSCTL->PCLKSEL[1] |= (SYSCTL_CLKDIV_1 << 20);	// Peripheral clock = System Clock
    	LPC_SYSCTL->PCONP |= _BIT(SYSCTL_CLOCK_SSP1);		// Power up SSP system

    	LPC_SSP1->CPSR |= 2; 									/* CPSDVSR=2 */
    	LPC_SSP1->CR0 |= 0x0007; 								/* Set mode: SPI mode 0, 8-bit */
    	LPC_SSP1->CR1 |= 0x2; 									/* Enable SSP with Master */
    	LPC_SSP1->CR0 |= ((SystemCoreClock/(2*5000000))-1)<<8;	//

    	/* make sure power is off */
    	OLED_PWR_OFF();
    	OLED_CS_OFF();
    	OLED_PWR_ON();
      break;
    case U8X8_MSG_BYTE_SET_DC:
    	if (arg_int) OLED_DATA();
    	else OLED_CMD();
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
    	OLED_CS_ON();
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
    	OLED_CS_OFF();
      break;
    default:
      return 0;
  }
  return 1;
}
