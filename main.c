#include <stdlib.h>
#include "ILI9341_lib/ILI9341.h"

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE);
    LCD_fillScreen(GREEN);

//    for (u16 x = 0; x < 25; x ++) {
//        LCD_fillScreen(BLACK);
//        LCD_fillScreen(WHITE);
//    }

    while (1);
}
