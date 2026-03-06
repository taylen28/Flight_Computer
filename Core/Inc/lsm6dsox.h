
#include "stm32f3xx_hal.h"

#ifndef LSM6DSOX_H
#define LSM6DSOX_H

#define WHO_AM_I 0x0F
#define WHO_AM_I_VAL 0x6C

//Function prototypes
uint8_t LSM6DSOX_Init(SPI_HandleTypeDef *hspi);


#endif /* LSM6DSOX_H */