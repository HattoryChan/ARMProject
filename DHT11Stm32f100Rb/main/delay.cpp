/*

Delay for debugging

*/

#include "stm32f10x.h"    // For uint32_t

//---------------------------------------------         Delay function (sec)
void delay(uint16_t sec)
{
	uint32_t i;
 for (; sec != 0; sec--)
	{
		for(i = 0x5B8D80; i !=0; i--);										// 1 second delay if Fmax = 72 Mhz
	}
}
//-----------------------------------------------    Delay Function (ms)
void delayms(uint16_t ms)
{
	uint32_t i;
 for(; ms !=0; ms--)
	{
		for(i = 0x2710; i != 0; i--);											// 1ms delay if Fmax = 72mHz
	}
}
