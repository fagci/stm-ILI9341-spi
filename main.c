#include "USART_lib/usart.h"
#include "ILI9341_lib/graph.h"

int main(void) {
    usartInit();
    usartSendString("Start\r\n");
    LCD_init();
    usartSendString("LCD init\r\n");
    LCD_setOrientation(ORIENTATION_LANDSCAPE);
    usartSendString("Orientation\r\n");
    LCD_fillScreen(GREEN);
    usartSendString("Fill\r\n");
    while (1);
}
