#include <stm32f10x_dma.h>
#include "USART_lib/usart.h"
#include "ILI9341_lib/text.h"
#include "test_suite/primitives.h"

#include "tiny_stdlib/itoa.h"

int main(void) {
    usartInit();
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);

    u16 testColor = 0xACE0;

    LCD_fillScreen(BLACK);

    u16 px[1];

    LCD_putPixel(50, 50, testColor);
    LCD_readPixels(50, 50, 50, 50, px);

    LCD_fillRect(100, 100, 3, 3, GREEN);

    //TEST_fillPrimitives(20);

    while (1);
}
