#include <stm32f10x_usart.h>

void usartInit(void) {
    GPIO_InitTypeDef  GPIO_InitStruct;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    USART_DeInit(USART3);
    USART_InitStructure.USART_BaudRate            = 9600;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStructure);

//    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//    NVIC_EnableIRQ(USART3_IRQn);

    USART_Cmd(USART3, ENABLE);
}

void usartSend(char chr) {
    while (!(USART3->SR & USART_FLAG_TXE));
    USART3->DR = (uint16_t) (chr & 0xFF);
}

void usartSendString(char *str) {
    int i = 0;
    while (str[i])
        usartSend(str[i++]);
}

//void USART3_IRQHandler(void) {
//    // Обработка события RXNE
//    if (USART_GetITStatus(USART3, USART_IT_RXNE)) {
//        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//        usartSendString("GOT!\r\n");
//    };
//};
