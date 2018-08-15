/*
*    Algorithm: If we need read - ReadPage
*               If we need write - Flash_UNLOCK, clear, WritePage, Flash_Lock
*
*/

/*=======================================================================
                           Work with FLASH parts
========================================================================*/
void FLASH_Lock();
void FLASH_Unlock();
void FLASH_ReadPage();
void FLASH_WritePage();
void FLASH_Clear();

/*=======================================================================
              Return data and work with g_aFlash array part
========================================================================*/
uint8_t FLASH_ArrayGetValue(uint8_t Pos);
void FLASH_ArraySetValue(uint8_t Pos, uint8_t OneSymbolData);
