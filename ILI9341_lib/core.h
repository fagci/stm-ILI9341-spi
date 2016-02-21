#include "config.h"
#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include "delay.h"

#ifndef TEST1_ILI9341_CORE_H
#define TEST1_ILI9341_CORE_H

void LCD_sendCommand8(u8 cmd);
void LCD_sendData8(u8 data);
void LCD_sendData16(u16 data);
void LCD_setSpi8(void);
void LCD_setSpi16(void);

#endif //TEST1_ILI9341_CORE_H
