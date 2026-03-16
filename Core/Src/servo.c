#include "servo.h"

void Servo_Init(TIM_HandleTypeDef *htim)
{
    HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
    // CH2 (PE3) disabled on Discovery board — PE3 is L3GD20 CS, conflicts with SPI1
    // HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);
    // Center active servos at 90 degrees on startup
    Servo_SetAngle(htim, 1, 90);
    Servo_SetAngle(htim, 3, 90);
}

void Servo_SetPulse(TIM_HandleTypeDef *htim, uint8_t servo, uint16_t us)
{
    if (us < 1000) us = 1000;
    if (us > 2000) us = 2000;
    switch (servo) {
        case 1: __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, us); break;
        case 2: __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, us); break;
        case 3: __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, us); break;
    }
}

void Servo_SetAngle(TIM_HandleTypeDef *htim, uint8_t servo, uint8_t angle)
{
    if (angle > 180) angle = 180;
    uint16_t us = 1000 + (uint16_t)((uint32_t)angle * 1000 / 180);
    Servo_SetPulse(htim, servo, us);
}
