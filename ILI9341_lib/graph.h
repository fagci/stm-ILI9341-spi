#ifndef TEST1_ILI9341_H
#define TEST1_ILI9341_H

#include "core.h"
#include "dma.h"

#ifndef _int16_swap
#define _int16_swap(a, b) { int16_t t = a; a = b; b = t; }
#endif

#ifndef abs
#define abs(a) ((a)<0?-(a):a)
#endif

#define ILI9341_COLOR(r, g, b)\
     ((((uint16_t)b) >> 3) |\
            ((((uint16_t)g) << 3) & 0x07E0) |\
            ((((uint16_t)r) << 8) & 0xf800))

void LCD_readPixels(u16 x1, u16 y1, u16 x2, u16 y2, u16 *buf);

void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color);
void LCD_fillScreen(u16 color);

void LCD_drawRect(u16 x, u16 y, u16 w, u16 h, u16 color);

void LCD_putPixel(u16 x, u16 y, u16 color);
void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color);
void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color);
void LCD_drawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);

void LCD_drawCircle(u16 x0, u16 y0, u16 r, u16 color);
void LCD_fillCircle(u16 x0, u16 y0, u16 r, u16 color);

void LCD_setVerticalScrolling(u16 startY, u16 endY);
void LCD_scroll(u16 v);

#endif //TEST1_ILI9341_H
