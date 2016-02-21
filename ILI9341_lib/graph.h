#include "core.h"

#ifndef TEST1_ILI9341_H
#define TEST1_ILI9341_H

void LCD_init();
void LCD_fillScreen(u16 color);
void LCD_setOrientation(u8 o);
void LCD_putPixel(u16 x, u16 y, u16 color);

#endif //TEST1_ILI9341_H
