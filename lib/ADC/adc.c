#include "adc.h"

#define ADCx ADC1
#define ADC_Channel_n ADC_Channel_7
#define RCC_APBnPeriph_ADCx RCC_APB2Periph_ADC1
#define ADC_RCC_APBnPeriphClockCmd RCC_APB2PeriphClockCmd
#define ADC_ExternalTrigConv_Tn_trigger ADC_ExternalTrigConv_T3_TRGO

#define GPIOx GPIOA
#define GPIO_Pin_n GPIO_Pin_7
#define RCC_APBnPeriph_GPIOx RCC_APB2Periph_GPIOA
#define GPIO_RCC_APBnPeriphClockCmd RCC_APB2PeriphClockCmd

#define TIMx TIM3
#define TIM_RCC_APBnPeriphClockCmd RCC_APB1PeriphClockCmd
#define RCC_APBnPeriph_TIMx RCC_APB1Periph_TIM3


#define DMAn_Channeln_IRQn DMA1_Channel1_IRQn
#define DMAn_IT_TCn DMA1_IT_TC1
#define DMAn_Channeln DMA1_Channel1
#define RCC_AHBPeriph_DMAn RCC_AHBPeriph_DMA1

static u16 adcDmaData[ADC_DATA_SIZE];

u16 *ADC_getData() {
    return adcDmaData;
}

void timer_config(void) {
    TIM_TimeBaseInitTypeDef tim;
    TIM_RCC_APBnPeriphClockCmd(RCC_APBnPeriph_TIMx, ENABLE);

    /* Time base configuration */
    TIM_TimeBaseStructInit(&tim);
    tim.TIM_Period        = (u16) (ADC_PERIOD - 1);//875 - 1;
    tim.TIM_Prescaler     = 0;
    tim.TIM_ClockDivision = 0;
    tim.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIMx, &tim);

    TIM_SelectOutputTrigger(TIMx, TIM_TRGOSource_Update);
}

void timer_start(void) {
    TIM_Cmd(TIMx, ENABLE);
}

void timer_stop(void) {
    TIM_Cmd(TIMx, DISABLE);
}

void dma_init() {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMAn, ENABLE);
    // DMA
    DMA_InitTypeDef dma;
    DMA_StructInit(&dma);

    dma.DMA_BufferSize         = ADC_DATA_SIZE;
    dma.DMA_MemoryBaseAddr     = (u32) adcDmaData;
    dma.DMA_PeripheralBaseAddr = (u32) &(ADCx->DR);
    dma.DMA_Mode               = DMA_Mode_Circular;
    dma.DMA_Priority           = DMA_Priority_High;
    dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_Init(DMAn_Channeln, &dma);

    NVIC_EnableIRQ(DMAn_Channeln_IRQn);
    DMA_ITConfig(DMAn_Channeln, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMAn_Channeln, ENABLE);
}

void gpio_init() {
    GPIO_RCC_APBnPeriphClockCmd(RCC_APBnPeriph_GPIOx, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Pin   = GPIO_Pin_n;
    gpio.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOx, &gpio);
}

void adc_init() {

//clock for ADC (max 14MHz --> 72/6=12MHz)
    //TODO: check for maximum clock
    //RCC_ADCCLKConfig(RCC_PCLK2_Div2);
    ADC_RCC_APBnPeriphClockCmd(RCC_APBnPeriph_ADCx, ENABLE);

    // ADC
    ADC_InitTypeDef adc;
    adc.ADC_ScanConvMode       = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_Tn_trigger;
    adc.ADC_DataAlign          = ADC_DataAlign_Right;
    adc.ADC_Mode               = ADC_Mode_Independent;
    adc.ADC_NbrOfChannel       = 1;
    ADC_Init(ADCx, &adc);

    ADC_RegularChannelConfig(ADCx, ADC_Channel_n, 1, ADC_SampleTime_1Cycles5);

    ADC_Cmd(ADCx, ENABLE);   //enable ADCx

    //Calibration
    ADC_ResetCalibration(ADCx);   // Reset previous calibration
    while (ADC_GetResetCalibrationStatus(ADCx));
    ADC_StartCalibration(ADCx);   // Start new calibration (ADC must be off at that time)
    while (ADC_GetCalibrationStatus(ADCx));

    ADC_ExternalTrigConvCmd(ADCx, ENABLE);
    ADC_DMACmd(ADCx, ENABLE);
    ADC_Cmd(ADCx, ENABLE);
}

void ADC_init() {
    gpio_init();
    dma_init();
    adc_init();
    timer_config();
    timer_start();
}

static u8 dataAvailable = 0;

inline u8 isDataAvailable() {
    return dataAvailable;
}

inline void markDataUsed() {
    dataAvailable = 0;
    timer_start();
}

void DMA1_Channel1_IRQHandler(void) {
    if (DMA_GetITStatus(DMAn_IT_TCn) == SET) {
//        DMA_Cmd(DMA1_Channel1, DISABLE);
        DMA_ClearITPendingBit(DMAn_IT_TCn);
        timer_stop();
        dataAvailable = 1;
    }
}
