#include "core.h"
#include "dma.h"

#ifndef TEST1_ILI9341_H
#define TEST1_ILI9341_H

#ifndef _int16_swap
#define _int16_swap(a, b) { int16_t t = a; a = b; b = t; }
#endif

typedef u16 ili9341_color_t;
typedef s16 ili9341_coord_t;

#define SWAP16(x) (((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))

#define ILI9341_COLOR(r, g, b)\
     SWAP16((((uint16_t)b) >> 3) |\
            ((((uint16_t)g) << 3) & 0x07E0) |\
            ((((uint16_t)r) << 8) & 0xf800))

void LCD_init();

void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_setOrientation(u8 o);

ili9341_color_t LCD_readPixel(void);
void LCD_getRect(u8 *data, u16 x1, u16 y1, u16 w, u16 h);

void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color);
void LCD_fillScreen(u16 color);

void LCD_drawRect(u16 x, u16 y, u16 w, u16 h, u16 color);

void LCD_putPixel(u16 x, u16 y, u16 color);
void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color);
void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color);
void LCD_drawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);

void LCD_drawCircle(u16 x0, u16 y0, u16 r, u16 color);
void LCD_fillCircle(u16 x0, u16 y0, u16 r, u16 color);

u16 LCD_getWidth();
u16 LCD_getHeight();

#endif //TEST1_ILI9341_H
