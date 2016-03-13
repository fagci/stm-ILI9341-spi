#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include "lib/ILI9341/text.h"
#include "lib/tinystd/itoa.h"

#include "lib/ADC/adc.h"
#include "lib/PWM/pwm.h"

static void plotData() {
    LCD_fillRect(0, 0, 10, 10, GREEN);

    s16 *adcDmaData = ADC_getData();

    u16 w = LCD_getWidth(),
        h = LCD_getHeight();

    u16 x  = 0;
    u16 h2 = (u16) (h / 2);
    u16 y  = h2,
        oy = h2;

    s16 s16h = 65535 / 2;

//        LCD_fillScreen(BLACK);
    LCD_drawFastHLine(0, h2, w, DGRAY);

    while (1) {
        y = (u16) ((adcDmaData[x] - s16h) / 30 + h2);
        LCD_drawLine(x, oy, (u16) (x + 1), y, GREEN);
        if (x == w - 1) {
            break;
        }
        x++;
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

    u8 i = 0;

//    DMA_Cmd(DMA1_Channel1, ENABLE);

    while (1) {
        LCD_fillRect(0, 0, 10, 10, YELLOW);
//        while (!isDataAvailable()) {
//            i = (u8) (i == 0 ? 1 : 0);
//            LCD_fillRect(0, 0, 10, 10, color[i]);
//            delay_ms(200);
//        }
        plotData();
        //markDataUsed();
        LCD_fillRect(0, 0, 10, 10, WHITE);
        delay_ms(1000);
    }

    while (1);
}

