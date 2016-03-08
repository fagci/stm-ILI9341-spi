#ifndef TEST1_USART_H
#define TEST1_USART_H

#include <stm32f10x_usart.h>
#include "../tiny_stdlib/itoa.h"

void usartWrite(char chr);
void usartInit(void);

void usartPrint(const char *str);
void usartPrintLn(const char *str);
void usartPrintNum(int val, u8 base);

#endif //TEST1_USART_H
