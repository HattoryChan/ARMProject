

#include "stm32f10x.h"


/*=======================================================================
                          Relay Initiall part
========================================================================*/
void Init_RelayWaterGND(); //Watering of roots
void Init_RelayWaterAir(); //Watering of leaves
void Init_RelayWateringMotor(); //tank filling(adding water to the tank)
void Init_RelayLight(); // Light 
void Init_RelayTankMixing(); //Tank Mixing

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
void Relay_WateringMotor(bool OnOff); //Set logical value to Relay TankFilling
void Relay_Light(bool OnOff); //Set logical value to Relay Light
void Relay_TankMixing(bool OnOff); //Set logical value to Relay TankHeating

/*=======================================================================
                           Work with return data part
========================================================================*/
char Light_LenghtReturn(uint8_t DayLenghtSymbNumb); //return LightDayLenght on char
char WaterGND_LenghtReturn(uint8_t SecondSymbNumb);//return WaterGNDLenght on char

void Watering_SetDate(); // Set date last watering
char Watering_LastHourReturn(uint8_t HourSymbNumb); //Return Last Watering hour on char
char Watering_LastMinReturn(uint8_t MinSymbNumb); //Return Last Watering minute on char

void TankFeeling_SetDate(); // Set date last TankFeeling
char TankFeeling_LastHourReturn(uint8_t HourSymbNumb); //Return Last TankFeeling hour on char
char TankFeeling_LastMinReturn(uint8_t MinSymbNumb); //Return Last TankFeeling minute on char

void WaterAir_SetDate(); // Set date last WaterAir
char WaterAir_LastHourReturn(uint8_t HourSymbNumb); //Return Last WaterAir hour on char
char WaterAir_LastMinReturn(uint8_t MinSymbNumb); //Return Last WaterAir minute on char

