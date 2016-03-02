#include <stm32f10x_dma.h>
#include "USART_lib/usart.h"
#include "ILI9341_lib/text.h"
#include "test_suite/primitives.h"

int main(void) {
    usartInit();
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);

    u16 px[1];
    u16 testColor = 0xACE0;

    LCD_fillScreen(BLACK);

    LCD_putPixel(50, 50, testColor);
    LCD_readPixels(50, 50, 50, 50, px);
    if(px[0] == testColor) usartSendString("READ OK\r\n");
    TEST_fillPrimitives(20);
    usartSendString("END\r\n");

    while (1);
}
