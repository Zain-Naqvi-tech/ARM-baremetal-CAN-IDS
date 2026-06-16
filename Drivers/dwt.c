#include "dwt.h"
#include "tm4c1294ncpdt.h"
#include "msp432e401y.h"
#include "uart.h"
#include <stdio.h>

void DWT_Init(void) {
	
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; //Enable the TRCENA bit in the DEMCR register to enable the overall block which DWT uses
	DWT->CYCCNT = 0; //Reset the register. This is for initialisation purposes
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; //Enables the free-running 32-bit cycle counter. It is used for time measurement
	
	uint32_t value1 = DWT->CYCCNT; //read the first value
	uint32_t value2 = DWT->CYCCNT; //read the second value
	
	uint32_t DWT_TIME = value2 - value1;
	
	UART_printf("Time Difference = ");
	UART_numeric_print(DWT_TIME);
	
}