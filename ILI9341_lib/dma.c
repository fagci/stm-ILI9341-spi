#include <stm32f10x_dma.h>
#include "dma.h"

u8 dmaWorking = 0;
#define dmaWait() while(dmaWorking);
DMA_InitTypeDef dma8, dma16;

void dmaInit(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA 8bit
    DMA_StructInit(&dma8);
    dma8.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dma8.DMA_DIR                = DMA_DIR_PeripheralDST;
    dma8.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma8.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma8.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dma8.DMA_Priority           = DMA_Priority_High;

    // DMA 16bit
    DMA_StructInit(&dma16);
    dma16.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dma16.DMA_DIR                = DMA_DIR_PeripheralDST;
    dma16.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma16.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma16.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dma16.DMA_Priority           = DMA_Priority_High;

    // IRQs
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
}
u8 repeats = 0;
void dmaFill16(u16 color, u32 n) {

    // DMA 16bit
    DMA_StructInit(&dma16);
    dma16.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dma16.DMA_DIR                = DMA_DIR_PeripheralDST;
    dma16.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma16.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dma16.DMA_Priority           = DMA_Priority_High;
    dma16.DMA_Mode               = DMA_Mode_Circular;


    repeats    = 7;
    dmaSendData16(&color,n);
    dmaInit();
}


void dmaSend(u8 *data, u32 n) {
    dma8.DMA_MemoryBaseAddr = (u32) data;
    dma8.DMA_BufferSize     = n;
    DMA_Init(DMA1_Channel3, &dma8);
    dmaWorking = 1;
    TFT_CS_RESET;
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void dmaSend16(u16 *data, u32 n) {
    dma16.DMA_MemoryBaseAddr = (u32) data;
    dma16.DMA_BufferSize     = n;
    DMA_Init(DMA1_Channel3, &dma16);
    dmaWorking = 1;
    TFT_CS_RESET;
    DMA_Cmd(DMA1_Channel3, ENABLE);
}


void dmaSendData8(u8 *data, u32 n) {
    TFT_DC_SET;
    dmaSend(data, n);
    dmaWait();
}

void dmaSendData16(u16 *data, u32 n) {
    TFT_DC_SET;
    dmaSend16(data, n);
    dmaWait();
}

// TX
void DMA1_Channel3_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC3)) {
        DMA_ClearITPendingBit(DMA1_IT_TC3);
        if (repeats > 0) repeats--; // TODO: Make it precisely
        if (repeats == 0) {
            DMA_Cmd(DMA1_Channel3, DISABLE);
            TFT_CS_SET;
            dmaWorking = 0;
        }
    }
}

void dmaSendCmd(u8 cmd) {
    TFT_DC_RESET;
    dmaSend(&cmd, 1);
    dmaWait();
}



