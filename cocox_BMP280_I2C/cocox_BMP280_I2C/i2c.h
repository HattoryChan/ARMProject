#include "stm32f10x_i2c.h"


#define I2C_MODE_READ	1
#define I2C_MODE_WRITE	0
#define I2C_ADDRESS(addr, mode)	((addr<<1) | mode)
#define I2C_ADDRESS_IAQ 	0b1000000	//0b1011010

#define I2C_MODE_READ	1
#define I2C_MODE_WRITE	0
#define I2C_ADDRESS(addr, mode)	((addr<<1) | mode)
#define I2C_ADDRESS_IAQ 	0b1000000	//0b1011010


void I2C1_ER_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_Init_master(void);
void I2C1_Write(uint8_t adress,uint8_t *buf, uint8_t size);
void I2C1_Read(uint8_t adress,uint8_t *buf, uint8_t size);
