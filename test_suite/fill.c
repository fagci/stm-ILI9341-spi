#include "fill.h"

void TEST_fill() {
    FPS_start();
    volatile u16           currentFps = 0;
    unsigned char buf[6];

    for (u8 i = 0; i < 30; i++) {
        LCD_setCursor(0, 0);
        LCD_fillScreen(RED);
        FPS_frame();
        currentFps = FPS_getCount();
        itoa(currentFps, buf, 10);
        LCD_writeString(buf);
        LCD_setCursor(0, 0);
        LCD_fillScreen(GREEN);
        FPS_frame();
        currentFps = FPS_getCount();
        itoa(currentFps, buf, 10);
        LCD_writeString(buf);
    }
}
