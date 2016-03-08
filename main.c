#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include "lib/ILI9341/text.h"
#include "lib/tinystd/itoa.h"

#include "lib/ADC/adc.h"

volatile u16 adcData;

void ADC1_IRQHandler() {
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC)) {
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
        adcData = ADC_GetConversionValue(ADC1);
    }
}

int main(void) {
    LCD_init();
    ADC_init();

    LCD_setOrientation(ORIENTATION_PORTRAIT);
    LCD_fillScreen(BLACK);

    u16 t  = 0;
    u16 w2 = LCD_PIXEL_WIDTH / 2;
    u16 x  = w2,
        ox = w2;

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    s16 halfU16Max = 65535 / 2;
    while (1) {
        x = ((s16) adcData - halfU16Max) / 30 + w2;
        LCD_scroll(t);
        LCD_fillRect(0, t, LCD_PIXEL_WIDTH, 1, BLACK);
        LCD_drawLine(ox, (u16) (t + 1), x, t, GREEN);
        if (t == 0) t = LCD_PIXEL_HEIGHT;
        t--;
        ox = x;
    }

    while (1);
}

