#include <stm32f10x_dma.h>
#include <stdlib.h>
#include "ILI9341_lib/graph.h"
#include "ILI9341_lib/text.h"

#define   SINE_RES          128

const uint16_t function[SINE_RES] = {2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897,
                                     2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640,
                                     3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002,
                                     4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894,
                                     3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344,
                                     3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483,
                                     2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517,
                                     1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679,
                                     610, 545, 484, 426, 373, 323, 278, 237, 201, 169,
                                     141, 118, 100, 87, 79, 75, 76, 82, 93, 109,
                                     129, 154, 184, 218, 257, 300, 347, 399, 455, 514,
                                     577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287,
                                     1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047};

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(WHITE);

    for (u16 x = 0; x < LCD_getWidth(); x += 10) {
        LCD_drawFastVLine(x, 0, LCD_getHeight(), LGRAY);
    }

    for (u16 y = 0; y < LCD_getWidth(); y += 10) {
        LCD_drawFastHLine(0, y, LCD_getWidth(), LGRAY);
    }

    u16 ch = (u16) (LCD_getHeight() / 2);
    u16 cw = (u16) (LCD_getWidth() / 2);

    LCD_drawFastHLine(0, ch, LCD_getWidth(), RED);
    LCD_drawFastVLine(cw, 0, LCD_getHeight(), RED);

    LCD_writeString("Test string\nwith linebreak");

    LCD_drawCircle(cw, ch, 10, DGREEN);

    LCD_drawRect(cw - 20, ch - 20, 41, 41, DGREEN);
    LCD_fillRect(cw, ch, 11, 11, BLUE);

    while (1);
}
