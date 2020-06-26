/*
 * menu_cmd.c
 *
 *  Created on: 19/12/2014
 *      Author: dsolano
 */

#include <menu_cmd.h>
#include <datetime.h>
#include <ring_buffer.h>
#include <string.h>
#include <xprintf.h>
#include <board.h>

#define RINGBUFFER_SIZE		32

static void prompt(void);


/* GLOBAL VARIABLES AND CONSTANST
 * ***************************************************************************************
 */
const char* menu[] =
{
		"\n\nCommand Line monitor for LPCXpresso 1769 v1.0\n",
		"In order to invoke this menu type help\n",
		"Copyright 2014, all rights reserved\n"
		"-----------------------------------------------------\n",
		"Menu\n"
		"-----------------------------------------------------\n",
		"set time hh:mm         -> set time in uC RTC\n",
		"time                   -> show current set time in uC RTC\n",
		"set date dd/mm/yy dow  -> Sets date in numbers, dow starts from sunday=0\n",
		"date                   -> shows current date set in uC RTC\n"
		"scani2c i2c#           -> scan I2C bus # - options are 0, 1 and 2\n",
		"reprom page            -> read lpcxpresso eeprom 24lc32 option is page, will show 32 bytes in a row\n",
		"setled	on/off          -> set led on lpcxpresso on or off, options are on / off\n",
		"showrb                 -> show the content of the ringbuffer from UART, all of it extent\n"
		"i2cstat                -> Show I2C bus status if they are enabled or not\n"
		"clock                  -> show internal clock rate currently running LPC1769\n"
		"\nhelp                   -> shows this current menu\n",
		0,
};

const COMMAND_sType CommandList[] =
	{
		{0}
	};

const char cmd_error[] = "command does not exists\n";
const char cmd_ok[] = "OK command will be processed\n";
const char cmd_notok[] = "?? command is wrong or do not exist\n";
const char ver[] = "LPCXpresso LPC1769 Command Line Monitor ver1.0 2014®\n";

uint8_t Command2Process[12];	// Ring buffer should be copied to this buffer in order to process the current entered command
RINGBUFF_T uart3_rb;		// Ring Buffer definition for UART3
volatile char* rbBuffer[RINGBUFFER_SIZE];
Bool CmdState;

/*
 * ***************************************************************************************
 */

/*********************************************************************//**
 * @brief		Interrupts every time a char get inputed from term uart3
 * @param[in]	None
 * @param[in]	None
 * @return		None
 **********************************************************************/
void UART3_IRQHandler(void)
{
	__disable_irq();
	uint8_t data;
	data = xfunc_in();

	switch (data){
		case '\b':		// Backspace, deletes a char in buffer
			xputc( data);
			uart3_rb.head--;
			break;
		case '\n':		// proceed to process the command
			data=0;	// end string in null char to prepare the string to be processed
			RingBuffer_Insert(&uart3_rb, &data);	// Place the null in the ring buffer
			RingBuffer_PopMult(&uart3_rb,Command2Process,RingBuffer_GetCount(&uart3_rb));	// Place the string entered in a variable to be processed by
																							// the Command Processor
			CmdState = TRUE;
			break;
		default:
			xputc(data);
			RingBuffer_Insert(&uart3_rb, &data);
			break;
	}
	__enable_irq();
}


void ClientMonitor_Init(void)
{
	Chip_UART_IntEnable(DEBUG_UART,UART_IER_RBRINT);	//Enable UART3 Module interrupt when Rx register gets data
	RingBuffer_Init(&uart3_rb, rbBuffer,sizeof(char), RINGBUFFER_SIZE);	// Enable Ring Buffer
	lpcRTC_initialize();
	prompt();
	NVIC_SetPriority(UART3_IRQn , 1);
	NVIC_EnableIRQ(UART3_IRQn);	/* Enable System Interrupt for UART channel */
	xdev_in(Board_UARTGetChar);
	xdev_out(Board_UARTPutChar);
}

/*********************************************************************//**
 * @brief		Show full menu defined in in menu_cmd lib
 * @param[in]	None
 * @param[in]	None
 * @return		None
 **********************************************************************/
void ShowMenu(void)
{
	int count = 0;
	for(count=0;menu[count];count++)
	{
		xputs(menu[count]);
	}
	prompt();
}



/*********************************************************************//**
 * @brief		Show prompt '01:37:34:> '
 * @param[in]	None
 * @param[in]	None
 * @return		None
 **********************************************************************/
void prompt(void)
{
	RTCDateTime ptime;
	lpcRTC_getDateTime(&ptime);
	xputc('\n');
	xputs(lpcRTC_dateFormat("H:i:s\n", &ptime));
}


void Process_Command(void)
{
	const COMMAND_sType *ptrcmd = CommandList;
	volatile int i = 0;
	if (CmdState)
	{
		for(i=0;ptrcmd[i].cmd;i++)
		{
			if (!memcmp(Command2Process, ptrcmd[i].cmd, ptrcmd[i].cmd_lenght)) // if equal
			{
				xputs( cmd_ok);
				CmdState = FALSE;
				prompt();
				memcpy(Command2Process,0,12);
				break;
			}
			else
			{
				xputs( cmd_notok);	// if not equal
				CmdState = FALSE;
				prompt();
				memcpy(Command2Process,0,12);
			}
		}
	}
}

void ExecuteCommand(COMMAND_sType *cmd){

}

Bool IsUARTDataReady(void){
	return CmdState;
}

// check if there is data ready
uint8_t* GetUARTData(void)
{
	uint8_t* ptr = Command2Process;
	CmdState = FALSE;
	prompt();
	return ptr;
}
