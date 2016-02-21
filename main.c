#include <stm32f10x_dma.h>
#include "USART_lib/usart.h"
#include "ILI9341_lib/graph.h"

u8 dmaWorking = 0;

void dmaSend(u8 *data, u32 n) {
    // DMA1
    // TX channel 3
    // RX channel 2

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef dma;

    // TX
    DMA_StructInit(&dma);

    dma.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dma.DMA_MemoryBaseAddr     = (u32) data;
    dma.DMA_DIR                = DMA_DIR_PeripheralDST;
    dma.DMA_BufferSize         = n;
    dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode               = DMA_Mode_Normal;
    dma.DMA_Priority           = DMA_Priority_High;
    DMA_Init(DMA1_Channel3, &dma);

    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

    dmaWorking = 1;
    TFT_CS_RESET;
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void dmaSend16(u16 *data, u32 n) {
    // DMA1
    // TX channel 3
    // RX channel 2

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef dma;

    // TX
    DMA_StructInit(&dma);

    dma.DMA_PeripheralBaseAddr = (u32) &(SPI1->DR);
    dma.DMA_MemoryBaseAddr     = (u32) data;
    dma.DMA_DIR                = DMA_DIR_PeripheralDST;
    dma.DMA_BufferSize         = n;
    dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode               = DMA_Mode_Normal;
    dma.DMA_Priority           = DMA_Priority_High;
    DMA_Init(DMA1_Channel3, &dma);

    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

    dmaWorking = 1;
    TFT_CS_RESET;
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void dmaWait() {
    while(dmaWorking);
}

// TX
void DMA1_Channel3_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC3)) {
        dmaWorking = 0;
        TFT_CS_SET;
        usartSend("DMA_TX\r\n");
        DMA_ClearITPendingBit(DMA1_IT_TC3);
        DMA_Cmd(DMA1_Channel3, DISABLE);
    }
}

void dmaSendCmd(u8 cmd) {
    TFT_DC_RESET;
    dmaSend(&cmd, 1);
    dmaWait();
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

int main(void) {
    usartInit();
    usartSendString("Start\r\n");
    LCD_init();
    usartSendString("LCD init\r\n");
    LCD_setOrientation(ORIENTATION_LANDSCAPE);
    usartSendString("Orientation\r\n");
    LCD_fillScreen(GREEN);
    usartSendString("Filled.\r\n");

    usartSendString("Filling with dma...\r\n");


    u8 x1 = 10, y1 = 10, x2 = 100, y2 = 100;

    dmaSendCmd(ILI9341_COLUMN_ADDR);
    u8 pointData1[] = {y1 >> 8, (u8) (y1 & 0xFF), y2 >> 8, (u8) (y2 & 0xFF)};
    dmaSendData8(pointData1, 4);

    dmaSendCmd(ILI9341_PAGE_ADDR);
    u8 pointData2[] = {x1 >> 8, (u8) (x1 & 0xFF), x2 >> 8, (u8) (x2 & 0xFF)};
    dmaSendData8(pointData2, 4);

    dmaSendCmd(ILI9341_GRAM);

    LCD_setSpi16();
    u32 count = (x2-x1) * (y2-y1);
    u16 colorData[count];
    for (u32 i = count; i < count; i++) {
        colorData[i] = BLACK;
    }
    dmaSendData16(colorData, count);
    LCD_setSpi8();

    usartSendString("Ready?\r\n");

    while (1);
}
