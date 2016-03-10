#ifndef TEST2_ADC_H
#define TEST2_ADC_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_dma.h>

#define ADC_DATA_SIZE 320

void ADC_init();
__IO u16 *ADC_getData();


#endif //TEST2_ADC_H
