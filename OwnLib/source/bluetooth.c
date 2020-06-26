/*
 * bluetooth.c
 *
 *  Created on: 5/1/2015
 *      Author: dsolano
 */

#include <bluetooth.h>
#include <hardware_delay.h>
#include <string.h>
#include <xprintf.h>


uint8_t bt_response[12];	// Ring buffer should be copied to this buffer in order to process the current entered command
RINGBUFF_T bt_uart2_rb;		// Ring Buffer definition for UART2
volatile char* bt_Buffer[BT_RINGBUFFER_SIZE];
volatile Bool response_State;		// indicate that a key character has arrived thru the UART and this flag is raised
volatile uint8_t rcvdata;			// used for the interrupt
volatile Bool BT_Ready;				// indicate that the BT device has being properly initialize by the Init routine
volatile Bool BT_Connected;			//indicate that a sequence of **** has arrived thru the uart requesting for service in the terminal
volatile Bool BT_AlreadyConn;		// Indicate that the system has already checked the flag  for BT_Connected so no repeat any actions

void UART2_IRQHandler(void);
static void init_uart2(void);

void UART2_IRQHandler(void)
{
	__disable_irq();
	rcvdata = Chip_UART_ReadByte(BTHC06_UART);
	if (rcvdata == 'K') response_State = TRUE;
	if (rcvdata == '*') BT_Connected = TRUE;	// Mark TTY for connection and data reply
	if (rcvdata == '#') BT_Connected = FALSE;	// Mark for disconnection all
	if (BT_Ready && BT_Connected)
		Chip_UART_SendByte(BTHC06_UART , rcvdata);
	RingBuffer_Insert(&bt_uart2_rb,(const void*) &rcvdata);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	__enable_irq();
}
static void init_uart2(void) // UART 2
{
	// PinMUX for UART2
	Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC1);	//RX
	Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC1);	//TX

	// Init UART2 for every parameter
	Chip_UART_Init(BTHC06_UART);
	Chip_UART_TXEnable(BTHC06_UART);
	Chip_UART_SetBaud(BTHC06_UART , 57600);
	Chip_UART_IntEnable(BTHC06_UART , UART_IER_RBRINT );

	NVIC_SetPriority(UART2_IRQn , 2);
	NVIC_EnableIRQ(UART2_IRQn);

	// Init ring buffer
	RingBuffer_Init( &bt_uart2_rb , bt_Buffer, sizeof(char) , BT_RINGBUFFER_SIZE );
}

static uint8_t* bt_SendCmd ( const uint8_t* cmd, int len, const uint8_t* response)
{
	Chip_UART_Send(BTHC06_UART , cmd , len);
	while (response_State == FALSE);
	while (RingBuffer_GetCount(&bt_uart2_rb) != len);
	RingBuffer_PopMult(&bt_uart2_rb , bt_response, len);
	if ((memcmp(bt_response , (const char*)"OK" , 2)) == 0)
	{
		RingBuffer_PopMult(&bt_uart2_rb , bt_response, len-2);
		response_State = FALSE;
		return bt_response+2;
	}
	else return 0;

}

//************************************************************
//			PUBLIC FUNCTION
//************************************************************
/*
 *BT need to be disconnected from host or data will pass thru
 */
Bool bt_init(void)
{
	init_uart2();
	if (bt_IsAlive()) BT_Ready = TRUE;
	else BT_Ready = FALSE;
	bt_SetDataBaudRate(B57600);// CHANGE MODULE BAUD FIRST
	return BT_Ready;
}



/*
 *
 */
uint8_t*  bt_IsAlive(void)
{
	return bt_SendCmd((uint8_t*) CMD_AT , (int)RESP_AT_LEN , (uint8_t*)RESP_AT);
}


/*
 *
 */
uint8_t*  bt_GetVersion(void)
{
	return bt_SendCmd((uint8_t*)CMD_GET_VERSION , (int)RESP_GET_VERSION_LEN , (uint8_t*)RESP_GET_VERSION);

}

/*
 *
 */
uint8_t* bt_ChangeDisplayName()
{
	return bt_SendCmd((uint8_t*)CMD_NAME , (int)RESP_NAME_LEN , (uint8_t*)RESP_NAME);
}

/*
 *
 */
uint8_t* bt_ChangePIN()
{
	return bt_SendCmd((uint8_t*)CMD_PIN , (int)RESP_PIN_LEN , (uint8_t*)RESP_PIN);
}

/*
 * Baud accept value between 1 and 12
 * be careful you can be with no communication
 * after applying this command. Remember that if you are using
 * UART you need to change Baud Rate too.
 */
uint8_t* bt_SetDataBaudRate(uint8_t baud)
{
	switch (baud)
	{
		case 0:
			return 0;
			break;
		case B1200:
			return bt_SendCmd((const uint8_t*)CMD_B1200_1 , (int)RESP_B1200_1_LEN ,
					(const uint8_t*)RESP_B1200_1);
			break;
		case B2400:
			return bt_SendCmd((const uint8_t*)CMD_B2400_2 , (int)RESP_B2400_2_LEN ,
					(const uint8_t*)RESP_B2400_2);
			break;
		case B4800:
			return bt_SendCmd((const uint8_t*)CMD_B4800_3 , (int)RESP_B4800_3_LEN ,
					(const uint8_t*)RESP_B4800_3);
			break;
		case B9600:
			return bt_SendCmd((const uint8_t*)CMD_B9600_4 , (int)RESP_B9600_4_LEN ,
					(const uint8_t*)RESP_B9600_4);
			break;
		case B19200:
			return bt_SendCmd((const uint8_t*)CMD_B19200_5 , (int)RESP_B19200_5_LEN ,
					(const uint8_t*)RESP_B19200_5);
			break;
		case B38400:
			return bt_SendCmd((const uint8_t*)CMD_B38400_6 , (int)RESP_B38400_6_LEN ,
					(const uint8_t*)RESP_B38400_6);
			break;
		case B57600:
			return bt_SendCmd((const uint8_t*)CMD_B57600_7 , (int)RESP_B57600_7_LEN ,
					(const uint8_t*)RESP_B57600_7);
			break;
		case B115200:
			return bt_SendCmd((const uint8_t*)CMD_B115200_8 , (int)RESP_B115200_8_LEN ,
					(const uint8_t*)RESP_B115200_8);
			break;
		case B230400:
			return bt_SendCmd((const uint8_t*)CMD_B230400_9 , (int)RESP_B230400_9_LEN ,
					(const uint8_t*)RESP_B230400_9);
			break;
		case B460800:
			return bt_SendCmd((const uint8_t*)CMD_B460800_A , (int)RESP_B460800_A_LEN ,
					(const uint8_t*)RESP_B460800_A);
			break;
		case B921600:
			return bt_SendCmd((const uint8_t*)CMD_B921600_B , (int)RESP_B921600_B_LEN ,
					(const uint8_t*)RESP_B921600_B);
			break;
		case B1382400:
			return bt_SendCmd((const uint8_t*)CMD_B1382400_C , (int)RESP_B1382400_B_LEN ,
					(const uint8_t*)RESP_B1382400_C);
			break;
		default:
			return 0;
			break;
	}
	return 0;
}

void bt_ModuleMonitor(void)
{
	if (BT_Connected && !BT_AlreadyConn)	//Check weather the BT has being connected to a host
	{
		xputs( "\nWelcome to BT slave TTY for LPCXpresso LPC1769\n");
		xputs( "Press * several to connect to the TTY\n");
		xputs( "Press # serveral time to disconnect from TTY or simply disconnect the BT connection\n");
		BT_AlreadyConn = TRUE;
	}
	if (!BT_Connected && BT_AlreadyConn)
	{
		xputs( "\nYou may proceed to disconnet the Bluetooth from the PC, MAC or Linux machine\n");
		xputs( "Goodbye, thanks for using the BT UART TTY..\n");
		BT_AlreadyConn = FALSE;
		BT_Connected = FALSE;
	}

}

