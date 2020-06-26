/*
 * pic32mx360f512l
  EEP24C32.h - library for 24C32 EEPROM from TinyRTC v1.1 by Weiman
 *Created on June 26, 2014, 12:36 AM
 * Author: Darwin O. Solano
 */

#include <EEP24C32.h>
#include <hardware_delay.h>

#define SPC					0x20
#define ENTER				0x0d


Status status;
uint8_t txbuff[32];
uint8_t rxbuff[32];

/** I2C Write Data*/
static Status I2CWriteData (  uint8_t* buffer, uint8_t len, uint8_t addr )
{
    /* Sets data to be send to RTC to init*/
    I2C_XFER_T i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2ctx.rxBuff = 0;
    i2ctx.rxSz = 0;
    i2ctx.slaveAddr = addr;
    i2ctx.txBuff = buffer;
    i2ctx.txSz = len;
    // Send data to I2C
    status = Chip_I2C_MasterTransfer(I2C1 , &i2ctx);
    return status;
}

/****
 * Send data thru I2C module stored in buffer, with Len, at i2c Device addr
 */
static Status I2CReadData ( uint8_t* buffer, uint8_t len, uint8_t addr )
{
    /* Sets data to be send to RTC to init*/
	I2C_XFER_T i2crx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2crx.rxBuff = buffer;
    i2crx.rxSz = len;
    i2crx.slaveAddr = addr;
    i2crx.txBuff = 0;
    i2crx.txSz = 0;
    // Send data to I2C
    status = Chip_I2C_MasterTransfer( I2C1 ,&i2crx );
    return status;
}

/******
 * Read a single byte from pointed address of EEPROM
 */
uint8_t EEPReadByte ( uint16_t address )
{
	uint8_t addrl;
	uint8_t addrh;

    addrl = address & 0x00ff;
    addrh = ((address & 0xff00) >> 8);

    txbuff[0] = addrh;
    txbuff[1] = addrl;
    status = I2CWriteData (txbuff, 2, I2C_24C32 );
    _delay_ms(10);
    status = I2CReadData (rxbuff, 1, I2C_24C32 );
    return (rxbuff[0]);
}

/***
 * Write a single Byte to EEPROM
 */
Status EEPWriteByte ( uint16_t addr, uint8_t data )
{
	uint8_t addrl;
	uint8_t addrh;

    addrl = addr & 0x00ff;
    addrh = ((addr & 0xff00) >> 8);

    txbuff[0] = addrh;
    txbuff[1] = addrl;
    txbuff[2] = data;
    status = I2CWriteData (txbuff, 3, I2C_24C32 );
    _delay_ms(10); // 1ms
    return (status);
}

/****
 * Write Multiple bytes to EEPROM 1 page + 32 bytes
 */
Status EEPWriteString ( uint16_t addr, uint8_t* buff, uint8_t size )
{
	uint8_t status;
	while (size--)
	{
		status = EEPWriteByte(addr , *buff++);
		if (!status) return ERROR;
	}
	return SUCCESS;
}

/**
 * Read multiple bytes from EEPROM
 */
Status EEPReadString ( uint16_t addr, uint8_t size, uint8_t *ptr )
{
	uint8_t data;
	while (size--)
	{
		data = EEPReadByte(addr);
		*ptr++ = data;
	}
    return (SUCCESS);
}

/***********
 * Read multiple page from EEPROM
 */
Status EEPReadPage (uint16_t page , uint8_t *buf)
{
    uint16_t fulladdr;
    uint8_t addr;

    uint8_t* ptr = buf;

    fulladdr = page << 5;
    for (addr = 0; addr < 32; addr++) {
    	*ptr++ = EEPReadByte (fulladdr | (addr & 0x1f));
    }
    return SUCCESS;
}


/********
 * Read multiple bytes from address and len from EEPROM
 */
/*
Status EEPReadByteLen (uint16_t address, uint16_t len )
{
    // Sets data to be send to RTC to init
	I2C_XFER_T i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

    uint8_t addrl;
    uint8_t addrh;
    uint16_t ctr;
    uint8_t page;
    uint8_t *ptr;

    page  = (address  & 0x001f) >> 5;
    addrl = address   & 0x00ff;
    addrh = ((address & 0xff00) >> 8);


    UARTPuts ( DEBUG_UART_PORT, "\nEEPROM 24C32 Full %d bytes on Page:%02X read");
    UARTPutDec16(DEBUG_UART_PORT, len);
    UARTPutDec(DEBUG_UART_PORT, page );
    UARTPuts ( DEBUG_UART_PORT, "\n----------------------------------------------------");
    UARTPuts ( DEBUG_UART_PORT, "\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" );
    UARTPuts ( DEBUG_UART_PORT, "\n====================================================" );

    txbuff[0] = addrh;
    txbuff[1] = addrl;
    rxbuff[0] = 0;

    // Fill Data Structure with proper data
    i2ctx.rxBuff = rxbuff;
    i2ctx.rxSz = len;
    i2ctx.slaveAddr = I2C_24C32;
    i2ctx.txBuff = txbuff;
    i2ctx.txSz = 2;

    // Send data to I2C
    status = chip_I2C_MasterTransfer ( I2C1, &i2ctx );
    ptr = rxbuff;
    for (ctr = 0; ctr < len; ctr++) {
        if (!(ctr & 0x0F)){
        	UARTPutChar(DEBUG_UART_PORT, ENTER);
        	UARTPutHex16(DEBUG_UART_PORT, ctr >> 4);
        	UARTPutChar(DEBUG_UART_PORT, ':');
    	}

        UARTPutHex ( DEBUG_UART_PORT, *ptr );
        UARTPutChar(DEBUG_UART_PORT, SPC);
        ptr++;
    }
    UARTPutChar(DEBUG_UART_PORT, ENTER );
    return SUCCESS;
}*/
