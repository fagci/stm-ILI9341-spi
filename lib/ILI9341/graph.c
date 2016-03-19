#include "graph.h"

inline static void setAddressWindowCont(u16 x1, u16 y1, u16 x2, u16 y2) {
    dmaSendCmdCont(LCD_COLUMN_ADDR);
    dmaSendDataCont16(&x1, 1);
    dmaSendDataCont16(&x2, 1);
    dmaSendCmdCont(LCD_PAGE_ADDR);
    dmaSendDataCont16(&y1, 1);
    dmaSendDataCont16(&y2, 1);
}

inline static void setXYCont(u16 x, u16 y) {
    dmaSendCmdCont(LCD_COLUMN_ADDR);
    dmaSendDataCont16(&x, 1);
    dmaSendDataCont16(&x, 1);
    dmaSendCmdCont(LCD_PAGE_ADDR);
    dmaSendDataCont16(&y, 1);
    dmaSendDataCont16(&y, 1);
}

inline void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color) {
    u16 sw = LCD_getWidth();
    u16 sh = LCD_getHeight();
    u16 mx = x1 + w;
    u16 my = y1 + h;
    if (x1 >= sw || y1 >= sh) return;
    if (mx >= sw) w = sw - x1;
    if (my >= sh) h = sh - y1;

    u32 count = w * h;
    TFT_CS_RESET;
    setAddressWindowCont(x1, y1, (u16) (x1 + w - 1), (u16) (y1 + h - 1));
    dmaFill16(color, count);
    TFT_CS_SET;
}

inline void LCD_fillScreen(u16 color) {
    LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), color);
}

void LCD_readPixels(u16 x1, u16 y1, u16 x2, u16 y2, u16 *buf) {
    u8  red, green, blue;
    u16 w     = x2 - x1 + (u8) 1,
        h     = y2 - y1 + (u8) 1;
    u32 count = w * h;
    TFT_CS_RESET;
    setAddressWindowCont(x1, y1, x2, y2);
    dmaSendCmdCont(LCD_RAMRD);

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

inline void LCD_putPixel(u16 x, u16 y, u16 color) {
    if (x >= LCD_getWidth() || y >= LCD_getHeight()) return;
    TFT_CS_RESET;
    setXYCont(x, y);
    dmaSendCmd(LCD_GRAM);
    dmaSendDataCont16(&color, 1);
    TFT_CS_SET;
}

inline void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color) {
    u16 sw = LCD_getWidth();
    u16 mx = x0 + w;
    if (x0 >= sw || y0 >= LCD_getHeight()) return;
    if (mx >= sw) w = sw - x0;
    if (w == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    LCD_fillRect(x0, y0, w, 1, color);
}

inline void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color) {
    u16 sh = LCD_getHeight();
    u16 my = y0 + h;
    if (x0 >= LCD_getWidth() || y0 >= sh) return;
    if (my >= sh) h = sh - y0;
    if (h == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    LCD_fillRect(x0, y0, 1, h, color);
}

inline void LCD_drawFastDashedHLine(u16 x0, u16 y0, u16 w, u16 color, u16 bgColor, u8 fillLength, u8 gapLength) {
    if (fillLength == 0 || gapLength == 0) return;
    if (w == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    u16 i = 0, f, g;
    while (i < w) {
        f = fillLength % (w - i);
        if (f == 0) return;
        LCD_fillRect(i + x0, y0, f, 1, color);
        i += f;

        g = gapLength % (w - i);
        if (g == 0) return;
        LCD_fillRect(i + x0, y0, g, 1, bgColor);
        i += g;
    }
}

inline void LCD_drawFastDashedVLine(u16 x0, u16 y0, u16 h, u16 color, u16 bgColor, u8 fillLength, u8 gapLength) {
    if (fillLength == 0 || gapLength == 0) return;
    if (h == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    u16 i = 0, f, g;
    while (i < h) {
        f = fillLength % (h - i);
        if (f == 0) return;
        LCD_fillRect(x0, i + y0, 1, f, color);
        i += f;

        g = gapLength % (h - i);
        if (g == 0) return;
        LCD_fillRect(x0, i + y0, 1, g, bgColor);
        i += g;
    }
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

    LCD_putPixel(x0, y0 + r, color);
    LCD_putPixel(x0, y0 - r, color);
    LCD_putPixel(x0 + r, y0, color);
    LCD_putPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            dy += 2;
            f += dy;
        }
        x++;
        dx += 2;
        f += dx;

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
    u16 i, dxabs, dyabs, x, y, px, py;
    s16 dx, dy;
    s8  sdx, sdy;

    dx    = x1 - x0;      /* the horizontal distance of the line */
    dy    = y1 - y0;      /* the vertical distance of the line */
    dxabs = (u16) abs(dx);
    dyabs = (u16) abs(dy);
    sdx   = (s8) sgn(dx);
    sdy   = (s8) sgn(dy);
    x     = dyabs >> 1;
    y     = dxabs >> 1;
    px    = x0;
    py    = y0;

//    VGA[(py << 8) + (py << 6) + px] = color;

    if (dxabs >= dyabs) { // more horizontal
        for (i = 0; i < dxabs; i++) {
            y += dyabs;
            if (y >= dxabs) {
                y -= dxabs;
                py += sdy;
            }
            px += sdx;
            LCD_putPixel(px, py, color);
        }
    }
    else { // more vertical
        for (i = 0; i < dyabs; i++) {
            x += dxabs;
            if (x >= dyabs) {
                x -= dyabs;
                px += sdx;
            }
            py += sdy;
            LCD_putPixel(px, py, color);
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
    dmaSendData16(d, 3);
}

void LCD_scroll(u16 v) {
    dmaSendCmd(LCD_VSCRSADD);
    dmaSendData16(&v, 1);
}

