#include "stm32f10x.h"
#include "delay.h"            // Delay for debugging
#include "Periphery_Init.h"   // Periphery Initiall
#include "GSMShieldSIM900.h"  // GSM Shield SIM900




int main(void)
{
   InitPeriphery(9600);       // Init perephery for USART1 and set boud rate = 9600
   
   //-------------================--------------------- TEST PART
   //--------------------------------------------					GPIO Port[C]
																																									//clocking Port [C]
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;  															
																																												//PC[8,9] - outputs, Fmax = 2Mhz
	GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9);	
	GPIOC->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_MODE9_0;
   //----------=============------------ TEST PART
   struct AdressBook 
   {																								//Structure for remembering numbers
     int Categories;																							//'0' - unknown, '1' - friendly, '2' - into black list
     char Number[12];	
   }; 
   
   
   //																		Create structures and get it to SendMessage function
	struct AdressBook MyWorkNumber;
	char* pMyWorkNumberNumb = MyWorkNumber.Number;	
	  pMyWorkNumberNumb = "+79193286730";
	  MyWorkNumber.Categories = 1;    
    
   //------------------------------------------------------------------------------------------					Initial GSM module configuration 	
   //------------------------------------------------			  	Send "AT" and check status GSM Module   	
   if(CheckGSMStatus() == 1)
   {   
   GPIOC->BSRR = GPIO_BSRR_BS9;			//ON   //STM32 ready
   }
   else
      GPIOC->BSRR = GPIO_BSRR_BS8;			//ON   //STM32 ready
   
	  CLIPSetting(1);                    //CLIP - ON
		
	  // GPRS Setting for Beeline(create GPRS connections)
	//  GPRSSetupSetting("GPRS","internet.beeline.ru","beeline","beeline");	
   
   //      delayms(500);
 //------------------------------------------------                                    Initial GSM module configuration  END
   /*         //	   Send Email 
    if(SendEmailToAuthSMTP("1","30","smtp.mail.ru","465","pyrion@mail.ru",
											 "Netatano123456","pyrion@mail.ru","FannyGays","pyrion@mail.ru",
											 "pyrion","Test Message from STM32F100RB","Test 123321 Test") == 1)
		{
			GPIOC->BSRR = GPIO_BSRR_BS8;			//ON
		}
		else
		{
			GPIOC->BSRR = GPIO_BSRR_BR9;			//OFF
		}
	*/		   	  
    
 
       
    GPIOC->BSRR = GPIO_BSRR_BR9;			//OFF
 
 ClearInputBuffer();       //Clear message
  while(1)
  {
            	//Hangup input call
     if(NumberFind(pMyWorkNumberNumb) == 1)										//if we have Indefier number
		{
			delay(1);
		 GPIOC->ODR ^= GPIO_ODR_ODR9;					//		ON\OFF	
	    USART1_Send_String("ATH0\r");														//Hangup	  	
			delay(1);																			//Time for CLIP
		 ClearInputBuffer();                            //Clear message buffer for New message(RING,SMS)
		}
	else if (NumberFind(pMyWorkNumberNumb) == 0)									//If number unknown
		{	
			delay(1);																		
	    USART1_Send_String("ATH0\r");														//Hangup	  	
				delay(1);																			//Time for CLIP
		 ClearInputBuffer();                            //Clear message buffer for New message(RING,SMS)
		}					
  }
}

/*																					 Debug instrument
			
GPIOC->BSRR = GPIO_BSRR_BR8;			//OFF
delayms(500);
GPIOC->BSRR = GPIO_BSRR_BS8;	//ON

GPIOC->ODR = GPIO_ODR_ODR8;			//OFF/ON

*/
