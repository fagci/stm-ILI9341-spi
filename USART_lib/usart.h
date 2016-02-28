#ifndef TEST1_USART_H
#define TEST1_USART_H

#include <stm32f10x_usart.h>
#include "../tiny_stdlib/itoa.h"

void usartSendString(char *str);
void usartSend(char chr);
void usartInit(void);
void usartWrite(int val, u8 base);

#endif //TEST1_USART_H
