#include "stm32f3xx_hal.h"
#include "E32_Lora.h"
#include "main.h"

uint8_t E32_Init(E32_Handle_t *dev)
{
    HAL_GPIO_WritePin(dev->m0Port, dev->m0Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->m1Port, dev->m1Pin, GPIO_PIN_RESET); // m0,m0 = 0,0 means on
    
    uint32_t start = HAL_GetTick(); //get tick start time
    while(HAL_GPIO_ReadPin(dev->auxPort,dev->auxPin) == GPIO_PIN_RESET)
    {
        if(HAL_GetTick() - start > 1000) //takesk tick to see if its been more than 1 second timeout
        {
            return 0; //return fail if so
        }
    }
    return 1;
}

uint8_t E32_Send(E32_Handle_t *dev, uint8_t *data, uint16_t len)
{
    if (HAL_GPIO_ReadPin(dev->auxPort, dev->auxPin) == GPIO_PIN_RESET) return 0;
    return (HAL_UART_Transmit(dev->huart, data, len, 1000) == HAL_OK) ? 1 : 0;
}
