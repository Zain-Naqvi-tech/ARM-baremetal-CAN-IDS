#include "gpio.h"
#include "tm4c1294ncpdt.h"
#include <stdint.h>

void PortN_Init(void) {
	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12; //enabling clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){}; //allow time for clock
	
	GPIO_PORTN_DIR_R |= 0x03; //Enables the first and second bits to be outputs
	GPIO_PORTN_DEN_R |= 0x03; //Enables Digital I/O on Port N
	
}

void PortF_Init(void) {
	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; //enabling clock for Port F
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){}; //allow time for clock
	
	GPIO_PORTF_DIR_R |= 0x11; //Enables the first and fourth bits to be outputs
	GPIO_PORTF_DEN_R |= 0xFF; //Enables Digital I/O on Port F
	
}

void GPIO_Init(void) {

  //PORT A
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0; //Enables clock for Port A
  while ((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R0) == 0) {} //waits until bit 0 is set
	GPIO_PORTA_AFSEL_R |= 0x03; //enables pin to peripheral feature
	GPIO_PORTA_PCTL_R |= 0x00000077; //write 7 into PMC0 (port A)
	GPIO_PORTA_DEN_R |= 0x03; //enable digital function
		
	//PORT B
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; //Enables clock for Port B
	while ((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R1) == 0) {} //waits until bit 1 is set
	GPIO_PORTB_AFSEL_R |= 0x03; //enables pin to peripheral feature
	GPIO_PORTB_PCTL_R |= 0x00000077; //write 7 into PMC1 (port B)
	GPIO_PORTB_DEN_R |= 0x03; //enable digital function
		
}

void LED1_ON(void) {
	GPIO_PORTN_DATA_R |= 0x02;
}

void LED2_ON(void) {
	GPIO_PORTN_DATA_R |= 0x01;
}

void LED3_ON(void) {
	GPIO_PORTF_DATA_R |= 0x10;
}

void LED4_ON(void) {
	GPIO_PORTF_DATA_R |= 0x01;
}

void LED1_OFF(void) {
	GPIO_PORTN_DATA_R &= ~0x02;
}

void LED2_OFF(void) {
	GPIO_PORTN_DATA_R &= ~0x01;
}

void LED3_OFF(void) {
	GPIO_PORTF_DATA_R &= ~0x10;
}

void LED4_OFF(void) {
	GPIO_PORTF_DATA_R &= ~0x01;
}