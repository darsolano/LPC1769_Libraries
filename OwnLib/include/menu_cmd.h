/*
 * menu_cmd.h
 *
 *  Created on: 19/12/2014
 *      Author: dsolano
 */

#ifndef MENU_CMD_H_
#define MENU_CMD_H_

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>


/******************************************************************
 * Command Format and delimeters
 */
#define CMD_DELIMETER		' ' 	// space
#define CMD_ENDS			'\n' 	// Enter finish a command line to be processed
#define CMD_ARG_DELIMETER	','		// comma is the argument separator
#define PROMPT				(const char*)'>> '	// Command Line dat entry indicator
#define BKSPC				'\b'	// Backspace, will delete entries and buffer data, pointer need to be decremented
#define SPC					0x20
#define ENTER				0x0d
#define MAX_ARGS			4		// Max number of arguments
/*******************************************************************/



/******************************************************************
 *  Command will have the following structure
 *  1 = a string recognized to be the command listed in the array
 *  2 = length of the command string
 *  3 = # of paramaters or arguments
 *  4 = parameter 1 type
 *
 */
typedef struct cmd_struct
{
	char *cmd;					// Command
	uint8_t cmdID;				// Command ID
	void(*cmdFunction)(uint8_t*);	// Command function handler
	uint8_t cmd_lenght;			// Lenght of the command string
	uint8_t cmd_args_number;	// Number of arguments that the command takes
	char *args[4];				// Array of arguments that the command get from input data entry
} COMMAND_sType;

void ClientMonitor_Init(void);
void ShowMenu(void);
void Process_Command(void);
Bool IsUARTDataReady(void);
uint8_t* GetUARTData(void);

#endif /* MENU_CMD_H_ */
