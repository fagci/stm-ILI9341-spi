#include "graph.h"

void LCD_readPixels(u16 x1, u16 y1, u16 x2, u16 y2, u16 *buf) {
    u8  red, green, blue;
    u32 count = (u32) ((x2 - x1 + 1) * (y2 - y1 + 1));

    LCD_setAddressWindowToRead(x1, y1, x2, y2);

    TFT_CS_RESET;
    TFT_DC_SET;

    dmaReceiveDataCont8(&red);

    for (u32 i = 0; i < count; ++i) {
        dmaReceiveDataCont8(&red);
        dmaReceiveDataCont8(&green);
        dmaReceiveDataCont8(&blue);

        buf[i] = (u16) ILI9341_COLOR(red, green, blue);
    }

    TFT_CS_SET;
}

inline void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color) {
    u32 count = w * h;
    LCD_setAddressWindowToWrite(x1, y1, (u16) (x1 + w - 1), (u16) (y1 + h - 1));
    LCD_setSpi16();
    dmaFill16(color, count);
    LCD_setSpi8();
}

void LCD_fillScreen(u16 color) {
    LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), color);
}

inline void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color) {
    if (w == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    LCD_fillRect(x0, y0, w, 1, color);
}

inline void LCD_putPixel(u16 x, u16 y, u16 color) {
    LCD_setAddressWindowToWrite(x, y, x, y);
    LCD_setSpi16();
    dmaFill16(color, 1);
    LCD_setSpi8();
}

inline static void LCD_putPixelCont(u16 x, u16 y, u16 color) {
    LCD_setAddressWindowToWrite(x, y, x, y);
    dmaFill16(color, 1);
}

inline void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color) {
    if (h == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    LCD_fillRect(x0, y0, 1, h, color);
}

void LCD_drawCircle(u16 x0, u16 y0, u16 r, u16 color) {
    if (r == 0) {
        LCD_putPixel(x0, y0, color);
        return;
    }

    s16 f  = (s16) (1 - r),
        dx = 1,
        dy = (s16) (-2 * r),
        x  = 0;

    u16 y = r;

    LCD_setSpi16();
    
    LCD_putPixelCont(x0, y0 + r, color);
    LCD_putPixelCont(x0, y0 - r, color);
    LCD_putPixelCont(x0 + r, y0, color);
    LCD_putPixelCont(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            dy += 2;
            f += dy;
        }
        x++;
        dx += 2;
        f += dx;

        LCD_putPixelCont(x0 + x, y0 + y, color);
        LCD_putPixelCont(x0 - x, y0 + y, color);
        LCD_putPixelCont(x0 + x, y0 - y, color);
        LCD_putPixelCont(x0 - x, y0 - y, color);
        LCD_putPixelCont(x0 + y, y0 + x, color);
        LCD_putPixelCont(x0 - y, y0 + x, color);
        LCD_putPixelCont(x0 + y, y0 - x, color);
        LCD_putPixelCont(x0 - y, y0 - x, color);
    }

    LCD_setSpi8();
}

// Used to do circles and roundrects
void LCD_fillCircleHelper(u16 x0, u16 y0, u16 r, u8 cornername, s16 delta, u16 color) {
    if (r == 0)
        return;
    if (r == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    s16 f  = (s16) (1 - r),
        dx = 1,
        dy = (s16) (-2 * r),
        x  = 0;

    u16 y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            dy += 2;
            f += dy;
        }
        x++;
        dx += 2;
        f += dx;

        if (cornername & 0x1) {
            LCD_drawFastVLine(x0 + x, y0 - y, (u16) (2 * y + 1 + delta), color);
            LCD_drawFastVLine(x0 + y, y0 - x, (u16) (2 * x + 1 + delta), color);
        }
        if (cornername & 0x2) {
            LCD_drawFastVLine(x0 - x, y0 - y, (u16) (2 * y + 1 + delta), color);
            LCD_drawFastVLine(x0 - y, y0 - x, (u16) (2 * x + 1 + delta), color);
        }
    }
}

void LCD_fillCircle(u16 x0, u16 y0, u16 r, u16 color) {
    LCD_drawFastVLine(x0, y0 - r, (u16) (2 * r + 1), color);
    LCD_fillCircleHelper(x0, y0, r, 3, 0, color);
}

void LCD_drawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 color) {
    s16 Dx = (s16) abs(x1 - x0),
        Dy = (s16) abs(y1 - y0);

    if (Dx == 0 && Dy == 0) {
        LCD_putPixel(x0, y0, color);
        return;
    }

    s16 steep = Dy > Dx;
    s16 dx, dy, err, yStep;

    if (steep) {
        _int16_swap(x0, y0);
        _int16_swap(x1, y1);
    }

    if (x0 > x1) {
        _int16_swap(x0, x1);
        _int16_swap(y0, y1);
    }

    dx = x1 - x0;
    dy = (s16) abs(y1 - y0);

    err = (s16) (dx / 2);

    if (y0 < y1) {
        yStep = 1;
    } else {
        yStep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            LCD_putPixel(y0, x0, color);
        } else {
            LCD_putPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += yStep;
            err += dx;
        }
    }
}

void LCD_drawRect(u16 x, u16 y, u16 w, u16 h, u16 color) {
    if (w == 0 || h == 0) return;
    if (w == 1) {
        LCD_drawFastVLine(x, y, h, color);
        return;
    }
    if (h == 1) {
        LCD_drawFastHLine(x, y, w, color);
        return;
    }
    LCD_drawFastHLine(x, y, w, color);
    LCD_drawFastHLine(x, (u16) (y + h - 1), w, color);
    LCD_drawFastVLine(x, y, h, color);
    LCD_drawFastVLine((u16) (x + w - 1), y, h, color);
}

void LCD_setVerticalScrolling(u16 startY, u16 endY) {
    dmaSendCmd(LCD_VSCRDEF);
    u16 d[] = {
            startY,
            (u16) (LCD_PIXEL_HEIGHT - startY - endY),
            endY
    };
    LCD_setSpi16();
    dmaSendData16(d, 3);
    LCD_setSpi8();
}

void LCD_scroll(u16 v) {
    dmaSendCmd(LCD_VSCRSADD);
    LCD_setSpi16();
    dmaSendData16(&v, 1);
    LCD_setSpi8();
}

