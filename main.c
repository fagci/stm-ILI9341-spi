#include <stm32f10x_dma.h>
#include "ILI9341_lib/graph.h"

int main(void) {
    LCD_init();

//    LCD_setOrientation(ORIENTATION_LANDSCAPE);
    LCD_fillScreen(BLACK);

    for (u16 x = 0; x < LCD_WIDTH; x += 20) {
        LCD_drawFastVLine(x, 0, LCD_HEIGHT, WHITE);
    }
    for (u16 y = 0; y < LCD_HEIGHT; y += 20) {
        LCD_drawFastHLine(0, y, LCD_WIDTH, WHITE);
        LCD_putPixel(10, y, GREEN);
    }

    for (u16 i = 0; i < 10; i++) {
        LCD_fillRect(i * 20 + 1, i * 20 + 1, 19, 19, WHITE);
        LCD_drawCircle(i * 20 + 10, i * 20 + 10, 9, GREEN);
        LCD_fillCircle(i * 20 + 10, i * 20 + 10, 5, GREEN);
        LCD_drawChar(i * 20 + 10, i * 20 + 10, i + 'a', BLUE,WHITE,1);
    }

    LCD_drawLine(0, 0, 140, 60, RED);
    LCD_drawRect(10,10,40,40,LGRAY);

    LCD_putPixel(20,80,RED);

    while (1);
}
