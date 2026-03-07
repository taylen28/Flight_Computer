#ifndef LSM6DSOX_H
#define LSM6DSOX_H
#include "stm32f3xx_hal.h"

#define WHO_AM_I 0x0F
#define WHO_AM_I_VAL 0x6C
#define CTRL1_XL 0x10 //ctrl reg for accelerometer
#define CTRL2_G 0x11 //ctrl reg for gyroscope
#define OUTX_L_A 0x28 //
#define OUTX_L_G 0x22

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}LSM6DSOX_Axes_t;


//Function prototypes
uint8_t LSM6DSOX_Init(SPI_HandleTypeDef *hspi);
void LSM6DSOX_ReadAccel(SPI_HandleTypeDef *hspi, LSM6DSOX_Axes_t *axes);


#endif /* LSM6DSOX_H */