#include "text.h"

u8 _cp437 = 0;

u16 cursorX = 0,
    cursorY = 0;

u8 textSize = 1;

u8 wrap = 1;

u16 textColor   = RED,
    textBgColor = TRANSPARENT_COLOR;

inline static void LCD_drawChar(u16 x0, u16 y0, unsigned char c, u16 color, u16 bg, uint8_t size) {
    u16 scaledWidth       = (u16) (size * 6),
        doubleScaledWidth = scaledWidth * size;

    u16 x1 = (u16) (x0 + scaledWidth - 1),
        y1 = (u16) (y0 + 8 * size - 1);

    u16 doubleSize = size * size;
    u16 count      = (u16) (48 * doubleSize);

    u16 charPixels[count];

    u16 mx, my;
    s8  i, j, sx, sy;
    u8  line;
    u16 pixelColor;

    if (x0 >= LCD_getWidth() || y0 >= LCD_getHeight() || x1 < 0 || y1 < 0) return;

    if (!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

    u16 characterNumber = (u16) (c * 5);

    if (bg == TRANSPARENT_COLOR) {
        LCD_readPixels(x0, y0, x1, y1, charPixels);
    }

    LCD_setAddressWindowToWrite(x0, y0, x1, y1);

    for (i = 0; i < 6; i++) {
        line = (u8) (i < 5 ? pgm_read_byte(font + characterNumber + i) : 0x0);
        my   = (u16) (i * size);


        for (j = 0; j < 8; j++, line >>= 1) {
            mx = (u16) (j * doubleScaledWidth);

            pixelColor = line & 0x1 ? color : bg;

            if (pixelColor == TRANSPARENT_COLOR) continue;

            for (sx = 0; sx < size; ++sx) {
                for (sy = 0; sy < size; ++sy) {
                    charPixels[mx + my + sy * scaledWidth + sx] = pixelColor;
                }
            }
        }
    }

    LCD_setSpi16();
    dmaSendData16(charPixels, count);
    LCD_setSpi8();
}

inline void LCD_write(unsigned char c) {
    if (c == '\n') {
        cursorY += textSize * 8;
        cursorX = 0;
    } else if (c == '\r') {
        cursorX = 0;
    } else {
        if (wrap && ((cursorX + textSize * 6) >= LCD_getWidth())) { // Heading off edge?
            cursorX = 0;            // Reset x to zero
            cursorY += textSize * 8; // Advance y one line
        }
        LCD_drawChar(cursorX, cursorY, c, textColor, textBgColor, textSize);
        cursorX += textSize * 6;
    }
}

void LCD_writeString(unsigned char *s) {
    while (*(s)) LCD_write(*s++);
}

void LCD_setCursor(u16 x, u16 y) {
    cursorX = x;
    cursorY = y;
}

void LCD_setTextSize(u8 size) {
    textSize = size;
}

void LCD_setTextColor(u16 color) {
    textColor = color;
}

void LCD_setTextBgColor(u16 color) {
    textBgColor = color;
}

u16 LCD_getCursorX() {
    return cursorX;
}

u16 LCD_getCursorY() {
    return cursorY;
}
