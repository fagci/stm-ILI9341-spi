#ifndef TEST2_CONTROL_H
#define TEST2_CONTROL_H

#include "config.h"
#include "commands.h"
#include "../usart/usart.h"

// <editor-fold desc="Functions">
#if DEBUG_MODE

#define TFT_DC_SET      GPIO_SetBits(GPIOA, TFT_DC_PIN); usartPrintLn("DC+");
#define TFT_DC_RESET    GPIO_ResetBits(GPIOA, TFT_DC_PIN); usartPrintLn("DC-");

#define TFT_RST_SET     GPIO_SetBits(GPIOA, TFT_RESET_PIN);usartPrintLn("RST+");
#define TFT_RST_RESET   GPIO_ResetBits(GPIOA, TFT_RESET_PIN);usartPrintLn("RST-");

#define TFT_CS_SET      GPIO_SetBits(GPIOA, TFT_CS_PIN);usartPrintLn("CS+");
#define TFT_CS_RESET    GPIO_ResetBits(GPIOA, TFT_CS_PIN);usartPrintLn("CS-");

#define TFT_LED_SET      GPIO_SetBits(GPIOA, TFT_LED_PIN);usartPrintLn("LED+");
#define TFT_LED_RESET    GPIO_ResetBits(GPIOA, TFT_LED_PIN);usartPrintLn("LED-");

#else

#define TFT_DC_SET      GPIO_SetBits(GPIOA, TFT_DC_PIN);
#define TFT_DC_RESET    GPIO_ResetBits(GPIOA, TFT_DC_PIN);

#define TFT_RST_SET     GPIO_SetBits(GPIOA, TFT_RESET_PIN);
#define TFT_RST_RESET   GPIO_ResetBits(GPIOA, TFT_RESET_PIN);

#define TFT_CS_SET      GPIO_SetBits(GPIOA, TFT_CS_PIN);
#define TFT_CS_RESET    GPIO_ResetBits(GPIOA, TFT_CS_PIN);

#define TFT_LED_SET      GPIO_SetBits(GPIOA, TFT_LED_PIN);
#define TFT_LED_RESET    GPIO_ResetBits(GPIOA, TFT_LED_PIN);

#endif
// </editor-fold>

#endif //TEST2_CONTROL_H
