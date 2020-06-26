#ifndef SD_H
#define SD_H

#include <chip.h>
#include <lpc_types.h>

#define SDCOMM				LPC_SSP1
#define SD_LOW_SPEED		200000
#define SD_HIGH_SPEED		20000000
#define SD_BLOCK_LEN 512
#define SD_MAX_RESP_TRIES 	100
#define SD_MAX_RESET_TRIES 	100
#define SD_INVALID_SECTOR 	0xFFFFFFFF

#define GPIO_SD_CS_m 		(1<<6) // 498A: Defined as P0
#define SSP_RNE 			(1<<2)
#define SSP_BSY 			(1<<4)

//***************************************************************
typedef struct
{
  uint32_t timeout_write;
  uint32_t timeout_read;
  uint8_t busyflag;
} sd_context_t;

#define R1 						1
#define R1B 					2
#define R2 						3
#define R3 						4
#define MSK_IDLE 				0x01
#define MSK_ERASE_RST 			0x02
#define MSK_ILL_CMD 			0x04
#define MSK_CRC_ERR 			0x08
#define MSK_ERASE_SEQ_ERR 		0x10
#define MSK_ADDR_ERR 			0x20
#define MSK_PARAM_ERR 			0x40


#define SD_TOK_READ_STARTBLOCK 		0xFE
#define SD_TOK_WRITE_STARTBLOCK 	0xFE
#define SD_TOK_READ_STARTBLOCK_M 	0xFE
#define SD_TOK_WRITE_STARTBLOCK_M 	0xFC
#define SD_TOK_STOP_MULTI 			0xFD
/* Error token is 111XXXXX */
#define MSK_TOK_DATAERROR 		0xE0
/* Bit fields */
#define MSK_TOK_ERROR 			0x01
#define MSK_TOK_CC_ERROR 		0x02
#define MSK_TOK_ECC_FAILED 		0x04
#define MSK_TOK_CC_OUTOFRANGE 	0x08
#define MSK_TOK_CC_LOCKED 		0x10
/* Mask off the bits in the OCR corresponding to voltage range 3.2V to
* 3.4V, OCR bits 20 and 21 */
#define MSK_OCR_33 				0xC0
/* Number of times to retry the probe cycle during initialization */
#define SD_INIT_TRY 			50
/* Number of tries to wait for the card to go idle during initialization */
#define SD_IDLE_WAIT_MAX 		100
/* Hardcoded timeout for commands. 8 words, or 64 clocks. Do 10
* words instead */
#define SD_CMD_TIMEOUT 			100
/******************************** Basic command set **************************/
/* Reset cards to idle state */
#define CMD0 					0
#define CMD0_R 					R1
/* Read the OCR (MMC mode, do not use for SD cards) */
#define CMD1 					1
#define CMD1_R 					R1
/* Card sends the CSD */
#define CMD9 					9
#define CMD9_R 					R1
/* Card sends CID */
#define CMD10 					10
#define CMD10_R 				R1
/* Stop a multiple block (stream) read/write operation */
#define CMD12 					12
#define CMD12_R 				R1B
/* Get the addressed card's status register */
#define CMD13 					13
#define CMD13_R 				R2
/***************************** Block read commands **************************/
/* Set the block length */
#define CMD16 					16
#define CMD16_R 				R1
/* Read a single block */
#define CMD17 					17
#define CMD17_R 				R1
/* Read multiple blocks until a CMD12 */
#define CMD18 					18
#define CMD18_R 				R1
/***************************** Block write commands *************************/
/* Write a block of the size selected with CMD16 */
#define CMD24 					24
#define CMD24_R 				R1
/* Multiple block write until a CMD12 */
#define CMD25 					25
#define CMD25_R 				R1
/* Program the programmable bits of the CSD */
#define CMD27 					27
#define CMD27_R 				R1
/***************************** Write protection *****************************/
/* Set the write protection bit of the addressed group */
#define CMD28 					28
#define CMD28_R 				R1B
/* Clear the write protection bit of the addressed group */
#define CMD29 					29
#define CMD29_R 				R1B
/* Ask the card for the status of the write protection bits */
#define CMD30 					30
#define CMD30_R 				R1
/***************************** Erase commands *******************************/
/* Set the address of the first write block to be erased */
#define CMD32 					32
#define CMD32_R 				R1
/* Set the address of the last write block to be erased */
#define CMD33 					33
#define CMD33_R 				R1
/* Erase the selected write blocks */
#define CMD38 					38
#define CMD38_R 				R1B
/***************************** Lock Card commands ***************************/
/* Commands from 42 to 54, not defined here */
/***************************** Application-specific commands ****************/
/* Flag that the next command is application-specific */
#define CMD55 					55
#define CMD55_R 				R1
/* General purpose I/O for application-specific commands */
#define CMD56 					56
#define CMD56_R 				R1
/* Read the OCR (SPI mode only) */
#define CMD58 					58
#define CMD58_R 				R3
/* Turn CRC on or off */
#define CMD59 					59
#define CMD59_R 				R1
/***************************** Application-specific commands ***************/
/* Get the SD card's status */
#define ACMD13 					13
#define ACMD13_R 				R2
/* Get the number of written write blocks (Minus errors ) */
#define ACMD22 					22
#define ACMD22_R 				R1
/* Set the number of write blocks to be pre-erased before writing */
#define ACMD23 					23
#define ACMD23_R 				R1
/* Get the card's OCR (SD mode) */
#define ACMD41 					41
#define ACMD41_R 				R1
/* Connect or disconnect the 50kOhm internal pull-up on CD/DAT[3] */
#define ACMD42 					42
#define ACMD42_R 				R1
/* Get the SD configuration register */
#define ACMD51 					42
#define ACMD51_R 				R1

//***************************************************************
typedef unsigned char    SD_BOOL;
#define SD_TRUE     1
#define SD_FALSE    0

/* Command definitions in SPI bus mode */
#define GO_IDLE_STATE           0
#define SEND_OP_COND            1
#define SWITCH_FUNC             6
#define SEND_IF_COND            8
#define SEND_CSD                9
#define SEND_CID                10
#define STOP_TRANSMISSION       12
#define SEND_STATUS             13
#define SET_BLOCKLEN            16
#define READ_SINGLE_BLOCK       17
#define READ_MULTIPLE_BLOCK     18
#define WRITE_SINGLE_BLOCK      24
#define WRITE_MULTIPLE_BLOCK    25
#define APP_CMD                 55
#define READ_OCR                58
#define CRC_ON_OFF              59

/* Application specific commands supported by SD.
All these commands shall be preceded with APP_CMD (CMD55). */
#define SD_STATUS               13
#define SD_SEND_OP_COND         41

/* R1 response bit flag definition */
#define R1_NO_ERROR         0x00
#define R1_IN_IDLE_STATE    0x01
#define R1_ERASE_RESET      0x02
#define R1_ILLEGAL_CMD      0x04
#define R1_COM_CRC_ERROR    0x08
#define R1_ERASE_SEQ_ERROR  0x10
#define R1_ADDRESS_ERROR    0x20
#define R1_PARA_ERROR       0x40
#define R1_MASK             0x7F

/* The sector size is fixed to 512bytes in most applications. */
#define SECTOR_SIZE 512


/* Memory card type definitions */
#define CARDTYPE_UNKNOWN        0
#define CARDTYPE_MMC            1   /* MMC */
#define CARDTYPE_SDV1           2   /* V1.x Standard Capacity SD card */
#define CARDTYPE_SDV2_SC        3   /* V2.0 or later Standard Capacity SD card */
#define CARDTYPE_SDV2_HC        4   /* V2.0 or later High/eXtended Capacity SD card */

/* SD/MMC card configuration */
typedef struct tagCARDCONFIG
{
    uint32_t sectorsize;    /* size (in byte) of each sector, fixed to 512bytes */
    uint32_t sectorcnt;     /* total sector number */
    uint32_t blocksize;     /* erase block size in unit of sector */
	uint8_t  ocr[4];		/* OCR */
	uint8_t  cid[16];		/* CID */
	uint8_t  csd[16];		/* CSD */
}  CARDCONFIG;

int  sd_init(void);
//void sd_command(uint8_t index, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t crc, uint8_t* response, uint16_t response_len);
char sd_read_block(uint8_t* block, uint32_t block_num);
char sd_write_block(uint8_t* block, uint32_t block_num);

#endif
