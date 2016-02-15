#include <stm32f10x.h>
#include "FONT/font.h"

#ifndef TEST1_ILI9341_CORE_H

#define TEST1_ILI9341_CORE_H

#define TFT_LED_PIN     GPIO_Pin_0
#define TFT_RESET_PIN   GPIO_Pin_2
#define TFT_DC_PIN      GPIO_Pin_3
#define TFT_CS_PIN      GPIO_Pin_4

#define TFT_DC_SET      GPIO_SetBits(GPIOA, TFT_DC_PIN);
#define TFT_DC_RESET    GPIO_ResetBits(GPIOA, TFT_DC_PIN);

#define TFT_RST_SET     GPIO_SetBits(GPIOA, TFT_RESET_PIN);
#define TFT_RST_RESET   GPIO_ResetBits(GPIOA, TFT_RESET_PIN);

#define TFT_CS_SET      GPIO_SetBits(GPIOA, TFT_CS_PIN);
#define TFT_CS_RESET    GPIO_ResetBits(GPIOA, TFT_CS_PIN);

#define TFT_LED_SET      GPIO_SetBits(GPIOA, TFT_LED_PIN);
#define TFT_LED_RESET    GPIO_ResetBits(GPIOA, TFT_LED_PIN);

#define LCD_WIDTH       240
#define LCD_HEIGHT      320
#define LCD_PIXEL_COUNT    LCD_WIDTH * LCD_HEIGHT

void LCD_Init(void);

void LCD_DMA_Init(void);

void LCD_Fill(uint16_t color);

void LCD_WriteString(unsigned char x0, unsigned int y0, unsigned char *s, unsigned int color);

void LCD_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

#endif //TEST1_ILI9341_CORE_H
