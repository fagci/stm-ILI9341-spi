#include "ILI9341_lib/text.h"
#include "test_suite/fps_counter.h"
#include "test_suite/primitives.h"

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);

    LCD_fillScreen(BLACK);

    TEST_fillPrimitives(160);

    LCD_setCursor(0, 0);
    LCD_setTextColor(GREEN);
    LCD_setTextBgColor(TRANSPARENT_COLOR);

    LCD_setTextSize(1);

    FPS_start();

    u8 t = 0;

    u16 colors[] = {RED, GREEN, BLUE, OLIVE, DGRAY};

    u16 h = LCD_getHeight();
    u16 w = LCD_getWidth();

    u16 dh = h - 8;

    u8 ci                = 0;

    unsigned char      ca[255 - '!' + 1];
    for (unsigned char i = '!'; i < 255; ++i) {
        ca[ci++] = i;
    }
    ca[ci]               = '\0';

    while (1) {
        LCD_setCursor(0, 0);
        LCD_setTextColor(colors[t]);
        for (int j = 0; j < 6; ++j) {
            LCD_writeString(ca);
        }
        FPS_frame();
        LCD_fillRect(0, dh, 40, 8, BLACK);
        LCD_setCursor(0, dh);
        LCD_writeString(FPS_getText());
        t = (u8) (++t % 5);
    }
    while (1);
}
