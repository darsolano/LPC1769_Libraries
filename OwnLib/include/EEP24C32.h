/* 
 * File:   24C32.h
 * Author: dsolano
 *
 * Created on June 26, 2014, 12:36 AM
 */
#pragma once
#ifndef EEP24C32_H
#define	EEP24C32_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <chip.h>
#include <lpc_types.h>

    //#define DBGPRINT
    // Peripheral ID for EEPROM 24C32 4KBytes from ATMEL
#define I2C_24C32                           0x50
#define I2C_24C32_ADDR                      (I2C_24C32 << 1)
#define I2C_24C32_BUS                       LPC_I2C1
#define EE_READ                             1
#define EE_WRITE                            0
#define DS1307_I2C_CLOCK_FREQ               400000      // 400KHz

#define PAGE_BYTES_SIZE                     0x001F
#define MAX_8BYTE_PAGES                     0xEF
#define FIRST_PAGE                          0x000
#define LAST_PAGE                           0xFE0
#define PAGE_INCREMENT                      

    // bitwise and for those mask
#define LOW_ADDRESS_MASK                    0b0000000000011111
#define PAGE_ADDRESS_MASK                   0b0000111111100000
#define PAGE(p)                             (p << 5)

#define EEP_ACK                             1
#define EEP_NACK                            0

    typedef union {
        struct EE_Address {
            unsigned Low : 5;
            unsigned High : 7;
            unsigned : 4;
        } EE_Address_t;

        struct EE_WordAddr {
            unsigned Addr : 12;
            unsigned : 4;
        } EE_WordAddr_t;

        struct WordAddr {
            uint16_t w_addr;
        } WordAddr_t;
    } EE24C32_Addr_t;


    /*
     * Function Prototypes
     */
    uint8_t EEPReadByte(uint16_t address);
    Status  EEPWriteByte(uint16_t addr, uint8_t data);
    Status  EEPWriteString(uint16_t addr, uint8_t* buff, uint8_t size);
    Status 	EEPReadString(uint16_t addr, uint8_t size, uint8_t* ptr);
    Status 	EEPReadPage (uint16_t page , uint8_t *buf);
    Status  EEPReadByteLen(uint16_t addr, uint16_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* EEP24C32_H */

