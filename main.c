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
    LCD_fillScreen(testColor);

    u16 px[81];

    LCD_fillCircle(3, 3, 3, RED);

    LCD_readPixels(0, 0, 8, 8, px);

    for (int i = 0; i < 81; ++i) {
        LCD_putPixel(9 + i % 9, i / 9, px[i]);
    }

    LCD_readPixels(9, 0, 17, 8, px);

    for (int i = 0; i < 81; ++i) {
        LCD_putPixel(17 + i % 9, i / 9, px[i]);
    }

    //TEST_fillPrimitives(20);

    while (1);
}
