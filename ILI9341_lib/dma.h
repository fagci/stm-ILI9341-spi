#include "config.h"
#include <stm32f10x.h>
#include <stm32f10x_dma.h>

#ifndef TEST1_DMA_H
#define TEST1_DMA_H

void dmaInit(void);
void dmaSendCmd(u8 cmd);
void dmaSendData8(u8 *data, u16 n);
void dmaSendData16(u16 *data, u16 n);

#endif //TEST1_DMA_H
