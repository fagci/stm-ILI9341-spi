#include <stm32f10x_dma.h>
#include <stdlib.h>
#include "ILI9341_lib/graph.h"
#include "ILI9341_lib/text.h"

u16  fps = 0;
char text[15];

void TIM2_IRQHandler(void) {
    TIM2->SR &= ~TIM_SR_UIF;
    itoa(fps, text, 10); //переводим FPS в стринг и кладем в массив
    fps = 0;
}

void initFpsTimer() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM_TimeBaseStructure.TIM_Prescaler     = (72000 / 2) - 1; //обновляем FPS раз в секунду
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period        = 1000 * 2;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(BLACK);


    LCD_setCursor(0, 0);
    LCD_write('5');

    LCD_writeString("Init array\n");

    u16      cnt = 6 * 8 * 3;
    u8       pixelComponents[cnt];
    for (int i   = 0; i < cnt; ++i) {
        pixelComponents[i] = 0;
    }
    LCD_writeString("GetRect\n");

    LCD_getRect(pixelComponents, 0, 0, 6, 8);

    LCD_setCursor(0, 50);
    for (int i = 0; i < cnt; ++i) {
        char buf[15];
        itoa(pixelComponents[i], buf, 16);
        LCD_writeString(buf);
        LCD_writeString(",");
    }

    LCD_writeString("Ready\n");


    while (1);
}
