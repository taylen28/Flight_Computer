#ifndef SERVO_H
#define SERVO_H
#include "stm32f3xx_hal.h"

// TIM3 @ 50Hz, 1 tick = 1us
// Servo pulse: 1000us = 0deg, 1500us = 90deg, 2000us = 180deg

void Servo_Init(TIM_HandleTypeDef *htim);
void Servo_SetAngle(TIM_HandleTypeDef *htim, uint8_t servo, uint8_t angle); // servo 1-3, angle 0-180
void Servo_SetPulse(TIM_HandleTypeDef *htim, uint8_t servo, uint16_t us);   // direct 1000-2000us

#endif /* SERVO_H */
