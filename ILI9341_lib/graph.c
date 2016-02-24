#include "graph.h"

u16 screen_width  = LCD_PIXEL_WIDTH;
u16 screen_height = LCD_PIXEL_HEIGHT;

void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2) {
    u16 pointData[2];

    dmaSendCmd(LCD_COLUMN_ADDR);
    pointData[0] = x1;
    pointData[1] = x2;
    LCD_setSpi16();
    dmaSendData16(pointData, 2);
    LCD_setSpi8();

    dmaSendCmd(LCD_PAGE_ADDR);
    pointData[0] = y1;
    pointData[1] = y2;
    LCD_setSpi16();
    dmaSendData16(pointData, 4);
    LCD_setSpi8();

    dmaSendCmd(LCD_GRAM);
}

void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color) {
    u32 count = w * h;
    LCD_setAddressWindow(x1, y1, (u16) (x1 + w - 1), (u16) (y1 + h - 1));
    LCD_setSpi16();
    dmaFill16(color, count);
    LCD_setSpi8();
}
void LCD_fillScreen(u16 color) {
    LCD_fillRect(0, 0, screen_width, screen_height, color);
}

void LCD_setOrientation(u8 o) {
    if (o == ORIENTATION_LANDSCAPE || o == ORIENTATION_LANDSCAPE_MIRROR) {
        screen_height = LCD_PIXEL_WIDTH;
        screen_width  = LCD_PIXEL_HEIGHT;
    }
    dmaSendCmd(LCD_MAC);
    dmaSendData8(&o, 1);
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

void LCD_drawCircle(u16 x0, u16 y0, u16 r, u16 color) {
    s16 f     = (s16) (1 - r);
    s16 ddF_x = 1;
    s16 ddF_y = (s16) (-2 * r);
    s16 x     = 0;
    u16 y     = r;

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

// Used to do circles and roundrects
void LCD_fillCircleHelper(u16 x0, u16 y0, u16 r, u8 cornername, s16 delta, u16 color) {

    s16 f     = (s16) (1 - r);
    s16 ddF_x = 1;
    s16 ddF_y = (s16) (-2 * r);
    s16 x     = 0;
    s16 y     = r;

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
    s16 steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    s16 dx, dy;
    dx = x1 - x0;
    dy = (s16) abs(y1 - y0);

    s16 err = (s16) (dx / 2);
    s16 ystep;

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

void LCD_drawRect(u16 x, u16 y, u16 w, u16 h, u16 color) {
    LCD_drawFastHLine(x, y, w, color);
    LCD_drawFastHLine(x, (u16) (y + h - 1), w, color);
    LCD_drawFastVLine(x, y, h, color);
    LCD_drawFastVLine((u16) (x + w - 1), y, h, color);
}

u16 LCD_getWidth() {
    return screen_width;
}

u16 LCD_getHeight() {
    return screen_height;
}
