#include <stm32f10x_dma.h>
#include <sys/cdefs.h>
#include "core.h"
#include "dma.h"

u8 dmaWorking = 0;
#define dmaWait() while(dmaWorking);
DMA_InitTypeDef dmaStructure;

void dmaInit(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_StructInit(&dmaStructure);
    dmaStructure.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dmaStructure.DMA_Priority           = DMA_Priority_High;

    // TX
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

    // RX
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
}

void dmaRecv(u8 *data, u32 n) {

    TFT_DC_SET;
    dmaStructure.DMA_MemoryBaseAddr = (u32) data;
    dmaStructure.DMA_BufferSize     = n;
    DMA_Init(DMA1_Channel2, &dmaStructure);
    dmaWorking = 1;
    TFT_CS_RESET;
    DMA_Cmd(DMA1_Channel2, ENABLE);
    dmaWait();
}

__always_inline static void dmaStartTx() {
    DMA_Init(DMA1_Channel3, &dmaStructure);
    dmaWorking = 1;
    TFT_CS_RESET;
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

__always_inline static void dmaSend8(u8 *data, u32 n) {
    dmaStructure.DMA_MemoryBaseAddr = (u32) data;
    dmaStructure.DMA_BufferSize     = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaStartTx();
}

__always_inline static void dmaSendCircular16(u16 *data, u32 n) {
    dmaStructure.DMA_MemoryBaseAddr = (u32) data;
    dmaStructure.DMA_BufferSize     = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Circular;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dmaStartTx();
}

__always_inline static void dmaSend16(u16 *data, u32 n) {
    dmaStructure.DMA_MemoryBaseAddr = (u32) data;
    dmaStructure.DMA_BufferSize     = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    dmaStartTx();
}

void dmaSendCmd(u8 cmd) {
    TFT_DC_RESET;
    dmaSend8(&cmd, 1);
    dmaWait();
}

void dmaSendData8(u8 *data, u32 n) {
    TFT_DC_SET;
    dmaSend8(data, n);
    dmaWait();
}

void dmaSendData16(u16 *data, u32 n) {
    TFT_DC_SET;
    dmaSend16(data, n);
    dmaWait();
}

void dmaSendDataCircular16(u16 *data, u32 n) {
    TFT_DC_SET;
    dmaSendCircular16(data, n);
    dmaWait();
}

void dmaFill16(u16 color, u32 n) {
    while (n != 0) {
        u16 ts = (u16) (n > UINT16_MAX ? UINT16_MAX : n);
        dmaSendDataCircular16(&color, ts);
        n -= ts;
    }
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
