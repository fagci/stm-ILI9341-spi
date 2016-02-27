#include "primitives.h"
#include "../ILI9341_lib/graph.h"

void TEST_fillPrimitives(u16 step) {
    LCD_fillScreen(BLACK);

    u16 halfStep      = (u16) (step / 2),
        quartStep     = (u16) (halfStep / 2),
        halfQuartStep = (u16) (quartStep / 2);

    u16 w = LCD_getWidth(),
        h = LCD_getHeight();

    for (u16 x = 0; x < w; x += step) {
        LCD_drawFastVLine(x, 0, h, DGRAY);
    }

    for (u16 y = 0; y < h; y += step) {
        LCD_drawFastHLine(0, y, w, DGRAY);
    }

    for (u16 x = 0; x < w; x += step) {
        for (u16 y = 0; y < h; y += step) {
            LCD_drawRect(x, y, halfStep, halfStep, DGREEN);
            LCD_fillRect(x + halfStep, y + halfStep, halfStep, halfStep, GREENYELLOW);
            LCD_drawCircle(x + quartStep, y + quartStep, halfQuartStep, GREENYELLOW);
            LCD_fillCircle(x + halfStep + quartStep, y + halfStep + quartStep, halfQuartStep, DGREEN);
            LCD_putPixel(x + quartStep, y + quartStep, YELLOW);

            LCD_drawLine(x + halfStep, y + halfStep, x + step, y + step, WHITE);
        }
    }
}