/*
 * AT45DB081D.h
 *
 *  Created on: Oct 30, 2017
 *      Author: dsolano
 */

#ifndef INCLUDE_AT45DB081D_H_
#define INCLUDE_AT45DB081D_H_

#include <chip.h>


#define HIGH_FREQ		66000000
#define LOW_FREQ		20000000

/*
 * DATA for current flash chip AT45DB081D
 */
#define  FLASH_MAX_SECTORS	 		16
#define  FLASH_MAX_BLOCKS	 		512
#define  FLASH_MAX_PAGES	 		4096
#define  FLASH_BYTES_PER_PAGE 		264
#define  FLASH_BYTES_PER_PAGE_POW2 	256
#define  FLASH_PAGE_PER_BLOCK		8
#define  FLASH_BLOCKS_PER_SECTOR	32
#define  FLASH_BUFFER_MAX_BYTES		264
#define  FLASH_BUFFER_MAX_BYTES_POW2	256

/*
 * command table for AT45DB081D operation
 */
//Read Commands
#define MAIN_MEMORY_PAGE_READ 						0xD2
#define CONTINUOUS_ARRAY_READ 						0xE8
#define CONTINUOUS_ARRAY_READ_LOW_FREQUENCY		 	0x03
#define CONTINUOUS_ARRAY_READ_HIGH_FREQUENCY		0x0B
#define BUFFER_1_READ_LOW_FREQ						0xD1
#define BUFFER_2_READ_LOW_FREQ						0xD3
#define BUFFER_1_READ								0xD4
#define BUFFER_2_READ								0xD6

//Program and Erase Commands
#define BUFFER_1_WRITE 												0x84
#define BUFFER_2_WRITE 												0x87
#define BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_BUILTIN_ERASE 	0x83
#define BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_BUILTIN_ERASE 	0x86
#define BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITHOUT_BUILTIN_ERASE 	0x88
#define BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITHOUT_BUILTIN_ERASE 	0x89
#define PAGE_ERASE 													0x81
#define BLOCK_ERASE 												0x50
#define SECTOR_ERASE 												0x7C
#define CHIP_ERASE 													0xC794809Aul
#define MAIN_MEMORY_PAGE_PROGRAM_THROUGH_BUFFER_1 					0x82
#define MAIN_MEMORY_PAGE_PROGRAM_THROUGH_BUFFER_2 					0x85

//Protection and Security Commands
#define ENABLE_SECTOR_PROTECTION 			0x3D2A7FA9
#define DISABLE_SECTOR_PROTECTION 			0x3D2A7F9A
#define ERASE_SECTOR_PROTECTION_REGISTER 	0x3D2A7FCF
#define PROGRAM_SECTOR_PROTECTION_REGISTER 	0x3D2A7FFC
#define READ_SECTOR_PROTECTION_REGISTER 	0x32
#define SECTOR_LOCKDOWN 					0x3D2A7F30
#define READ_SECTOR_LOCKDOWN_REGISTER 		0x35
#define PROGRAM_SECURITY_REGISTER 			0x9B000000
#define READ_SECURITY_REGISTER 				0x77

//Additional Commands
#define MAIN_MEMORY_PAGE_TO_BUFFER_1_TRANSFER 	0x53
#define MAIN_MEMORY_PAGE_TO_BUFFER_2_TRANSFER 	0x55
#define MAIN_MEMORY_PAGE_TO_BUFFER_1_COMPARE 	0x60
#define MAIN_MEMORY_PAGE_TO_BUFFER_2_COMPARE 	0x61
#define AUTO_PAGE_REWRITE_THROUGH_BUFFER_1 		0x58
#define AUTO_PAGE_REWRITE_THROUGH_BUFFER_2 		0x59
#define DEEP_POWER_DOWN 						0xB9
#define RESUME_FROM_DEEP_POWER_DOWN 			0xAB
#define STATUS_REGISTER_READ 					0xD7
#define MANUFACTURER_AND_DEVICE_ID_READ 		0x9F

#define DUMMY_BYTE								0x00
// Status Register
#define STATUS_MASK								0xC3
typedef enum{
	FLASH_PAGE_SIZE_256     = 	0x01,
	FLASH_PROTECTED 	    =	0x02,
	FLASH_MEM_BUFF_NOMATCH 	=   0x40,
	FLASH_READY 		    = 	0x80,
	FLASH_ERROR			    =   0xA5,
	FLASH_DISABLE			=	0xA6,
	FLASH_ENABLE			=	0xA7
}STATUS_t;

//Buffer Number
typedef enum{
	BUFFER1 = 1,
	BUFFER2
}BUFFER_t;

typedef enum{
	FLASH_READ =1,
	FLASH_WRITE,
	FLASH_ERASE_PAGE,
	FLASH_READ_PAGE,
	FLASH_WRITE_PAGE,
	FLASH_BUFFER_READ,
	FLASH_BUFFER_WRITE,
	FLASH_INIT,
	FLASH_DEINIT,
	FLASH_VERIFY,
	FLASH_GET_ID
}FLASH_IOCTL_t;

typedef struct{
	uint8_t jedec;
	uint8_t id1;
	uint8_t id2;
	uint8_t Extended_Info;
}__attribute__((__packed__)) DEVICEID_t;

typedef struct{
	STATUS_t status;
	BUFFER_t buffer_no;
	uint16_t buffer_offset;
	uint16_t page_offset;
	uint16_t buff_addr;
	uint16_t page_address;
	uint16_t page;
	uint16_t block;
	uint16_t sector;
	DEVICEID_t id;
	STATUS_t dev_state;
} __attribute__((__packed__)) FLASH_CTL_t;

/*
 * multiple function Subroutine
 * INPUT - Task define by enum FLASH_IOCTL_t, Data Structure for spifi
 * and buffer to hold data and integer to tell how long the data is.
 *
 * OUTPUT -  i carried aout inside the dat structures and direct response
 * from the function as a Success or Error
 */
Status flash_IOCTL(FLASH_IOCTL_t control, FLASH_CTL_t* spifi, uint8_t* buffer, int len );


#endif /* INCLUDE_AT45DB081D_H_ */
