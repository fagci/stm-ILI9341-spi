#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include "delay.h"

#ifndef TEST1_ILI9341_CORE_H
#define TEST1_ILI9341_CORE_H

// <editor-fold desc="Defaults">

// TODO: set to 1 after generic SPI mode testing
#define SPI_DMA_MODE 1

#define TFT_LED_PIN     GPIO_Pin_0
#define TFT_RESET_PIN   GPIO_Pin_2
#define TFT_DC_PIN      GPIO_Pin_3
#define TFT_CS_PIN      GPIO_Pin_4

#define SPI_MASTER                   SPI1
#define SPI_MASTER_CLK               RCC_APB2Periph_SPI1
#define SPI_MASTER_GPIO              GPIOA
#define SPI_MASTER_GPIO_CLK          RCC_APB2Periph_GPIOA
#define SPI_MASTER_PIN_NSS           GPIO_Pin_4
#define SPI_MASTER_PIN_SCK           GPIO_Pin_5
#define SPI_MASTER_PIN_MISO          GPIO_Pin_6
#define SPI_MASTER_PIN_MOSI          GPIO_Pin_7

#define LCD_WIDTH       240
#define LCD_HEIGHT      320
#define LCD_PIXEL_COUNT    LCD_WIDTH * LCD_HEIGHT

// </editor-fold>

//<editor-fold desc="Orientation">

#define ORIENTATION_PORTRAIT 0x48
#define ORIENTATION_LANDSCAPE 0x28
#define ORIENTATION_PORTRAIT_MIRROR 0x88
#define ORIENTATION_LANDSCAPE_MIRROR 0xE8

//</editor-fold>

// <editor-fold desc="Functions">

#define TFT_DC_SET      GPIO_SetBits(GPIOA, TFT_DC_PIN);
#define TFT_DC_RESET    GPIO_ResetBits(GPIOA, TFT_DC_PIN);

#define TFT_RST_SET     GPIO_SetBits(GPIOA, TFT_RESET_PIN);
#define TFT_RST_RESET   GPIO_ResetBits(GPIOA, TFT_RESET_PIN);

#define TFT_CS_SET      GPIO_SetBits(GPIOA, TFT_CS_PIN);
#define TFT_CS_RESET    GPIO_ResetBits(GPIOA, TFT_CS_PIN);

#define TFT_LED_SET      GPIO_SetBits(GPIOA, TFT_LED_PIN);
#define TFT_LED_RESET    GPIO_ResetBits(GPIOA, TFT_LED_PIN);

// </editor-fold>

// <editor-fold desc="Commands">

#define ILI9341_RESET                0x01
#define ILI9341_SLEEP_OUT            0x11
#define ILI9341_GAMMA                0x26
#define ILI9341_DISPLAY_OFF            0x28
#define ILI9341_DISPLAY_ON            0x29
#define ILI9341_COLUMN_ADDR            0x2A
#define ILI9341_PAGE_ADDR            0x2B
#define ILI9341_GRAM                0x2C
#define ILI9341_MAC                    0x36
#define ILI9341_PIXEL_FORMAT        0x3A
#define ILI9341_WDB                    0x51
#define ILI9341_WCD                    0x53
#define ILI9341_RGB_INTERFACE       0xB0
#define ILI9341_FRC                    0xB1
#define ILI9341_BPC                    0xB5
#define ILI9341_DFC                    0xB6
#define ILI9341_POWER1                0xC0
#define ILI9341_POWER2                0xC1
#define ILI9341_VCOM1                0xC5
#define ILI9341_VCOM2                0xC7
#define ILI9341_POWERA                0xCB
#define ILI9341_POWERB                0xCF
#define ILI9341_PGAMMA                0xE0
#define ILI9341_NGAMMA                0xE1
#define ILI9341_DTCA                0xE8
#define ILI9341_DTCB                0xEA
#define ILI9341_POWER_SEQ            0xED
#define ILI9341_3GAMMA_EN            0xF2
#define ILI9341_INTERFACE            0xF6
#define ILI9341_PRC                    0xF7

// </editor-fold>

// <editor-fold desc="Colors">

#define BLACK           0x0000      /*   0,   0,   0 */
#define NAVY            0x000F      /*   0,   0, 128 */
#define DGREEN          0x03E0      /*   0, 128,   0 */
#define DCYAN           0x03EF      /*   0, 128, 128 */
#define MAROON          0x7800      /* 128,   0,   0 */
#define PURPLE          0x780F      /* 128,   0, 128 */
#define OLIVE           0x7BE0      /* 128, 128,   0 */
#define LGRAY           0xC618      /* 192, 192, 192 */
#define DGRAY           0x7BEF      /* 128, 128, 128 */
#define BLUE            0x001F      /*   0,   0, 255 */
#define GREEN           0x07E0      /*   0, 255,   0 */
#define CYAN            0x07FF      /*   0, 255, 255 */
#define RED             0xF800      /* 255,   0,   0 */
#define MAGENTA         0xF81F      /* 255,   0, 255 */
#define YELLOW          0xFFE0      /* 255, 255,   0 */
#define WHITE           0xFFFF      /* 255, 255, 255 */
#define ORANGE          0xFD20      /* 255, 165,   0 */
#define GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define BROWN                 0XBC40 //
#define BRRED                 0XFC07 //

// </editor-fold>

void LCD_sendCommand8(u8 cmd);
void LCD_sendData8(u8 data);
void LCD_sendData16(u16 data);
void LCD_setSpi8(void);
void LCD_setSpi16(void);

#endif //TEST1_ILI9341_CORE_H
