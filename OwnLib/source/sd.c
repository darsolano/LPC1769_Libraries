/*
 * RAW data on SD Card Read and Write
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sd.h>
#include <define_pins.h>

DEFINE_PIN(CS,1,18)
//#define CS_ON			LPC_GPIO->CLR = GPIO_SD_CS_m
//#define CS_OFF			LPC_GPIO->SET = GPIO_SD_CS_m

static void spi_init(void);
static void spi_txrx(uint8_t* tx, uint8_t* rx, uint16_t len);

static int sd_version;
/* Global variables */
uint8_t CardType; /* card type */
CARDCONFIG CardConfig; /* Card configuration */

/* Embed : SPI initialization code
 *  Clock:
 *    Start the SPI clock at < 200 KHz, after the card is
 *    initalized you can increase the clock rate to < 50 MHz
 *  Bits:
 *    Use 8-bit mode
 *  Polarity:
 *    CPOL = 0 and CPHA = 0.
 *    These tell the SPI peripheral the edges of clock to set up
 *    and lock in data
 *  Mode:
 *    Master mode
 */
static void spi_init() {	//SSP1

	// Power SSP1
	LPC_SYSCTL->PCONP |= (1 << 10);	// PWR SSP1

	// Peripheral clock - select undivided clock for SSP1
	LPC_SYSCTL->PCLKSEL[0] &= ~(3 << 20); // First reset to 0
	LPC_SYSCTL->PCLKSEL[0] |= (1 << 20); // Div by 1

	// Select pin functions
	//   P0.7 as SCK1 (2 at 15:14)
	LPC_IOCON->PINSEL[0] &= ~(3 << 14);
	LPC_IOCON->PINSEL[0] |= (2 << 14);

	//   P0.6 as SSEL1 (2 at 13:12)
	//   P0.8 as MISO1 (2 at 17:16)
	//   P0.9 as MOSI1 (2 at 19:18)
	LPC_IOCON->PINSEL[0] &= ~(3 << 12 | (3 << 16) | (3 << 18));	//Reset config for pins
	LPC_IOCON->PINSEL[0] |= (2 << 12) | (2 << 16) | (2 << 18);// set function for pins
	CS_OUTPUT();
	// SSP1 Control Register 0
	//   8-bit transfers (7 at 3:0)
	//   SPI (0 at 5:4)
	//   Polarity and Phase default to Mode 0
	SDCOMM->CR0 = 7;

	// SSP0 Prescaler
	// The SD spec requires a slow start at 200khz
	SDCOMM->CPSR = SystemCoreClock / SD_LOW_SPEED;

	// SPI Control Register 1
	//   Defaults to Master
	//   Start serial communications (bit 1)
	SDCOMM->CR1 |= (1 << 1);
}

static void spi_txrx(uint8_t* tx, uint8_t* rx, uint16_t len) {
	/* Embed: transmit and receive len bytes
	 * Remember:
	 *   SPI transmits and receives at the same time
	 *   If tx == NULL and you are only receiving then transmit all 0xFF
	 *   If rx == NULL and you are only transmitting then dump all recieved bytes
	 */
	volatile uint_fast8_t dummy = 0;

	while (len--) {
		if (tx == NULL) {
			SDCOMM->DR = 0xff;
		} else {
			SDCOMM->DR = *tx++;
		}

		while ((SDCOMM->SR & (SSP_BSY | SSP_RNE)) != SSP_RNE)
			;
		if (rx == NULL) {
			dummy = SDCOMM->DR;
		} else {
			*rx++ = SDCOMM->DR;
		}
	}
}

static void sd_command(uint8_t index, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4,
		uint8_t crc, uint8_t* response, uint16_t response_len) //{{{
{
	uint16_t tries;
	uint8_t command[6];
	uint8_t rx = 0;

	memset(response, 0, response_len);

	// fill command buffer
	command[0] = 0b01000000 | index; // command index
	command[1] = a1; // arg 0
	command[2] = a2; // arg 1
	command[3] = a3; // arg 2
	command[4] = a4; // arg 3
	command[5] = crc; // CRC

	// transmit command
	spi_txrx(command, NULL, 6);

	// read until stop bit
	tries = 0;
	response[0] = 0xFF;
	while ((response[0] & 0x80) != 0 && tries < SD_MAX_RESP_TRIES) {
		spi_txrx(NULL, response, 1);
		tries++;
	}

	// special bit for the idle command, if no idle response give up now
	if ((response[0] & 0x80) != 0 || (index == 0 && !(response[0] & 0x01)))
		return;

	if (response_len > 1) {
		// get the rest of the response
		spi_txrx(NULL, (response + 1), response_len);
	}

	/* read until the busy flag is cleared,
	 * this also gives the SD card at least 8 clock pulses to give
	 * it a chance to prepare for the next CMD */
	rx = 0;
	while (rx == 0)
		spi_txrx(NULL, &rx, 1);
} //}}}

int sd_init() //{{{
{

	spi_init();

	unsigned char resp[10];

	// send at least 74 clock pulses so the card enters native mode
	spi_txrx(NULL, NULL, 10);

	// keep trying to reset
	uint16_t tries = 0;
	resp[0] = 0;
	while (resp[0] != 0x01 && tries < SD_MAX_RESET_TRIES) {
		CS_ON();
		sd_command(0x00, 0x00, 0x00, 0x00, 0x00, 0x95, resp, 1); // CMD0, R1
		CS_OFF();
		tries++;
	}
	if (tries >= SD_MAX_RESET_TRIES)
		return -1;

	// check voltage range and check for V2
	CS_ON();
	sd_command(0x08, 0x00, 0x00, 0x01, 0xAA, 0x87, resp, 5); // CMD8, R7
	CS_OFF();

	// V2 and voltage range is correct, have to do this for V2 cards
	if (resp[0] == 0x01) {
		if (!(resp[1] == 0 && resp[2] == 0 && resp[3] == 0x01 && resp[4] == 0xAA))
			// voltage range is incorrect
			return -2;
	}

	// the initialization process
	while (resp[0] != 0x00) // 0 when the card is initialized
	{
		CS_ON();
		sd_command(55, 0x00, 0x00, 0x00, 0x00, 0x00, resp, 1); // CMD55
		CS_OFF();
		if (resp[0] != 0x01)
			return -3;
		CS_ON();

		// ACMD41 with HCS (bit 30) HCS is ignored by V1 cards
		sd_command(41, 0x40, 0x00, 0x00, 0x00, 0x00, resp, 1);

		CS_OFF();
	}

	// Set clock speed to High Speed
	SDCOMM->CPSR = SystemCoreClock / SD_HIGH_SPEED;

	// check the OCR register to see if it's a high capacity card
	CS_ON();
	sd_command(58, 0x00, 0x00, 0x00, 0x00, 0x00, resp, 5); // CMD58
	CS_OFF();
	if ((resp[1] & 0x40) > 0)
		sd_version = 2; // V2 card
	else
		// set the block length CMD16 to 512
		sd_version = 1; // V1 card
	return 0;
} //}}}

char sd_read_block(uint8_t* block, uint32_t block_num) //{{{
{
	// TODO bounds checking
	uint8_t rx = 0xFF;

	// send the single block command
	CS_ON();
	sd_command(CMD17, (0xFF000000 & block_num) >> 24,
			(0xFF0000 & block_num) >> 16, (0xFF00 & block_num) >> 8,
			0xFF & block_num, 0, &rx, 1); // CMD17

	// Could be an issue here where the last 8 of SD command contains
	// the token, but I doubt this happens

	if (rx != 0x00)
		return 0;

	// read until the data token is received
	rx = 0xFF;
	while (rx != 0b11111110)
		spi_txrx(NULL, &rx, 1);

	spi_txrx(NULL, block, SD_BLOCK_LEN); // read the block
	spi_txrx(NULL, NULL, 2); // throw away the CRC
	spi_txrx(NULL, NULL, 1); // 8 cycles to prepare the card for the next command

	CS_OFF();

	return 1;
} //}}}

char sd_write_block(uint8_t* block, uint32_t block_num) //{{{
{
	// TODO bounds checking
	uint8_t rx = 0xFF;
	uint8_t tx[2];

	// send the single block write
	CS_ON();
	sd_command(CMD24, (0xFF000000 & block_num) >> 24,
			(0xFF0000 & block_num) >> 16, (0xFF00 & block_num) >> 8,
			0xFF & block_num, 0, &rx, 1); // CMD24

	// Could be an issue here where the last 8 of SD command contains
	// the token, but I doubt this happens
	if (rx != 0x00)
		return 0;

	// tick clock 8 times to start write operation
	spi_txrx(NULL, NULL, 1);

	// write data token
	tx[0] = 0xFE;
	spi_txrx(tx, NULL, 1);

	// write data
	memset(tx, 0, sizeof(tx));
	spi_txrx(block, NULL, SD_BLOCK_LEN); // write the block
	spi_txrx(tx, NULL, 2); // write a blank CRC
	spi_txrx(NULL, &rx, 1); // get the response

	// check if the data is accepted
	if (!((rx & 0xE) >> 1 == 0x2))
		return 0;

	// wait for the card to release the busy flag
	rx = 0;
	while (rx == 0)
		spi_txrx(NULL, &rx, 1);

	spi_txrx(NULL, NULL, 1); // 8 cycles to prepare the card for the next command

	CS_OFF();
	return 1;
} //}}}

/**
 * @brief  Read card configuration and fill structure CardConfig.
 *
 * @param  None
 * @retval SD_TRUE or SD_FALSE.
 */

/*
SD_BOOL sd_read_configuration(void) {
	uint8_t buf[16];
	uint32_t i, c_size, c_size_mult, read_bl_len;
	SD_BOOL retv;

	retv = SD_FALSE;

	// Read OCR
	if (SD_SendCommand(READ_OCR, 0, CardConfig.ocr, 4) != R1_NO_ERROR)
		goto end;

	// Read CID
	   if ((sd_command(SEND_CID, 0, NULL, 0) != R1_NO_ERROR) ||
	       SD_RecvDataBlock (CardConfig.cid, 16)==SD_FALSE) goto end;

	// Read CSD
	if ((SD_SendCommand(SEND_CSD, 0, NULL, 0) != R1_NO_ERROR)
			|| SD_RecvDataBlock(CardConfig.csd, 16) == SD_FALSE)
		goto end;

	// sector size
	CardConfig.sectorsize = 512;

	// sector count
	if (((CardConfig.csd[0]>>6) & 0x3) == 0x1) // CSD V2.0 (for High/eXtended Capacity)
	{
		// Read C_SIZE
		c_size = (((uint32_t) CardConfig.csd[7] << 16)
				+ ((uint32_t) CardConfig.csd[8] << 8) + CardConfig.csd[9])
				& 0x3FFFFF;
		// Calculate sector count
		CardConfig.sectorcnt = (c_size + 1) * 1024;

	}
	else   //CSD V1.0 (for Standard Capacity)
	{
		// C_SIZE
		c_size = (((uint32_t)(CardConfig.csd[6]&0x3)<<10) + ((uint32_t)CardConfig.csd[7]<<2) + (CardConfig.csd[8]>>6)) & 0xFFF;
		// C_SIZE_MUTE
		c_size_mult = ((CardConfig.csd[9]&0x3)<<1) + ((CardConfig.csd[10]&0x80)>>7);
		/// READ_BL_LEN
		read_bl_len = CardConfig.csd[5] & 0xF;
		//sector
		count = BLOCKNR*BLOCK_LEN/512; 		//we manually set SECTOR_SIZE to 512
		CardConfig.sectorcnt = (c_size+1)*(1<<read_bl_len) * (1<<(c_size_mult+2)) / 512;
		CardConfig.sectorcnt = (c_size+1) << (read_bl_len + c_size_mult - 7);
	}

	// Get erase block size in unit of sector
	switch (CardType) {
	case CARDTYPE_SDV2_SC:
	case CARDTYPE_SDV2_HC:
		if ((SD_SendACommand(SD_STATUS, 0, buf, 1) != R1_NO_ERROR)
				|| SD_RecvDataBlock(buf, 16) == SD_FALSE)
			goto end;
		// Read partial block
		for (i = 64 - 16; i; i--)
			spi_txrx(0, 0, 0);  // Purge trailing data
		CardConfig.blocksize = 16UL << (buf[10] >> 4); // Calculate block size based on AU size
		break;
	case CARDTYPE_MMC:
		CardConfig.blocksize =
				((uint16_t) ((CardConfig.csd[10] & 124) >> 2) + 1)
						* (((CardConfig.csd[10] & 3) << 3)
								+ ((CardConfig.csd[11] & 224) >> 5) + 1);
		break;
	case CARDTYPE_SDV1:
		CardConfig.blocksize = (((CardConfig.csd[10] & 63) << 1)
				+ ((uint16_t) (CardConfig.csd[11] & 128) >> 7) + 1)
				<< ((CardConfig.csd[13] >> 6) - 1);
		break;
	default:
		goto end;
	}

	retv = SD_TRUE;
	end:
	CS_OFF;

	return retv;
}
*/

