#include "ILI9341_dma_core.h"

#include "ILI9341_commands.h"
#include "ILI9341_colors.h"
#include "spi_init.h"
#include "delay.h"

static const uint8_t init_commands[] = {
        4, 0xEF, 0x03, 0x80, 0x02,
        4, 0xCF, 0x00, 0XC1, 0X30,
        5, 0xED, 0x64, 0x03, 0X12, 0X81,
        4, 0xE8, 0x85, 0x00, 0x78,
        6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
        2, 0xF7, 0x20,
        3, 0xEA, 0x00, 0x00,
        2, ILI9341_PWCTR1, 0x23, // Power control
        2, ILI9341_PWCTR2, 0x10, // Power control
        3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
        2, ILI9341_VMCTR2, 0x86, // VCM control2
        2, ILI9341_MADCTL, 0x48, // Memory Access Control
        2, ILI9341_PIXFMT, 0x55,
        3, ILI9341_FRMCTR1, 0x00, 0x18,
        4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
        2, 0xF2, 0x00, // Gamma Function Disable
        2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
        16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
        0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
        16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
        0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
        0
};

uint8_t SPI_work = 0;

// RX
void DMA1_Channel2_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC2) == SET) {
        DMA_ClearITPendingBit(DMA1_IT_TC2);
        //TFT_CS_RESET;
        SPI_work = 0;
    }
}

// TX
void DMA1_Channel3_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC3) == SET) {
        DMA_ClearITPendingBit(DMA1_IT_TC3);
    }
}

void pins_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void LCD_DMA_Tx(uint16_t size) {
    DMA_SetCurrDataCounter(DMA1_Channel2, size);
    DMA_SetCurrDataCounter(DMA1_Channel3, size);

    SPI_work = 1;
    TFT_CS_SET;

    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void LCD_DMA_Wait(void) {
    while (SPI_work);
}

void LCD_SendCommand(u8 com) {
    TFT_DC_RESET;       // прижимаем вывод DC LCD
    TFT_CS_RESET;            // тоже с выводом CS, CS это выбор кристалла, используется при нескольких устройствах на одном SPI
    SPI1->DR = com;            // отправляем байт в SPI (на отправку) ! изменить под свой МК,
    TFT_CS_SET;         // отпускаем выбор (кристалла) LCD, (PS. CS - позволяет посадить на одну шину SPI несколько устройств)
}

int cb = 0;

void write8_cont(uint8_t b) {
    SPI_DMA_Write((uint16_t) cb++, b);
    if (cb >= 8) {
        cb = 0;
        LCD_DMA_Tx(8);
        LCD_DMA_Wait();
    }
}

void write8_last(uint8_t b) {
    SPI_DMA_Write((uint16_t) cb, b);
    cb = 0;
    LCD_DMA_Tx(8);
    LCD_DMA_Wait();
}

void write16_cont(uint16_t d) {
    write8_cont(d >> 8);
    write8_cont(d & 0xFF);
}

#define setDCForCommand TFT_DC_RESET
#define setDCForData TFT_DC_SET

void writecommand_cont(uint8_t c) {
    TFT_DC_RESET;
    TFT_CS_RESET;
    write8_cont(c);
}

void writedata8_cont(uint8_t c) {
    TFT_DC_SET;                    // для понимания см.дефайн
    TFT_CS_RESET;
    write8_cont(c);
}

void writecommand_last(uint8_t c) {
    TFT_DC_RESET;
    TFT_CS_RESET;
    write8_last(c);
    TFT_CS_SET;
}


void setAddrAndRW_cont(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    setDCForCommand;
    write8_cont(ILI9341_CASET); // Column addr set
    setDCForData;
    write16_cont(x);   // XSTART
    write16_cont((uint16_t) (x + w - 1));   // XEND
    setDCForCommand;
    write8_cont(ILI9341_PASET); // Row addr set
    setDCForData;
    write16_cont(y);   // YSTART
    write16_cont((uint16_t) (y + h - 1));   // YEND
    setDCForCommand;
    write8_cont(ILI9341_RAMWR); // RAM write
}

void LCD_DMA_Init(void) {
    pins_init();
    SPI_init_master();

    TFT_RST_SET;
    delay_ms(5);
    TFT_RST_RESET;
    delay_ms(20);
    TFT_RST_SET;
    delay_ms(150);

    const uint8_t *addr = init_commands;
    while (1) {
        uint8_t count = *(addr++);
        if (count-- == 0) break;
        writecommand_cont(*(addr++));
        while (count-- > 0) {
            writedata8_cont(*(addr++));
        }
    }

    writecommand_last(ILI9341_SLPOUT);    // Exit Sleep
    delay_ms(120);
    writecommand_last(ILI9341_DISPON);    // Display on
    delay_ms(120);
    TFT_LED_SET;
}

void LCD_DMA_Fill(uint16_t color) {
    TFT_CS_RESET;
    setAddrAndRW_cont(0, 0, LCD_WIDTH, LCD_HEIGHT);
    setDCForData;
    for (uint32_t l = 0; l < 400; l++) {
        write16_cont(color);
    }
    TFT_CS_SET;
}
