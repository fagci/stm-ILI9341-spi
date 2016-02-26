#include "core.h"
#include "dma.h"

//<editor-fold desc="Init commands">

static const uint8_t init_commands[] = {
//        4, 0xEF, 0x03, 0x80, 0x02,
//        4, LCD_POWERB, 0x00, 0XC1, 0X30,
//        5, LCD_POWER_SEQ, 0x64, 0x03, 0X12, 0X81,
//        4, LCD_DTCA, 0x85, 0x00, 0x78,
//        6, LCD_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
//        2, LCD_PRC, 0x20,
//        3, LCD_DTCB, 0x00, 0x00,
        2, LCD_POWER1, 0x23, // Power control
        2, LCD_POWER2, 0x10, // Power control
//        3, LCD_VCOM1, 0x3e, 0x28, // VCM control
//        2, LCD_VCOM2, 0x86, // VCM control2
        2, LCD_MAC, 0x48, // Memory Access Control
        2, LCD_PIXEL_FORMAT, 0x55,
//        3, LCD_FRMCTR1, 0x00, 0x18,
//        4, LCD_DFC, 0x08, 0x82, 0x27, // Display Function Control
//        2, LCD_3GAMMA_EN, 0x00, // Gamma Function Disable
//        2, LCD_GAMMA, 0x01, // Gamma curve selected
//        16, LCD_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
//        16, LCD_NGAMMA, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,

        0
};

//</editor-fold>

//<editor-fold desc="Generic SPI operations">

void LCD_setSpi8(void) {
    SPI1->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI1->CR1 &= ~SPI_CR1_DFF; // SPI 8
    SPI1->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

void LCD_setSpi16(void) {
    SPI1->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI1->CR1 |= SPI_CR1_DFF;  // SPI 16
    SPI1->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

// </editor-fold>

void LCD_pinsInit() {
    SPI_InitTypeDef spiStructure;
    GPIO_InitTypeDef gpioStructure;

    RCC_PCLK2Config(RCC_HCLK_Div2);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | SPI_MASTER_CLK, ENABLE);

    // GPIO for CS/DC/LED/RESET
    gpioStructure.GPIO_Pin   = TFT_CS_PIN | TFT_DC_PIN | TFT_RESET_PIN | TFT_LED_PIN;
    gpioStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioStructure);

    // GPIO for SPI
    gpioStructure.GPIO_Pin   = SPI_MASTER_PIN_NSS | SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
    gpioStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_MASTER_GPIO, &gpioStructure);

    SPI_StructInit(&spiStructure);
    spiStructure.SPI_Mode = SPI_Mode_Master;
    spiStructure.SPI_NSS  = SPI_NSS_Soft;
    SPI_Init(SPI_MASTER, &spiStructure);

    SPI_SSOutputCmd(SPI_MASTER, ENABLE);
    SPI_Cmd(SPI_MASTER, ENABLE);
}

void LCD_configure() {
    u8 count,
       *address = (u8 *) init_commands;

    TFT_RST_SET;
    dmaSendCmd(LCD_SWRESET);
    delay_ms(100);

    while (1) {
        count = *(address++);
        if (count-- == 0) break;
        dmaSendCmd(*(address++));
        dmaSendData8(address, count);
        address += count;
    }

    dmaSendCmd(LCD_SLEEP_OUT);
    dmaSendCmd(LCD_DISPLAY_ON);
}

void LCD_init() {
    LCD_pinsInit();
    dmaInit();
    LCD_configure();
    TFT_LED_SET;
}
