/*****************************************************************************
 *   acc.c:  Driver for the MMA7455 Accelerometer
 *
 *   Copyright(C) 2009, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************/

/*
 * NOTE: I2C must have been initialized before calling any functions in this
 * file.
 */

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "EA_BaseBoard/inc/acc.h"
#include <board.h>

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define I2CDEV LPC_I2C2

#define ACC_I2C_ADDR    (0x1D)

#define ACC_ADDR_XOUTL  0x00
#define ACC_ADDR_XOUTH  0x01
#define ACC_ADDR_YOUTL  0x02
#define ACC_ADDR_YOUTX  0x03
#define ACC_ADDR_ZOUTL  0x04
#define ACC_ADDR_ZOUTH  0x05
#define ACC_ADDR_XOUT8  0x06
#define ACC_ADDR_YOUT8  0x07
#define ACC_ADDR_ZOUT8  0x08
#define ACC_ADDR_STATUS 0x09
#define ACC_ADDR_DETSRC 0x0A
#define ACC_ADDR_TOUT   0x0B
#define ACC_ADDR_I2CAD  0x0D
#define ACC_ADDR_USRINF 0x0E
#define ACC_ADDR_WHOAMI 0x0F
#define ACC_ADDR_XOFFL  0x10
#define ACC_ADDR_XOFFH  0x11
#define ACC_ADDR_YOFFL  0x12
#define ACC_ADDR_YOFFH  0x13
#define ACC_ADDR_ZOFFL  0x14
#define ACC_ADDR_ZOFFH  0x15
#define ACC_ADDR_MCTL   0x16
#define ACC_ADDR_INTRST 0x17
#define ACC_ADDR_CTL1   0x18
#define ACC_ADDR_CTL2   0x19
#define ACC_ADDR_LDTH   0x1A
#define ACC_ADDR_PDTH   0x1B
#define ACC_ADDR_PW     0x1C
#define ACC_ADDR_LT     0x1D
#define ACC_ADDR_TW     0x1E

#define ACC_MCTL_MODE(m) ((m) << 0)
#define ACC_MCTL_GLVL(g) ((g) << 2)


#define ACC_STATUS_DRDY 0x01
#define ACC_STATUS_DOVR 0x02
#define ACC_STATUS_PERR 0x04


/******************************************************************************
 * External global variables
 *****************************************************************************/
int proportion = 0;
int factor = 0;
/******************************************************************************
 * Local variables
 *****************************************************************************/


/******************************************************************************
 * Local Functions
 *****************************************************************************/

static uint8_t getStatus(void)
{
    uint8_t status;
    Chip_I2C_MasterCmdRead(I2C2, ACC_I2C_ADDR, ACC_ADDR_STATUS, &status, 1);
    return status;
}

static uint8_t getModeControl(void)
{
    uint8_t ctrl=0;
    Chip_I2C_MasterCmdRead(I2C2, ACC_I2C_ADDR, ACC_ADDR_MCTL, &ctrl, 1);
    return ctrl;
}

static void setModeControl(uint8_t mctl)
{
    uint8_t buf[2];

    buf[0] = ACC_ADDR_MCTL;
    buf[1] = mctl;
    Chip_I2C_MasterSend(I2C2, ACC_I2C_ADDR, buf, 2);
}

/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize the ISL29003 Device
 *
 *****************************************************************************/
void acc_init (ACC_READ_t *acc)
{

    Board_I2C_Init(I2C2);
    Chip_I2C_Init(I2C2);
    Chip_I2C_SetClockRate(I2C2, 400000);
    Chip_I2C_SetMasterEventHandler(I2C2, Chip_I2C_EventHandlerPolling);

    /* set to measurement mode by default */

    setModeControl( (ACC_MCTL_MODE(ACC_MODE_MEASURE)
            | ACC_MCTL_GLVL(acc->G_range) ));

    // Determine the range that accelerometer is working and assign the proportion for measures

    uint8_t ctrlmode = (getModeControl()>>2)&0x03;
    if      (ctrlmode == ACC_RANGE_2G) proportion = 64;
    else if (ctrlmode == ACC_RANGE_4G) proportion = 32;
    else if (ctrlmode == ACC_RANGE_8G) proportion = 16;

    factor = 128/proportion;	// determines the steps for each g count

    acc_read(acc);
    acc->x_off = 0-acc->x;
    acc->y_off = 0-acc->y;
    acc->z_off = proportion-acc->z;
}

/******************************************************************************
 *
 * Description:
 *    Read accelerometer data
 *
 * Params:
 *   [out] x - read x value
 *   [out] y - read y value
 *   [out] z - read z value
 *
 *****************************************************************************/
/*
Table 13. Acceleration vs. Output (8-bit data)
FS Mode Acceleration Output
-----------------------------
2g Mode 	-2g 		$80
			-1g 		$C1
			 0g			$00
			+1g 		$3F
			+2g 		$7F
			================
4g Mode 	-4g 		$80
			-1g 		$E1
			 0g 		$00
			+1g 		$1F
			+4g 		$7F
			================
8g Mode 	-8g 		$80
			-1g 		$F1
			 0g 		$00
			+1g 		$0F
			+8g 		$7F
*/

void acc_read (ACC_READ_t *acc)
{
    /* wait for ready flag */
    while ((getStatus() & ACC_STATUS_DRDY) == 0);

    /*
     * Have experienced problems reading all registers
     * at once. Change to reading them one-by-one.
     */
    Chip_I2C_MasterCmdRead(I2C2, ACC_I2C_ADDR, ACC_ADDR_XOUT8, (uint8_t*)&acc->x, 1);
    Chip_I2C_MasterCmdRead(I2C2, ACC_I2C_ADDR, ACC_ADDR_YOUT8, (uint8_t*)&acc->y, 1);
    Chip_I2C_MasterCmdRead(I2C2, ACC_I2C_ADDR, ACC_ADDR_ZOUT8, (uint8_t*)&acc->z, 1);

    acc->x += acc->x_off;
    acc->y += acc->y_off;
    acc->z += acc->z_off;

	// Calculate proportion of 64 for 2g measures and others
	if  (acc->x > 127) acc->x_g = -((256-acc->x)/proportion);
	else acc->x_g = acc->x/proportion;

	if  (acc->y > 127) acc->y_g = -((256-acc->y)/proportion);
	else acc->y_g = acc->y/proportion;

	if  (acc->z > 127) acc->z_g = -((256-acc->z)/proportion);
	else acc->z_g = acc->z/proportion;

}

/******************************************************************************
 *
 * Description:
 *    Set the g-Range
 *
 * Params:
 *   [in] range - the g-Range
 *
 *****************************************************************************/
void acc_setRange(acc_range_t range)
{
    uint8_t mctl = 0;

    mctl = getModeControl();

    mctl &= ~(0x03 << 2);
    mctl |= ACC_MCTL_GLVL(range);

    setModeControl(mctl);
}

uint8_t acc_getRange(void)
{
	uint8_t range = 0;
	range = (getModeControl()>>2)&0x03;
	return range;
}

/******************************************************************************
 *
 * Description:
 *    Set sensor mode
 *
 * Params:
 *   [in] mode - the mode to set
 *
 *****************************************************************************/
void acc_setMode(acc_mode_t mode)
{
    uint8_t mctl = 0;

    mctl = getModeControl();

    mctl &= ~(0x03 << 0);
    mctl |= ACC_MCTL_MODE(mode);

    setModeControl(mctl);
}

