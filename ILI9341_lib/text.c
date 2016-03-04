#include "text.h"

u8  _cp437   = 0;
u16 cursorX  = 0, cursorY = 0;
u8  textSize = 1, wrap = 1;

u16 textColor = RED, textBgColor = WHITE;

// Draw a character
void LCD_drawChar(u16 x, u16 y, unsigned char c, u16 color, u16 bg, uint8_t size) {

    if ((x >= LCD_getWidth()) || // Clip right
        (y >= LCD_getHeight()) || // Clip bottom
        ((x + 6 * size - 1) < 0) || // Clip left
        ((y + 8 * size - 1) < 0))   // Clip top
        return;

    if (!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

    int8_t i, j;

    u8  line;
    u16 ds = size * size;
    u16 charPixels[48 * ds];

    if (size == 1) {
        LCD_setAddressWindowToWrite(x, y, (u16) (x + 5), (u16) (y + 7));
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
        // TODO: make chunks to send large amount of data over DMA
        u16 sw = size * 6;
        LCD_setAddressWindowToWrite(x, y, x + sw - 1, y + 8 * size - 1);
        for (i = 0; i < 6; i++) {
            line   = (u8) (i < 5 ? pgm_read_byte(font + (c * 5) + i) : 0x0);
            for (j = 0; j < 8; j++, line >>= 1) {
                u16      mx           = j * 6 * ds, my = i * size;
                u16      colorCurrent = line & 0x1 ? color : bg;
                for (int sx           = 0; sx < size; ++sx) {
                    for (int sy = 0; sy < size; ++sy) {
                        charPixels[mx + my + sy * sw + sx] = colorCurrent;
                    }
                }
            }
        }
        LCD_setSpi16();
        dmaSendData16(charPixels, 48 * size * size);
        LCD_setSpi8();


//        for (i = 0; i < 6; i++) {
//            line   = (u8) (i < 5 ? pgm_read_byte(font + (c * 5) + i) : 0x0);
//            for (j = 0; j < 8; j++, line >>= 1) {
//                LCD_fillRect(x + (i * size), y + (j * size), size, size, line & 0x1 ? color : bg);
//            }
//        }
    }
}


void LCD_write(unsigned char c) {
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
