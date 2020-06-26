/*
 * AT45DB081D.h
 *
 *  Created on: Oct 30, 2017
 *      Author: dsolano
 */


#include <AT45DB081D.h>
#include <define_pins.h>
#include <hardware_delay.h>
#include <board.h>
#define SSPx	LPC_SSP1
DEFINE_PIN(FLASH_PIN,1,18)

uint8_t flash_buffer[FLASH_BYTES_PER_PAGE];
FLASH_CTL_t flash;
/******************************************************************************
 * Local Functions
 *****************************************************************************/
// Read byte from SPI
uint8_t SpiXchg(char TX_Data) {
	while (!(SSPx->SR & SSP_STAT_TFE));
	SSPx->DR = TX_Data;	// send data
	while (!(SSPx->SR & SSP_STAT_RNE));
	return SSPx->DR;	// Receive data
}



static void flash_Read_spi(uint8_t* data, int len) {
	while (len--){
		*data++ =SpiXchg(0);	// Receive data
	}
}

// Write Byte to SPI
static void flash_Write_spi(uint8_t* data, int len) {
	while (len--){
		SpiXchg(*data++);	// send data
	}
}

// Read the status register and strip out the middle bits
static Bool flash_CheckForStatus(STATUS_t stat){
	uint8_t cmd = STATUS_REGISTER_READ;
	uint8_t status = 0;

	FLASH_PIN_SELECTED();

	flash_Write_spi(&cmd, 1);
	flash_Read_spi(&status, 1);

	FLASH_PIN_DESELECTED();
	return (((status & STATUS_MASK) & stat) ? TRUE:FALSE);
}

// Read Buffer 1 or 2 from flash at low freq
static Bool flash_bufferRead(uint8_t addr_offset,uint8_t* read_buf, int read_len, BUFFER_t buffer_no){
	uint8_t cmd [5];

	while (!flash_CheckForStatus(FLASH_READY));	// make sure flash is ready
	if ((addr_offset + read_len) > FLASH_BUFFER_MAX_BYTES-1){ // verify is address space is within buffer scope
		return ERROR;
	}

	if (buffer_no == BUFFER1)
		cmd [0] = BUFFER_1_READ_LOW_FREQ;
	else if (buffer_no == BUFFER2)
		cmd [0] = BUFFER_2_READ_LOW_FREQ;

	cmd [1] = 0x00;
	cmd [2] = 0x00;
	cmd [3] = 0xBF;
	cmd [4] = 0xA0+addr_offset;

	FLASH_PIN_SELECTED();

	flash_Write_spi(cmd, 5);
	flash_Read_spi(read_buf, read_len);

	FLASH_PIN_DESELECTED();
	return SUCCESS;
}

/******************************************************************************
 *
 * Description:
 * Write Buffer 1 or 2 from flash at low freq
 *
 * Params:
 *   [in] buf - data to write to flash
 *   [in] offset - offset into the flash
 *   [in] len - number of bytes to write
 *
 * Returns:
 *   number of written bytes
 *
 *****************************************************************************/
static Bool flash_bufferWrite(uint8_t addr_offset,uint8_t* write_buf, int write_len, BUFFER_t buffer_no){
	uint8_t cmd [5];

	while (!flash_CheckForStatus(FLASH_READY));	// make sure flash is ready
	if ((addr_offset + write_len) > FLASH_BUFFER_MAX_BYTES-1){ // verify is address space is within buffer scope
		return ERROR;
	}

	if (buffer_no == BUFFER1)
		cmd [0] = BUFFER_1_WRITE;
	else if (buffer_no == BUFFER2)
		cmd [0] = BUFFER_2_WRITE;

	cmd [1] = 0x00;
	cmd [2] = 0x00;
	cmd [3] = 0xBF;
	cmd [4] = 0xA0+addr_offset;

	FLASH_PIN_SELECTED();

	flash_Write_spi(cmd, 5);
	flash_Write_spi(write_buf, write_len);

	FLASH_PIN_DESELECTED();
	return SUCCESS;
}

/******************************************************************************
 *
 * Description:
 *    Gets Device ID and Manufacturer from SPIFI
 *
 * Returns:
 *   TRUE if initialization successful, otherwise FALSE
 *
 *****************************************************************************/
static Bool flash_GetManufactureDeviceID(FLASH_CTL_t* spifi){
	uint8_t cmd = MANUFACTURER_AND_DEVICE_ID_READ;
	uint8_t len = 4;
	uint8_t* buf = (uint8_t*)&spifi->id.jedec;

	FLASH_PIN_SELECTED();

	flash_Write_spi(&cmd, 1);
	flash_Read_spi(buf, len);
	FLASH_PIN_DESELECTED();

	if (spifi->id.jedec == 0x1f) return TRUE;
	else return FALSE;
}

/******************************************************************************
 *
 * Description:
 *    Get SPIFI from Power down or stand by mode
 *
 * Returns:
 *   NONE
 *
 *****************************************************************************/
static void flash_ExitDeepPowerDown(void){
	uint8_t cmd = RESUME_FROM_DEEP_POWER_DOWN;

	FLASH_PIN_OUTPUT();							// PIN CS as output
	FLASH_PIN_DESELECTED();						//
	Chip_Clock_SetPCLKDiv(SYSCTL_CLOCK_SSP1, SYSCTL_CLKDIV_1);

    /* Init SSP1 */
//	Board_SSP_Init(LPC_SSP1,20000000);
//	Chip_SSP_Init(LPC_SSP1);
//	Chip_SSP_SetBitRate(LPC_SSP1, 10000000);
//	Chip_SSP_Enable(LPC_SSP1);

	FLASH_PIN_SELECTED();

	flash_Write_spi(&cmd, 1);

	FLASH_PIN_DESELECTED();

	return;

}

/******************************************************************************
 *
 * Description:
 *    Get SPIFI from Power down or stand by mode
 *
 * Returns:
 *   NONE
 *
 *****************************************************************************/
static void flash_DeepPowerDOWN(void){
	uint8_t cmd = DEEP_POWER_DOWN;

	FLASH_PIN_SELECTED();

	flash_Write_spi(&cmd, 1);

	FLASH_PIN_DESELECTED();

	return;

}

static void flash_Page2Address(FLASH_CTL_t* spifi){
	spifi->page = 0;

}


static void flash_Address2Page(FLASH_CTL_t* spifi){

}

/******************************************************************************
 *
 * Description:
 *    Get SPIFI Read a page boundary (264 bytes max)
 *
 * Returns:
 *   Buffer with main page read
 *
 *****************************************************************************/
static void flash_MainMemoryPageread(FLASH_CTL_t* spifi){

}


/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize the SPI Flash Driver
 *
 * Returns:
 *   TRUE if initialization successful, otherwise FALSE
 *
 *****************************************************************************/
Status flash_IOCTL(FLASH_IOCTL_t control, FLASH_CTL_t* spifi, uint8_t* buffer, int len ){

	switch (control) {
		case FLASH_INIT:
			if (spifi->dev_state == FLASH_DISABLE) break;
			flash_ExitDeepPowerDown();
			if (flash_GetManufactureDeviceID(&flash) && (flash_CheckForStatus(FLASH_READY))){
				spifi->status = FLASH_READY;
				spifi->dev_state = FLASH_ENABLE;
				return SUCCESS;
			}
			break;
		case FLASH_BUFFER_READ:
			if (spifi->dev_state == FLASH_ENABLE){
				if (flash_bufferRead(spifi->buffer_offset, buffer, len, spifi->buffer_no)){
					spifi->status = FLASH_READY;
					return SUCCESS;
				}else return ERROR;
			}else return ERROR;
			break;
		case FLASH_BUFFER_WRITE:
			if (spifi->dev_state == FLASH_ENABLE){
				if (flash_bufferWrite(spifi->buffer_offset, buffer, len, spifi->buffer_no)){
					spifi->status = FLASH_READY;
					return SUCCESS;
				}else return ERROR;
			}else return ERROR;
			break;
		case FLASH_VERIFY:
			if (spifi->dev_state == FLASH_ENABLE){

			}

			break;
		case FLASH_READ:
			if (spifi->dev_state == FLASH_ENABLE){

			}
			break;
		case FLASH_WRITE:
			if (spifi->dev_state == FLASH_ENABLE){

			}

			break;
		case FLASH_READ_PAGE:
			if (spifi->dev_state == FLASH_ENABLE){

			}

			break;
		case FLASH_WRITE_PAGE:
			if (spifi->dev_state == FLASH_ENABLE){

			}

			break;
		case FLASH_ERASE_PAGE:
			if (spifi->dev_state == FLASH_ENABLE){

			}

			break;
		case FLASH_GET_ID:
			if (spifi->dev_state == FLASH_ENABLE){

			}

			break;
		case FLASH_DEINIT:
			if (spifi->dev_state == FLASH_ENABLE){

			}

			break;
		default:
			spifi->status = FLASH_ERROR;
			break;
	}
	return ERROR;
}
