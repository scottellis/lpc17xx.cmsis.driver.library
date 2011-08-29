#include "clock-arch.h"
#include "LPC17xx.h"

__IO clock_time_t Ticks;

#define LED_PIN 	(1<<6)

/* SysTick timer interrupt  handler */
void SysTick_Handler (void)
{
	++Ticks;
	if (!(Ticks & 0x07)){
		LPC_GPIO2->FIOPIN ^= LED_PIN;
	}
}

/* Timer init */
void clock_init(void)
{
	Ticks = 0;

	// NXP: Initialize System tick timer
	// Generate interrupt each SYSTICK_PERIOD microsecond
	if (SysTick_Config((SystemCoreClock/CLOCK_CONF_SECOND))){
		// Capture error
		while (1);
	}
}

/* returned The current clock time, measured in system ticks */
clock_time_t clock_time(void)
{
  return(Ticks);
}
