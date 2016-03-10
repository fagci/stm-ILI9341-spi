#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include "lib/ILI9341/text.h"
#include "lib/tinystd/itoa.h"

#include "lib/ADC/adc.h"
#include "lib/PWM/pwm.h"

static void plotData(){
    LCD_fillRect(0, 0, 10, 10, GREEN);

    __IO u16 *adcDmaData = ADC_getData();

    u16 w = LCD_getWidth(),
        h = LCD_getHeight();

    u16 t  = 0;
    u16 h2 = (u16) (h / 2);
    u16 y  = h2,
        oy = h2;

    s16 halfU16Max = 65535 / 2;

//        LCD_fillScreen(BLACK);
    LCD_drawFastHLine(0, h2, w, DGRAY);

    while (1) {
        y = (u16) (((s16) adcDmaData[t] - halfU16Max) / 30 + h2);
        LCD_drawLine(t, y, (u16) (t + 1), oy, GREEN);
        if (t == w - 1) {
            break;
        }
        t++;
        oy = y;
    }
}


int main(void) {
    LCD_init();
    ADC_init();
    PWM_init();

    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(BLACK);


    u16 color[] = {RED, GREEN};
    u8  i       = 0;
    while (1) {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        DMA_Cmd(DMA1_Channel1, ENABLE);

        LCD_fillRect(0, 0, 10, 10, YELLOW);
        while(DMA_GetITStatus(DMA1_IT_TC1)) {
            i = (u8) (i == 0 ? 1 : 0);
            LCD_fillRect(0, 0, 10, 10, color[i]);
            delay_ms(200);
        }
        plotData();
        LCD_fillRect(0, 0, 10, 10, WHITE);
        delay_ms(1000);
    }


    while (1);
}

