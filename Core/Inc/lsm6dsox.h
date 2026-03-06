#ifndef LSM6DSOX_H
#define LSM6DSOX_H
#include "stm32f3xx_hal.h"

#define WHO_AM_I 0x0F
#define WHO_AM_I_VAL 0x6C
#define CTRL1_XL 0x10 //ctrl reg for accelerometer
#define CTRL2_G 0x11 //ctrl reg for gyroscope

//Function prototypes
uint8_t LSM6DSOX_Init(SPI_HandleTypeDef *hspi);


#endif /* LSM6DSOX_H */