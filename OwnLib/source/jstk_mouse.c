/*
 * jstk_mouse.c
 *
 *  Created on: Apr 17, 2016
 *      Author: dsolano
 */

#include <chip.h>
#include <jstk_mouse.h>
#include <define_pins.h>

# if defined TFT32
#include <ssd1289.h>
#endif
# if defined OLED
#include <ssd1306.h>
#endif


struct MOUSE_s
{
	uint16_t xpos;
	uint16_t ypos;
	uint16_t curr_xpos;
	uint16_t curr_ypos;
	uint16_t color;
	uint16_t on_click_color;
};
struct MOUSE_s mouse;
struct MOUSE_s *pmouse;
uint8_t mouse_state;

DEFINE_PIN(UP,2,3)
DEFINE_PIN(DOWN,0,15)
DEFINE_PIN(LEFT,2,4)
DEFINE_PIN(RIGHT,0,16)
DEFINE_PIN(CENTER,0,17)

void mouse_init(void)
{
	Chip_GPIO_Init(LPC_GPIO);
	UP_INPUT();
	DOWN_INPUT();
	RIGHT_INPUT();
	LEFT_INPUT();
	CENTER_INPUT();
	//mouse_draw_pointer(center,125,YELLOW);
}

static uint8_t mouse_get_state(void)
{
	uint8_t state = JS_NONE;
	if (UP_READ() == 0) state |= JS_UP;
	if (DOWN_READ() == 0) state |= JS_DOWN;
	if (RIGHT_READ() == 0) state |= JS_RIGHT;
	if (LEFT_READ() == 0) state |= JS_LEFT;
	if (CENTER_READ() == 0) state |= JS_CENTER;
	return state;

}

void mouse_draw_pointer(uint16_t x , uint16_t y, uint16_t color)
{


}

void mouse_set_pos(uint16_t x , uint16_t y)
{

}

uint16_t mouse_get_pos_x(void)
{
	return pmouse->xpos;
}


uint16_t mouse_get_pos_y(void)
{
	return pmouse->ypos;
}

Bool mouse_on_click(void)
{

}

Bool mouse_on_hoover(void)
{

}

void mouse_pointer_state(void)
{
	uint8_t state = mouse_get_state();

	switch (state)
	{
	case JS_UP:
		if (pmouse->curr_ypos == 1) break;
		pmouse->curr_ypos--;
		pmouse->ypos = pmouse->curr_ypos;
		state = JS_UP;
		break;
	case JS_DOWN:
		break;
	case JS_RIGHT:
		break;
	case JS_LEFT:
		break;
	case JS_CENTER:
		break;
	default:
		break;
	}
}
