// #include "stm32f3xx_hal.h" //for HAL
#include <stdint.h>
#include "lsm6dsox.h"
#include "main.h"

// //This function reads from the register in the IMU getting the Data
// static void LSM6DSOX_ReadReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t *data, uint16_t size)
// {
//     //set 7th bit to indicate read operation
//     //cs need to be low
//     //send data MOSI
//     //read data MISO
//     //cs need to be high after to end it
//     uint8_t txByte = reg | 0x80; // Set MSB for read operation
//     HAL_GPIO_WritePin(IMU_SPI_CS_GPIO_Port, IMU_SPI_CS_Pin, GPIO_PIN_RESET); // Pull CS low
//     HAL_SPI_Transmit(hspi, &txByte, 1, 10); // Send register address
//     HAL_SPI_Receive(hspi, data, size, 10); // Read data
//     HAL_GPIO_WritePin(IMU_SPI_CS_GPIO_Port, IMU_SPI_CS_Pin, GPIO_PIN_SET); // Pull CS high
    
// }
// //This function writes to the IMU's control register
// static void LSM6DSOX_WriteReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value)
// {
//     HAL_GPIO_WritePin(IMU_SPI_CS_GPIO_Port,IMU_SPI_CS_Pin, GPIO_PIN_RESET);
//     HAL_SPI_Transmit(hspi, &reg, 1, 10);
//     HAL_SPI_Transmit(hspi,&value,1, 10);
//     HAL_GPIO_WritePin(IMU_SPI_CS_GPIO_Port,IMU_SPI_CS_Pin, GPIO_PIN_SET);
    
// }

// uint8_t LSM6DSOX_Init(SPI_HandleTypeDef *hspi)
// {
//     uint8_t who_am_i = 0;
//     LSM6DSOX_ReadReg(hspi, WHO_AM_I, &who_am_i, 1);
//     if (who_am_i == WHO_AM_I_VAL)
//     {
//         LSM6DSOX_WriteReg(hspi, CTRL1_XL, 0x50);
//         LSM6DSOX_WriteReg(hspi, CTRL2_G, 0x50);
//         return 1;// Success
//     } else {
//         return 0; // Failure
//     }
// }
// void LSM6DSOX_ReadAccel(SPI_HandleTypeDef *hspi, LSM6DSOX_Axes_t *axes)
// {
//     uint8_t buffer[6];
//     LSM6DSOX_ReadReg(hspi, OUTX_L_A, buffer, 6);
//     axes->x = (int16_t)(buffer[1] << 8 | buffer[0]); //combines low and high bytes for each x,y,z direction
//     axes->y = (int16_t)(buffer[3] << 8 | buffer[2]);
//     axes->z = (int16_t)(buffer[5] << 8 | buffer[4]);

// }
// void LSM6DSOX_ReadGyro(SPI_HandleTypeDef *hspi, LSM6DSOX_Axes_t *axes)
// {
//     uint8_t buffer[6];
//     LSM6DSOX_ReadReg(hspi, OUTX_L_G, buffer, 6);
//     axes->x = (int16_t)(buffer[1] << 8 | buffer[0]);
//     axes->y = (int16_t)(buffer[3] << 8 | buffer[2]);
//     axes->z = (int16_t)(buffer[5] << 8 | buffer[4]);

// }
static void LSM6DSOX_GPIO_Init()
{
    //MODER
    GPIOA_MODER &= ~(0x3 << 10);   // clear 
    GPIOA_MODER |=  (0x2 << 10);   // set AF mode — same shift
    GPIOA_MODER &= ~(0x3 << 12);   // clear 
    GPIOA_MODER |=  (0x2 << 12);
    GPIOA_MODER &= ~(0x3 << 14);   // clear 
    GPIOA_MODER |=  (0x2 << 14);
    GPIOA_MODER &= ~(0x3 << 20);   // clear 
    GPIOA_MODER |=  (0x01 << 20); //CS pin Output

    //high speed for MOSI(output) and SCK(clock)
    GPIOA_OSPEEDR &= ~(0x3 << 10 ); //PA5
    GPIOA_OSPEEDR |= (0x3 << 10);
    GPIOA_OSPEEDR &= ~(0x3 << 14); //PA7
    GPIOA_OSPEEDR |= (0x3 << 14);

    //AFRL
    GPIOA_AFRL &= ~((uint32_t)0xF << 20);
    GPIOA_AFRL |= ((uint32_t)0b0101 << 20); //AF5 = 4b'0101
    GPIOA_AFRL &= ~((uint32_t)0xF << 24);
    GPIOA_AFRL |= ((uint32_t)0b0101 << 24);
    GPIOA_AFRL &= ~((uint32_t)0xF << 28);
    GPIOA_AFRL |= ((uint32_t)0b0101 << 28);

    //BRSS For Chip Select Drive High   
    GPIOA_BSRR = (1 << 10);
  

}
static void LSM6DSOX_RCC_Init() 
{
    RCC_AHBENR  |= (1 << 17); //enable GPIOAEN
    RCC_APB2ENR |= (1 << 12); //enable SPI1EN
}

static void LSM6DSOX_SPI_Init()
{

}


static void LSM6dsox_WriteReg( uint8_t reg, uint8_t value)
{

    
}


