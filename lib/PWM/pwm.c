#include "pwm.h"

void PWM_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin   = GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio);

    u16 period = (SystemCoreClock / 1000000) /2; // 1 MHz

    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode   = TIM_CounterMode_Up;
    tim.TIM_Period        = period - 1;
    tim.TIM_Prescaler     = 1000;
    TIM_TimeBaseInit(TIM4, &tim);

    TIM_OCInitTypeDef timOc;
    timOc.TIM_OCMode       = TIM_OCMode_PWM1;
    timOc.TIM_OCPolarity   = TIM_OCPolarity_High;
    timOc.TIM_OutputState  = TIM_OutputState_Enable;
    timOc.TIM_Pulse        = period / 2;
    TIM_OC2Init(TIM4, &timOc);
    TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);

    TIM_Cmd(TIM4, ENABLE);
}

void PWM_setFreq(u16 f) {

}