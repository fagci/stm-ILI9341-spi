#ifndef TEST1_ILI9341_CORE_H
#define TEST1_ILI9341_CORE_H

#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include "delay.h"
#include "commands.h"
#include "dma.h"

#define LCD_setAddressWindowToWrite(x1,y1,x2,y2) \
    LCD_setAddressWindow(x1, y1, x2, y2); \
    dmaSendCmd(LCD_GRAM)

#define LCD_setAddressWindowToRead(x1,y1,x2,y2) \
    LCD_setAddressWindow(x1, y1, x2, y2); \
    dmaSendCmd(LCD_RAMRD)

u16 LCD_getWidth();
u16 LCD_getHeight();

void LCD_init();

void LCD_setSpi8(void);
void LCD_setSpi16(void);

void LCD_setOrientation(u8 o);
void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2);

#endif //TEST1_ILI9341_CORE_H
