/* Header for Timer Library
*
*
*
*/

/*=======================================================================
                    Initial and work with 74HC595 Part
========================================================================*/ 
void ShiftReg_74HC595_Init(); //Initiall 74HC595
void ShiftReg_ArrayToReg(); //Work with 74HC595 function
/*=======================================================================
                    Data return Part
========================================================================*/ 
int ShiftReg_WaterLevelIndication(); //Return WaterLevel value and set its on Led Indication
char ShiftReg_WaterLevelReturn(uint8_t WLSymbNumb); //Return Water Level percent on char

/*=======================================================================
                    CallTimer function Part
========================================================================*/ 
bool CallTimer_WaterLevel(); //Water Level call and controll function

