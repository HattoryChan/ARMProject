/*   Working with FLASH 
*
*
*
*
*/
#include "main.h"
#define FLASH_PTR         0x0801FC00              // A pointer to the page in which the settings are stored
#define FLASH_KEY1 ((uint32_t)0x45670123)          //Unlock Flash(First key)

#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)          //Unlock Flash(Second key)



int g_aFlash[128];                  //Flash data array


/*=======================================================================
                           Work with FLASH part
========================================================================*/
/*   Block access to the flash
*
*/
void FLASH_Lock()
{
   FLASH->CR |= FLASH_CR_LOCK;
}

/*  Unlocking access to the flash
*
*/
void FLASH_Unlock()
{
   // flash locked?
   if(FLASH->CR & FLASH_CR_LOCK)
   {
      // Unlock flash
      FLASH->KEYR = FLASH_KEY1;
      FLASH->KEYR = FLASH_KEY2;
   }
}

/* Read page(128 byte) from Flash to array
*
*/
void FLASH_ReadPage()
{
   FLASH_Unlock();
   // Read 128 byte
   for(uint8_t i = 0; i < 128; i++)
      g_aFlash[i] = *(__IO u8*)(FLASH_PTR + i);
   
  FLASH_Lock();
   
   for(int i = 0; i < 128; i++)
   {
      if(g_aFlash[i] == 255)
         g_aFlash[i] = 0;
   }
   // DEBUG
 //  g_aFlash[3] = 1;   //hour first
 //  g_aFlash[4] = 4;   //hour second
 //  g_aFlash[5] = 3;   //minute first
 //  g_aFlash[6] = 0;   //minute second
}

/*  Write to FLASH g_aFlash array
*
*/
void FLASH_WritePage()
{   
   // Wait for now flash ready
   while(FLASH->SR & FLASH_SR_BSY) ;

   // If action successfully. Reset flag. Put on hardware, remote writing - "1" 
   if(FLASH->SR & FLASH_SR_EOP)             
		FLASH->SR = FLASH_SR_EOP;

   // Flesh write access
	FLASH->CR |= FLASH_CR_PG;

   // Write array to Flash Page
	for(uint8_t i = 0; i < 128; i += 2)
	{
		*(volatile uint16_t*)(FLASH_PTR + i) = (uint16_t)(g_aFlash[i + 1] << 8 | g_aFlash[i]);
		
      // Wait before action completed
      while(!(FLASH->SR & FLASH_SR_EOP)) ;

      //Reset flag. Put on hardware, reset - write "1" 
      FLASH->SR &= ~FLASH_SR_EOP;
   }

   // This bit allow write to flash. Reset him
   FLASH->CR &= ~(FLASH_CR_PG);
}

/* Unlock and Clear flash(need use before wtite new data)
*
*/
void FLASH_Clear()
{
   // Whait before Flash ready
   while(FLASH->SR & FLASH_SR_BSY) ;

   // If action successfully. Reset flag. Put on hardware, remote writing - "1" 
   if(FLASH->SR & FLASH_SR_EOP)          
      FLASH->SR = FLASH_SR_EOP;

   // Set clearint one page bit
   FLASH->CR |= FLASH_CR_PER;
   // Set clearing page adress
   FLASH->AR = FLASH_PTR;
   // Start Clearing 
   FLASH->CR |= FLASH_CR_STRT;
  
   // Whait clearing page
   while(!(FLASH->SR & FLASH_SR_EOP)) ;

   //Reset flag. Put on hardware, reset - write "1" 
   FLASH->SR = FLASH_SR_EOP;
   // Reset flag
   FLASH->CR &= ~FLASH_CR_PER;
}

/*=======================================================================
              Return data and work with g_aFlash array part
========================================================================*/
/*  Return data from g_aFlash array
*
*/
uint8_t FLASH_ArrayGetValue(uint8_t Pos)
{
   return g_aFlash[Pos];
}

/* Set OneSymbolData to Array
*
*/
void FLASH_ArraySetValue(uint8_t Pos, uint8_t OneSymbolData)
{
   g_aFlash[Pos] = OneSymbolData;
}
