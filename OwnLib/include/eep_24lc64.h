/*
 * eep_24lc64.c on board lpcxpresso1769 eeprom
 *
 *  Created on: Oct 7, 2017
 *      Author: dsolano
 */

#ifndef INCLUDE_EEP_24LC64_H_
#define INCLUDE_EEP_24LC64_H_

#include <chip.h>

#define EEP24LC64_I2C_ADDR			0x50
#define EEP24LC64_I2C_BUS			I2C1

#define PAGE_MASK					0x1FE0
#define ADDR_MASK					0x1FFF
#define OFFSET_MASK					0x001F
#define ADDRL_MASK					0x00FF
#define ADDRH_MASK					0x1F00

#define EEP24LC64_TOTAL_SIZE		0x2000
#define EEP24LC64_PAGE_SIZE			(32)
#define EEP24LC64_TOTAL_PAGES		EEP24LC64_TOTAL_SIZE/EEP24LC64_PAGE_SIZE

#define EEP24LC64_START_ADDR		0x0
#define EEP24LC64_LAST_ADDR			0x1FFF

typedef enum{
	ADDR = 0,
	PAGE
}Query_Type_t;

typedef enum{
	EE_READ = 0,
	EE_WRITE
}RW_EEP_t;

typedef struct {
	uint8_t page;
	uint8_t offset;
	uint8_t addrh;
	uint8_t addrl;
	uint8_t byte_to_rw;
	uint8_t rw_len;
	uint8_t* rw_buf;
	uint16_t addr;
}EEPROM_t;

extern EEPROM_t peep;
extern uint8_t xch_buffer[34];


uint8_t  ee_read_byte(uint16_t addr);
Status   ee_write_byte(uint16_t addr, uint8_t data);
Status   ee_read_stream(uint16_t addr, uint8_t* buf, int len);
Status ee_write_stream(uint16_t addr, uint8_t* data, int len);
#endif /* INCLUDE_EEP_24LC64_H_ */
