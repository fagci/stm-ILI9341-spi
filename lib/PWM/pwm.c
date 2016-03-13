#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include "pwm.h"

void PWM_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin   = GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio);

    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode   = TIM_CounterMode_Up;
    tim.TIM_Period        = (uint16_t) (SystemCoreClock / 10000 - 1);
    tim.TIM_Prescaler     = 0;
    TIM_TimeBaseInit(TIM4, &tim);

    TIM_OCInitTypeDef timOc;
    timOc.TIM_OCMode       = TIM_OCMode_PWM1;
    timOc.TIM_OCPolarity   = TIM_OCPolarity_Low;
    timOc.TIM_OutputState  = TIM_OutputState_Enable;
    timOc.TIM_Pulse        = (uint16_t) (SystemCoreClock / 40000 - 1);
    TIM_OC2Init(TIM4, &timOc);

    TIM_Cmd(TIM4, ENABLE);

    TIM_SetCompare1(TIM4, 512);
}

void PWM_setFreq(u16 f) {

}