#include <stm32f10x_dma.h>
#include "core.h"
#include "dma.h"

u8 dmaWorking = 0;
DMA_InitTypeDef dmaStructure;

#if __DEBUG_LEVEL > 2
#define dmaWait() usartSendString("(wait) "); while(dmaWorking);
#else
#define dmaWait() while(dmaWorking);
#endif

void dmaInit(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_StructInit(&dmaStructure);
    dmaStructure.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dmaStructure.DMA_Priority           = DMA_Priority_Medium;

    // TX
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

    // RX
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
}

#define always_read(x) asm(""::"r"(x))

static void dmaStartRx() {
    DMA_Init(DMA1_Channel2, &dmaStructure);
    dmaWorking = 1;
    DMA_Cmd(DMA1_Channel2, ENABLE);
}

static void dmaStartTx() {
    while (!(SPI1->SR & SPI_SR_TXE)); // Wait for bus free
    while (SPI1->SR & SPI_SR_BSY);
    always_read(SPI1->DR); // Clear RX flags
    always_read(SPI1->SR);
    DMA_Init(DMA1_Channel3, &dmaStructure);
    dmaWorking = 1;
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

static void dmaReceive8(u8 *data, u32 n) {
    dmaStructure.DMA_MemoryBaseAddr = (u32) data;
    dmaStructure.DMA_BufferSize     = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

    dmaStartRx();
}

static void dmaSend8(u8 *data, u32 n) {
    dmaStructure.DMA_MemoryBaseAddr = (u32) data;
    dmaStructure.DMA_BufferSize     = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

    dmaStartTx();
}

static void dmaSendCircular16(u16 *data, u32 n) {
    dmaStructure.DMA_MemoryBaseAddr = (u32) data;
    dmaStructure.DMA_BufferSize     = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Circular;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    dmaStartTx();
}

static void dmaSend16(u16 *data, u32 n) {
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
    TFT_CS_RESET;
#if __DEBUG_LEVEL > 3
    usartSendString("$ 0x");
    usartWrite(cmd, 16);
    usartSendString(" $ ");
#endif
    dmaSend8(&cmd, 1);
    dmaWait();
    TFT_CS_SET;
}

void dmaSendCmdCont(u8 cmd) {
    TFT_DC_RESET;
#if __DEBUG_LEVEL > 3
    usartSendString("$ 0x");
    usartWrite(cmd, 16);
    usartSendString(" $ ");
#endif
    dmaSend8(&cmd, 1);
    dmaWait();
}

void dmaSendSomeCont(u8 cmd) {
#if __DEBUG_LEVEL > 3
    usartSendString("$ 0x");
    usartWrite(cmd, 16);
    usartSendString(" $ ");
#endif
    dmaSend8(&cmd, 1);
    dmaWait();
}


void dmaReceiveData8(u8 *data) {
    u8 dummy = 0xFF;
    dmaSend8(&dummy, 1);
    dmaReceive8(data, 1);
    dmaWait();
}

void dmaSendData8(u8 *data, u32 n) {
    TFT_DC_SET;
    TFT_CS_RESET;
    dmaSend8(data, n);
    dmaWait();
    TFT_CS_SET;
}

void dmaSendDataCont8(u8 *data, u32 n) {
    TFT_DC_SET;
    dmaSend8(data, n);
    dmaWait();
}

void dmaSendData16(u16 *data, u32 n) {
    TFT_DC_SET;
    TFT_CS_RESET;
    dmaSend16(data, n);
    dmaWait();
    TFT_CS_SET;
}

void dmaSendDataCont16(u16 *data, u32 n) {
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
    TFT_CS_RESET;
    while (n != 0) {
        u16 ts = (u16) (n > UINT16_MAX ? UINT16_MAX : n);
        dmaSendDataCircular16(&color, ts);
        n -= ts;
    }
    TFT_CS_SET;
}

void DMA1_Channel2_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC2) != RESET) {
        DMA_ClearFlag(DMA1_FLAG_TC2);
        DMA_Cmd(DMA1_Channel2, DISABLE);
#if __DEBUG_LEVEL > 2
        usartSendString("{R_IRQ}\r\n");
#endif
        dmaWorking = 0;
    }
}

void DMA1_Channel3_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC3) != RESET) {
        DMA_ClearFlag(DMA1_FLAG_TC3);
        DMA_Cmd(DMA1_Channel3, DISABLE);
#if __DEBUG_LEVEL > 2
        usartSendString("{T_IRQ}\r\n");
#endif
        dmaWorking = 0;
    }
}
