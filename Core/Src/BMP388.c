#include "main.h"
#include "BMP388.h"
#include "stm32f3xx_hal.h"
#include "math.h"

static void BMP388_ReadReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t *data, uint16_t size)
{
    //set 7th bit to indicate read operation
    //cs need to be low
    //send data MOSI
    //read data MISO
    //cs need to be high after to end it
    uint8_t txByte = reg | 0x80; // Set MSB for read operation
    uint8_t dummy;
    HAL_GPIO_WritePin(BMU_CS_SPI_GPIO_Port, BMU_CS_SPI_Pin, GPIO_PIN_RESET); // Pull CS low
    HAL_SPI_Transmit(hspi, &txByte, 1, HAL_MAX_DELAY); // Send register address
    HAL_SPI_Receive(hspi, &dummy, 1, HAL_MAX_DELAY);   // discard BMP388 dummy byte
    HAL_SPI_Receive(hspi, data, size, HAL_MAX_DELAY);  // Read actual data
    HAL_GPIO_WritePin(BMU_CS_SPI_GPIO_Port, BMU_CS_SPI_Pin, GPIO_PIN_SET); // Pull CS high
    
}
//This function writes to the IMU's control register
static void BMP388_WriteReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value)
{
    HAL_GPIO_WritePin(BMU_CS_SPI_GPIO_Port,BMU_CS_SPI_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, &reg, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(hspi,&value,1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(BMU_CS_SPI_GPIO_Port,BMU_CS_SPI_Pin, GPIO_PIN_SET);
    
}

static BMP388_Calib_t calib;

uint8_t BMP388_Init(SPI_HandleTypeDef *hspi)
{
    uint8_t chip_id = 0;
    BMP388_ReadReg(hspi, CHIP_ID_REG,&chip_id,1 );
    if(chip_id == CHIP_ID_VAL)
    {
        BMP388_WriteReg(hspi, PWR_CTRL, 0x33);

        uint8_t cal[21];//callibration buffer
        BMP388_ReadReg(hspi, CALIB_REG, cal, 21);

        //ADC CALIBRRATION
        calib.par_t1  = (uint16_t)cal[0]  | ((uint16_t)cal[1]  << 8);
        calib.par_t2  = (uint16_t)cal[2]  | ((uint16_t)cal[3]  << 8);
        calib.par_t3  = (int8_t)cal[4];
        calib.par_p1  = (int16_t)((uint16_t)cal[5]  | ((uint16_t)cal[6]  << 8));
        calib.par_p2  = (int16_t)((uint16_t)cal[7]  | ((uint16_t)cal[8]  << 8));
        calib.par_p3  = (int8_t)cal[9];
        calib.par_p4  = (int8_t)cal[10];
        calib.par_p5  = (uint16_t)cal[11] | ((uint16_t)cal[12] << 8);
        calib.par_p6  = (uint16_t)cal[13] | ((uint16_t)cal[14] << 8);
        calib.par_p7  = (int8_t)cal[15];
        calib.par_p8  = (int8_t)cal[16];
        calib.par_p9  = (int16_t)((uint16_t)cal[17] | ((uint16_t)cal[18] << 8));
        calib.par_p10 = (int8_t)cal[19];
        calib.par_p11 = (int8_t)cal[20];

    }
    else{
        return 0;
    }
    return 1;
}
void BMP388_Read(SPI_HandleTypeDef *hspi, BMP388_Data_t *data)
{
    uint8_t buf[6];
    BMP388_ReadReg(hspi, DATA_REG, buf, 6);

    uint32_t raw_press = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16);
    uint32_t raw_temp  = (uint32_t)buf[3] | ((uint32_t)buf[4] << 8) | ((uint32_t)buf[5] << 16);

    // BMP388 datasheet Appendix B — scale raw NVM coefficients before use
    double par_T1 = (double)calib.par_t1 * 256.0;
    double par_T2 = (double)calib.par_t2 / 1073741824.0;          // 2^30
    double par_T3 = (double)calib.par_t3 / 281474976710656.0;     // 2^48

    double var1 = (double)raw_temp - par_T1;
    double t_lin = var1 * par_T2 + var1 * var1 * par_T3;
    data->temperature = t_lin;

    double par_P1  = ((double)calib.par_p1 - 16384.0) / 1048576.0;        // 2^20
    double par_P2  = ((double)calib.par_p2 - 16384.0) / 536870912.0;      // 2^29
    double par_P3  = (double)calib.par_p3  / 4294967296.0;                // 2^32
    double par_P4  = (double)calib.par_p4  / 137438953472.0;              // 2^37
    double par_P5  = (double)calib.par_p5  * 8.0;                         // 2^-3
    double par_P6  = (double)calib.par_p6  / 64.0;                        // 2^6
    double par_P7  = (double)calib.par_p7  / 256.0;                       // 2^8
    double par_P8  = (double)calib.par_p8  / 32768.0;                     // 2^15
    double par_P9  = (double)calib.par_p9  / 281474976710656.0;           // 2^48
    double par_P10 = (double)calib.par_p10 / 281474976710656.0;           // 2^48
    double par_P11 = (double)calib.par_p11 / 36893488147419103232.0;      // 2^65

    double p1  = par_P6 * t_lin;
    double p2  = par_P7 * (t_lin * t_lin);
    double p3  = par_P8 * (t_lin * t_lin * t_lin);
    double out1 = par_P5 + p1 + p2 + p3;

    double p4  = par_P2 * t_lin;
    double p5  = par_P3 * (t_lin * t_lin);
    double p6  = par_P4 * (t_lin * t_lin * t_lin);
    double out2 = (double)raw_press * (par_P1 + p4 + p5 + p6);

    double p7  = (double)raw_press * (double)raw_press;
    double p8  = par_P9 + par_P10 * t_lin;
    double p9  = p7 * p8;
    double p10 = p9 + ((double)raw_press * (double)raw_press * (double)raw_press) * par_P11;
    data->pressure = out1 + out2 + p10;

    data->altitude = 44330.0 * (1.0 - pow(data->pressure / 101325.0, 0.1903));
}
