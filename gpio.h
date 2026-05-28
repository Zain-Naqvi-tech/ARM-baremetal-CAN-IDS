#ifndef GPIO_H
#define GPIO_H

#include "tm4c1294ncpdt.h"
#include <stdint.h>

void PortN_Init(void);
void PortF_Init(void);

//So now we need to add Ports A and B to this file (initialise them in terms of CAN)

#endif