#ifndef E32_LORA_H
#define E32_LORA_H
#include "stm32f3xx_hal.h"

typedef struct {
    UART_HandleTypeDef *huart;
    GPIO_TypeDef *m0Port;
    uint16_t m0Pin;
    GPIO_TypeDef *m1Port;
    uint16_t m1Pin;
    GPIO_TypeDef *auxPort;
    uint16_t auxPin;
} E32_Handle_t;

//Function prototypes
uint8_t E32_Init(E32_Handle_t *dev);
uint8_t E32_Send(E32_Handle_t *dev, uint8_t *data, uint16_t len);



#endif /* E32_LORA_H */