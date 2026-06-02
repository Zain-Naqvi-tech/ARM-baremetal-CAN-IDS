#include "can.h"
#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "gpio.h"

#define TX_MSG_OBJ 1
#define RX_MSG_OBJ 1
#define DA1 0x11
#define DA2 0x22
#define DB1 0x33
#define DB2 0x44

const uint16_t CanID = 0x100; //Specific CANID for a message

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

    //Set up CAN1 for RX. This needs to be done before anything ever transmits
    CAN1_IF2CMSK_R |= CAN_IF2CMSK_WRNRD; //Set WRNRD bit to 1 to write. Transfer the data in the CANIF2 register to the CAN message object
    CAN1_IF2CMSK_R |= CAN_IF2CMSK_ARB; //Set ARB (Access Arbitration Bits) bit to 1 to Transfer ID + DIR + XTD + MSGVAL of the message object into the Interface registers
    CAN1_IF2CMSK_R |= CAN_IF2CMSK_CONTROL; //Set CONTROL bit to 1 to  Transfer control bits from the CANIFnMCTL register into the Interface registers.

    CAN1_IF2ARB2_R &= ~CAN_IF2ARB2_DIR; //Clear DIR bit to RECEIVE
    CAN1_IF2ARB2_R |= (CanID << 2); //Lands in register [12:2]
    CAN1_IF2ARB2_R |= CAN_IF2ARB2_MSGVAL; //Set MSGVAL bit to 1 to indicate that the message object is valid
    CAN1_IF2ARB2_R &= ~CAN_IF2ARB2_XTD; //Clear XTD bit to indicate that this is a standard ID (11 bits)

    CAN1_IF2MCTL_R |= 0x08; //Set the DLC field to a data frame of 8 bytes
    CAN1_IF2MCTL_R |= CAN_IF2MCTL_EOB; //Set EOB bit to 1 to indicate that this is the last message object that will be received

    //write object number to CANIF2CRQ Register's MNUM and wait for BUSY to clear
    CAN1_IF2CRQ_R = (1 << CAN_IF2CRQ_MNUM_S);
    while ((CAN1_IF2CRQ_R & CAN_IF2CRQ_BUSY) != 0) {} //polls until BUSY clears
			
    LED2_ON(); //Turn on LED2 to indicate success

}

//A clarification on the Interface registers. IF1 is used for TX and IF2 is used for RX. 
//Setting up CAN0 for TX
void CAN0_Transmit(void) {

    //Set up CAN0 for TX
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_WRNRD; //Set WRNRD bit to 1 to write. Transfer the data in the CANIF2 register to the CAN message object
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_ARB; //Set ARB (Access Arbitration Bits) bit to 1 to Transfer ID + DIR + XTD + MSGVAL of the message object into the Interface registers
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_CONTROL; //Set CONTROL bit to 1 to  Transfer control bits from the CANIFnMCTL register into the Interface registers.
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_DATAA;
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_DATAB;

    //For future, build in one go or clear first then do this
    CAN0_IF1ARB2_R |= CAN_IF1ARB2_DIR; //Set DIR bit to TRANSMIT
    CAN0_IF1ARB2_R |= (CanID << 2); //Lands in register [12:2]
    CAN0_IF1ARB2_R |= CAN_IF1ARB2_MSGVAL; //Set MSGVAL bit to 1 to indicate that the message object is valid
    CAN0_IF1ARB2_R &= ~CAN_IF1ARB2_XTD; //Clear XTD bit to indicate that this is a standard ID (11 bits)

    CAN0_IF1MCTL_R |= 0x08; //Set the DLC field to a data frame of 8 bytes
    CAN0_IF1MCTL_R |= CAN_IF1MCTL_EOB; //Set EOB (End of buffer) bit to 1 to indicate that this is the last message object
    CAN0_IF1MCTL_R |= CAN_IF1MCTL_TXRQST; //Set TXRQST (transmit request) bit to 1 to request transmission of the message object

    //add payload - The pattern i'm expecting is [11 00 22 00 33 00 44 00]
    CAN0_IF1DA1_R = DA1;
    CAN0_IF1DA2_R = DA2;
    CAN0_IF1DB1_R = DB1;
    CAN0_IF1DB2_R = DB2;

    //write object number
    CAN0_IF1CRQ_R = (1 << CAN_IF1CRQ_MNUM_S);
    while ((CAN0_IF1CRQ_R & CAN_IF1CRQ_BUSY) != 0) {} //polls until BUSY clears
			
		LED3_ON();

}

//Setting up CAN1 to receive and show the message object
void CAN1_Receive(Msg* message) {
		
		while ((CAN1_NWDA1_R & (1 << CAN_NWDA1_NEWDAT_S)) == 0) {} //poll NEWDAT until a new frame is received. It is only received when the NEWDAT bit is set. 
		CAN1_IF2CMSK_R &= ~CAN_IF2CMSK_WRNRD; //Set WRNRD bit to 0 to read. Transfer the data in the CANIF2 
		CAN1_IF2CMSK_R |= CAN_IF2CMSK_ARB; //Set ARB (Access Arbitration Bits) bit to 1 to Transfer ID + DIR + XTD + MSGVAL of the message object into the Interface registers
		CAN1_IF2CMSK_R |= CAN_IF2CMSK_CONTROL; //Set CONTROL bit to 1 to  Transfer control bits from the CANIFnMCTL register into the Interface registers.
		CAN1_IF2CMSK_R |= CAN_IF2CMSK_DATAA;
		CAN1_IF2CMSK_R |= CAN_IF2CMSK_DATAB;
		CAN1_IF2CMSK_R |= CAN_IF2CMSK_CLRINTPND; //Set CLRINTPND bit to 1 to clear the interrupt pending bit for the message object
		CAN1_IF2CMSK_R |= CAN_IF2CMSK_NEWDAT; //Set NEWDAT bit to 1 to transfer the new data received into the Interface registers
		CAN1_IF2CRQ_R = (1 << CAN_IF2CRQ_MNUM_S); //write object number. 
		while ((CAN1_IF2CRQ_R & CAN_IF2CRQ_BUSY) != 0) {} //polls until BUSY clears

		message->canID = (CAN1_IF2ARB2_R >> 2) & 0x7FF; //get the message ID and save it to the struct field
		message->DLC = CAN1_IF2MCTL_R & 0x0000000F; //extract the first 4 bits of the MCTL register to get the DLC (Data length code) - Supposed to be 8 bytes based on the transmit function
		message->payload[0] = CAN1_IF2DA1_R & 0x00FF; //Low Byte (0x11)
		message->payload[1] = (CAN1_IF2DA1_R >> 8) & 0xFF; //High Byte (0x00)
		message->payload[2] = CAN1_IF2DA2_R & 0x00FF;
		message->payload[3] = (CAN1_IF2DA2_R >> 8) & 0xFF;
		message->payload[4] = CAN1_IF2DB1_R & 0x00FF;
		message->payload[5] = (CAN1_IF2DB1_R >> 8) & 0xFF;
		message->payload[6] = CAN1_IF2DB2_R & 0x00FF;
		message->payload[7] = (CAN1_IF2DB2_R >> 8) & 0xFF;
		
	
}