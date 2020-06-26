/*
 * cli_cmd.c
 *
 *  Created on: Sep 19, 2015
 *      Author: dsolano
 */

#include <string.h>
#include <Cli/cli.h>
#include <Cli/cli_cmd.h>
#include <datetime.h>
#include <xprintf.h>
#include <EA_BaseBoard/inc/pca9532.h>
#include <EA_BaseBoard/inc/rgb.h>
#include <Cli/vt100.h>
#include <eep_24lc64.h>
#include <EA_BaseBoard/inc/eabb_sound.h>
#include <data/at45d.h>
#include <EA_BaseBoard/inc/led7seg.h>
#include <hardware_delay.h>
#include <board.h>
#include <FatFs/diskio.h>
#include <FatFs/ff.h>
#include <rtc.h>

Bool new_char_arrived;	//  variable for Cli ISR
char new_cli_data;		// Char received thru UART3
char local_buf[256];
extern void ee_util_page_offset_to_addr(EEPROM_t* eep);

extern const char* songs[];

const char msg[] =
"********************************************************************************\n\r"
" Hello NXP Semiconductors \n\r"
" CLIENT COMMAND LINE PROCESSOR: \n\r"
"\t - MCU: LPC1769 LPCXPRESSO BASEBOARD \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - UART Communication: 115200 bps \n\r"
" EXAMPLE TO USE ALL PERIPHERAL AND MAKE THEM WORK THRU CLI \n\r"
"********************************************************************************\n\r";

static void put_rc (FRESULT rc)
{
	const char *str =
		"OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
		"INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
		"INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
		"LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0" "INVALID_PARAMETER\0";
	FRESULT i;

	for (i = 0; i != rc && *str; i++) {
		while (*str++) ;
	}
	xprintf("rc=%u FR_%s\n", (UINT)rc, str);
}




/* Sets up board specific I2C interface */
static const char* cli_cmd_I2C_Init_fn(u8_t argc, char* argv[]) {
	uint8_t i2cdev = 0;
	uint32_t speed;

	if (argc != 0) {
		if (!cli_util_argv_to_u8(0, 1, 2))
			return "Invalid argument value...";
		i2cdev = cli_argv_val.u8;

		if (argc > 1) {
			switch (cli_util_argv_to_option(1, "100khz\0400khz\0"))	// verify second parameter for speed
			{
			case 0:
				speed = 100000;
				break;
			case 1:
				speed = 400000;
				break;
			default:
				return "Bad I2C Bitrate argument";
				break;
			}
		} else
			speed = 400000;

		switch (i2cdev) {
		case I2C0:
			Chip_IOCON_PinMux(LPC_IOCON, 0, 27, IOCON_MODE_INACT, IOCON_FUNC1);
			Chip_IOCON_PinMux(LPC_IOCON, 0, 28, IOCON_MODE_INACT, IOCON_FUNC1);
			Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);
			break;

		case I2C1:
			Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_INACT, IOCON_FUNC3);	// Corrected it was func2
			Chip_IOCON_PinMux(LPC_IOCON, 0, 20, IOCON_MODE_INACT, IOCON_FUNC3);	// Corrected it was func2
			Chip_IOCON_EnableOD(LPC_IOCON, 0, 19);
			Chip_IOCON_EnableOD(LPC_IOCON, 0, 20);
			break;

		case I2C2:
			Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC2);
			Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC2);
			Chip_IOCON_EnableOD(LPC_IOCON, 0, 10);
			Chip_IOCON_EnableOD(LPC_IOCON, 0, 11);
			break;
		}
		Chip_I2C_Init(i2cdev);
		Chip_I2C_SetClockRate(i2cdev, speed);
		Chip_I2C_SetMasterEventHandler(i2cdev, Chip_I2C_EventHandlerPolling);
		return "I2C Bus initialized I2C bus";
	}
	return "I2C Bus initialized, no arguments";
}

static const char* cli_cmd_I2C_Disable_fn(u8_t argc, char* argv[]) {
	uint8_t i2cdev;
	if (argc > 0) {
		Chip_I2C_DeInit(cli_util_argv_to_u8(0, 1, 2));
		return "I2C Bus disable...";
	} else {
		for (i2cdev = 1; i2cdev > 2; i2cdev++)
			Chip_I2C_DeInit(i2cdev);
		return "ALL I2C Bus have being disabled";
	}
}

static const char* cli_cmd_I2C_verify_fn(u8_t argc, char* argv[]) {
	uint8_t rate;
	uint32_t pClk;

	/* For I2C0*/
	pClk = SystemCoreClock / Chip_Clock_GetPCLKDiv(SYSCTL_CLOCK_I2C0);
	rate = pClk / (LPC_I2C0->SCLH+LPC_I2C0->SCLL);
	xputs(  "\n====================================\n");
	xputs("** Configuration I2C0 ** \n");
	xprintf("I2C0 Clock Source -> %d Mhz\n",pClk/1000000);
	xprintf("I2C0 Clock Rate   -> %d\n",rate);
	xprintf("I2C0 is -> %s\n",(LPC_I2C0->CONSET & I2C_I2CONSET_I2EN)?"ENABLE":"DISABLE");

	/* For I2C1*/
	pClk = SystemCoreClock/Chip_Clock_GetPCLKDiv(SYSCTL_CLOCK_I2C1);
	rate = pClk / (LPC_I2C1->SCLH+LPC_I2C1->SCLL);
	xputs(  "====================================\n");
	xputs(  "** Configuration I2C1 ** \n");
	xprintf("I2C1 Clock Source -> %d Mhz\n",pClk/1000000);
	xprintf("I2C1 Clock Rate   -> %d\n",rate);
	xprintf("I2C1 is -> %s\n",(LPC_I2C1->CONSET & I2C_I2CONSET_I2EN)?"ENABLE":"DISABLE");

	/* For I2C2*/
	pClk = SystemCoreClock/Chip_Clock_GetPCLKDiv(SYSCTL_CLOCK_I2C2);
	rate = pClk / (LPC_I2C2->SCLH+LPC_I2C2->SCLL);
	xputs(  "====================================\n");
	xputs(  "** Configuration I2C2 ** \n");
	xprintf("I2C2 Clock Source -> %d Mhz\n",pClk/1000000);
	xprintf("I2C2 Clock Rate   -> %d\n",rate);
	xprintf("I2C2 is -> %s\n",(LPC_I2C2->CONSET & I2C_I2CONSET_I2EN)?"ENABLE":"DISABLE");

	return NULL;
}


/*
 * Turn on board led on lpcXpresso
 */
static const char* cli_cmd_Xpresso_led_on_fn(u8_t argc, char* argv[]) {
	Board_LED_Set(0, SET);
	return "Turned ON";
}

/*
 * Turn on board led off lpcXpresso
 */
static const char* cli_cmd_Xpresso_led_off_fn(u8_t argc, char* argv[]) {
	Board_LED_Set(0, RESET);
	return "Turned OFF";
}

/*
 * Toggle on board led lpcXpresso
 */
static const char* cli_cmd_Xpresso_led_toggle_fn(u8_t argc, char* argv[]) {
	u8_t iter = 0;
	cli_util_argv_to_u8(0, 0, 99);
	iter = cli_argv_val.u8;

	while (iter--) {
		Board_LED_Toggle(0);
		_delay_ms(100);
	}
	return "Toggle DONE";
}

static const char* cli_cmd_rtc_set_fn(u8_t argc, char* argv[]) {
	// rtc set dd/mm/yyyy 13:12:00
	RTC rtc;
	char tmp[2];
	char* ptr1,*ptr2;
	long pData;
	int i=0;
	if (argc != 0) {
		// Get arguments from command line
//		xprintf("Argument 1 %s\n", argv[1]);
		ptr2 = argv[0];
		memcpy(tmp,ptr2,2);
		ptr1 = tmp;
		xatoi(&ptr1, &pData);
		rtc.mday = (BYTE)pData;
		while (*ptr2++ != '/');
		memcpy(tmp,ptr2+i,2);
		ptr1 = tmp;
		xatoi(&ptr1, &pData);
		rtc.month = (BYTE)pData;



		xprintf("Argument 0 %s and Variable 1-> %u/%u\n", argv[0], rtc.mday,rtc.month);

//		rtc.mday = (BYTE)tmpdata;
//		while(*ptr++ != '/');
//		++argv;
//		xatoi(&ptr,&tmpdata);
//		rtc.month = (BYTE)tmpdata;
//		while(*ptr++ != '/');
//		++argv;
//		xatoi(&ptr, &tmpdata);
//		rtc.year = (BYTE)tmpdata;
//
//
//		// set the time on RTC on chip
//
//		//Get time and print it back
//		xprintf("Date set@ %02d:%02d:%02d\n", rtc.mday, rtc.month, rtc.year);
//		return "Time Set DONE...";
//	} else {
//		//Get time and print it back
//		return "NO arguments received...";
	}
	return NULL;
}


static const char* cli_cmd_rtc_set_time_fn(u8_t argc, char* argv[]) {
	RTCDateTime time;
	if (argc != 0) {
		// Get arguments from command line
		lpcRTC_getDateTime(&time);
		cli_util_argv_to_u8(0, 0, 23);
		time.hour = cli_argv_val.u8;
		cli_util_argv_to_u8(1, 0, 59);
		time.minute = cli_argv_val.u8;
		cli_util_argv_to_u8(2, 0, 59);
		time.second = cli_argv_val.u8;

		// set the time on RTC on chip
		lpcRTC_setDateTime(time.year, time.month, time.day, time.hour, time.minute, time.second);
		lpcRTC_getDateTime(&time);

		//Get time and print it back
		xprintf("Time set@ %02d:%02d:%02d\n", time.hour, time.minute, time.second);
		return "Time Set DONE...";
	} else {
		lpcRTC_getDateTime(&time);
		//Get time and print it back
		xprintf("Time set@ %02d:%02d:%02d\n", time.hour, time.minute, time.second);
		return "NO arguments received...";
	}
}

static const char* cli_cmd_rtc_set_date_fn(u8_t argc, char* argv[]) {
	RTCDateTime date;

	if (argc != 0) {
		lpcRTC_getDateTime(&date);

		// Get arguments from command line
		cli_util_argv_to_u8(0, 0, 31);
		date.day = cli_argv_val.u8;
		cli_util_argv_to_u8(1, 0, 12);
		date.month = cli_argv_val.u8;
		cli_util_argv_to_u16(2, 0, 2099);
		date.year = cli_argv_val.u16;
		cli_util_argv_to_u8(3, 1, 7);
		date.dayOfWeek = cli_argv_val.u8;

		lpcRTC_setDateTime(date.year, date.month, date.day, date.hour, date.minute, date.second);
		lpcRTC_getDateTime(&date);
		xputs(lpcRTC_dateFormat("d/m/Y D M\n", &date));
		return "Date Set DONE...";
	} else {
		lpcRTC_getDateTime(&date);
		xputs(lpcRTC_dateFormat("d/m/Y D M\n", &date));
		return "NO args received...";
	}
}

static const char* cli_cmd_rtc_show_datetime_fn(u8_t argc, char* argv[]) {
	RTCDateTime dt;

	lpcRTC_getDateTime(&dt);

	xputs(lpcRTC_dateFormat("M j Y H:i:s", &dt));
	xputs(" GMT\n");
	return NULL;
}

static const char* cli_cmd_io_start_fn(u8_t argc, char* argv[]) {
	// No arguments needed
	pca9532_init();
	return "PCA9532 Port IO Expander Initialized...";
}

static const char* cli_cmd_io_led_on_fn(u8_t argc, char* argv[]) {
	uint8_t led = 0;
	if (argc > 0) {
		if (cli_util_argv_to_u8(0, 1, 16) == 0)
			return "ERROR:Wrong argument...";
		led = cli_argv_val.u8 - 1;
		pca9532_setLeds(1 << led, 0xffff);
		return "Led command accepted...";
	} else {
		return "ERROR: Bad arguments or NO arguments...";
	}
}

/* Function that probes all available slaves connected to an I2C bus */
static const char* cli_cmd_i2c_probe_slaves_fn(u8_t argc, char* argv[]) {
	int i;
	uint8_t ch[2];
	uint8_t i2cdev = 0;
	LPC_I2C_T i2c[3];

	if (argc != 0) {
		if (!cli_util_argv_to_u8(0, 1, 2))
			return "Invalid argument value...I2C device Bus not used or Not implemented in EA BaseBoard";
		i2cdev = cli_argv_val.u8;
		xprintf(
				"Probing available I2C devices...I2C Bus %01d I2C state= %02X\r\n",
				i2cdev, (uint8_t) i2c[i2cdev].CONSET);
		xputs("\r\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
		xputs("\r\n====================================================");
		for (i = 0; i <= 0x7F; i++) {
			if (!(i & 0x0F))
				xprintf("\r\n%02X  ", i >> 4);
			if (i <= 7 || i > 0x78) {
				xputs("   ");
				continue;
			}
			/* Address 0x48 points to LM75AIM device which needs 2 bytes be read */
			if (Chip_I2C_MasterRead(i2cdev, i, ch, 1 + (i == 0x48)) > 0)
				xprintf(" %02X", i);
			else
				xputs(" --");
		}
		xputs("\r\n");
		return "I2C Scan DONE...";
	} else
		return "No argument in command...";
}


/*
 * RGB LED INTERFACE INITIALIZATION FOR EA BASEBOARD
 */
static const char* cli_cmd_rgb_start_fn(u8_t argc, char* argv[]) {
	rgb_init();
	return "RGB Led initialized, ready...";
}

/*
 * RGB LED INTERFACE ON FUNCTION, TURNS ON DESIRED LED COLOR
 */
static const char* cli_cmd_rgb_on_off_fn(u8_t argc, char* argv[]) {
	u8_t option;
	u8_t color;
	if (argc > 0){
		option = cli_util_argv_to_option(0, "on\0off\0");	// SELECT THE OPTION FOR ON OR OFF
		color  = cli_util_argv_to_option(1, "n\0r\0b\0m\0g\0");	// SELECT THE OPTION FOR r, g or b
		if (option == 0){ // ON CONDITION
			rgb_setLeds(color);
		}
		else if (option == 1){	//OFF CONDITION
			rgb_setLeds(0);
		}
	}
	return "RGB Led executed...";
}

/*
 * CLEAR SCREEN
 */
static const char* cli_cmd_clear_screen_fn(u8_t argc, char* argv[]) {
	vt100_clr_screen();
	return NULL;
}

/*
 * READ LPCXPRESSO ONBOARD EEPROM BYTE
 */
static const char* cli_cmd_eeprom_read_byte_fn(u8_t argc, char* argv[]) {
	uint16_t address;
	uint8_t byte;
	if (argc > 0){
		if (cli_util_argv_to_s16(0, 0, 0x1fff) == FALSE) return "Invalid address scope";
		address = cli_argv_val.u16;
		byte = ee_read_byte(address);
		xprintf("\nByte Read @ 0x%04X is 0x%02X\n" , address, byte);
		return NULL;
	}
	return NULL;
}

/*
 * WRITE LPCXPRESSO ONBOARD EEPROM BYTE
 */
static const char* cli_cmd_eeprom_write_byte_fn(u8_t argc, char* argv[]) {
	uint16_t address;
	uint8_t byte;
	u8_t byte_read;
	if (argc > 0){
		if (cli_util_argv_to_u16(0, 0, 0x1fff) == FALSE) return "Invalid address scope";
		address = cli_argv_val.u16;
		if (cli_util_argv_to_u8(1, 0, 255) == 0) return "Byte value out of range";
		byte = cli_argv_val.u8;
		if (ee_write_byte(address, byte) == ERROR) return "Write EEPROM operation fail!!!";
		byte_read = ee_read_byte(address);
		if (byte != byte_read) return "Write operation mismatch...";
		xprintf("Byte Written @ 0x%04X is 0x%02X, Byte Read is 0x%02X\n" , address, byte, byte_read);
		return NULL;
	}
	return NULL;
}

/*
 * READ DATA STREAM FROM ONBOARD EEPROM ON LPCXPRESSO
 */
static const char* cli_cmd_eeprom_read_stream_fn(u8_t argc, char* argv[]) {
	uint16_t address;
	int len;

	if (argc > 0) {
		if (cli_util_argv_to_u16(0, 0, 0x1fff) == FALSE) return "Invalid address scope";
		address = cli_argv_val.u16;
		if (cli_util_argv_to_u16(1, 1, 256) == 0) return "Length out of range";
		len = cli_argv_val.u16;
		if (ee_read_stream(address, (uint8_t*)local_buf, len) == ERROR) return "Error reading eeprom data stream";
		xprintf("EEPROM STREAM @ 0x%X Length of %d\n", address, len );
		cli_util_disp_buf((const u8_t*)local_buf, len);
		return "Done EEPROM stream read...";
	}else return "Error!!!";
}

/*
 * FILL PAGE WITH DESIRE VALUE
 */
static const char* cli_cmd_eeprom_fill_fn(u8_t argc, char* argv[]) {
	uint16_t address;
	int len;
	uint8_t byte_value;

	if (argc > 0){
		if (cli_util_argv_to_u16(0, 0, 8191) == FALSE) return "Invalid address scope";
		address = cli_argv_val.u16;
		if (cli_util_argv_to_u16(1, 1, 32) == 0) return "Length out of range";
		len = cli_argv_val.u16;
		if (cli_util_argv_to_u8(2, 0, 255) == 0) return "Byte greater than expected!!";
		byte_value = cli_argv_val.u8;
		memset(local_buf,byte_value,len);
		if (ee_write_stream(address, (uint8_t*)local_buf, len) == ERROR) return "Error writing to eeprom...!!!";
		xprintf("EEPROM WRITTEN STREAM @ 0x%X Length of %d\n", address, len );
		cli_util_disp_buf((const u8_t*)local_buf, len);
		memset(local_buf,0,len);
		xprintf("EEPROM READ STREAM @ 0x%X Length of %d\n", address, len );
		ee_read_stream(address, (uint8_t*)local_buf, len);
		cli_util_disp_buf((const u8_t*)local_buf, len);
		return "Done EEPROM stream write...";
	}
	else return "Error!!!";
}


static const char* cli_cmd_eeprom_verify_fn(u8_t argc, char* argv[]) {
	EEPROM_t eep = {0};
	int bytes_unused = 0;
	int bytes_used = 0;
	int total_bytes_unused = 0;
	int total_bytes_used = 0;
	int page;
	int offset = 0;;

	for (page=0;page<EEP24LC64_TOTAL_PAGES;page++){
		eep.page = page;
		eep.offset = 0;
		ee_util_page_offset_to_addr(&eep);
		if (ee_read_stream(eep.addr, (uint8_t*)local_buf, 32) == ERROR) return "Error reading current page";
		offset = 0;
		bytes_unused = 0;
		bytes_used = 0;
		while (offset<32){
			if (local_buf[offset] == 0xff) bytes_unused++;
			else bytes_used++;
			offset++;
		}
		total_bytes_unused += bytes_unused;
		total_bytes_used += bytes_used;
		if (bytes_used)
			xprintf("PAGE:%d @ ADDRESS:0x%X EMPTY BYTES:%d  USED BYTES:%d\n",page,eep.addr,bytes_unused,bytes_used);
	}
	xputs("*******************************************************************\n");
	xprintf("PAGES:%d EMPTY BYTES:%d  USED BYTES:%d\n",page,total_bytes_unused,total_bytes_used);
	xputs("*******************************************************************\n");
	return NULL;
}

static const char* cli_cmd_eeprom_page_erase_fn(u8_t argc, char* argv[]) {
	EEPROM_t eep = {0};
	uint8_t page_init = 0;
	uint8_t page_end = 0;
	int page = 0;
	int offset = 0;
	int byte_count = 0;
	int byte_count_current = 0;

	if (argc > 0){
		if (cli_util_argv_to_u8(0, 0, 255) == FALSE) return "Page Init Number out of range";
		page_init = cli_argv_val.u8;		// Page value
		if (cli_util_argv_to_u8(1, 0, 255) == FALSE) return "Page End Number out of range";
		page_end = cli_argv_val.u8;
		if ((page_end - page_init) > 255) return "Scope out of range...!!!";

		/* RUN ALL THE PAGES TO FIND NOT EMPTY BYTE TO ERASE*/
		for (page=page_init;page<=page_end;page++){
			eep.page = page;
			eep.offset = 0;
			ee_util_page_offset_to_addr(&eep);
			xprintf("Verify to Erase Page:%d from address:0x%X\n",page,eep.addr);
			xputs("Not empty\n");
			offset = 0;

			/*READ FULL PAGE INTO LOCAL BUFFER*/
			if (ee_read_stream(eep.addr,(uint8_t*) local_buf, 32) == ERROR) return "Error reading current page!!!";

			/*ERASE BYTE NOT EMPTY MARKED BY DIFFERENT FROM OXFF*/
			while (offset < 32){
				if (local_buf[offset] != 0xff){
					if (ee_write_byte(eep.addr+offset, 0xff) == ERROR) return "Error writing erased values to eeprom!!!";
					xprintf("Address:0x%X Content:0x%02X\n",eep.addr+offset, local_buf[offset] );
					byte_count++;
					byte_count_current++;
				}
				offset++;
			}
			xprintf("Total bytes %d\n",byte_count_current);
			byte_count_current = 0;
		}
		xprintf("%d Pages verified and erased %d bytes\n", page,byte_count);
		return "Success...";
	}else return "Error...!!!";
}

/*
 * Initialize SPIFI on board BaseBoard
 */
static const char* cli_cmd_spifi_init_fn(u8_t argc, char* argv[]) {
	uint8_t flashbuf[AT45D_PAGE_SIZE] = {0};
#define PAGE_TO_READ	7
	const char* pAscii = "0123456789abcdefghijklmnopqrstuvwxyz";
	const char* flash_device[] =
	{
			"AT45DB011",
			"AT45DB021",
			"AT45DB041",
			"AT45DB081",
			"AT45DB161",
			"AT45DB321",
			"AT45DB642"
	};

	at45d_init(1);
	at45d_resume_from_power_down();
	if (!at45d_ready()) return "Initialization failed...!!!";
	xputs(  "*****************************\n");
	xprintf("Flash Device: %s\n",flash_device[AT45D_CFG_DEVICE]);
	xprintf("Total Pages : %d\n", AT45D_PAGES);
	xprintf("Page Size   : %d\n", AT45D_PAGE_SIZE);
	xprintf("Total Size  : %d\n", AT45D_FLASH_SIZE_BYTES);
	xprintf("Max Address : 0x%X\n", AT45D_ADR_MAX);
	xputs(  "*****************************\n\n");
	xputs("Start writing data string to flash...\n");
	at45d_wr_page_offset(pAscii, PAGE_TO_READ,0,strlen(pAscii));
	_delay_ms(200);
	xputs("Done writing...Will start reading and compare...\n");
	_delay_ms(200);
	at45d_rd_page_offset(flashbuf, PAGE_TO_READ,0,strlen(pAscii));
	xprintf("Data from flash memory: %s\n",flashbuf);
	if (memcmp(pAscii,flashbuf,strlen(pAscii)) != 0) return "Flash fail integrity check";
	xputs("\nCompare test pass...\n");
	return "Successful flash device init...";
}

/*
 * Initialize 7 segment on board BaseBoard
 */
static const char* cli_cmd_7lcd_init_fn(u8_t argc, char* argv[]) {
	int i=0;
	led7seg_init();
	for (i=0;i<10;i++){
		led7seg_setChar(i+'0', 0);
		_delay_ms(100);
	}
	return " initialization done...";
}

static const char* cli_cmd_7lcd_set_fn(u8_t argc, char* argv[]) {

	return " initialization done...";
}

/*
 * Initialize SD/MMC on board BaseBoard
 */
DWORD AccSize;				/* Work register for fs command */
WORD AccFiles, AccDirs;
FILINFO Finfo;

char Line[256];				/* Console input buffer */
BYTE Buff[16384] __attribute__ ((aligned (4))) ;	/* Working buffer */

FATFS FatFs;				/* File system object for each logical drive */
FIL File[2];				/* File objects */
DIR Dir;					/* Directory object */

volatile UINT Timer;
extern void disk_timerproc (void);
//******************************************************************
static const char* cli_cmd_sd_init_fn(u8_t argc, char* argv[]) {
	long p1, p2;
	BYTE res, b;
	UINT s1 ;

	if (argc > 0){
		if (!cli_util_argv_to_u8(0, 0, 0)) return "Wrong Disk Drive...";
		p1 = cli_argv_val.u8;
		res = disk_initialize((BYTE)p1);

		if (res & STA_NODISK){
			put_rc(res);
			return "Disk not found...";
		}
		else if (res & STA_NOINIT){
			put_rc(res);
			return "Disk not Initialized...";
		} else if(!res){

			put_rc(res);
			if (disk_ioctl((BYTE)p1, GET_SECTOR_COUNT, &p2) == RES_OK)
				{ xprintf("Drive size: %lu sectors\n", p2); }
			if (disk_ioctl((BYTE)p1, GET_BLOCK_SIZE, &p2) == RES_OK)
				{ xprintf("Block size: %lu sectors\n", p2); }
			if (disk_ioctl((BYTE)p1, MMC_GET_TYPE, &b) == RES_OK)
				{ xprintf("Media type: %u\n", b); }
			if (disk_ioctl((BYTE)p1, MMC_GET_CSD, Buff) == RES_OK)
				{ xputs("CSD:\n"); put_dump(Buff, 0, 16, DW_CHAR); }
			if (disk_ioctl((BYTE)p1, MMC_GET_CID, Buff) == RES_OK)
				{ xputs("CID:\n"); put_dump(Buff, 0, 16, DW_CHAR); }
			if (disk_ioctl((BYTE)p1, MMC_GET_OCR, Buff) == RES_OK)
				{ xputs("OCR:\n"); put_dump(Buff, 0, 4, DW_CHAR); }
			if (disk_ioctl((BYTE)p1, MMC_GET_SDSTAT, Buff) == RES_OK){
				xputs("SD Status:\n");
				for (s1 = 0; s1 < 64; s1 += 16) put_dump(Buff+s1, s1, 16, DW_CHAR);
			}
			return "Disk initialization done...";
		}
	} else return "Invalid argument, no disk selected...!!!";
	return "Disk error...!!!";
}

static const char* cli_cmd_sd_dump_sector_fn(u8_t argc, char* argv[]) {
    char *ptr;
	BYTE res, drv = 0;
	DWORD ofs = 0, sect = 0;

	if (argc > 0){
		if (!cli_util_argv_to_u8(0, 0, 0)) return "Wrong physical drive...";
		drv = cli_argv_val.u8;
		if (!cli_util_argv_to_u16(1, 0, 512)) return "Wrong sector/LBA number...";
		sect = cli_argv_val.u16;
		res = disk_read(drv, Buff, sect, 1);
		if (res) { xprintf("rc=%d\n", (WORD)res); return "Disk read fail...!!!"; }
		xprintf("PD#:%u LBA:%lu\n", drv, sect++);
		for (ptr=(char*)Buff, ofs = 0; ofs < 0x200; ptr += 16, ofs += 16)
			put_dump((BYTE*)ptr, ofs, 16, DW_CHAR);

	} else return "Invalid argument...!!!";
	return "Dump successful";
}

static const char* cli_cmd_sd_read_sector_fn(u8_t argc, char* argv[]) {
	long  p3;
	BYTE  res,drv = 0;
	DWORD sect = 0;

	if (argc > 0){
		if (!cli_util_argv_to_u8(0, 0, 0)) return "Wrong physical drive...";
		drv = cli_argv_val.u8;
		if (!cli_util_argv_to_u16(1, 0, 128)) return "Wrong sector/LBA number...";
		sect = cli_argv_val.u16;
		if (!cli_util_argv_to_u16(2, 0, 128)) return "Exceed bytes amount...";
		p3 = cli_argv_val.u16;
		res = (WORD)disk_read((BYTE)drv, Buff, sect, p3);
		if (res) { xprintf("rc=%u\n", res); return "Disk not initialized"; }
		xprintf("rc=%u\n", res);
		cli_util_disp_buf(Buff, p3 /*strlen((const char*)Buff)*/);
	} else return "Invalid argument...!!!";
	return "read sector successful";
}


/*
 * READ ENTIRE PAGE FOR 264 BYTES from SPIFLASH AT45DB081D
 */
//static const char* cli_cmd_spifi_page_fn(u8_t argc, char* argv[]) {
//	int page = 0;
//	if (argc > 0){
//		if (cli_util_argv_to_u8(0, 0, 255) == FALSE) return "Page Init Number out of range";
//		page_init = cli_argv_val.u8;		// Page value
//	}else return "Error...!!!";
//
//	return NULL;
//}


/*
 * SHOW DATA FROM HISTORY BUFFER
 */
static const char* cli_cmd_hist_buffer_fn(u8_t argc, char* argv[]) {
	char* pcmd = cli_hist_circ_buf;
	xprintf("Buffer @ %X ram address\n", pcmd);
	cli_util_disp_buf((const u8_t*)pcmd, CLI_CFG_HISTORY_SIZE);
	return "DONE History Command Buffer display...";
}

/*
 * SHOW DATA FROM COMMAND BUFFER
 */
static const char* cli_cmd_command_buffer_fn(u8_t argc, char* argv[]) {
	char* pcmd = cli_line_buf;
	xprintf("Buffer @ %X ram address\n", pcmd);
	cli_util_disp_buf((const u8_t*)pcmd, CLI_CFG_LINE_LENGTH_MAX);
	return "DONE Line Command Buffer display...";
}


/*
 * PLay songs
 */
static const char* cli_cmd_play_song_fn(u8_t argc, char* argv[]) {
	Chip_DAC_Init(LPC_DAC);
	if (argc > 0){
		sound_init();
		if (cli_util_argv_to_u8(0, 1, 10) == FALSE) return "Error in song number";
		playSong((const char*)songs[cli_argv_val.u8]);
	}
	return "Song played...";
}

// hard fault handler wrapper in assembly
// it extract the location of stack frame and pass it
// to handler in C as pointer.
void HardFault_Handler(void) {
	__asm__(
			"TST LR, #4\n"
			"ITE EQ\n"
			"MRSEQ R0, MSP\n"
			"MRSNE R0, PSP\n"
			"B (hard_fault_handler_c )\n"
	);

	while (1) {
	}
}

// hard fault handler in C,
// with stack frame location as input parameter
void hard_fault_handler_c(unsigned int * hardfault_args) {
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;
	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);
	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);
	xprintf("[Hard fault handler]\n");
	xprintf("R0   = %x\n", stacked_r0);
	xprintf("R1   = %x\n", stacked_r1);
	xprintf("R2   = %x\n", stacked_r2);
	xprintf("R3   = %x\n", stacked_r3);
	xprintf("R12  = %x\n", stacked_r12);
	xprintf("LR   = %x\n", stacked_lr);
	xprintf("PC   = %x\n", stacked_pc);
	xprintf("PSR  = %x\n", stacked_psr);
	xprintf("BFAR = %x\n", (*((volatile unsigned long *) (0xE000ED38))));
	xprintf("CFSR = %x\n", (*((volatile unsigned long *) (0xE000ED28))));
	xprintf("HFSR = %x\n", (*((volatile unsigned long *) (0xE000ED2C))));
	xprintf("DFSR = %x\n", (*((volatile unsigned long *) (0xE000ED30))));
	xprintf("AFSR = %x\n", (*((volatile unsigned long *) (0xE000ED3C))));
	return;
}

void UART3_IRQHandler(void) {
	__disable_irq();
	new_cli_data = Board_UARTGetChar();
	new_char_arrived = TRUE;
	__enable_irq();
}

/*
 * INITIALIZE UART3 IN ORDER TO ESTABLISH THE CONSOLE FOR THE COMMAND LINE INTERPRETER
 */
void cli_cmd_init(void) {
	Chip_UART_IntEnable(DEBUG_UART, UART_IER_RBRINT);//Enable UART3 Module interrupt when Rx register gets data
	NVIC_SetPriority(UART3_IRQn, 1);
	NVIC_EnableIRQ(UART3_IRQn); /* Enable System Interrupt for UART channel */
	xdev_out(Board_UARTPutChar);
	xdev_in(Board_UARTGetChar);
	cli_init(msg);
}

void vCLI(void) {
	if (new_char_arrived) {
		cli_on_rx_char(new_cli_data);
		new_char_arrived = FALSE;
	}
}
//=====================================================================================================
// Create CLI LED command structures
CLI_CMD_CREATE(cli_cmd_led_on, "on", 0, 0, "",
		"Turn onboard LPCXpresso board led on")
CLI_CMD_CREATE(cli_cmd_led_off, "off", 0, 0, "",
		"Turn onboard LPCXpresso board led off")
CLI_CMD_CREATE(cli_cmd_led_toggle, "toggle", 1, 1, "<iter>",
		"Toggle on board led n iter for x milliseconds")
//Led commands grouped as On Board LED
CLI_GROUP_CREATE(cli_group_led, "led") CLI_CMD_ADD(cli_cmd_led_on, cli_cmd_Xpresso_led_on_fn)
CLI_CMD_ADD(cli_cmd_led_off, cli_cmd_Xpresso_led_off_fn)
CLI_CMD_ADD(cli_cmd_led_toggle, cli_cmd_Xpresso_led_toggle_fn)
CLI_GROUP_END()
//----------------------------------
// RTC related commands
CLI_CMD_CREATE(cli_cmd_set, "set", 1, 1, "<dd/mm/yyyy> <hh:mm:ss>", "Set RTC date and time ")
CLI_CMD_CREATE(cli_cmd_time_set, "time", 0, 3, "<hh mm ss> in 24H format ", "Set RTC time ")
CLI_CMD_CREATE(cli_cmd_date_set, "date", 0, 4, "<dd mm yyyy day#> sun=1,...","Set RTC date ")
CLI_CMD_CREATE(cli_cmd_time_show, "show", 0, 0, "", "Show current time and date on system rtc")
//RTC commands grouped as RTC
CLI_GROUP_CREATE(cli_group_rtc, "rtc")
	CLI_CMD_ADD(cli_cmd_set, cli_cmd_rtc_set_fn)
	CLI_CMD_ADD(cli_cmd_time_set, cli_cmd_rtc_set_time_fn)
	CLI_CMD_ADD(cli_cmd_date_set, cli_cmd_rtc_set_date_fn)
	CLI_CMD_ADD(cli_cmd_time_show, cli_cmd_rtc_show_datetime_fn)
CLI_GROUP_END()
//----------------------------------
//PCA9532 i2c port expander IO
CLI_CMD_CREATE(cli_cmd_PCA9532_start, "init", 0, 0, "",                     "Start i2c port for PCA9532 I2C Port IO Expander")
CLI_CMD_CREATE(cli_cmd_PCA9532_on, "on", 1, 1, "<led#>",                    "Turn led # on on PCA 9532 port IO expander")
//IO commands grouped as io
CLI_GROUP_CREATE(cli_group_PCA9532, "io") CLI_CMD_ADD(cli_cmd_PCA9532_start, cli_cmd_io_start_fn)
CLI_CMD_ADD(cli_cmd_PCA9532_on, cli_cmd_io_led_on_fn)
CLI_GROUP_END()
//----------------------------------
//RGB Leds
CLI_CMD_CREATE(cli_cmd_rgb_init,"init", 0,0,  "",                  "Start IO Digital lines fro RGB leds")
CLI_CMD_CREATE(cli_cmd_rgb_set,	"set"	  , 2,2,  "<on/off> <color>",  "Turn on rgb color Led, red = r, Green = g, Blue = b or all")
//IO commands grouped as io
CLI_GROUP_CREATE(cli_group_rgb, "rgb")
   CLI_CMD_ADD(cli_cmd_rgb_init,  cli_cmd_rgb_start_fn)
   CLI_CMD_ADD(cli_cmd_rgb_set,  cli_cmd_rgb_on_off_fn)
CLI_GROUP_END()
//----------------------------------
// I2C Bus
CLI_CMD_CREATE(cli_cmd_i2c_init, "init", 1, 2,	"<I2C0=0...> [speed=100khz or 400khz]",	"Init LPC_i2c device except for i2c0, default 400000.")
CLI_CMD_CREATE(cli_cmd_i2c_disable, "dis", 0, 1,"[I2C0=0 ...] default All Buses", "Disable I2C Bus.")
CLI_CMD_CREATE(cli_cmd_i2c_scan, "scan", 1, 1, "[I2C0=0 ...] default I2C2=2","Display devices conected to i2c bus.")
CLI_CMD_CREATE(cli_cmd_i2c_verify, "veri", 0, 0, "", "Display devices conected to i2c bus.")
//IO commands grouped as i2c
CLI_GROUP_CREATE(cli_group_i2c, "i2c")
	CLI_CMD_ADD(cli_cmd_i2c_init, cli_cmd_I2C_Init_fn)
	CLI_CMD_ADD(cli_cmd_i2c_disable, cli_cmd_I2C_Disable_fn)
	CLI_CMD_ADD(cli_cmd_i2c_scan, cli_cmd_i2c_probe_slaves_fn)
	CLI_CMD_ADD(cli_cmd_i2c_verify, cli_cmd_I2C_verify_fn)
CLI_GROUP_END()
//----------------------------------
// LPCXPRESSO ONBOARD EEPROM
CLI_CMD_CREATE(cli_cmd_eeprom_read, "r", 1, 1,	"<address>",	"Read byte at specified address at onboard lpcxpresso eeprom")
CLI_CMD_CREATE(cli_cmd_eeprom_write, "wr", 2, 2,"<address> <byte>", "Write byte at specified address at onboard lpcxpresso eeprom")
CLI_CMD_CREATE(cli_cmd_eeprom_read_stream, "rs", 2, 2, "<address> <length>","Display bytes stream from onboard eeprom lpcxpresso")
CLI_CMD_CREATE(cli_cmd_eeprom_fill, "fill", 3, 3, "<address> <length> <value>","Fill eeprom cells from address to amount of bytes with the value")
CLI_CMD_CREATE(cli_cmd_eeprom_verify, "veri", 0, 0, "","Verify state of inner cells per page and bytes, how many pages, bytes used and bytes free")
CLI_CMD_CREATE(cli_cmd_eeprom_page_erase, "erase", 2, 2, "<pagestart#> <pageend#>","Verify and Erase all 32 bytes in a page")
//IO commands grouped as i2c
CLI_GROUP_CREATE(cli_group_eeprom, "eep")
	CLI_CMD_ADD(cli_cmd_eeprom_read, cli_cmd_eeprom_read_byte_fn)
	CLI_CMD_ADD(cli_cmd_eeprom_write, cli_cmd_eeprom_write_byte_fn)
	CLI_CMD_ADD(cli_cmd_eeprom_read_stream, cli_cmd_eeprom_read_stream_fn)
	CLI_CMD_ADD(cli_cmd_eeprom_fill, cli_cmd_eeprom_fill_fn)
	CLI_CMD_ADD(cli_cmd_eeprom_verify, cli_cmd_eeprom_verify_fn)
	CLI_CMD_ADD(cli_cmd_eeprom_page_erase, cli_cmd_eeprom_page_erase_fn)
CLI_GROUP_END()
//----------------------------------
// LPCXPRESSO ONBOARD SPIFLASH
CLI_CMD_CREATE(cli_cmd_spifi_init, "init", 0, 0,	"",	"Init SPIFI")
//CLI_CMD_CREATE(cli_cmd_spifi_write, "write", 2, 2,"<address> <byte>", "Write byte at specified address at onboard lpcxpresso eeprom")
//CLI_CMD_CREATE(cli_cmd_spifi_read_buffer, "rb", 2, 2, "<address> <length>","Display bytes stream from onboard eeprom lpcxpresso")
//CLI_CMD_CREATE(cli_cmd_spifi_fill, "fill", 3, 3, "<address> <length> <value>","Fill eeprom cells from address to amount of bytes with the value")
//CLI_CMD_CREATE(cli_cmd_spifi_verify, "verify", 0, 0, "","Verify state of inner cells per page and bytes, how many pages, bytes used and bytes free")
//CLI_CMD_CREATE(cli_cmd_spifi_page_erase, "erase", 2, 2, "<pagestart#> <pageend#>","Verify and Erase all 32 bytes in a page")
//IO commands grouped as SPI
CLI_GROUP_CREATE(cli_group_spifi, "spifi")
	CLI_CMD_ADD(cli_cmd_spifi_init, cli_cmd_spifi_init_fn)
//	CLI_CMD_ADD(cli_cmd_spifi_write, cli_cmd_spifi_write_byte_fn)
//	CLI_CMD_ADD(cli_cmd_spifi_read_buffer, cli_cmd_spifi_read_stream_fn)
//	CLI_CMD_ADD(cli_cmd_spifi_fill, cli_cmd_spifi_fill_fn)
//	CLI_CMD_ADD(cli_cmd_spifi_verify, cli_cmd_spifi_verify_fn)
//	CLI_CMD_ADD(cli_cmd_spifi_page_erase, cli_cmd_spifi_page_erase_fn)
CLI_GROUP_END()
//----------------------------------
// LPCXPRESSO ONBOARD 7 Segment LCD
CLI_CMD_CREATE(cli_cmd_7lcd_init, "init", 0, 0,	"",	"Init 7 segment lcd")
//IO commands grouped as ss
CLI_GROUP_CREATE(cli_group_7lcd, "ss")
	CLI_CMD_ADD(cli_cmd_7lcd_init, cli_cmd_7lcd_init_fn)
CLI_GROUP_END()
//----------------------------------
// LPCXPRESSO ONBOARD SD/MMC card reader
CLI_CMD_CREATE(cli_cmd_sd_init, "init", 1, 1, "<drive #>","Initialize SD drive")
CLI_CMD_CREATE(cli_cmd_sd_dump_sector, "ds", 2, 2, "<drive #> <sector #>","Dump a sector data")
CLI_CMD_CREATE(cli_cmd_sd_read_sector, "sr", 3, 3, "<drive #> <sector #> <bytes to read>","Read a sector data")
//IO commands grouped as sd
CLI_GROUP_CREATE(cli_group_sd, "sd")
	CLI_CMD_ADD(cli_cmd_sd_init, cli_cmd_sd_init_fn)
	CLI_CMD_ADD(cli_cmd_sd_dump_sector, cli_cmd_sd_dump_sector_fn)
	CLI_CMD_ADD(cli_cmd_sd_read_sector, cli_cmd_sd_read_sector_fn)
CLI_GROUP_END()
//----------------------------------

CLI_CMD_CREATE(cli_cmd_play_song, "play", 1, 1, "<song title to play>","Play different tones and song")
CLI_CMD_CREATE(cli_cmd_cmd_buffer, "cmd", 0, 0, "","Display command buffer content.")
CLI_CMD_CREATE(cli_cmd_history, "hist", 0, 0, "","Display history buffer content.")
CLI_CMD_CREATE(cli_cmd_help, "help", 0, 1, "<cmd(s) starts with...>","Display list of commands with help. Optionally, the list can be reduced.")
CLI_CMD_CREATE(cli_cmd_cls, "cls", 0, 0,"",	"Clear Display")
//------------------------------------------------------------------------------------------------------

// Declare CLI command list and add commands and groups
CLI_CMD_LIST_CREATE()
	CLI_GROUP_ADD(cli_group_led)
	CLI_GROUP_ADD(cli_group_rtc)
	CLI_GROUP_ADD(cli_group_PCA9532)
	CLI_GROUP_ADD(cli_group_rgb)
	CLI_GROUP_ADD(cli_group_i2c)
	CLI_GROUP_ADD(cli_group_7lcd)
	CLI_GROUP_ADD(cli_group_eeprom)
	CLI_GROUP_ADD(cli_group_spifi)
	CLI_GROUP_ADD(cli_group_sd)
	CLI_CMD_ADD (cli_cmd_play_song, cli_cmd_play_song_fn)
	CLI_CMD_ADD (cli_cmd_cmd_buffer, cli_cmd_command_buffer_fn)
	CLI_CMD_ADD (cli_cmd_history, cli_cmd_hist_buffer_fn)
	CLI_CMD_ADD (cli_cmd_cls, cli_cmd_clear_screen_fn)
	CLI_CMD_ADD (cli_cmd_help, cli_cmd_help_fn)
CLI_CMD_LIST_END()

/*---------------------------------------------*/
/* 1kHz timer process for FF and disc_io       */
/*---------------------------------------------*/
extern void disk_timerproc (void);
void SysTick_Handler (void)
{
	static int led_timer;


	Timer++;	/* Increment performance counter */

	if (++led_timer >= 500) {
		led_timer = 0;
		Board_LED_Toggle(0);
	}

	disk_timerproc();	/* Disk timer process */
}
