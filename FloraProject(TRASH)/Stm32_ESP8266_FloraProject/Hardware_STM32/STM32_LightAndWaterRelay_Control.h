
#include "stdbool.h"
#include "stm32f10x.h"


/*=======================================================================
                          Relay Initiall part
========================================================================*/
void Init_RelayWaterGND(); //Watering of roots
void Init_RelayWaterAir(); //Watering of leaves
void Init_RelayTankFilling(); //tank filling(adding water to the tank)
void Init_RelayLight(); // Light 
void Init_RelayTankHeating(); //Tank Heating

/*=======================================================================
                          Call timer relay part
========================================================================*/
bool CallTimer_WaterGND(); //  Water-GND relay call and control function
bool CallTimer_WaterAir(); //  Water Air relay call and control function
bool CallTimer_Light();    //  Light relay call and control function

/*=======================================================================
                           Work with Relay part
========================================================================*/
void Relay_WaterGND(bool OnOff); //Set logical value to Relay WaterGND
void Relay_WaterAir(bool OnOff); //Set logical value to Relay WaterAir
void Relay_TankFilling(bool OnOff); //Set logical value to Relay TankFilling
void Relay_Light(bool OnOff); //Set logical value to Relay Light
void Relay_TankHeating(bool OnOff); //Set logical value to Relay TankHeating
