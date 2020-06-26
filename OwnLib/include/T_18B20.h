/* 
 * File:   T_18B20.h
 * Author: dsolano
 *
 * Created on June 27, 2014, 6:42 PM
 */

#ifndef T_18B20_H
#define	T_18B20_H

#define _1ENABLED
#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _1ENABLED
#include <_1wire.h>

// Commands for DS18B20 Dallas Semiconductor 18B20
    // ROM Commands
#define SEARCH_ROM          0xF0    //Verify existance od Devices on multislave network
#define READ_ROM            0x33    //Get ROM Code out of Device
#define MATCH_ROM           0x55    //ID device with the address collected
#define SKIP_ROM            0xCC
#define ALARM_SEARCH        0xEC

    //Funtion Set Commands
#define CONVERT_T           0x44    // Start Temperature convertions
#define WRITE_SCRATCHPAD    0x4E    // Writes USer Byte and Config Register
#define READ_SCRATCHPAD     0xBE    // Read all 9 Bytes from memory on DS18B20
#define COPY_SCRATCHPAD     0x48    // Copy Memory to EEPROM on DS18B20
#define RECALL_E2           0xB8    // Restore EEPROM values to memory
#define READ_POWER_SUPPLY   0xB4    // Get the Power Supply status on DS18B20


typedef struct{
	uint8_t ROM_ID_BYTE[8];
}ROMADDR_sType;
ROMADDR_sType RomID;

struct{
    unsigned reserved   :   5;      //reads 1 always
    unsigned R0         :   1;
    unsigned R1         :   1;
    unsigned            :   1;      // reads 0 always
}CONFIGBits_s;

typedef union{
	struct
	{
		uint8_t TempLow;
		uint8_t TempHi;
		uint8_t UserByte1;
		uint8_t UserByte2;
		uint8_t Config;                  //config register
    	uint8_t reserved0;                // unknown
    	uint8_t reserved1;                // reads 0x10
    	uint8_t reserved2;
    	uint8_t CRC;                     // Verify CRC calculations
	};
		uint8_t ScratchBYTES[9];
}SCARTCHPAD_sType;
SCARTCHPAD_sType ScratchPAD;

typedef enum{
	Celcius = 0,
	Farenheit
}Temp_Convert_sType;


typedef struct{
    uint8_t Temp_LowByte;
    uint8_t Temp_HighByte;
    uint32_t Whole;
    uint32_t Decimal;
    uint8_t Temp_Type;
}Temperature_s;

//Define the type of Power Supply that a device has in a particular time.
typedef enum{
    PARASITE,
    EXTERNAL
}POWERTYPE_t;

// Funtions Prototypes
PRESENCE_t 	DS18B20Init(void);
void	 	DS18B20CmdGetROMIDCode(ROMADDR_sType *promid);
Bool    	DS18B20ConfirmIDofDevice(ROMADDR_sType *promid);
Bool    	DS18B20WriteAlarm_CfgReg(uint16_t Alarm, uint8_t Config);
void  		DS18B20GetTemperature(Temperature_s *temp);
void    	DS18B20RestoreEEPROMValues(void);
void    	DS18B20SaveSRAMValues(void);
POWERTYPE_t DS18B20GetPWRSupplyStatus(void);
uint8_t   	DS18B20CalcAndCompareCRC(uint8_t _crc);
void 		DS18B20GetScrachtPadReg(SCARTCHPAD_sType *pad);



#ifdef	__cplusplus
}
#endif

#endif	/* T_18B20_H */

#endif
