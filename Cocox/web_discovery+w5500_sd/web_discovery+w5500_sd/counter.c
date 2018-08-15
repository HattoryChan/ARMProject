#include "counter.h"

uint16_t ms_count;

uint32_t tick_count;
uint32_t second_count;

void SysTick_Handler(void)
{

	++ms_count;
	++tick_count;

	if(ms_count == 1000)
	{
		++second_count;
		ms_count = 0;
	}
}

void counter_init()
{

		SystemCoreClockUpdate ();

		if (SysTick_Config((SystemCoreClock) / (1000)))
	    while (1);/* Capture error */

}
