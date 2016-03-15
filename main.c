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

u16 lcdWidth,
    lcdHeight;

u16 graphPosX,
    graphPosY;

u16 plotYMax,
    plotYMin;

u16 adcOldValues[ADC_DATA_SIZE];

static void plotData() {
    u16 *adcDmaData = ADC_getData();

    u16 graphHalfHeight = GRAPH_H / 2;

    u16 maxValHalf = MAX_ADC_VALUE / 2;

    u16 adcValueDivision = MAX_ADC_VALUE / (GRAPH_H - GRAPH_VERTICAL_PADDING * 2);

    u16 plotX        = 0,
        plotY        = graphHalfHeight,
        plotyHistory = plotY;

    u16 plotYOld        = plotY,
        plotYHistoryOld = plotY;

    u16 maxValue = 0,
        minValue = 65535;

    u16 currentValue;


    u16 pYMaxO = plotYMax;
    u16 pYMinO = plotYMin;

    plotYMax = 0;
    plotYMin = GRAPH_H;

    // GRAPH with grid
    LCD_drawFastHLine(graphPosX, graphHalfHeight + graphPosY, GRAPH_W, LGRAY);
    LCD_drawFastDashedHLine(graphPosX, graphPosY + GRAPH_H - GRAPH_VERTICAL_PADDING, GRAPH_W, LGRAY, DGRAY, 2, 4);
    LCD_drawFastDashedHLine(graphPosX, graphPosY + GRAPH_VERTICAL_PADDING, GRAPH_W, LGRAY, DGRAY, 2, 4);

    for (u16 x = ADC_SAMPLES_PER_FREQ; x < GRAPH_W; x += ADC_SAMPLES_PER_FREQ) {
        LCD_drawFastDashedVLine(x + graphPosX, graphPosY, GRAPH_H, LGRAY, DGRAY, 2, 4);
    }


    while (plotX < GRAPH_W) {
        currentValue = adcDmaData[plotX];
        maxValue     = currentValue > maxValue ? currentValue : maxValue;
        minValue     = currentValue < minValue ? currentValue : minValue;

        currentValue = adcDmaData[plotX] = (u16) (graphHalfHeight -
                                                  (adcDmaData[plotX] - maxValHalf) / adcValueDivision); // 0 is larger Y
        plotYMax     = currentValue > plotYMax ? currentValue : plotYMax;
        plotYMin     = currentValue < plotYMin ? currentValue : plotYMin;

        plotX++;
    }

    u16 vMin = (u16) (MAX_MV_VOLTAGE * (u32) 1000000 / (MAX_ADC_VALUE - (u8) 1) * minValue / (u32) 1000000);
    u16 vMax = (u16) (MAX_MV_VOLTAGE * (u32) 1000000 / (MAX_ADC_VALUE - (u8) 1) * maxValue / (u32) 1000000);

    char buf[16];

    LCD_setTextBgColor(DGRAY);

    LCD_drawFastHLine(graphPosX, graphPosY + pYMinO, GRAPH_W, DGRAY);
    LCD_drawFastHLine(graphPosX, graphPosY + pYMaxO, GRAPH_W, DGRAY);
    LCD_fillRect(graphPosX, (u16) (graphPosY + pYMinO - 10), 100, 10, DGRAY);
    LCD_fillRect(graphPosX, (u16) (graphPosY + pYMaxO), 100, 10, DGRAY);

    itoa(vMin, buf, 10);
    LCD_setCursor(0, (u16) (graphPosY + plotYMax + 2));
    LCD_writeString("Min: ");
    LCD_writeString(buf);
    LCD_writeString("mv");

    itoa(vMax, buf, 10);
    LCD_setCursor(0, (u16) (graphPosY + plotYMin - 10));
    LCD_writeString("Max: ");
    LCD_writeString(buf);
    LCD_writeString("mv");

    LCD_drawFastDashedHLine(graphPosX, graphPosY + plotYMin, GRAPH_W, BLUE, DGRAY, 1, 2);
    LCD_drawFastDashedHLine(graphPosX, graphPosY + plotYMax, GRAPH_W, RED, DGRAY, 1, 2);

    LCD_setTextColor(YELLOW);

    plotX = 0;
    while (plotX < GRAPH_W) {
        plotY        = adcDmaData[plotX];
        plotyHistory = adcOldValues[plotX];
        LCD_drawLine(plotX + graphPosX, plotYHistoryOld + graphPosY, (u16) (plotX + graphPosX),
                     plotyHistory + graphPosY, DGRAY);
        LCD_drawLine(plotX + graphPosX, plotYOld + graphPosY, (u16) (plotX + graphPosX), plotY + graphPosY, GREEN);
        adcOldValues[plotX] = plotY;

        plotX++;
        plotYOld        = plotY;
        plotYHistoryOld = plotyHistory;
    }
}

int main(void) {
    u16 encVal,
        encOldVal = 0;

    LCD_init();
    ADC_init();
    PWM_init();
    ENC_init();
    ENC_setValue(1710);

    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(BLACK);

    lcdWidth  = LCD_getWidth();
    lcdHeight = LCD_getHeight();
    graphPosX = (lcdWidth - (u16) GRAPH_W) / (u8) 2;
    graphPosY = (lcdHeight - (u16) GRAPH_H) / (u8) 2;

    LCD_fillRect(graphPosX, graphPosY, GRAPH_W, GRAPH_H, DGRAY);

    plotYMax = plotYMin = GRAPH_H / 2;
    for (u16 i = 0; i < ADC_DATA_SIZE; ++i) {
        adcOldValues[i] = GRAPH_H / 2;
    }

    while (1) {
        while (!ADC_isDataAvailable());
        plotData();

        encVal = ENC_getValue();
        if (encOldVal != encVal) {
            ADC_changeFreq((u32) (encVal % 17));
            LCD_fillRect(0, (u16) (graphPosY + GRAPH_H), LCD_getWidth(), graphPosY, BLACK);
            LCD_setTextBgColor(BLACK);
            LCD_setCursor(0, 221);
            LCD_writeString((unsigned char *) "F mode: ");
            LCD_writeString((unsigned char *) ADC_getFreqMode());
            encOldVal = encVal;
        }
        ADC_markDataUsed();
//        delay_ms(120);
    }

    while (1);
}

