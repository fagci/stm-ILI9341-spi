#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include "lib/ILI9341/text.h"
#include "lib/tinystd/itoa.h"

#include "lib/ADC/adc.h"
#include "lib/PWM/pwm.h"

#define GRAPH_H 200
#define GRAPH_W ADC_DATA_SIZE

#define MAX_MV_VOLTAGE 3300

static void plotData() {
    u16 *adcDmaData = ADC_getData();

    u16 graphWidth  = GRAPH_W,
        graphHeight = GRAPH_H;

    u16 graphHalfHeight = (u16) (graphHeight / 2);

    u16 plotX = 0,
        plotY = graphHalfHeight;

    u16 plotYOld = plotY;

    s16 maxVal     = 4096;
    s16 maxValHalf = (s16) (maxVal / 2);

    u16 graphVerticalPadding = 20;

    u16 maxValHeight = (u16) (GRAPH_H - (graphVerticalPadding) * 2);

    u16 div = maxVal / maxValHeight;

    u16 graphPosX = (u16) ((LCD_getWidth() - GRAPH_W) / 2),
        graphPosY = (u16) ((LCD_getHeight() - GRAPH_H) / 2);

    LCD_fillRect(graphPosX, graphPosY, graphWidth, graphHeight, DGRAY);

    LCD_drawFastHLine(graphPosX, graphHalfHeight + graphPosY, graphWidth, LGRAY);

    LCD_drawFastHLine(graphPosX, graphPosY + graphHeight - graphVerticalPadding, graphWidth, LGRAY);
    LCD_drawFastHLine(graphPosX, graphPosY + graphVerticalPadding, graphWidth, LGRAY);

    u16 max = 0, min = 65535;

    while (1) {
        u16 val = adcDmaData[plotX];

        max = val > max ? val : max;
        min = val < min ? val : min;

        plotY = (u16) (graphHalfHeight - (val - maxValHalf) / div); // 0 is larger x

        if (plotX == 0) plotYOld = plotY;

        LCD_drawLine(
                plotX + graphPosX, plotYOld + graphPosY,
                (u16) (plotX + graphPosX + 1), plotY + graphPosY,
                GREEN
        );
        if (plotX == graphWidth - 1) {
            break;
        }

        plotX++;
        plotYOld = plotY;
    }

    u16 plotYMin = (u16) (graphHalfHeight - (min - maxValHalf) / div); // 0 is larger x
    u16 plotYMax = (u16) (graphHalfHeight - (max - maxValHalf) / div); // 0 is larger x

    u32 vMin = MAX_MV_VOLTAGE * 1000 / maxVal * min / 1000;
    u32 vMax = MAX_MV_VOLTAGE * 1000 / maxVal * max / 1000;

    LCD_drawFastHLine(graphPosX, graphPosY + plotYMin, graphWidth, BLUE);
    LCD_drawFastHLine(graphPosX, graphPosY + plotYMax, graphWidth, RED);

    LCD_setTextColor(YELLOW);
    char buf[16];

    LCD_setTextBgColor(DGRAY);

    itoa(vMin, buf, 10);
    LCD_setCursor(0, graphPosY + plotYMin + 2);
    LCD_writeString("Min: ");
    LCD_writeString(buf);
    LCD_writeString("mv");

    itoa(vMax, buf, 10);
    LCD_setCursor(0, graphPosY + plotYMax - 10);
    LCD_writeString("Max: ");
    LCD_writeString(buf);
    LCD_writeString("mv");


    LCD_fillRect(0, graphPosY + graphHeight, LCD_getWidth(), graphPosY, BLACK);
    LCD_setTextBgColor(BLACK);

    itoa(min, buf, 10);
    LCD_setCursor(0, 221);
    LCD_writeString("Min: ");
    LCD_writeString(buf);

    itoa(max, buf, 10);
    LCD_setCursor(0, 230);
    LCD_writeString("Max: ");
    LCD_writeString(buf);

    itoa(div, buf, 10);
    LCD_setCursor(80, 221);
    LCD_writeString("YDiv: ");
    LCD_writeString(buf);

    itoa(ADC_PERIOD, buf, 10);
    LCD_setCursor(80, 230);
    LCD_writeString("ADC Period: ");
    LCD_writeString(buf);


}


int main(void) {
    LCD_init();
    ADC_init();
    PWM_init();

    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(BLACK);

    while (1) {
        while (!isDataAvailable());
        plotData();
        markDataUsed();
        delay_ms(150);
    }

    while (1);
}

