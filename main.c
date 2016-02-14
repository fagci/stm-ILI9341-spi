#include <stm32f10x.h>
#include <stdlib.h>
#include "lib/ILI9341_core.h"
#include "lib/ILI9341_colors.h"

int main(void) {
    TFT_LED_SET;

    LCD_Init();
    LCD_Fill(BLACK);
    LCD_WriteString(0, 0, "Hallo, world!", YELLOW);
    while (1);
}
