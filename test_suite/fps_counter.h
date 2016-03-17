#ifndef TEST1_FPS_COUNTER_H
#define TEST1_FPS_COUNTER_H

#include <stm32f10x.h>

void FPS_start();
void FPS_frame();
u16 FPS_getCount();

#endif //TEST1_FPS_COUNTER_H
