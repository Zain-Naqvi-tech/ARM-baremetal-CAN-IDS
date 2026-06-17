#include "can.h"
#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "gpio.h"
#include "uart.h"
#include "systick.h"
#include "msp432e401y.h"

void CAN_Message_Table_Init(volatile Msg* msg) {

        //Populating the simulated ENGINE RPM Message object
        msg[ENGINE_RPM].canID = 0x100;
        msg[ENGINE_RPM].period = 500; //500ms of difference between each message transmission
        msg[ENGINE_RPM].DLC = 8;
        msg[ENGINE_RPM].lastTransmitted = 0;
        msg[ENGINE_RPM].overrunFlag = 0;
        msg[ENGINE_RPM].timeStamp = 0;
        msg[ENGINE_RPM].maxValue = 6000; //Maximum allowed RPM
        msg[ENGINE_RPM].valueMargin = 20; //Allowed margin of error for the timing (20ms)
        msg[ENGINE_RPM].arrivalFlag = 0;
        msg[ENGINE_RPM].lastArrived = 0;
		//800 rpm -> 0x0320
				msg[ENGINE_RPM].payload[0] = 0x03;
        msg[ENGINE_RPM].payload[1] = 0x20;
				msg[ENGINE_RPM].lastValue = 800;
				msg[ENGINE_RPM].maxMargin = 500;
		
		//Populating the simulated THROTTLE Message object
		msg[THROTTLE].canID = 0x200;
        msg[THROTTLE].period = 1000; //1000ms of difference between each message transmission
        msg[THROTTLE].DLC = 8;
        msg[THROTTLE].lastTransmitted = 0;
        msg[THROTTLE].overrunFlag = 0;
        msg[THROTTLE].timeStamp = 0;
		msg[THROTTLE].maxValue = 100; //100 percent throttle is the maximum limit
		msg[THROTTLE].valueMargin = 100; //Allowed margin of error for the timing (100ms)
        msg[THROTTLE].arrivalFlag = 0;
        msg[THROTTLE].lastArrived = 0;
		//90 percent throttle
        msg[THROTTLE].payload[1] = 0x5A;
				msg[THROTTLE].lastValue = 90;
				msg[THROTTLE].maxMargin = 40;

        //Populating the simulated VEHICLE SPEED Message object
        msg[VEHICLE_SPEED].canID = 0x300;
        msg[VEHICLE_SPEED].period = 2000; //2000ms of difference between each message transmission
        msg[VEHICLE_SPEED].DLC = 8;
        msg[VEHICLE_SPEED].lastTransmitted = 0;
        msg[VEHICLE_SPEED].overrunFlag = 0;
        msg[VEHICLE_SPEED].timeStamp = 0;
		msg[VEHICLE_SPEED].maxValue = 255; //km per hour
		msg[VEHICLE_SPEED].valueMargin = 200; //Allowed margin of error for timing (200ms)	
        msg[VEHICLE_SPEED].arrivalFlag = 0;
        msg[VEHICLE_SPEED].lastArrived = 0;
		//120km/h
        msg[VEHICLE_SPEED].payload[1] = 0x78;
				msg[VEHICLE_SPEED].lastValue = 120;
				msg[VEHICLE_SPEED].maxMargin = 30;

        //Populating the simulated COOLANT TEMP Message object
        msg[COOLANT_TEMP].canID = 0x400;
        msg[COOLANT_TEMP].period = 4000; //4000ms of difference between each message transmission
        msg[COOLANT_TEMP].DLC = 8;
        msg[COOLANT_TEMP].lastTransmitted = 0;
        msg[COOLANT_TEMP].overrunFlag = 0;
        msg[COOLANT_TEMP].timeStamp = 0;
        msg[COOLANT_TEMP].maxValue = 120; //Celcius
        msg[COOLANT_TEMP].valueMargin = 400; //Allowed margin of error for timing (400ms)
        msg[COOLANT_TEMP].arrivalFlag = 0;
        msg[COOLANT_TEMP].lastArrived = 0;
		//30 degrees C
        msg[COOLANT_TEMP].payload[1] = 0x1E;
				msg[COOLANT_TEMP].lastValue = 30;
				msg[COOLANT_TEMP].maxMargin = 10; //10 degree 
		
}

void CAN0_TX_Setup(volatile Msg* msg) {
	
		for (int i = 0; i < NUMBER_OF_EVENTS; i++) {

			CAN0_IF1CMSK_R = CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_ARB | CAN_IF1CMSK_CONTROL; //Setting the WRNRD, ARB, and CONTROL bits as a one-time assignment. The XTD bit is cleared so we know it is 11-bit
			CAN0_IF1ARB2_R = (msg[i].canID << 2) | CAN_IF1ARB2_MSGVAL | CAN_IF1ARB2_DIR; //This puts the specific CANID in the Arbitration register. It also enables transmission and is ready to be considered by the message handler
			CAN0_IF1MCTL_R = msg[i].DLC | CAN_IF1MCTL_EOB; //Indicates end of buffer and configures the DLC of the specific event
		
			CAN0_IF1CRQ_R = ((i + 1) << CAN_IF1CRQ_MNUM_S); //Message object i (1,2,3,4) - this creates the distinction in hardware
			while ((CAN0_IF1CRQ_R & CAN_IF1CRQ_BUSY) != 0) {} //polls until BUSY clears
			
		}
		
		CAN0_IF1CMSK_R = CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_ARB | CAN_IF1CMSK_CONTROL; //Setting the WRNRD, ARB, and CONTROL bits as a one-time assignment. The XTD bit is cleared so we know it is 11-bit
		CAN0_IF1ARB2_R = (0x500 << 2) | CAN_IF1ARB2_MSGVAL | CAN_IF1ARB2_DIR; //This puts the specific CANID in the Arbitration register. It also enables transmission and is ready to be considered by the message handler
		CAN0_IF1MCTL_R = message[4].DLC | CAN_IF1MCTL_EOB; //Indicates end of buffer and configures the DLC of the specific event
	
		CAN0_IF1CRQ_R = ((4 + 1) << CAN_IF1CRQ_MNUM_S); //Message object (index + 1)
		while ((CAN0_IF1CRQ_R & CAN_IF1CRQ_BUSY) != 0) {} //polls until BUSY clears
			
}

//This is TX
void CAN0_Init(void) {

    //RCGCCAN Register setup. It is used to let software enable and disable the clock to the CAN module
    SYSCTL_RCGCCAN_R |= SYSCTL_RCGCCAN_R0; //enables clock for CAN0
    while ((SYSCTL_PRCAN_R&SYSCTL_PRCAN_R0) == 0) {} //waits until bit 0 is set

    CAN0_CTL_R |= CAN_CTL_INIT; //INIT bit set to 1
    CAN0_CTL_R |= CAN_CTL_CCE; //CCE bit set to 1

    //Calculated values for the CANBIT Register
    uint32_t brp = 14; //14
    uint32_t sjw = 1 - 1; //0
    uint32_t tseg1 = 12 - 1; //11
    uint32_t tseg2 = 3 - 1; //2

    //CAN0_BIT_R is the CAN Bit Timing Register. [0:5] is the BRP field, [6:7] is the SJW field, [8:11] is the TSEG1 field, and [12:14] is the TSEG2 field
    CAN0_BIT_R = (brp << CAN_BIT_BRP_S) | (sjw << CAN_BIT_SJW_S) | (tseg1 << CAN_BIT_TSEG1_S) | (tseg2 << CAN_BIT_TSEG2_S);
    //CAN0_BIT_R = (brp << 0) || (sjw << 6) || (tseg1 << 8) || (tseg2 << 12);

    CAN0_CTL_R &= ~CAN_CTL_CCE; //Clear CCE bit 
    CAN0_CTL_R &= ~CAN_CTL_INIT; //Clear INIT bit to leave initialisation stage
			
		LED1_ON(); //Turn on LED1 to indicate that the initialisation was a success
		
}

//A clarification on the Interface registers. IF1 is used for TX (CAN0) and IF2 is used for RX (CAN1). 
//Setting up CAN0 for TX
void CAN0_Transmit(volatile Msg* message, uint32_t index) {

    //Set up CAN0 for TX
    CAN0_IF1CMSK_R = CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB | CAN_IF1CMSK_TXRQST; //Set the write not read bit to transfer the data from the interface registers specified by the MNUM in the command request
		//The TXRQST bit is responsible for starting the transmission in this case

    //add payload
    CAN0_IF1DA1_R = (message[index].payload[0]) | (message[index].payload[1] << 8); //Low byte >> 8 OR High Byte << 8 - similar to the way we extracted at first 
    CAN0_IF1DA2_R = (message[index].payload[2]) | (message[index].payload[3] << 8);
    CAN0_IF1DB1_R = (message[index].payload[4]) | (message[index].payload[5] << 8);
    CAN0_IF1DB2_R = (message[index].payload[6]) | (message[index].payload[7] << 8);
		
		CAN0_IF1CRQ_R = ((index + 1) << CAN_IF1CRQ_MNUM_S); //Message object index
    while ((CAN0_IF1CRQ_R & CAN_IF1CRQ_BUSY) != 0) {} //polls until BUSY clears
			
		LED3_ON();

}

void CAN1_RX_Setup(volatile Msg* msg) {
	
		for (int i = 0; i < NUMBER_OF_EVENTS; i++) {
	
			CAN1_IF2CMSK_R = CAN_IF2CMSK_WRNRD | CAN_IF2CMSK_ARB | CAN_IF2CMSK_CONTROL; //Set WRNRD bit to transfer the data in the interface register to the CAN message object in the Command Request Register. Transfer the data in the CANIF2. Set ARB (Access Arbitration Bits) bit to 1 to Transfer ID + DIR + XTD + MSGVAL of the message object into the Interface registers. Set CONTROL bit to 1 to  Transfer control bits from the CANIFnMCTL register into the Interface registers.
			CAN1_IF2ARB2_R = CAN_IF2ARB2_MSGVAL | (msg[i].canID << 2); //Set the MSGVAL bit to 1 to show that the message is valid. Shift CANID left by two spaces to ensure that it lands in bits [2:12] of the ARB2 register
			CAN1_IF2MCTL_R = 0x08 | CAN_IF2MCTL_EOB | CAN_IF2MCTL_RXIE; //Set the DLC value (8 bytes). Set EOB to show end of buffer. Set the receive interrupt enable bit
			//This also keeps the TXRQST bit clear to prevent a remote frame transmission
	
			//write object number to CANIF2CRQ Register's MNUM and wait for BUSY to clear
			CAN1_IF2CRQ_R = ((i + 1) << CAN_IF2CRQ_MNUM_S);
			while ((CAN1_IF2CRQ_R & CAN_IF2CRQ_BUSY) != 0) {} //polls until BUSY clears
				
		}
		
			//OBJECT 5
			//This is where the Catch-all object goes. This is used for catching unkown IDs on the BUS. This is the NUMBER_OF_EVENTS+1 object
			CAN1_IF2CMSK_R = CAN_IF2CMSK_WRNRD | CAN_IF2CMSK_ARB | CAN_IF2CMSK_CONTROL | CAN_IF2CMSK_MASK; //Set the MASK bit as well so the transfer writes the mask registers into the object
			CAN1_IF2ARB2_R = CAN_IF2ARB2_MSGVAL; //Set the MSGVAL to show that the message is valid. No ID
			
			//We work with one more registers responsible for Masking
			CAN1_IF2MSK2_R &= ~CAN_IF2MSK2_IDMSK_M; //clear bits 0-12. Leave the rest
		
			CAN1_IF2MCTL_R = 0x08 | CAN_IF2MCTL_EOB | CAN_IF2MCTL_RXIE | CAN_IF2MCTL_UMASK; //Setting the UMASK bit to use mask for acceptance filtering
		
			//write object number to CANIF2CRQ Register's MNUM and wait for BUSY to clear
			CAN1_IF2CRQ_R = ((5) << CAN_IF2CRQ_MNUM_S); //Object number 5
			while ((CAN1_IF2CRQ_R & CAN_IF2CRQ_BUSY) != 0) {} //polls until BUSY clears
			//OBJECT 5 ENDS
		
			CAN1_CTL_R = CAN_CTL_IE; //Set bit 1 of CANCTL register to enable CAN interrupts
				
			//Set up NVIC CAN1_IRQHandler
			NVIC_SetPriority(CAN1_IRQn, 0xFF); //Set PendSV to the lowest possible priority. 
		
			//Enable
			NVIC_EnableIRQ(CAN1_IRQn);
	
}

//This is RX
void CAN1_Init(void) {

    //RCGCCAN Register setup. It is used to let software enable and disable the clock to the CAN module
    SYSCTL_RCGCCAN_R |= SYSCTL_RCGCCAN_R1; //enables clock for CAN1
    while ((SYSCTL_PRCAN_R&SYSCTL_PRCAN_R1) == 0) {} //waits until bit 1 is set

    CAN1_CTL_R |= CAN_CTL_INIT; //INIT bit set to 1
    CAN1_CTL_R |= CAN_CTL_CCE; //CCE bit set to 1

    //Calculated values for the CANBIT Register
    uint32_t brp = 14; //14. This value is not subtracted by 1 by the register
    uint32_t sjw = 1 - 1; //0
    uint32_t tseg1 = 12 - 1; //11
    uint32_t tseg2 = 3 - 1; //2

    //CAN1_BIT_R is the CAN Bit Timing Register. [0:5] is the BRP field, [6:7] is the SJW field, [8:11] is the TSEG1 field, and [12:14] is the TSEG2 field
    CAN1_BIT_R = (brp << CAN_BIT_BRP_S) | (sjw << CAN_BIT_SJW_S) | (tseg1 << CAN_BIT_TSEG1_S) | (tseg2 << CAN_BIT_TSEG2_S);
    //CAN1_BIT_R = (brp << 0) | (sjw << 6) | (tseg1 << 8) | (tseg2 << 12);

    CAN1_CTL_R &= ~CAN_CTL_CCE; //Clear CCE bit 
    CAN1_CTL_R &= ~CAN_CTL_INIT; //Clear INIT bit to leave initialisation stage
			
    LED2_ON(); //Turn on LED2 to indicate success

}

//Setting up CAN1 Interrupt Service Routine to receive and pass the message object forward
void CAN1_IRQHandler(void) {
		
		uint32_t messageObject = CAN1_INT_R & CAN_INT_INTID_M; //extracts object number
	
		if (messageObject == 0x8000) {
				//read the cansts register
				uint32_t var = CAN1_STS_R; //the act of reading it clears the STS register
		}
	
		else {
		
			//We need to check the message object. If it reaches 0, we leave the loop
			while (messageObject > 0) {
	
				CAN1_IF2CMSK_R |= CAN_IF2CMSK_CLRINTPND; //Set CLRINTPND bit to 1 to clear the interrupt pending bit for the message object
		
				CAN1_IF2CMSK_R &= ~CAN_IF2CMSK_WRNRD; //Set WRNRD bit to 0 to read. Transfer the data in the CANIF2 
				CAN1_IF2CMSK_R |= CAN_IF2CMSK_ARB; //Set ARB (Access Arbitration Bits) bit to 1 to Transfer ID + DIR + XTD + MSGVAL of the message object into the Interface registers
				CAN1_IF2CMSK_R |= CAN_IF2CMSK_CONTROL; //Set CONTROL bit to 1 to  Transfer control bits from the CANIFnMCTL register into the Interface registers.
				CAN1_IF2CMSK_R |= CAN_IF2CMSK_DATAA;
				CAN1_IF2CMSK_R |= CAN_IF2CMSK_DATAB;
				CAN1_IF2CMSK_R |= CAN_IF2CMSK_NEWDAT; //Set NEWDAT bit to 1 to transfer the new data received into the Interface registers
		
				CAN1_IF2CRQ_R = messageObject; //write object number. 
				while ((CAN1_IF2CRQ_R & CAN_IF2CRQ_BUSY) != 0) {} //polls until BUSY clears
			
				uint32_t index = messageObject - 1; //Extracts the object number into a variable (index friendly)

				message[index].canID = (CAN1_IF2ARB2_R >> 2) & 0x7FF; //get the message ID and save it to the struct field
				message[index].DLC = CAN1_IF2MCTL_R & 0x0000000F; //extract the first 4 bits of the MCTL register to get the DLC (Data length code) - Supposed to be 8 bytes based on the transmit function
				message[index].payload[0] = CAN1_IF2DA1_R & 0x00FF; //Low Byte
				message[index].payload[1] = (CAN1_IF2DA1_R >> 8) & 0xFF; //High Byte
				message[index].payload[2] = CAN1_IF2DA2_R & 0x00FF;
				message[index].payload[3] = (CAN1_IF2DA2_R >> 8) & 0xFF;
				message[index].payload[4] = CAN1_IF2DB1_R & 0x00FF;
				message[index].payload[5] = (CAN1_IF2DB1_R >> 8) & 0xFF;
				message[index].payload[6] = CAN1_IF2DB2_R & 0x00FF;
				message[index].payload[7] = (CAN1_IF2DB2_R >> 8) & 0xFF;
				message[index].timeStamp = ticks;
					
				uint16_t payloadValue = message[index].payload[0] << 8| message[index].payload[1];
					
				//Check value for OVER_RANGE status
				if (payloadValue >= message[index].maxValue) {
					message[index].status = OVER_RANGE;
				}
        else {
           message[index].status = OK;
        }
				
				if (messageObject == 5) {
					message[index].status = UNKNOWN_ID;
				}
					
				InterruptFlag |= (1 << index); //sets the specific bit of the bitmask to show which index to use in main
								
				messageObject = CAN1_INT_R & CAN_INT_INTID_M;
	
			}
		}
		
		return;
			
}