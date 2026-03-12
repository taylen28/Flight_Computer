#ifndef BMP388_H
#define BMP388_H
#include "stm32f3xx_hal.h"

#define CHIP_ID_REG   0x00  // (WHO_AM_I equivalent)
#define CHIP_ID_VAL   0x50
#define PWR_CTRL      0x1B  // (enable press+temp, normal mode)
#define OSR           0x1C  // (oversampling — default 0x00 is fine)
#define DATA_REG      0x04  // (6 bytes: 3 pressure + 3 temp, XLSB/LSB/MSB order)
#define CALIB_REG     0x31  // (21 bytes of factory calibration)

//struct for adc calibration register (0x31)
typedef struct{
    uint16_t par_t1;
    uint16_t par_t2;
    int8_t   par_t3;
    int16_t  par_p1, par_p2;
    int8_t   par_p3, par_p4;
    uint16_t par_p5, par_p6;
    int8_t   par_p7, par_p8;
    int16_t  par_p9;
    int8_t   par_p10, par_p11;
}BMP388_Calib_t;

//struct for data
typedef struct{
    double pressure; //Pa
    double temperature;//Celcius

}BMP388_Data_t;

//Function prototypes
uint8_t BMP388_Init(SPI_HandleTypeDef *hspi);
void BMP388_Read(SPI_HandleTypeDef *hspi, BMP388_Data_t *data);



#endif /* BMP388_H */