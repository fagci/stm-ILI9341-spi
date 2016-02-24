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

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(BLACK);

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

//    LCD_setTextSize(3);
//    char b[15];
//    u32 cnt = LCD_PIXEL_COUNT;
//    itoa(UINT16_MAX & cnt,b,10);
//    LCD_writeString(b);
//    LCD_writeString("\n");
//
//    cnt -= cnt & UINT16_MAX;
//
//    itoa(UINT16_MAX & cnt,b,10);
//    LCD_writeString(b);
//    while(1);

    u8 i = 30;

    while (i--) {
        LCD_setCursor(0, 0);
        LCD_fillScreen(RED);
        LCD_writeString(text);
        fps++;
        LCD_setCursor(0, 0);
        LCD_fillScreen(GREEN);
        LCD_writeString(text);
        fps++;
    }

    while (1);
}
