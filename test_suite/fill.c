#include "fill.h"

void TEST_fill() {
    FPS_start();

    for (u8 i = 0; i < 30; i++) {
        LCD_setCursor(0, 0);
        LCD_fillScreen(RED);
        FPS_frame();
        LCD_writeString(FPS_getText());
        LCD_setCursor(0, 0);
        LCD_fillScreen(GREEN);
        FPS_frame();
        LCD_writeString(FPS_getText());
    }
}