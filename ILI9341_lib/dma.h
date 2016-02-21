#include <stm32f10x.h>
#include <stm32f10x_dma.h>

#ifndef TEST1_DMA_H
#define TEST1_DMA_H

#define DMA_BUFFER_SIZE 1024

void dmaInit(void);
void LCD_dmaSendCommand8(u8 cmd);
void LCD_dmaSendData8(u8 data);
void LCD_dmaSendData16(u16 data);

#endif //TEST1_DMA_H
