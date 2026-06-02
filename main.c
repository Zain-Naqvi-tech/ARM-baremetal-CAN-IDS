#include <stdint.h>
#include "PLL.h"
#include "tm4c1294ncpdt.h"
#include "uart.h"
#include "gpio.h"
#include "can.h"

int main(void) {

	PLL_Init(); //Sets the clock speed at 120MHz
	GPIO_Init(); //Initializes GPIO for Port A and Port B for CAN functionality
	//UART_Init();
	PortN_Init(); //Initializes Port N
	PortF_Init(); //Initializes Port F
	CAN0_Init(); //Initialise CAN0 as TX
	CAN1_Init(); //Initialise CAN1 sa RX
	CAN0_Transmit(); //Send Data out of CAN0
	
}