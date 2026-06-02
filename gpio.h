#ifndef GPIO_H
#define GPIO_H

#include "tm4c1294ncpdt.h"
#include <stdint.h>

void PortN_Init(void);
void PortF_Init(void);
void GPIO_Init(void);

void LED1_ON(void);
void LED2_ON(void);
void LED3_ON(void);
void LED4_ON(void);

void LED1_OFF(void);
void LED2_OFF(void);
void LED3_OFF(void);
void LED4_OFF(void);

#endif