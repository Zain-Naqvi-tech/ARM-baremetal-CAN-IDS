#include "dwt.h"
#include "tm4c1294ncpdt.h"
#include "msp432e401y.h"
#include "uart.h"
#include <stdio.h>

//Initialize the DWT unit on the MCU
void DWT_Init(void) {
	
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; //Enable the TRCENA bit in the DEMCR register to enable the overall block which DWT uses
	DWT->CYCCNT = 0; //Reset the register. This is for initialisation purposes
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; //Enables the free-running 32-bit cycle counter. It is used for time measurement
	
}