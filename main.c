#include <stm32f10x_dma.h>
#include "USART_lib/usart.h"
#include "ILI9341_lib/graph.h"

int main(void) {
    u16 px[1];
    u16 testColor = 0xACE0;

    LCD_init();
    usartInit();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);

    usartSendString("BEGIN\r\n");

    LCD_fillScreen(BLACK);
    LCD_putPixel(50, 50, testColor);
    LCD_readPixels(50, 50, 50, 50, px);
    if(px[0] == testColor) usartSendString("READ OK\r\n");
    usartWrite(px[0], 16);
    LCD_fillScreen(RED);

    usartSendString("END\r\n");

    while (1);
}
