#include <stm32f10x_dma.h>
#include "ILI9341_lib/text.h"
#include "test_suite/primitives.h"

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);

    u16 steps[7]        = {2, 5, 10, 20, 40, 80, 160};
    u8  orientations[4] = {
            ORIENTATION_PORTRAIT,
            ORIENTATION_LANDSCAPE,
            ORIENTATION_PORTRAIT_MIRROR,
            ORIENTATION_LANDSCAPE_MIRROR
    };

    for (int orient = 0; orient < 4; ++orient) {
        LCD_setOrientation(orientations[orient]);
        for (int i = 0; i < 7; ++i) {
            TEST_fillPrimitives(steps[i]);
            delay_ms(1000);
        }
    }
//    LCD_fillScreen(BLACK);
//
//    LCD_setCursor(0, 0);
//    LCD_write('5');
//
//    LCD_writeString("Init array\n");
//
//    u16 cnt = 4;
//    u8  pixelComponents[cnt];
//
//    for (int i = 0; i < cnt; ++i) {
//        pixelComponents[i] = 0;
//    }
//
//    LCD_writeString("GetRect\n");
//    ili9341_color_t color = LCD_readPixel();
//    LCD_writeString("Ready\n");

    while (1);
}
