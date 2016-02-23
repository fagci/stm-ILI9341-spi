#include "core.h"


#if SPI_DMA_MODE

#include "dma.h"

#endif

#ifndef TEST1_ILI9341_H
#define TEST1_ILI9341_H

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#define abs(a) ((a)<0?-(a):a)

void LCD_init();

void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2);

void LCD_fillScreen(u16 color);
void LCD_fillRect2(u16 x1, u16 y1, u16 w, u16 h, u16 color);

void LCD_setOrientation(u8 o);
void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color);
void LCD_putPixel(u16 x, u16 y, u16 color);
void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color);
void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color);

void LCD_drawCircle(u16 x0, u16 y0, u16 r, u16 color);
void LCD_fillCircle(u16 x0, u16 y0, u16 r, u16 color);
void LCD_fillCircleHelper(u16 x0, u16 y0, u16 r, u8 cornername, s16 delta, u16 color);

void LCD_drawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
void LCD_drawRect(u16 x, u16 y, u16 w, u16 h, u16 color);

u16 LCD_getWidth();
u16 LCD_getHeight();

#endif //TEST1_ILI9341_H
