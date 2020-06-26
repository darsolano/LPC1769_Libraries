/*
 * wireless.c with nrf24l01+
 *
 *  Created on: 16/1/2015
 *      Author: dsolano
 */

#include <wireless.h>
#include <string.h>
#include <hardware_delay.h>


	 //* CE PIN 0.21
	 //* CSN PIN 0.16
#define PIN_CE			_BIT(21)	// @ port 2
#define PIN_CSN			_BIT(16)	// @ port 0
/**
 * Set chip enable
 */

#define RADIO_ON			Chip_GPIO_SetValue(LPC_GPIO , 0 , PIN_CE)
#define RADIO_OFF			Chip_GPIO_ClearValue( LPC_GPIO , 0 , PIN_CE)

/**
 * Set chip select pin
  */
#define CSN_ON				Chip_GPIO_ClearValue(LPC_GPIO , 0, PIN_CSN)
#define CSN_OFF				Chip_GPIO_SetValue(LPC_GPIO , 0 , PIN_CSN)


const uint8_t TX_PAYLOAD[16] = 		{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                                 	 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};

const uint8_t _TX_ADDR[] = {0x0a,0x0a,0x0a,0x0a,0x10};
uint8_t databuff[32];
uint8_t rxbuff[5];

//*********************************************
//     Start of local functions prototypes
//*********************************************

/*
 * SPI_RW low level function to write and read SPI_RW HWD device on LPC1769
 */
static char SPI_RW(char TX_Data)
{
	while (!(RF24L01p->SR & SSP_STAT_TFE));
		RF24L01p->DR = TX_Data;	// send data
	while (!(RF24L01p->SR & SSP_STAT_RNE) );
		return RF24L01p->DR;	// Receive data

}


/****************************************************************
 * RADIO BASIC LOW LEVEL FUNCTIONS - ALL PRIVATE
 ****************************************************************/

/**
 Read a register

 @param Reg Register to read

 @return Registry Value
*/
char NRF24L01_ReadReg(char Reg) {
	char Result;

	CSN_ON;
	SPI_RW(Reg);
	Result = SPI_RW(NOP);
	CSN_OFF;

	return Result;
}

/**
 Returns the STATUS register and then modify a register

 @param Reg Register to change
 @param Value New value

 @return STATUS Register
*/
char NRF24L01_WriteReg(char Reg, char Value) {
	char Result;

	CSN_ON;
	Result = SPI_RW(Reg);
	SPI_RW(Value);
	CSN_OFF;

	return Result;
}

/**
 Returns the STATUS register and then read "n" registers

 @param Reg Register to read
 @param Buf Pointer to a buffer
 @param Size Buffer Size

 @return STATUS Register
*/
char NRF24L01_ReadRegBuf(char Reg, char *Buf, int Size) {
	int i;
	char Result;

	CSN_ON;
	Result = SPI_RW(Reg);

	for (i = 0; i < Size; i++) {
		Buf[i] = SPI_RW(NOP);
	}

	CSN_OFF;

	return Result;
}

/**
 Returns the STATUS register and then write "n" registers

 @param Reg Registers to change
 @param Buf Pointer to a buffer
 @param Size Buffer Size

 @return STATUS Register
*/
char NRF24L01_WriteRegBuf(char Reg, char *Buf, int Size) {
	int i;
	char Result;

	CSN_ON;
	Result = SPI_RW(Reg);

	for (i = 0; i < Size; i++) {
		SPI_RW(Buf[i]);
	}
	CSN_OFF;

	return Result;
}

/**
 Returns the STATUS register

 @return STATUS Register
*/
char NRF24L01_Get_Status(void) {
	char Result;

	CSN_ON;
	Result = SPI_RW(NOP);
	CSN_OFF;

	return Result;
}

/**
 Returns the carrier signal in RX mode (high when detected)

 @return CD
*/
char NRF24L01_Get_CD(void) {
	if (NRF24L01_ReadReg(CD) & 1)
	{
		Chip_GPIO_SetValue(LPC_GPIO , 2 , _BIT(13));
		return 1;
	}
	else
	{
		Chip_GPIO_ClearValue(LPC_GPIO , 2 , _BIT(13));
		return 0;
	}
}

/**
 Select power mode

 @param Mode = _POWER_DOWN, _POWER_UP

 @see _POWER_DOWN
 @see _POWER_UP

*/
void NRF24L01_Set_Power(char Mode) {
	char Result;

	Result = NRF24L01_ReadReg(CONFIG) & 0b01111101; // Read Conf. Reg. AND Clear bit 1 (PWR_UP) and 7 (Reserved)
	NRF24L01_WriteReg(W_REGISTER | CONFIG, Result | Mode);
}

/**
 Select the radio channel

 @param CH = 0..125

*/
void NRF24L01_Set_CH(char CH) {
	NRF24L01_WriteReg(W_REGISTER | RF_CH, (CH & 0b01111111)); // Clear bit 8
}

/**
 Select Enhanced ShockBurst ON/OFF

 Disable this functionality to be compatible with nRF2401

 @param Mode = _ShockBurst_ON, _ShockBurst_OFF

 @see _ShockBurst_ON
 @see _ShockBurst_OFF

*/
void NRF24L01_Set_ShockBurst(char Mode) {
	NRF24L01_WriteReg(W_REGISTER | SETUP_RETR, Mode);
	NRF24L01_WriteReg(W_REGISTER | EN_AA, Mode);
}

/**
 Select the address width

 @param Width = 3..5
*/
void NRF24L01_Set_Address_Width(char Width) {
	NRF24L01_WriteReg(W_REGISTER | SETUP_AW, (Width & 3) -2);
}

/**
 Select mode receiver or transmitter

 @param Device_Mode = _TX_MODE, _RX_MODE

 @see _TX_MODE
 @see _RX_MODE
*/
void NRF24L01_Set_Device_Mode(char Device_Mode) {
	char Result;

	Result = NRF24L01_ReadReg(CONFIG) & 0b01111110; // Read Conf. Reg. AND Clear bit 0 (PRIM_RX) and 7 (Reserved)
	NRF24L01_WriteReg(W_REGISTER | CONFIG, Result | Device_Mode);
}

/**
 Enables and configures the pipe receiving the data

 @param PipeNum Number of pipe
 @param Address Address
 @param AddressSize Address size
 @param PayloadSize Buffer size, data receiver

*/
void NRF24L01_Set_RX_Pipe(char PipeNum, char *Address, int AddressSize, char PayloadSize) {
	char Result;

	Result = NRF24L01_ReadReg(EN_RXADDR);
	NRF24L01_WriteReg(W_REGISTER | EN_RXADDR, Result | (1 << PipeNum));

	NRF24L01_WriteReg(W_REGISTER | (RX_PW_P0 + PipeNum), PayloadSize);
	NRF24L01_WriteRegBuf(W_REGISTER | (RX_ADDR_P0 + PipeNum), Address, AddressSize);
}

/**
 Disable all pipes
*/
void NRF24L01_Disable_All_Pipes(void) {
	NRF24L01_WriteReg(W_REGISTER | EN_RXADDR, 0);
}


/** Returns the STATUS register and then clear all interrupts
 *
 * @return STATUS Register
 */
char NRF24L01_Clear_Interrupts(void) {
	return NRF24L01_WriteReg(W_REGISTER | STATUS, _RX_DR | _TX_DS | _MAX_RT);
}

/**
 Sets the direction of transmission

 @param Address Address
 @param Size Address size 3..5

*/
void NRF24L01_Set_TX_Address(char *Address, int Size) {
	NRF24L01_WriteRegBuf(W_REGISTER | TX_ADDR, Address, Size);
}

/**
 Empty the transmit buffer

*/
void NRF24L01_Flush_TX(void) {
	CSN_ON;
	SPI_RW(FLUSH_TX);
	CSN_OFF;
}

/**
 Empty the receive buffer
*/
void NRF24L01_Flush_RX(void) {
	CSN_ON;
	SPI_RW(FLUSH_RX);
	CSN_OFF;
}

/**
 Initializes the device
 @param Device_Mode = _TX_MODE, _RX_MODE
 @param CH = 0..125
 @param DataRate = _1Mbps, _2Mbps
 @param Address Address
 @param Address_Width Width direction: 3..5
 @param Size_Payload Data buffer size

 @see _TX_MODE
 @see _RX_MODE
 @see _1Mbps
 @see _2Mbps
*/
void NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload) {

	//nrf24_begin();
	// Disable Enhanced ShockBurst
	NRF24L01_Set_ShockBurst(_ShockBurst_OFF);

	// RF output power in TX mode = 0dBm (Max.)
	// Set LNA gain
	NRF24L01_WriteReg(W_REGISTER | RF_SETUP, 0b00000111 | DataRate);

	NRF24L01_Set_Address_Width(Address_Width);

	NRF24L01_Set_RX_Pipe(0, Address, Address_Width, Size_Payload);

	NRF24L01_Set_CH(CH);

	NRF24L01_Set_TX_Address(Address, Address_Width); // Set Transmit address

	// Bits 4..6: Reflect interrupts as active low on the IRQ pin
	// Bit 3: Enable CRC
	// Bit 2: CRC 1 Byte
	// Bit 1: Power Up
	NRF24L01_WriteReg(W_REGISTER | CONFIG, 0b00001010 | Device_Mode);

	_delay_uS(1500);
}

/**
 Turn on transmitter, and transmits the data loaded into the buffer
*/
void NRF24L01_RF_TX(void) {
	RADIO_OFF;
	RADIO_ON;
	_delay_uS(10);
	RADIO_OFF;
}

/**
 Writes the buffer of data transmission

 @param Buf Buffer with data to send
 @param Size Buffer size

*/
void NRF24L01_Write_TX_Buf(char *Buf, int Size) {
	NRF24L01_WriteRegBuf( W_TX_PAYLOAD, Buf, Size);
}

/**
 Read the data reception buffer

 @param Buf Buffer with data received
 @param Size Buffer size

*/
void NRF24L01_Read_RX_Buf(char *Buf, int Size) {
	NRF24L01_ReadRegBuf(R_RX_PAYLOAD, Buf, Size);
}


/*
 * Set as a Receiver and get data from RX FIFO
 */
void NRF24L01_Receive(char* Buf, int len) {
	RADIO_ON;
	_delay_uS(130);

	while ((NRF24L01_Get_Status() & _RX_DR) != _RX_DR);

	RADIO_OFF;

	NRF24L01_Read_RX_Buf(Buf, len);
	NRF24L01_Clear_Interrupts();
	NRF24L01_Flush_RX();
}

/*
 * Setup as a transmitter a put data payload
 * in TX FIFO.
 */
void NRF24L01_Send(char* Buf, int len) {
	NRF24L01_Write_TX_Buf(Buf, len);

	NRF24L01_RF_TX();

	while ((NRF24L01_Get_Status() & _TX_DS) != _TX_DS);

	NRF24L01_Clear_Interrupts();
	NRF24L01_Flush_TX();
}

//unsigned char * data must be at least 35 or 38 bytes long
void NRF24L01_get_all_registers(unsigned char * data)
{
	unsigned int outer;
	unsigned int inner;
	unsigned int dataloc = 0;
	char buffer[5];

	for(outer = 0; outer <= 0x1E; outer++)
	{
		if (outer >= 0x18 && outer <= 0x1B) continue;
		NRF24L01_ReadRegBuf(outer, buffer, 5);

		for(inner = 0; inner < 5; inner++)
		{
			if(inner >= 1 && (outer != 0x0A && outer != 0x0B && outer != 0x10))
				break;

			data[dataloc] = buffer[inner];
			dataloc++;
		}
	}
}
