#include <stdint.h>
#include "PLL.h"
#include "tm4c1294ncpdt.h"
#include "uart.h"
#include "gpio.h"
#include "can.h"

Msg message;

int main(void) {

	PLL_Init(); //Sets the clock speed at 120MHz
	GPIO_Init(); //Initialize GPIO for Port A and Port B for CAN functionality
	UART_Init(); //Initialize UART2
	PortN_Init(); //Initialize Port N
	PortF_Init(); //Initialize Port F
	CAN0_Init(); //Initialize CAN0 as TX
	CAN1_Init(); //Initialize CAN1 as RX
	CAN0_Transmit(); //Send Data out of CAN0
	CAN1_Receive(&message); //Extract the data from the registers
	
}