/*
 * LPCXpresso1769
 T_18B20.c - library for 18B20 Thermometer
 * Created on June 26, 2014, 12:36 AM
 */

#define _1ENABLED
#ifdef _1ENABLED

#include <T_18B20.h>
#include <hardware_delay.h>
#include <xprintf.h>

PRESENCE_t DS18B20Init(void)
{

	OW_PinInit(2,13);
	if (OW_reset_pulse() == OW_DEV_PRESENT)
	{
		DS18B20CmdGetROMIDCode(&RomID);
		DS18B20GetScrachtPadReg(&ScratchPAD);
		return OW_DEV_PRESENT;
	}
	else
	{
		return OW_DEV_NOT_PRESENT;
	}
}

void DS18B20CmdGetROMIDCode(ROMADDR_sType *promid)
{
	int loop;
	OW_reset_pulse();
	OW_write_byte(READ_ROM);
	for (loop = 0; loop < 8; loop++)
	{
		promid->ROM_ID_BYTE[loop] = OW_read_byte();
	}
}

Bool DS18B20ConfirmIDofDevice(ROMADDR_sType *promid)
{
	uint8_t i;
	if (OW_reset_pulse())
		return FALSE;
	OW_write_byte(0x55); // match ROM

	for (i = 0; i < 8; i++)
	{
		OW_write_byte(promid->ROM_ID_BYTE[i]); //send ROM code
		_delay_uS(DELAY_10Us);
	}
	return TRUE;
}
/*
Bool DS18B20WriteAlarm_CfgReg(uint16_t Alarm, uint8_t Config)
{

}
*/
void DS18B20GetTemperature(Temperature_s *temp)
{

	int c16, c2, f10;

	OW_reset_pulse();
	OW_write_byte(0xCC); //Skip ROM
	OW_write_byte(0x44); // Start Conversion
	_delay_uS(5);
	OW_reset_pulse();
	OW_write_byte(0xCC); // Skip ROM
	OW_write_byte(0xBE); // Read Scratch Pad
	//from scratchpad read, the first 2 byte are from temp register, others are dropped
	temp->Temp_LowByte = OW_read_byte();
	temp->Temp_HighByte = OW_read_byte();
	OW_reset_pulse();
	// Celsius calculation
	if (temp->Temp_Type == Celcius)
	{
		temp->Whole = (temp->Temp_HighByte & 0x07) << 4;// grab lower 3 bits of t1
		temp->Whole |= temp->Temp_LowByte >> 4;		// and upper 4 bits of t0
		temp->Decimal = temp->Temp_LowByte & 0x0F;// decimals in lower 4 bits of t0
		temp->Decimal *= 625;		// conversion factor for 12-bit resolution
	}
	// Farenheit convertion
	if (temp->Temp_Type == Farenheit)
	{
		c16 = (temp->Temp_HighByte << 8) + temp->Temp_LowByte;// result is temp*16, in celcius
		c2 = c16 / 8;					// get t*2, with fractional part lost
		f10 = c16 + c2 + 320;	// F=1.8C+32, so 10F = 18C+320 = 16C + 2C + 320
		temp->Whole = f10 / 10;								// get whole part
		temp->Decimal = f10 % 10;						// get fractional part
	}
}
/*
void DS18B20RestoreEEPROMValues(void)
{

}

void DS18B20SaveSRAMValues(void)
{

}

POWERTYPE_t DS18B20GetPWRSupplyStatus(void)
{

}

uint8_t DS18B20CalcAndCompareCRC(uint8_t _crc)
{

}
*/
void DS18B20GetScrachtPadReg(SCARTCHPAD_sType *pad)
{
	uint8_t i=0;
	OW_reset_pulse();
	OW_write_byte(0xCC); // Skip ROM
	OW_write_byte(0xBE); // Read Scratch Pad

	for(i=0;i<9;i++)
	{
		pad->ScratchBYTES[i] = OW_read_byte();
	}
}

#endif
