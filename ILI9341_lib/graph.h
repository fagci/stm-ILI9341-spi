#include "core.h"


#if SPI_DMA_MODE

#include "dma.h"

#endif

#ifndef TEST1_ILI9341_H
#define TEST1_ILI9341_H

#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))


#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#define abs(a) ((a)<0?-(a):a)

void LCD_init();
void LCD_fillScreen(u16 color);
void LCD_setOrientation(u8 o);
void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color);
void LCD_putPixel(u16 x, u16 y, u16 color);
void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color);
void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color);

void LCD_drawCircle(int16_t x0, int16_t y0, int16_t r, u16 color);
void LCD_fillCircle(int16_t x0, int16_t y0, int16_t r, u16 color);
void LCD_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, u8 cornername, int16_t delta, u16 color);

void LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, u16 color);
void LCD_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, u16 color);

void LCD_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void LCD_write(unsigned char c);

#endif //TEST1_ILI9341_H
