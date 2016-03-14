#include "lib/ILI9341/text.h"
#include "lib/tinystd/itoa.h"

#include "lib/ADC/adc.h"
#include "lib/PWM/pwm.h"
#include "lib/encoder/encoder.h"

#define GRAPH_H 200
#define GRAPH_W ADC_DATA_SIZE
#define GRAPH_VERTICAL_PADDING 30

#define MAX_MV_VOLTAGE (u16)3300
#define MAX_ADC_VALUE (u16)4096

static void plotData() {
    u16 *adcDmaData = ADC_getData();

    u16 graphWidth  = GRAPH_W,
        graphHeight = GRAPH_H;

    u16 graphHalfHeight = graphHeight / (u8) 2;

    u16 maxValHalf = MAX_ADC_VALUE / (u8) 2;

    u16 lcdWidth  = LCD_getWidth(),
        lcdHeight = LCD_getHeight();

    u16 graphPosX = (lcdWidth - (u16) GRAPH_W) / (u8) 2,
        graphPosY = (lcdHeight - (u16) GRAPH_H) / (u8) 2;

    u16 adcValueDivision = MAX_ADC_VALUE / (GRAPH_H - GRAPH_VERTICAL_PADDING * 2);

    u16 plotX = 0,
        plotY = graphHalfHeight;

    u16 plotYOld = plotY;

    u16 maxValue = 0,
        minValue = 65535;


    // GRAPH with grid
    LCD_fillRect(graphPosX, graphPosY, graphWidth, graphHeight, DGRAY);
    LCD_drawFastHLine(graphPosX, graphHalfHeight + graphPosY, graphWidth, LGRAY);
    LCD_drawFastDashedHLine(graphPosX, graphPosY + graphHeight - GRAPH_VERTICAL_PADDING, graphWidth, LGRAY, DGRAY, 2,
                            4);
    LCD_drawFastDashedHLine(graphPosX, graphPosY + GRAPH_VERTICAL_PADDING, graphWidth, LGRAY, DGRAY, 2, 4);

    for (u16 x = ADC_SAMPLES_PER_FREQ; x < graphWidth; x += ADC_SAMPLES_PER_FREQ) {
        LCD_drawFastDashedVLine(x + graphPosX, graphPosY, graphHeight, LGRAY, DGRAY, 2, 4);
    }

    while (1) {
        u16 val = adcDmaData[plotX];

        maxValue = val > maxValue ? val : maxValue;
        minValue = val < minValue ? val : minValue;

        plotY = (u16) (graphHalfHeight - (val - maxValHalf) / adcValueDivision); // 0 is larger Y

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

    u16 plotYMin = (u16) (graphHalfHeight - (minValue - maxValHalf) / adcValueDivision); // 0 is larger x
    u16 plotYMax = (u16) (graphHalfHeight - (maxValue - maxValHalf) / adcValueDivision); // 0 is larger x

    u16 vMin = (u16) (MAX_MV_VOLTAGE * (u32) 1000000 / (MAX_ADC_VALUE - (u8) 1) * minValue / (u32) 1000000);
    u16 vMax = (u16) (MAX_MV_VOLTAGE * (u32) 1000000 / (MAX_ADC_VALUE - (u8) 1) * maxValue / (u32) 1000000);

//    LCD_drawFastHLine(graphPosX, graphPosY + plotYMin, graphWidth, BLUE);
//    LCD_drawFastHLine(graphPosX, graphPosY + plotYMax, graphWidth, RED);

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

    itoa(minValue, buf, 10);
    LCD_setCursor(0, 221);
    LCD_writeString("Min: ");
    LCD_writeString(buf);

    itoa(maxValue, buf, 10);
    LCD_setCursor(0, 230);
    LCD_writeString("Max: ");
    LCD_writeString(buf);

    itoa(adcValueDivision, buf, 10);
    LCD_setCursor(80, 221);
    LCD_writeString("YDiv: ");
    LCD_writeString(buf);

    LCD_setCursor(160, 221);
    LCD_writeString("F mode: ");
    LCD_writeString(ADC_getFreqMode());
}

int main(void) {
    LCD_init();
    ADC_init();
    PWM_init();
    ENC_init();

    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(BLACK);

    plotData();

    u16 encVal, encValOld;

    encVal = encValOld = ENC_getValue();

    ENC_setValue(1710);

    while (1) {
        encVal = ENC_getValue();
        while (!ADC_isDataAvailable());
        plotData();
        ADC_markDataUsed();
        delay_ms(1000);
        if (encVal != encValOld) {
            ADC_changeFreq((u32) (encVal % 17));
            encValOld = encVal;
        }
    }

    while (1);
}

