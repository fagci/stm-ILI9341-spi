#include <stm32f10x_dma.h>
#include "core.h"
#include "dma.h"

u8 dmaWorking = 0;
#define dmaWait() while(dmaWorking);
DMA_InitTypeDef dmaTx8, dmaTx16, dmaRx8;

void dmaInit(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // TX
    // DMA 8bit
    DMA_StructInit(&dmaTx8);
    dmaTx8.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dmaTx8.DMA_DIR                = DMA_DIR_PeripheralDST;
    dmaTx8.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaTx8.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaTx8.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaTx8.DMA_Priority           = DMA_Priority_High;

    // DMA 16bit
    DMA_StructInit(&dmaTx16);
    dmaTx16.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dmaTx16.DMA_DIR                = DMA_DIR_PeripheralDST;
    dmaTx16.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaTx16.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dmaTx16.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dmaTx16.DMA_Priority           = DMA_Priority_High;

    // RX
    // DMA 8bit
    DMA_StructInit(&dmaRx8);
    dmaRx8.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dmaRx8.DMA_DIR                = DMA_DIR_PeripheralSRC;
    dmaRx8.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaRx8.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaRx8.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaRx8.DMA_Priority           = DMA_Priority_High;

    // IRQs
    // TX
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

    // RX
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
}

void dmaFill16(u16 color, u32 n) {
    // DMA 16bit
    DMA_StructInit(&dmaTx16);
    dmaTx16.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dmaTx16.DMA_DIR                = DMA_DIR_PeripheralDST;
    dmaTx16.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dmaTx16.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dmaTx16.DMA_Priority           = DMA_Priority_Medium;
    dmaTx16.DMA_Mode               = DMA_Mode_Circular;

    while (n != 0) {
        u16 ts = (u16) (n > UINT16_MAX ? UINT16_MAX : n);
        dmaSendData16(&color, ts);
        n -= ts;
    }
    dmaInit();
}

void dmaSend(u8 *data, u32 n) {
    dmaTx8.DMA_MemoryBaseAddr = (u32) data;
    dmaTx8.DMA_BufferSize     = n;
    DMA_Init(DMA1_Channel3, &dmaTx8);
    dmaWorking = 1;
    TFT_CS_RESET;
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void dmaRecv(u8 *data, u32 n) {
    TFT_DC_SET;
    dmaRx8.DMA_MemoryBaseAddr = (u32) data;
    dmaRx8.DMA_BufferSize     = n;
    DMA_Init(DMA1_Channel2, &dmaRx8);
    dmaWorking = 1;
    TFT_CS_RESET;
    DMA_Cmd(DMA1_Channel2, ENABLE);
    dmaWait();
}



void dmaSend16(u16 *data, u32 n) {
    dmaTx16.DMA_MemoryBaseAddr = (u32) data;
    dmaTx16.DMA_BufferSize     = n;
    DMA_Init(DMA1_Channel3, &dmaTx16);
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
    if (DMA_GetITStatus(DMA1_FLAG_TC3) != RESET) {
        DMA_ClearITPendingBit(DMA1_FLAG_TC3);
        DMA_Cmd(DMA1_Channel3, DISABLE);
        TFT_CS_SET;
        dmaWorking = 0;
    }
}

// RX
void DMA1_Channel2_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_FLAG_TC2) != RESET) {
        DMA_ClearITPendingBit(DMA1_FLAG_TC2);
        DMA_Cmd(DMA1_Channel2, DISABLE);
        TFT_CS_SET;
        dmaWorking = 0;
    }
}

void dmaSendCmd(u8 cmd) {
    TFT_DC_RESET;
    dmaSend(&cmd, 1);
    dmaWait();
}



