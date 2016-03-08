#include "lib/ILI9341/text.h"
#include "lib/tinystd/itoa.h"
#include "lib/encoder/encoder.h"

int main(void) {
    LCD_init();
    ENC_init();
    LCD_setOrientation(ORIENTATION_PORTRAIT);
    LCD_fillScreen(BLACK);

    char buf[16];
    LCD_setTextBgColor(BLACK);
    LCD_setTextSize(3);
    u16 ov = 111;
    u16 encVal;
    while (1) {
        if (ov != (encVal = ENC_getValue())) {
            LCD_setCursor(0, 0);
            itoa(encVal, buf, 10);
            LCD_fillRect(0, 0, 100, 24, BLACK);
            LCD_writeString(buf);
            ov = encVal;
        }
        delay_ms(10);
    }

    while (1);
}

