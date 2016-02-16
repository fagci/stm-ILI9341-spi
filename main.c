#include <stm32f10x.h>
#include <stdlib.h>
#include "lib/ILI9341_dma_core.h"
#include "lib/ILI9341_colors.h"

int main(void) {


    LCD_DMA_Init();
    LCD_DMA_Fill(BLACK);
    //LCD_WriteString(0, 0, "Hallo, world!", YELLOW);
    while (1);
}
