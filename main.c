#include <stm32f10x_dma.h>
#include "ILI9341_lib/text.h"

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(BLACK);

    LCD_setCursor(0, 0);
    LCD_write('5');

    LCD_writeString("Init array\n");

    u16 cnt = 4;
    u8  pixelComponents[cnt];

    for (int i = 0; i < cnt; ++i) {
        pixelComponents[i] = 0;
    }

    LCD_writeString("GetRect\n");
    LCD_getRect(pixelComponents, 0, 0, 1, 1);
    LCD_writeString("Ready\n");


    while (1);
}
