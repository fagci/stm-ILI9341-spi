#include <stm32f10x_spi.h>
#include "core.h"

//<editor-fold desc="Init commands">

static const uint8_t init_commands[] = {
        4, 0xEF, 0x03, 0x80, 0x02,
        4, LCD_POWERB, 0x00, 0XC1, 0X30,
        5, LCD_POWER_SEQ, 0x64, 0x03, 0X12, 0X81,
        4, LCD_DTCA, 0x85, 0x00, 0x78,
        6, LCD_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
        2, LCD_PRC, 0x20,
        3, LCD_DTCB, 0x00, 0x00,
        2, LCD_POWER1, 0x23,                        //Powercontrol
        2, LCD_POWER2, 0x10,                        //Powercontrol
        3, LCD_VCOM1, 0x3e, 0x28,                   //VCMcontrol
        2, LCD_VCOM2, 0x86,                         //VCMcontrol2
        2, LCD_MAC, 0x48,                           //MemoryAccessControl
        2, LCD_PIXEL_FORMAT, 0x55,
        3, LCD_FRMCTR1, 0x00, 0x18,
        4, LCD_DFC, 0x08, 0x82, 0x27,               //DisplayFunctionControl
        2, LCD_3GAMMA_EN, 0x00,                     //GammaFunctionDisable
        2, LCD_GAMMA, 0x01,                         //Gammacurveselected
        16, LCD_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
        16, LCD_NGAMMA, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
        0
};

//</editor-fold>

u16 screen_width  = LCD_PIXEL_WIDTH;
u16 screen_height = LCD_PIXEL_HEIGHT;

//<editor-fold desc="SPI functions">

void LCD_setSpi8(void) {
    SPI_MASTER->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI_MASTER->CR1 &= ~SPI_CR1_DFF; // SPI 8
    SPI_MASTER->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

void LCD_setSpi16(void) {
    SPI_MASTER->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI_MASTER->CR1 |= SPI_CR1_DFF;  // SPI 16
    SPI_MASTER->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

// </editor-fold>

void LCD_pinsInit() {
    SPI_InitTypeDef  spiStructure;
    GPIO_InitTypeDef gpioStructure;

    RCC_PCLK2Config(RCC_HCLK_Div2);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
    RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | SPI_MASTER_CLK, ENABLE);

    // GPIO speed by default
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // GPIO for CS/DC/LED/RESET
    gpioStructure.GPIO_Pin  = TFT_CS_PIN | TFT_DC_PIN | TFT_RESET_PIN | TFT_LED_PIN;
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpioStructure);

    // GPIO for SPI
    gpioStructure.GPIO_Pin  = SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
    gpioStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_MASTER_GPIO, &gpioStructure);

    // GPIO for SPI
    gpioStructure.GPIO_Pin  = SPI_MASTER_PIN_MISO;
    gpioStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(SPI_MASTER_GPIO, &gpioStructure);

    SPI_StructInit(&spiStructure);
    spiStructure.SPI_Mode = SPI_Mode_Master;
    spiStructure.SPI_NSS  = SPI_NSS_Soft;
    spiStructure.SPI_CPOL = SPI_CPOL_High;
    spiStructure.SPI_CPHA = SPI_CPHA_2Edge;
    spiStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_Init(SPI_MASTER, &spiStructure);

    SPI_SSOutputCmd(SPI_MASTER, ENABLE);
    SPI_Cmd(SPI_MASTER, ENABLE);
}

void LCD_reset() {
    TFT_RST_SET;
    delay_ms(10);
    TFT_RST_RESET;
    delay_ms(10);
    TFT_RST_SET;
    delay_ms(150);
}

void LCD_exitStandby() {
    dmaSendCmd(LCD_SLEEP_OUT);
    delay_ms(150);
    dmaSendCmd(LCD_DISPLAY_ON);
}

u16 LCD_getWidth() {
    return screen_width;
}

u16 LCD_getHeight() {
    return screen_height;
}

void LCD_setOrientation(u8 o) {
    if (o == ORIENTATION_LANDSCAPE || o == ORIENTATION_LANDSCAPE_MIRROR) {
        screen_height = LCD_PIXEL_WIDTH;
        screen_width  = LCD_PIXEL_HEIGHT;
    } else {
        screen_height = LCD_PIXEL_HEIGHT;
        screen_width  = LCD_PIXEL_WIDTH;
    }
    TFT_CS_RESET;
    dmaSendCmdCont(LCD_MAC);
    dmaSendDataCont8(&o, 1);
    TFT_CS_SET;
}

void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2) {
    u16 pointData[2];

    TFT_CS_RESET;
    dmaSendCmdCont(LCD_COLUMN_ADDR);
    pointData[0] = x1;
    pointData[1] = x2;
    LCD_setSpi16();
    dmaSendDataCont16(pointData, 2);
    LCD_setSpi8();

    dmaSendCmdCont(LCD_PAGE_ADDR);
    pointData[0] = y1;
    pointData[1] = y2;
    LCD_setSpi16();
    dmaSendDataCont16(pointData, 2);
    LCD_setSpi8();
    TFT_CS_SET;
}

void LCD_configure() {
    u8 count;
    u8 *address = (u8 *) init_commands;

    TFT_CS_RESET;
    while (1) {
        count = *(address++);
        if (count-- == 0) break;
        dmaSendCmdCont(*(address++));
        dmaSendDataCont8(address, count);
        address += count;
    }
    TFT_CS_SET;

    LCD_setOrientation(0);
    LCD_setAddressWindow(0, 0, screen_width, screen_height);
}

void LCD_init() {
    LCD_pinsInit();
    dmaInit();

    LCD_reset();
    LCD_exitStandby();
    LCD_configure();

    TFT_LED_SET;
}
