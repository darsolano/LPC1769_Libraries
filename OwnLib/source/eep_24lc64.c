/*
 * eep_24lc64.c on board lpcxpresso1769 eeprom
 *
 *  Created on: Oct 7, 2017
 *      Author: dsolano
 */

/*
 * INCLUDES
 */
#include <eep_24lc64.h>
#include <string.h>
#include <hardware_delay.h>

/*
 * DEFINES
 */

/*
 * LOCAL VARIABLES AND PROTOTYPES
 */
uint8_t xch_buffer[34];
static uint8_t page_buffer[EEP24LC64_PAGE_SIZE];	// to hold addrh and addrl

EEPROM_t peep;

/*
 * LOCAL FUNCTIONS
 */

// Reads entire page
static Status ee_util_i2c_read(uint16_t addr, uint8_t* data, int len){
	uint8_t ctrl_byte[2];
	I2C_XFER_T xfer = {0};

	ctrl_byte[0] = (addr&ADDRH_MASK) >> 8;
	ctrl_byte[1] = addr&ADDRL_MASK;

	xfer.slaveAddr = EEP24LC64_I2C_ADDR;
	xfer.txBuff = ctrl_byte;
	xfer.txSz = 2;
	xfer.rxBuff = data;
	xfer.rxSz = len;

	Chip_I2C_MasterTransfer(EEP24LC64_I2C_BUS, &xfer);

	if (xfer.status == I2C_STATUS_DONE){
		return TRUE;
	}else return FALSE;
}

//write entire page
static Status ee_util_i2c_write(uint16_t addr, uint8_t* data, int len){
	I2C_XFER_T xfer = {0};

	xch_buffer[0] = (addr&ADDRH_MASK) >> 8;
	xch_buffer[1] = addr&ADDRL_MASK;

	memcpy(xch_buffer+2, data , len);

	xfer.slaveAddr = EEP24LC64_I2C_ADDR;
	xfer.txBuff = xch_buffer;
	xfer.txSz = 2+len;
	xfer.rxBuff = 0;
	xfer.rxSz = 0;

	Chip_I2C_MasterTransfer(EEP24LC64_I2C_BUS, &xfer);

	if ((xfer.status) == I2C_STATUS_DONE){
		return TRUE;
	}else return FALSE;

}

/*
 * Inputs a Page Number and Offset byte and return
 * the absolute  address of the page in EEPROM
 */
void ee_util_page_offset_to_addr(EEPROM_t* eep) {
	eep->addr = (uint16_t)((eep->page)*(EEP24LC64_PAGE_SIZE));
	eep->addr += eep->offset;
	eep->addr &= ADDR_MASK;
	return ;
}

/*
 * Input the address elements in structure
 * and fill out the page and offset elements
 * in structure
 */
static void ee_util_addr_to_page_offset(EEPROM_t* eep) {
	eep->page =   (eep->addr / EEP24LC64_PAGE_SIZE);
	eep->offset = (eep->addr & OFFSET_MASK);
}

/*
 * Input address element and return addrh and addrl
 */
static void ee_util_split_addr_to_addrHL(EEPROM_t* eep){
	eep->addrh = (eep->addr & ADDRH_MASK) >> 8;
	eep->addrl = eep->addr & ADDRL_MASK;
}

/*
 * Inputs the Page and Offset and transform in addrh and addrl
 */
static void ee_util_split_page_offset_to_addrHL(EEPROM_t* eep){
	EEPROM_t* temp = eep;

	ee_util_page_offset_to_addr(temp);
	eep->addr = temp->addr;
	ee_util_split_addr_to_addrHL(temp);
	eep->addrh = temp->addrh;
	eep->addrl = temp->addrl;
}


//GLOBAL FUNCTIONS

/*
 * WRITE A SINGLE BYTE TO LPCXPRESSO ONBOARD EEPROM
 * I2C ADDR 0x50
 */
Status ee_write_byte(uint16_t addr, uint8_t data){
	page_buffer[0] = data;
	uint8_t readbyte;

	/*VERIFY THAT THE CELL HAS PROPER DATA IN ORDER TO WRITE IT
	 * IF EMPTY IS OK IF NOT AND DATA IS NOT 0XFF THEN ERROR*/
	readbyte = ee_read_byte(addr);
	if ((readbyte != 0xff) && (data != 0xff))return ERROR;

	if (ee_util_i2c_write(addr, page_buffer, 1)){
		_delay_ms(5);
		readbyte = ee_read_byte(addr);
		if ( data != readbyte) return ERROR;
		return SUCCESS;
	}
	else return ERROR;
}

/*
 * READ A SINGLE BYTE FROM LPCXPRESSO ONBOARD EEPROM
 */
uint8_t ee_read_byte(uint16_t addr){
	uint8_t byte;
	if (ee_util_i2c_read(addr, &byte, 1))
		return byte;
	else return -1;
}

/*
 * WRITE STREAM OF BYTES INTO EEPROM, OBSERVING PAGE BOUNDS
 */
Status ee_write_stream(uint16_t addr, uint8_t* data, int len){
	EEPROM_t eep = {0};
	uint8_t len_tmp = 0;
	eep.addr = addr;
	ee_util_addr_to_page_offset(&eep);
	if (eep.page > 255) return ERROR;	// Page are 32 bytes, there are 256 pages = 8192bytes
	if ((eep.offset+len) > (EEP24LC64_PAGE_SIZE)){
		len_tmp = EEP24LC64_PAGE_SIZE - eep.offset;	// Len to complete the current page
		if (ee_util_i2c_write(addr, data, len_tmp) == ERROR) return ERROR;
		_delay_ms(5);
		len -= len_tmp;
		eep.page += 1;
		if (eep.page > 255) return ERROR;	// Page are 32 bytes, there are 256 pages = 8192bytes
		eep.offset = 0;
		ee_util_page_offset_to_addr(&eep);
		if (ee_util_i2c_write(eep.addr, &data[len_tmp], len) == ERROR) return ERROR;
	}
	else if (ee_util_i2c_write(addr, data, len) == ERROR) return ERROR;
	return SUCCESS;
}

/*
 * READ 32 BYTES DATA STREAM FROM LPCXPRESSO ONBOARD EEPROM
 */
Status ee_read_stream(uint16_t addr, uint8_t* buf, int len){
	if (ee_util_i2c_read(addr, buf, len))
		return SUCCESS;
	else return ERROR;
}


