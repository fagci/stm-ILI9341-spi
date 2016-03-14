#include "pwm.h"

void PWM_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin   = GPIO_Pin_1;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);

    u16 prescaler = (u16) (SystemCoreClock / (u32) 1000000);

    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode   = TIM_CounterMode_Up;
    tim.TIM_Period        = 1000 - (u8) 1;
    tim.TIM_Prescaler     = prescaler - (u8) 1;
    TIM_TimeBaseInit(TIM2, &tim);

    TIM_OCInitTypeDef timOc;
    timOc.TIM_OCMode      = TIM_OCMode_PWM1;
    timOc.TIM_OCPolarity  = TIM_OCPolarity_High;
    timOc.TIM_OutputState = TIM_OutputState_Enable;
    timOc.TIM_Pulse       = 1000 / (u8) 2 - 1;
    TIM_OC2Init(TIM2, &timOc);

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_Cmd(TIM2, ENABLE);
}

void PWM_setFreq(u16 f) {

}
