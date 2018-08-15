#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32f10x.h"


uint8_t stateBlue, stateGreen;

 
// ќтдадочна€ затычка. —юда можно вписать код обработки ошибок.
#define	ERROR_ACTION(CODE,POS)		do{}while(0)
 
void vFreeRTOSInitAll()
{
   //SystemInit();
   RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
   
   GPIOC->CRH &= ~GPIO_CRH_CNF8;
   GPIOC->CRH |= GPIO_CRH_MODE8_0;
   
    GPIOC->CRH &= ~GPIO_CRH_CNF9;
   GPIOC->CRH |= GPIO_CRH_MODE9_0;
}


//Blinking for Blue led(slow)
void vLedTaskBlue (void *pvParameters)
{
    while(1)
    {
	if (stateBlue == 0)
	{
      GPIOC->BSRR = GPIO_BSRR_BR8;
	    stateBlue = 1;
			vTaskDelay(500);
	}
	else
	{
      GPIOC->BSRR = GPIO_BSRR_BS8;
	    stateBlue = 0;
			vTaskDelay(500);
	}
    }
    vTaskDelete(NULL);	
}

//Blinking for Gren leg(fast)
void vLedTaskGreen (void *pvParameters)
{
    while(1)
    {
	if (stateGreen == 0)
	{
      GPIOC->BSRR = GPIO_BSRR_BR9;
	    stateGreen = 1;
			vTaskDelay(100);
	}
	else
	{
      GPIOC->BSRR = GPIO_BSRR_BS9;
	    stateGreen = 0;
			vTaskDelay(100);
	}
    }
    vTaskDelete(NULL);	
}
 



int main(void)
{
		vFreeRTOSInitAll();
   //Create two task
      xTaskCreate(&vLedTaskBlue,"LedTaskBlue", configMINIMAL_STACK_SIZE,
					NULL, tskIDLE_PRIORITY + 1, NULL);
      if(pdTRUE != xTaskCreate(&vLedTaskGreen,"LedTaskGreen", 	configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL)) ERROR_ACTION(TASK_NOT_CREATE,0);	
   //Start Scheduler (task start)
     vTaskStartScheduler();
}

