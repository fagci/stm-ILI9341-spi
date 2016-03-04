#ifndef TEST2_CONTROL_H
#define TEST2_CONTROL_H

#include "config.h"
#include "commands.h"

// <editor-fold desc="Functions">
#define TFT_DC_SET      GPIO_SetBits(GPIOA, TFT_DC_PIN);
#define TFT_DC_RESET    GPIO_ResetBits(GPIOA, TFT_DC_PIN);

#define TFT_RST_SET     GPIO_SetBits(GPIOA, TFT_RESET_PIN);
#define TFT_RST_RESET   GPIO_ResetBits(GPIOA, TFT_RESET_PIN);

#define TFT_CS_SET      GPIO_SetBits(GPIOA, TFT_CS_PIN);
#define TFT_CS_RESET    GPIO_ResetBits(GPIOA, TFT_CS_PIN);

#define TFT_LED_SET      GPIO_SetBits(GPIOA, TFT_LED_PIN);
#define TFT_LED_RESET    GPIO_ResetBits(GPIOA, TFT_LED_PIN);
// </editor-fold>

#endif //TEST2_CONTROL_H
