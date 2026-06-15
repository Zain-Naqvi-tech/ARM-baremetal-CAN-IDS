#include "systick.h"
#include "tm4c1294ncpdt.h"
#include <stddef.h>
#include "msp432e401y.h"
#include <stdint.h>

volatile uint32_t ticks = 0;

void SysTick_Init(void) {
	
	//Configure STRELOAD
	NVIC_ST_RELOAD_R = 119999; //For 1ms delay, we need 120,000 cycles. This can be found using 1ms/8.33ns at 120MHz clock speed. 
	
	//Configure STCURRENT
	NVIC_ST_CURRENT_R = 0; //writing any value to it to clear and reset the counter to zero before starting.  
	
	//Configure STCTRL - Enable systick counter, enable the SysTick Interrupt, and select the clock source. 
	NVIC_ST_CTRL_R |= 0x01; //Bit 0 set - Enables SysTick to operate in a multi-shot way.
	NVIC_ST_CTRL_R |= 0x02; //Bit 1 set - Enables the interrupt
	NVIC_ST_CTRL_R |= 0x04; //Bit 2 set - Selects the System Clock
	
	ticks = 0;
	
}

void SysTick_Handler(void) {
	
	ticks++;
	
}