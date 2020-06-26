/*****************************************************************************
 *   joystick.c:  Driver for the Joystick switch
 *
 *   Copyright(C) 2009, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************/

/*
 * NOTE: GPIOInit must have been called before using any functions in this
 * file.
 * Modify by Daruin Solano September 2017 for LPC-Open
 */


/******************************************************************************
 * Includes
 *****************************************************************************/


#include <EA_BaseBoard/inc/joystick.h>
#include <define_pins.h>

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
DEFINE_PIN(LEFT ,   2, 4)
DEFINE_PIN(UP ,     2, 3)
DEFINE_PIN(DOWN ,   0, 15)
DEFINE_PIN(RIGHT ,  0, 16)
DEFINE_PIN(CENTER , 0, 17)
/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/


/******************************************************************************
 * Local Functions
 *****************************************************************************/


/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize the Joystick Driver
 *
 *****************************************************************************/
void joystick_init (void)
{
    /* set the GPIOs as inputs */
    CENTER_INPUT();
    UP_INPUT();
    DOWN_INPUT();
    LEFT_INPUT();
    RIGHT_INPUT();
}

/******************************************************************************
 *
 * Description:
 *    Read the joystick status
 *
 * Returns:
 *   The joystick status. The returned value is a bit mask. More than one
 *   direction may be active at any given time (e.g. UP and RIGHT)
 *
 *****************************************************************************/
uint8_t joystick_read(void)
{
    uint8_t status = 0;

    if (CENTER_READ() == 0){
        status |= JOYSTICK_CENTER;
    }

    if (DOWN_READ() == 0){
        status |= JOYSTICK_DOWN;
    }

    if (RIGHT_READ() == 0){
        status |= JOYSTICK_RIGHT;
    }

    if (UP_READ() == 0) {
        status |= JOYSTICK_UP;
    }

    if (LEFT_READ() == 0) {
        status |= JOYSTICK_LEFT;
    }

    return status;
}





