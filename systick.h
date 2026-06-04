#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

extern volatile uint32_t ticks;
void SysTick_Init(void);

#endif