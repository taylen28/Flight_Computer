#include "stm32f3xx_hal.h"
#include "lsm6dsox.h"
#include "main.h"

//This function reads from the register in the IMU getting the Data
static void LSM6DSOX_ReadReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t *data, uint16_t size)
{
    //set 7th bit to indicate read operation
    //cs need to be low
    //send data MOSI
    //read data MISO
    //cs need to be high after to end it
    uint8_t txByte = reg | 0x80; // Set MSB for read operation
    HAL_GPIO_WritePin(IMU_SPI_CS_GPIO_Port, IMU_SPI_CS_Pin, GPIO_PIN_RESET); // Pull CS low
    HAL_SPI_Transmit(hspi, &txByte, 1, HAL_MAX_DELAY); // Send register address
    HAL_SPI_Receive(hspi, data, size, HAL_MAX_DELAY); // Read data
    HAL_GPIO_WritePin(IMU_SPI_CS_GPIO_Port, IMU_SPI_CS_Pin, GPIO_PIN_SET); // Pull CS high
    
}
//This function writes to the IMU's control register
static void LSM6DSOX_WriteReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value)
{
    HAL_GPIO_WritePin(IMU_SPI_CS_GPIO_Port,IMU_SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, &reg, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(hspi,&value,1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_SPI_CS_GPIO_Port,IMU_SPI_CS_Pin, GPIO_PIN_SET);
    
}

uint8_t LSM6DSOX_Init(SPI_HandleTypeDef *hspi)
{
    uint8_t who_am_i = 0;
    LSM6DSOX_ReadReg(hspi, WHO_AM_I, &who_am_i, 1);
    if (who_am_i == WHO_AM_I_VAL)
    {
        LSM6DSOX_WriteReg(hspi, CTRL1_XL, 0x50);
        LSM6DSOX_WriteReg(hspi, CTRL2_G, 0x50);
        return 1;// Success
    } else {
        return 0; // Failure
    }
}

