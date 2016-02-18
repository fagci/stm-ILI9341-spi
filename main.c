#include "ILI9341_lib/ILI9341.h"

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE);
    LCD_fillScreen(GREEN);

    u16 step = LCD_WIDTH / 40;

    for (u16 x = 0; x < LCD_WIDTH; x += step * 2) {
        for (u16 y = 0; y < LCD_HEIGHT; y += step * 2) {
            LCD_fillRect(x, y, step, step, BLACK);
        }
    }

    while (1);
}
