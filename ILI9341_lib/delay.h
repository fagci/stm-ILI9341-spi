#ifndef DELAY_H
#define    DELAY_H

#include "stm32f10x.h"

#define _XTAL_FREQ 24L

#define delay_us(val) _delay((val * _XTAL_FREQ) / 7 )
#define delay_ms(val) do{volatile unsigned int a=val; while(a--) delay_us(1000); }while(0)

void _delay(unsigned long i);

#endif	/* DELAY_H */
