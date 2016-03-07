#include "ILI9341_lib/text.h"
#include "test_suite/primitives.h"


int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_PORTRAIT);

    LCD_fillScreen(BLACK);

    TEST_fillPrimitives(40);

    LCD_setVerticalScrolling(20, 60);

    u16 t = 0;

    while (1) {
        if (t == 0) t = LCD_PIXEL_HEIGHT;
        LCD_scroll(t--);
        delay_ms(10);
    }

    while (1);
}
