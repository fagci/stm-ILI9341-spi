#ifndef TEST2_PWM_H
#define TEST2_PWM_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>

void PWM_init();
void PWM_setFreq(u16 f);

#endif //TEST2_PWM_H
