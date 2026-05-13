#ifndef LSM6DSOX_H
#define LSM6DSOX_H
//#include "stm32f3xx_hal.h"

#include <stdint.h>
#define WHO_AM_I 0x0F
#define WHO_AM_I_VAL 0x6C
//RCC Macros
#define RCC_BASE 0x40021000
#define RCC_AHBENR  (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))

//GPIO Macros
#define GPIOA_BASE 0x48000000
#define GPIOA_MODER (*(volatile uint32_t *)(GPIOA_BASE + 0x00)) //which mode
#define GPIOA_OSPEEDR (*(volatile uint32_t *)(GPIOA_BASE + 0x08)) //speed
#define GPIOA_AFRL (*(volatile uint32_t *)(GPIOA_BASE + 0x20))
#define GPIOA_AFRH (*(volatile uint32_t*)(GPIOA_BASE + 0x24) ) //ALternate function
#define GPIOA_BSRR (*(volatile uint32_t*)(GPIOA_BASE + 0x18)) //for CS


#define CTRL1_XL 0x10 //ctrl reg for accelerometer
#define CTRL2_G 0x11 //ctrl reg for gyros   cope
#define OUTX_L_A 0x28
#define OUTX_L_G 0x22

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}LSM6DSOX_Axes_t;


//Function prototypes
uint8_t LSM6DSOX_Init();
void LSM6DSOX_ReadAccel( LSM6DSOX_Axes_t *axes);
void LSM6DSOX_ReadGyro( LSM6DSOX_Axes_t *axes);


#endif /* LSM6DSOX_H */