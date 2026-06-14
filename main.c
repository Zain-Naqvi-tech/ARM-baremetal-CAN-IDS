#include <stdint.h>
#include "PLL.h"
#include "tm4c1294ncpdt.h"
#include "uart.h"
#include "gpio.h"
#include "can.h"
#include "systick.h"
#include "msp432e401y.h"
#include "attacker.h"

volatile uint32_t InterruptFlag = 0;
volatile Msg message[NUMBER_OF_EVENTS + 1];

volatile uint32_t RXFlag = 0;

char data;

bool suppressed;

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
		
		data = UART_InChar();
		if (RXFlag == 1) {
			//now we do case switch?
			switch (data)
			{
			case 'f': //injects TOO_FAST frames for THROTTLE
				INJECT_TOO_FAST(message, 1); //Injecting TOO_FAST frames frames for THROTTLE
				break;
			
			case 'o': //Injecting over_range values for ENGINE_RPM
				INJECT_OVER_RANGE(message, 0); //Injecting over_range values for ENGINE_RPM
				break;
			
			case 'u': //Injecting UNKNOWN ID frames for object 5
				INJECT_UNKNOWN_ID(message, 4); //Injecting UNKNOWN ID frames for object 5 (which is not used in the normal system)
				break;
			
			default:
				break;
			}
			
			RXFlag = 0;
		}
		
		for (int i = 0; i < NUMBER_OF_EVENTS; i++) {
			if (ticks - message[i].lastTransmitted >= message[i].period) {
				suppressed = INJECT_MISSING(2500, 4500, i);
				if (!suppressed) {
					CAN0_Transmit(message, i);
				}
				
				message[i].lastTransmitted += message[i].period;
			
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
					message[i].timeStamp += message[i].period;
					Message_Object_UART_Print(0, &message[i]);
					message[i].arrivalFlag = 0; //Resets the arrival flag to check for the next arrival
				}
			}
		}
		if (InterruptFlag & (1 << 4)) { //object 5 is flagged
			Message_Object_UART_Print(0,&message[4]);
			
			__disable_irq();
			InterruptFlag &= ~(1 << 4);
			__enable_irq();
			
		}
	}
	
}