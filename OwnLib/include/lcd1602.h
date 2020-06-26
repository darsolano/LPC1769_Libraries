/* 
 * File:   lcd1602.h
 * Author: dsolano
 *
 * Created on August 17, 2014, 6:08 PM
 */

#ifndef LCD1602_H
#define	LCD1602_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <chip.h>
#include <lpc_types.h>


// PIN distribution for the breakout board is
//    IO7 IO6 IO5 IO4 IO3 IO2 IO1 IO0
//     BL  RS  RW  E  DB7 DB6 DB5 DB4
// Clock Constants
//#ifndef SYS_CLOCK
//	#error "Define SYS_CLOCK (ex. -DSYS_CLOCK=80000000) on compiler command line"
//#endif



/* INSTRUCTION CODE FOR LCD HD 44780
Instruction Code
                                RS R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
                                == === === === === === === === === ===
Clear Display                    0  0   0   0   0   0   0   0   0   1
Return Home                      0  0   0   0   0   0   0   0   1   *
Entry Mode Set                   0  0   0   0   0   0   0   1   I/D S
Display ON/OFF                   0  0   0   0   0   0   1   D   C   B
Cursor and Display Shift         0  0   0   0   0   1  S/C R/L  *   *
Function Set                     0  0   0   0   1   DL  N   F   *   *
Set CG RAM address               0  0   0   1   A   A   A   A   A   A
Set DD RAM address               0  0   1   A   A   A   A   A   A   A
Read busy flag and address       0  1   BF  A   A   A   A   A   A   A
Write data to CG or DD RAM       1  0   D   D   D   D   D   D   D   D
Read data from CG or DD RAM      1  1   D   D   D   D   D   D   D   D
 */

#define LCD1602_I2C_ADDR                 0x20            // 0x20


// BASIC INSTRUCTION SET MUST BE ORED WITH OTHER PARAMETERS TO CONFORM A BYTE 8 BITS
#define LCD_CLEARDISPLAY            0x01
#define LCD_RETURNHOME              0x02
#define LCD_ENTRYMODESET            0x04
#define LCD_DISPLAYCONTROL          0x08
#define LCD_CURSOR_DISPLAY_SHIFT    0x10
#define LCD_FUNCTIONSET             0x20
#define LCD_SETCGRAMADDR            0x40
#define LCD_SETDDRAMADDR            0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT              0x00
#define LCD_ENTRYLEFT               0x02
#define LCD_ENTRYSHIFTINCREMENT     0x01
#define LCD_ENTRYSHIFTDECREMENT     0x00

// flags for display on/off control
#define LCD_DISPLAYON               0x04
#define LCD_DISPLAYOFF              0x00
#define LCD_CURSORON                0x02
#define LCD_CURSOROFF               0x00
#define LCD_BLINKON                 0x01
#define LCD_BLINKOFF                0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE_LEFT        0x08
#define LCD_DISPLAYMOVE_RIGHT       0x0C
#define LCD_CURSOR_MOVERIGHT        0x04
#define LCD_CURSOR_MOVELEFT         0x00

// flags for function set
#define LCD_8BITMODE                0x10
#define LCD_4BITMODE                0x00
#define LCD_2LINE                   0x08
#define LCD_1LINE                   0x00
#define LCD_5x10DOTS                0x04
#define LCD_5x8DOTS                 0x00

// flags for backlight control
#define LCD_BACKLIGHT               0x80
#define LCD_NOBACKLIGHT             0x00


// Following line tested for CCS compiler and best goes into main program body
// #use i2c(MASTER, FAST, SCL=PIN_A3, SDA=PIN_A2, FORCE_SW, STREAM=LCDI2C)
// Note: Using PICs with HW I2C capability can increase speed up to 2x over CCS SW I2

// Line addresses for LCDs which use
// the Hitachi HD44780U controller chip
#define LCD2004_LINE_1_ADDRESS      0x00
#define LCD2004_LINE_2_ADDRESS      0x40

#define BL                          0b10000000  // LCD Backlight
#define RS                          0b01000000  // register Select bit 1 Data
#define RW                          0b00100000  // Read bit
#define EN                          0b00010000  // Enable bit

#define RS_CMD                      0x00
#define RS_DATA                     0x40
#define READ_LCD                    0x20
#define WRITE_LCD                   0x00

/*
typedef enum{
    cmd  = 0,
    data = 1
}LCD_RegisterSelect_eType;

typedef union {
    struct{
        unsigned lcd_DB4    :   1;
        unsigned lcd_DB5    :   1;
        unsigned lcd_DB6    :   1;
        unsigned lcd_DB7    :   1;
        unsigned lcd_EN     :   1;
        unsigned lcd_RW     :   1;
        unsigned lcd_RS     :   1;
        unsigned lcd_BL     :   1;
    };

        uint8_t lcd_byte;

}LCD_DATA_sType;
*/


void LCD1602_Init(void);
void LCD1602_Write(uint8_t data , uint8_t RegSel);
void LCD1602_SetCursorPos(uint8_t x, uint8_t y);
void LCD1602_SetDisplayOnOff(Bool);
void LCD1602_SetCursorBlink(Bool);
void LCD1602_SetCursorOnOff(Bool);
void LCD1602_SetDisplayClear();
void LCD1602_SetCursorHome();
void LCD1602_SetChrDisplayBlink();
void LCD1602_SetCursorShift();
void LCD1602_SetDisplayShift();
void LCD1602_Printrs(const char * buffer);
void LCD1602_Prints(char * buffer);
void LCD1602_Putc(uint8_t c);
//-----------------------------------
// Print numeric and ASCII functions
//-----------------------------------
void LCD1602PutDec99(uint8_t decnum);
void LCD1602PutDec(uint8_t decnum);
void LCD1602PutDec16(uint16_t decnum);
void LCD1602PutDec32(uint32_t decnum);
void LCD1602PutHex_(uint8_t hexnum);
void LCD1602PutHex(uint8_t hexnum);
void LCD1602PutHex16(uint16_t hexnum);
void LCD1602PutHex32(uint32_t hexnum);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD1602_H */

