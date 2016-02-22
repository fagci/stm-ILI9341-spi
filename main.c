#include <stm32f10x_dma.h>
#include <stdlib.h>
#include "ILI9341_lib/graph.h"



int main(void) {
    LCD_init();

//    LCD_setOrientation(ORIENTATION_LANDSCAPE);
    LCD_fillScreen(BLACK);

    for(u32 i = 0; i < 10000; i++) {
        LCD_writeString("Testing testing testing\n");
    }
    while (1);
}
