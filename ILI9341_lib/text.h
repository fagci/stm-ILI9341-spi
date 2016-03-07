#ifndef TEST1_TEXT_H
#define TEST1_TEXT_H

#include "graph.h"
#include "fonts/glcdfont.h"

#define TRANSPARENT_COLOR CYAN

#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))

void LCD_write(unsigned char c);
void LCD_writeString(unsigned char *s);
void LCD_writeString2(unsigned char *string);

void LCD_setCursor(u16 x, u16 y);
void LCD_setTextSize(u8 size);
void LCD_setTextColor(u16 color);
void LCD_setTextBgColor(u16 color);

u16 LCD_getCursorX();
u16 LCD_getCursorY();

#endif //TEST1_TEXT_H
