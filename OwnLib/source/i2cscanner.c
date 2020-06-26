/*
 * i2cscanner.c
 *
 *  Created on: Aug 6, 2014
 *      Author: dsolano
 */

#include  <i2cscanner.h>
#include <hardware_delay.h>
#include <board.h>
#include <xprintf.h>

#define SPC					0x20
#define ENTER				0x0d

Status _status;
uint8_t buff[3];

//*********************************************************
// Local Functions
//*********************************************************
static Status I2CWriteData(uint8_t* buffer, uint32_t len, uint32_t addr,I2C_ID_T module);


/* I2C Write Data*/
static Status I2CWriteData(uint8_t* buffer, uint32_t len, uint32_t addr,I2C_ID_T module) {
	/* Sets data to be send to RTC to init*/
	I2C_XFER_T i2ctx;	//Data structure to be used to send byte thru I2C Master Data Transfer
	// Fill Data Structure with proper data
	i2ctx.rxBuff = 0;
	i2ctx.rxSz = 0;
	i2ctx.slaveAddr = addr;
	i2ctx.txBuff = buffer;
	i2ctx.txSz = len;
	// Send data to I2C
	_status = Chip_I2C_MasterTransfer(module, &i2ctx);
	if (_status == 0)
		return (_status);
	else
		return (1);
}
//*********************************************************
// End of Local Functions
//*********************************************************

//*********************************************************
// Public Functions
//*********************************************************


/* Function that probes all available slaves connected to an I2C bus */
void i2cscan(I2C_ID_T module)
{
	uint8_t i;
	xdev_out( Board_UARTPutChar );

	xputs("NXP/Embbeded Artist LPCXpresso 1769 Cortex M3...\n");
	xprintf("Probing available I2C devices on LPC Bus: %d" , module);
	xputs("\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	xputs("\n====================================================\n");
	for (i = 0; i <= 0x7F; i++) {
		if (!(i & 0x0F)){
			xputs("\n ");
			xprintf("%2x" , i >> 4);
			xputc(SPC);
		}
		if (i <= 7 || i > 0x78) {
			xputs("   ");
			continue;
		}
		// Sets buffer to send
		buff[0] = 1;
		_status = I2CWriteData(buff, 1, i,module );
		if(_status == 0){
			xputc(SPC);
			xprintf("%2x" , i);
		}
		else
			xputs(" --");
		_delay_ms(1);
	}
	xputs("\r\n");
}
