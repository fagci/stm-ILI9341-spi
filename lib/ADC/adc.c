#include <stm32f10x_dma.h>
#include <stm32f10x_adc.h>
#include "adc.h"
#include "../ILI9341/graph.h"

__IO u16 adcDmaData[ADC_DATA_SIZE];

__IO u16 *ADC_getData() {
    return adcDmaData;
}

void ADC_init() {
    // PA7

    //clock for ADC (max 14MHz --> 72/6=12MHz)
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA
    DMA_InitTypeDef dma;
    DMA_StructInit(&dma);

    dma.DMA_BufferSize         = ADC_DATA_SIZE;
    dma.DMA_MemoryBaseAddr     = (u32) adcDmaData;
    dma.DMA_PeripheralBaseAddr = (u32) &(ADC1->DR);
    dma.DMA_Mode               = DMA_Mode_Circular;
    dma.DMA_Priority           = DMA_Priority_High;
    dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_Init(DMA1_Channel1, &dma);

    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);


    // ADC pin
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Pin   = GPIO_Pin_7;
    gpio.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio);


    // ADC
    ADC_InitTypeDef adc;
    adc.ADC_ScanConvMode       = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign          = ADC_DataAlign_Right;
    adc.ADC_Mode               = ADC_Mode_Independent;
    adc.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &adc);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_1Cycles5);

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);   //enable ADC1

    //Calibration
    ADC_ResetCalibration(ADC1);   // Reset previous calibration
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);   // Start new calibration (ADC must be off at that time)
    while (ADC_GetCalibrationStatus(ADC1));

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
}

static void DMA1_Channel1_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC1) == SET) {
        DMA_Cmd(DMA1_Channel1, DISABLE);
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}
