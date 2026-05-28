#include "gpio.h"

void PortN_Init(void) {
	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12; //enabling clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){}; //allow time for clock
	
	GPIO_PORTN_DIR_R |= 0x03; //Enables the first and second bits to be outputs
	GPIO_PORTN_DEN_R |= 0x03; //Enables Digital I/O on Port N
		
	return; 

}

void PortF_Init(void) {
	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; //enabling clock for Port F
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){}; //allow time for clock
	
	GPIO_PORTF_DIR_R |= 0x11; //Enables the first and fourth bits to be outputs
	GPIO_PORTF_DEN_R |= 0xFF; //Enables Digital I/O on Port F
	
}