#include <stdint.h>
#include "PLL.h"
#include "tm4c1294ncpdt.h"
#include "uart.h"
#include "gpio.h"
#include "can.h"
#include "systick.h"

Msg message[NUMBER_OF_EVENTS];

int main(void) {

	PLL_Init(); //Sets the clock speed at 120MHz
	SysTick_Init(); //Initialize SysTick 
	GPIO_Init(); //Initialize GPIO for Port A and Port B for CAN functionality
	UART_Init(); //Initialize UART2
	PortN_Init(); //Initialize Port N
	PortF_Init(); //Initialize Port F
	CAN0_Init(); //Initialize CAN0 as TX
	CAN1_Init(); //Initialize CAN1 as RX
	CAN_Message_Table_Init(message); 
	CAN0_TX_Setup(message);
	while (1) {
		for (int i = 0; i < NUMBER_OF_EVENTS; i++) {
			if (ticks - message[i].lastTransmitted >= message[i].period) {
				CAN0_Transmit(message, i);
				message[i].lastTransmitted += message[i].period;
			}
		}
	}
	
}