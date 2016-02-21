#include "graph.h"

void LCD_setAddrWindow(u16 x1, u16 y1, u16 x2, u16 y2) {
    LCD_sendCommand8(ILI9341_COLUMN_ADDR);
    LCD_sendData8((u8) (y1 >> 8));
    LCD_sendData8((u8) (y1 & 0xFF));
    LCD_sendData8((u8) (y2 >> 8));
    LCD_sendData8((u8) (y2 & 0xFF));

    LCD_sendCommand8(ILI9341_PAGE_ADDR);
    LCD_sendData8((u8) (x1 >> 8));
    LCD_sendData8((u8) (x1 & 0xFF));
    LCD_sendData8((u8) (x2 >> 8));
    LCD_sendData8((u8) (x2 & 0xFF));

    LCD_sendCommand8(ILI9341_GRAM);
}

void LCD_fillScreen(u16 color) {
    LCD_setAddrWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    LCD_setSpi16();
    for (u32 n = LCD_PIXEL_COUNT; n--;) {
        if (SPI1->SR | SPI_SR_TXE) LCD_sendData16(color);
    }
    LCD_setSpi8();
}

void LCD_setOrientation(u8 o) {
    LCD_sendCommand8(ILI9341_MAC);
    LCD_sendData8(o);
}

void LCD_putPixel(u16 x, u16 y, u16 color) {
    LCD_setAddrWindow(x, y, x, y);
    LCD_setSpi16();
    LCD_sendData16(color);
    LCD_setSpi8();
}
