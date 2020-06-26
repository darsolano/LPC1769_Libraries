/*
 * lm4811.c
 *
 *  Created on: Dec 31, 2018
 *      Author: dsolano
 *      2 channel amplifier
 *      LM4811 Dual 105mW Headphone Amplifier with Digital Volume Control and Shutdown Mode
 *      This amplifier is part of the EA Baseboard lpcXpresso1769
 */

#include <lm4811.h>
#include <define_pins.h>
#include <hardware_delay.h>

#define LM4811_AUDIO		LPC_DAC

DEFINE_PIN(DIGITAL_AUDIO,0,26)	// Sound Output
DEFINE_PIN(CLK,0,27)			// LM4811 Clock
DEFINE_PIN(UP_DN,0,28)			// Volume up and Down
DEFINE_PIN(SHTDWN,2,13)			// Shutdown chip amplifier

// Expressed in dB
int8_t lm4811_cvolume_power[] = {
		-33,-30,-27,-24,-21,-18,-15,-12,-9,-6,-3, 0, 3, 6, 9, 12
};

static uint32_t notes[] = {
        2272, // A - 440 Hz do grave
        2024, // B - 494 Hz re
        3816, // C - 262 Hz mi
        3401, // D - 294 Hz fa
        3030, // E - 330 Hz sol
        2865, // F - 349 Hz la
        2551, // G - 392 Hz si
        1136, // a - 880 Hz do agudo
        1012, // b - 988 Hz re
        1912, // c - 523 Hz mi
        1703, // d - 587 Hz fa
        1517, // e - 659 Hz sol
        1432, // f - 698 Hz la
        1275, // g - 784 Hz si
};


// UP signal HIGH
static void lm4811_volume_up(LM4811_t* amp, int steps){

}

// DOWN signal LOW
static void lm4811_volume_down(LM4811_t* amp, int steps){

}

// SHutdown pin goes high
static void lm4811_shutdown(LM4811_t* amp){

}

// shutdown pin goes low
static void lm4811_wakeup(LM4811_t* amp){

}

void lm4811_init(LM4811_t* amp){
	Chip_IOCON_Init(LPC_IOCON);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 26, IOCON_MODE_INACT, IOCON_FUNC2);
	DIGITAL_AUDIO_OUTPUT();
	CLK_OUTPUT();
	UP_DN_OUTPUT();
	SHTDWN_OUTPUT();
	LM4811_AUDIO->CNTVAL = 0;
	LM4811_AUDIO->CR = 0;
	LM4811_AUDIO->CTRL = 0;
}

void lm4811_produce_sound_stream(LM4811_t* amp){

}

void lm4811_produce_sound_data(LM4811_t* amp, uint8_t data){
	LM4811_AUDIO->CR = (data << 6);
}
