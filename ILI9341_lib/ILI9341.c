#include "ILI9341.h"

SPI_InitTypeDef SPI_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

uint32_t dmaTxBuf[DMA_TX_BUF_SIZE];

//<editor-fold desc="Init commands">

static const uint8_t init_commands[] = {
        4, 0xEF, 0x03, 0x80, 0x02,
        4, ILI9341_POWERB, 0x00, 0XC1, 0X30,
        5, ILI9341_POWER_SEQ, 0x64, 0x03, 0X12, 0X81,
        4, ILI9341_DTCA, 0x85, 0x00, 0x78,
        6, ILI9341_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
        2, ILI9341_PRC, 0x20,
        3, ILI9341_DTCB, 0x00, 0x00,
        2, ILI9341_POWER1, 0x23, // Power control
        2, ILI9341_POWER2, 0x10, // Power control
        3, ILI9341_VCOM1, 0x3e, 0x28, // VCM control
        2, ILI9341_VCOM2, 0x86, // VCM control2
        2, ILI9341_MAC, 0x48, // Memory Access Control
        2, ILI9341_PIXEL_FORMAT, 0x55,
        3, ILI9341_FRC, 0x00, 0x18,
        4, ILI9341_DFC, 0x08, 0x82, 0x27, // Display Function Control
        2, ILI9341_3GAMMA_EN, 0x00, // Gamma Function Disable
        2, ILI9341_GAMMA, 0x01, // Gamma curve selected
        16, ILI9341_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
        16, ILI9341_NGAMMA, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
        0
};

//</editor-fold>

//<editor-fold desc="Generic SPI operations">

void LCD_sendCommand8(u8 cmd) {
    TFT_DC_RESET;
    TFT_CS_RESET;
    SPI1->DR = cmd;
    TFT_CS_SET;
}

void LCD_sendData8(u8 data) {
    TFT_DC_SET;
    TFT_CS_RESET;
    SPI1->DR = data;
    TFT_CS_SET;
}

void LCD_sendData16(u16 data) {
    TFT_DC_SET;
    TFT_CS_RESET;
    SPI1->DR = data;
    TFT_CS_SET;
}

void LCD_allocateField(u16 x1, u16 y1, u16 x2, u16 y2) {
    LCD_sendCommand8(ILI9341_COLUMN_ADDR);
    LCD_sendData8((u8) (y1 >> 8));
    LCD_sendData8((u8) (y1 & 0xFF));
    LCD_sendData8((u8) (y2 >> 8));
    LCD_sendData8((u8) (y2 & 0xFF));

    LCD_sendCommand8(ILI9341_PAGE_ADDR);
    LCD_sendData8((u8) (x1 >> 8));
    LCD_sendData8((u8) (x1 & 0xFF));
    LCD_sendData8((u8) (x2 >> 8));
    LCD_sendData8((u8) (x2 & 0xFF));

    LCD_sendCommand8(ILI9341_GRAM);
}

void LCD_setSpi16(void) {
    SPI1->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI1->CR1 |= SPI_CR1_DFF;  // SPI 16
    SPI1->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

void LCD_setSpi8(void) {
    SPI1->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI1->CR1 &= ~SPI_CR1_DFF; // SPI 8
    SPI1->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

// </editor-fold>

void LCD_spiInit() {
    RCC_PCLK2Config(RCC_HCLK_Div2);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | SPI_MASTER_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // GPIO for CS/DC/LED/RESET
    GPIO_InitStructure.GPIO_Pin   = TFT_CS_PIN | TFT_DC_PIN | TFT_RESET_PIN | TFT_LED_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO for SPI
    GPIO_InitStructure.GPIO_Pin   = SPI_MASTER_PIN_NSS | SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI_MASTER, &SPI_InitStructure);

    SPI_CalculateCRC(SPI_MASTER, DISABLE);

    SPI_SSOutputCmd(SPI_MASTER, ENABLE);
    SPI_Cmd(SPI_MASTER, ENABLE);
}

void LCD_dmaInit() {
    // TX
    DMA_DeInit(DMA1_Channel3); //Set DMA registers to default values
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &SPI1->DR; //Address of peripheral the DMA must map to
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t) dmaTxBuf; //Variable from which data will be transmitted
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = DMA_TX_BUF_SIZE; //Buffer size
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel3, ENABLE);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

    SPI_I2S_DMACmd(SPI_MASTER, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
}

void LCD_configure() {
    TFT_RST_SET;
    LCD_sendCommand8(ILI9341_RESET);
    delay_ms(100);

    const uint8_t *address = init_commands;
    while (1) {
        u8 count = *(address++);
        if (count-- == 0) break;
        LCD_sendCommand8(*(address++));
        while (count--) LCD_sendData8(*(address++));
    }

    LCD_sendCommand8(ILI9341_SLEEP_OUT);
    delay_ms(100);
    LCD_sendCommand8(ILI9341_DISPLAY_ON);
    LCD_sendCommand8(ILI9341_GRAM);
    TFT_LED_SET;
}

void LCD_init() {
    LCD_spiInit();
#if SPI_DMA_MODE
    dmaInit();
#endif
    LCD_configure();
}

void LCD_fillScreen(u16 color) {
    LCD_allocateField(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    LCD_setSpi16();
    for (u32 n = LCD_PIXEL_COUNT; n--;) {
        if (SPI1->SR | SPI_SR_TXE) LCD_sendData16(color);
    }
    LCD_setSpi8();
}

void LCD_setOrientation(u8 o) {
    LCD_sendCommand8(ILI9341_MAC);
    LCD_sendData8(o);
}

void LCD_putPixel(u16 x, u16 y, u16 color) {
    LCD_allocateField(x, y, x, y);
    LCD_setSpi16();
    LCD_sendData16(color);
    LCD_setSpi8();
}
