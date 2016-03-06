#include "ILI9341_lib/graph.h"

int main(void) {
    u16 px[200];
    u16 testColor = 0xACE0;

    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);

    for (u8 i = 0; i < 10; i++) {
        LCD_fillScreen(GREEN);
        LCD_fillScreen(BLACK);
        LCD_drawCircle(10, 10, 5, testColor);


        LCD_readPixels(4, 4, 16, 16, px);

        LCD_fillScreen(RED);

        u16 d = 13;

        for (u16 x = 0; x < d; ++x) {
            for (u16 y = 0; y < d; ++y) {
                LCD_putPixel(x, y, px[y * d + x]);
            }
        }
    }

    while (1);
}
