#ifndef TEST1_DMA_H
#define TEST1_DMA_H

#include "control.h"
#include <stm32f10x_dma.h>

void dmaInit(void);

void dmaSendCmd(u8 cmd);
void dmaSendCmdCont(u8 cmd);
void dmaSendSomeCont(u8 cmd);

void dmaReceiveData8(u8 *data);

void dmaSendData8(u8 *data, u32 n);
void dmaSendData16(u16 *data, u32 n);

void dmaSendDataCont8(u8 *data, u32 n);
void dmaSendDataCont16(u16 *data, u32 n);

void dmaFill16(u16 color, u32 n);

#endif //TEST1_DMA_H
