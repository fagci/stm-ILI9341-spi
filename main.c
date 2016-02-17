#include "ILI9341_lib/ILI9341.h"

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE);
    LCD_fillScreen(GREEN);
    while (1);
}
