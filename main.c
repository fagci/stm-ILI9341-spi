#include <stm32f10x_dma.h>
#include <stdlib.h>
#include "ILI9341_lib/graph.h"

int main(void) {
    LCD_init();

    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(BLACK);

    u16 colors[] = {RED, CYAN, MAGENTA, YELLOW, GREEN, DGRAY, DGREEN, WHITE};
    LCD_setTextSize(15);
    unsigned char buf[8];
    for (u16 i = 0; i < 1000; i++) {
        LCD_setCursor(0, 0);
        itoa(i,buf,10);
        LCD_writeString(buf);
    }
    while (1);
}
