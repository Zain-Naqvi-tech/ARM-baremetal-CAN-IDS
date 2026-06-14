#include "attacker.h"
#include "tm4c1294ncpdt.h"

void INJECT_TOO_FAST(volatile Msg* message, uint32_t index) {

	CAN0_Transmit(message, index); //attacks the bus with the THROTTLE 

}

void INJECT_OVER_RANGE(volatile Msg* message, uint32_t index) {
	
	//Inject RPM value of 6500 (0x1964)
	message[index].payload[0] = 0x19; //Set value for RPM Test
	message[index].payload[1] = 0x64; //Set value for RPM Test

}

bool INJECT_MISSING(uint32_t startTime, uint32_t stopTime, uint32_t index) {

	if ((ticks >= startTime) && (ticks <= stopTime) && (index == 2)) {//targeting index=2, VEHICLE_SPEED 
		return 1; //Being Attacked
	}
	
	else {
		return 0; //Not being attacked
	}
	
}

void INJECT_UNKNOWN_ID(volatile Msg* message, uint32_t index) {
	
	CAN0_Transmit(message, index); //transmits the unknown ID message. 
	
}