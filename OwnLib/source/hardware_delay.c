
#include <hardware_delay.h>

#define PCLK	(SystemCoreClock/2)

// ****************
// _delay - creates a delay of the appropriate number of ticks (happens every 1 us)
void _delay_uS (uint32_t delayTicks) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER1);
	Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_TIMER1, SYSCTL_CLKDIV_1);
	TIMER1->CTCR = 0x0;
	TIMER1->PR = SystemCoreClock/1000000;
	TIMER1->TCR = 0x02;	//reset TIMER0 match and counter and prescaler
	TIMER1->TCR = 0x01;	// Enable TIMER00
	while (TIMER1->TC < delayTicks);
	TIMER1->TCR = 0;	// TIMER00 disable
}

void _delay_ms (uint32_t delayTicks) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER1);
	Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_TIMER1, SYSCTL_CLKDIV_1);
	TIMER1->CTCR = 0x0;
	TIMER1->PR = SystemCoreClock/1000;
	TIMER1->TCR = 0x02;	//reset TIMER0 match and counter and prescaler
	TIMER1->TCR = 0x01;	// Enable TIMER00
	while (TIMER1->TC < delayTicks);
	TIMER1->TCR = 0;	// TIMER00 disable
}

void initTimer0(uint32_t freq)
{
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER0);
	Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_TIMER0, SYSCTL_CLKDIV_1);

	TIMER0->CTCR = 0x0;
	TIMER0->PR = SystemCoreClock / freq;//PRESCALE; //Increment TC at every 24999+1 clock cycles
	TIMER0->TCR = 0x02; //Reset TIMER0
}

void startTimer0(void)
{
	TIMER0->TCR = 0x02; //Reset TIMER0
	TIMER0->TCR = 0x01; //Enable TIMER0
}

uint_fast32_t stopTimer0(void)
{
	TIMER0->TCR = 0x00; //Disable TIMER0
	return TIMER0->TC;
}

uint_fast32_t getTimer0_counter(void)
{
	return TIMER0->TC;
}

void delayUS(unsigned int microseconds) //Using TIMER00
{
	TIMER0->TCR = 0x02; //Reset TIMER0
	TIMER0->TCR = 0x01; //Enable TIMER0
	while(TIMER0->TC < microseconds); //wait until TIMER0 counter reaches the desired delay
	TIMER0->TCR = 0x00; //Disable TIMER0
}

void delayMS(unsigned int milliseconds) //Using TIMER00
{
	delayUS(milliseconds * 1000);
}
