#include <stm32f10x_dma.h>
#include "USART_lib/usart.h"
#include "ILI9341_lib/graph.h"

int main(void) {
    u16 px[1];
    u16 testColor = 0xACE0;

    LCD_init();
    usartInit();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);

    usartPrintLn("BEGIN");

    LCD_fillScreen(BLACK);
    LCD_putPixel(50, 50, testColor);
    LCD_readPixels(50, 50, 50, 50, px);

    if(px[0] == testColor) usartPrintLn("READ OK");

    usartPrint("0x");
    usartPrintNum(px[0], 16);
    usartPrintLn("");

    LCD_fillScreen(RED);

    usartPrintLn("END");

    while (1);
}
