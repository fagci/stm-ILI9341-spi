#include "fps_counter.h"

u16  fps = 0;
char text[15];

void TIM2_IRQHandler(void) {
    TIM2->SR &= ~TIM_SR_UIF;
    itoa(fps, text, 10);
    fps = 0;
}

void initFpsTimer() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM_TimeBaseStructure.TIM_Prescaler     = (72000 / 2) - 1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period        = 1000 * 2;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void FPS_start() {
    initFpsTimer();
}
void FPS_frame() {
    fps++;
}
unsigned char *FPS_getText() {
    return (unsigned char *) text;
}
