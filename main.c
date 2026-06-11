#include <stdint.h>
#include "PLL.h"
#include "tm4c1294ncpdt.h"
#include "uart.h"
#include "gpio.h"
#include "can.h"
#include "systick.h"
#include "msp432e401y.h"

volatile uint32_t InterruptFlag = 0;
volatile Msg message[NUMBER_OF_EVENTS];

volatile uint32_t attackerTime_F = 0 ;
volatile uint32_t attackerTime_M = 0;

int main(void) {

	PLL_Init(); //Sets the clock speed at 120MHz
	SysTick_Init(); //Initialize SysTick 
	GPIO_Init(); //Initialize GPIO for Port A and Port B for CAN functionality
	UART_Init(); //Initialize UART2
	PortN_Init(); //Initialize Port N
	PortF_Init(); //Initialize Port F
	CAN0_Init(); //Initialize CAN0 as TX
	CAN1_Init(); //Initialize CAN1 as RX
	CAN_Message_Table_Init(message); //Set up the table which initializes all the event's message structs seperately
	CAN0_TX_Setup(message); //Conducts a one-time initialization for the registers responsible for TX
	CAN1_RX_Setup(message); //Conducts a one-time initialization for the registers responsible for RX
	while (1) {
		
		for (int i = 0; i < NUMBER_OF_EVENTS; i++) {
			if (ticks - message[i].lastTransmitted >= message[i].period) {
				CAN0_Transmit(message, i);
				message[i].lastTransmitted += message[i].period;
			}
						
			//THIS IS WHERE THE ATTACKER WORKS FOR TOO_FAST
			if (ticks - attackerTime_F >= 1600) {
				CAN0_Transmit(message, 1); //attacks the bus with the THROTTLE message every 1600 ms
				attackerTime_F += 1600;
			}
			//ATTACKER ENDS FOR TOO_FAST
			
			//THIS IS WHERE THE ATTACKER WORKS FOR MISSING
			if (ticks - attackerTime_M >= 5000) {
				
			}

			if (InterruptFlag & (1 << i)) {
				
				uint32_t delta = ticks - message[i].lastArrived; //Finds the difference between the current time and the last time 
				
				if (message[i].arrivalFlag == 0) {
					Message_Object_UART_Print(0, &message[i]); 
					message[i].arrivalFlag = 1;
					message[i].lastArrived = ticks;
					
					__disable_irq();
					InterruptFlag &= ~(1 << i);
					__enable_irq();
				}
				
				else {
				
					if (delta < message[i].period - message[i].valueMargin) //Too fast
					{
						message[i].status = TOO_FAST;
					}
					
					message[i].lastArrived = ticks;
					Message_Object_UART_Print(0, &message[i]); 
					
					//We need to disable interrupts for a read-modify-write sequence in order to ensure we do not miss any messages
					__disable_irq();
					InterruptFlag &= ~(1 << i);
					__enable_irq();
				
				}
 				
			}

			//Watchdog which runs on the events that have already been received once and periodically checks if they are even arriving (and on time)
			if (message[i].arrivalFlag == 1) {
				uint32_t delta = ticks - message[i].lastArrived;
				if (delta > message[i].period + message[i].valueMargin) { //Slow or MISSING
					message[i].status = MISSING;
					Message_Object_UART_Print(0, &message[i]);
					message[i].arrivalFlag = 0; //Resets the arrival flag to check for the next arrival
				}
			}
		}
	}
	
}