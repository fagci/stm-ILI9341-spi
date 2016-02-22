#include "graph.h"

#include "fonts/glcdfont.h"

#if SPI_DMA_MODE
u16 colorData[DMA_MAX_LENGTH];
#endif

u16 screen_width = LCD_WIDTH;
u16 screen_height = LCD_HEIGHT;

void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2) {
#if SPI_DMA_MODE
    u8 pointData[4];

    dmaSendCmd(ILI9341_COLUMN_ADDR);
    pointData[0] = (u8) (x1 >> 8);
    pointData[1] = (u8) (x1 & 0xFF);
    pointData[2] = (u8) (x2 >> 8);
    pointData[3] = (u8) (x2 & 0xFF);
    dmaSendData8(pointData, 4);

    dmaSendCmd(ILI9341_PAGE_ADDR);
    pointData[0] = (u8) (y1 >> 8);
    pointData[1] = (u8) (y1 & 0xFF);
    pointData[2] = (u8) (y2 >> 8);
    pointData[3] = (u8) (y2 & 0xFF);
    dmaSendData8(pointData, 4);

    dmaSendCmd(ILI9341_GRAM);
#else
    LCD_sendCommand8(ILI9341_COLUMN_ADDR);
    LCD_sendData8((u8) (x1 >> 8));
    LCD_sendData8((u8) (x1 & 0xFF));
    LCD_sendData8((u8) (x2 >> 8));
    LCD_sendData8((u8) (x2 & 0xFF));

    LCD_sendCommand8(ILI9341_PAGE_ADDR);
    LCD_sendData8((u8) (y1 >> 8));
    LCD_sendData8((u8) (y1 & 0xFF));
    LCD_sendData8((u8) (y2 >> 8));
    LCD_sendData8((u8) (y2 & 0xFF));

    LCD_sendCommand8(ILI9341_GRAM);
#endif
}

void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color) {
#if SPI_DMA_MODE
    u32 count = w * h;

    LCD_setAddressWindow(x1, y1, (u16) (x1 + w - 1), (u16) (y1 + h - 1));
    LCD_setSpi16();

    u16 i = 0;

    while (count > DMA_MAX_LENGTH) {
        for (i = 0; i < DMA_MAX_LENGTH; i++) colorData[i] = color;
        dmaSendData16(colorData, DMA_MAX_LENGTH);
        count -= DMA_MAX_LENGTH;
    }

    for (i = 0; i < count; i++) colorData[i] = color;
    dmaSendData16(colorData, (u16) count);

    LCD_setSpi8();
#else
    LCD_setAddressWindow(x1, y1, (u16) (x1 + w - 1), (u16) (y1 + h - 1));

    LCD_setSpi16();
    for (u32 n = LCD_PIXEL_COUNT; n--;) {
        if (SPI1->SR | SPI_SR_TXE) LCD_sendData16(color);
    }
    LCD_setSpi8();
#endif
}

void LCD_fillScreen(u16 color) {
    LCD_fillRect(0, 0, screen_width, screen_height, color);
}

void LCD_setOrientation(u8 o) {
    if(o == ORIENTATION_LANDSCAPE || o == ORIENTATION_LANDSCAPE_MIRROR) {
        screen_height = LCD_WIDTH;
        screen_width = LCD_HEIGHT;
    }
#if SPI_DMA_MODE
    dmaSendCmd(ILI9341_MAC);
    dmaSendData8(&o, 1);
#else
    LCD_sendCommand8(ILI9341_MAC);
    LCD_sendData8(o);
#endif
}

void LCD_putPixel(u16 x, u16 y, u16 color) {
    LCD_fillRect(x, y, 1, 1, color);
}

void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color) {
    LCD_fillRect(x0, y0, w, 1, color);
}

void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color) {
    LCD_fillRect(x0, y0, 1, h, color);
}

void LCD_drawCircle(int16_t x0, int16_t y0, int16_t r, u16 color) {
    int16_t  f     = 1 - r;
    int16_t  ddF_x = 1;
    int16_t  ddF_y = -2 * r;
    int16_t  x     = 0;
    uint16_t y     = r;

    LCD_putPixel(x0, y0 + r, color);
    LCD_putPixel(x0, y0 - r, color);
    LCD_putPixel(x0 + r, y0, color);
    LCD_putPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        LCD_putPixel(x0 + x, y0 + y, color);
        LCD_putPixel(x0 - x, y0 + y, color);
        LCD_putPixel(x0 + x, y0 - y, color);
        LCD_putPixel(x0 - x, y0 - y, color);
        LCD_putPixel(x0 + y, y0 + x, color);
        LCD_putPixel(x0 - y, y0 + x, color);
        LCD_putPixel(x0 + y, y0 - x, color);
        LCD_putPixel(x0 - y, y0 - x, color);
    }
}

void LCD_fillCircle(int16_t x0, int16_t y0, int16_t r, u16 color) {
    LCD_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    LCD_fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void LCD_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, u8 cornername, int16_t delta, u16 color) {

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            LCD_drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
            LCD_drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
        }
        if (cornername & 0x2) {
            LCD_drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
            LCD_drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
        }
    }
}

void LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, u16 color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            LCD_putPixel(y0, x0, color);
        } else {
            LCD_putPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void LCD_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, u16 color) {
    LCD_drawFastHLine(x, y, w, color);
    LCD_drawFastHLine(x, y + h - 1, w, color);
    LCD_drawFastVLine(x, y, h, color);
    LCD_drawFastVLine(x + w - 1, y, h, color);
}

u8 _cp437 = 0;

// Draw a character
void LCD_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {

    if ((x >= screen_width) || // Clip right
        (y >= screen_height) || // Clip bottom
        ((x + 6 * size - 1) < 0) || // Clip left
        ((y + 8 * size - 1) < 0))   // Clip top
        return;

    if (!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

    int8_t i, j;

    u8  line;
    u16 charPixels[48];

    if (size == 1) {
        LCD_setAddressWindow(x, y, x + 6 - 1, y + 8 - 1);
        for (i = 0; i < 6; i++) {
            line   = (u8) (i < 5 ? pgm_read_byte(font + (c * 5) + i) : 0x0);
            for (j = 0; j < 8; j++, line >>= 1) {
                charPixels[j * 6 + i] = line & 0x1 ? color : bg;
            }
        }
        LCD_setSpi16();
        dmaSendData16(charPixels, 48);
        LCD_setSpi8();
    } else {
        for (i = 0; i < 6; i++) {
            line   = (u8) (i < 5 ? pgm_read_byte(font + (c * 5) + i) : 0x0);
            for (j = 0; j < 8; j++, line >>= 1) {
                LCD_fillRect(x + (i * size), y + (j * size), size, size, line & 0x1 ? color : bg);
            }
        }
    }
}

int16_t cursor_x = 0, cursor_y = 0;
u8      textsize = 1, wrap = 1;

u16 textcolor = RED, textbgcolor = WHITE;

void LCD_write(unsigned char c) {
    if (c == '\n') {
        cursor_y += textsize * 8;
        cursor_x = 0;
    } else if (c == '\r') {
        // skip em
    } else {
        if (wrap && ((cursor_x + textsize * 6) >= screen_width)) { // Heading off edge?
            cursor_x = 0;            // Reset x to zero
            cursor_y += textsize * 8; // Advance y one line
        }
        LCD_drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
        cursor_x += textsize * 6;
    }
}

void LCD_writeString(unsigned char *s) {
    while (*(s)) LCD_write(*s++);
}

void LCD_setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

void LCD_setTextSize(u8 size) {
    textsize = size;
}

void LCD_setTextColor(u16 color) {
    textcolor = color;
}

void LCD_setTextBgColor(u16 color) {
    textbgcolor = color;
}
