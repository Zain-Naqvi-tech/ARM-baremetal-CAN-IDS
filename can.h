#ifndef CAN_H
#define CAN_H

#include <stdint.h>

#define NUMBER_OF_EVENTS 4
#define TX_MSG_OBJ 1
#define RX_MSG_OBJ 1
#define DA1 0x11
#define DA2 0x22
#define DB1 0x33
#define DB2 0x44

typedef struct {
	uint16_t canID; //Event specific ID
  uint8_t DLC; //Data Length Code
  uint8_t payload[8]; //Actual Data the event is sending
  uint32_t overrunFlag;
  uint32_t timeStamp; //Number of ticks that have taken place since the system started working
	uint32_t lastTransmitted; //This is the last time the message for this event was transmitted
	uint32_t period; //This is the time it takes for this particular event to send a message
} Msg;

typedef enum {
	ENGINE_RPM,
	THROTTLE,
	VEHICLE_SPEED,
	COOLANT_TEMP
} PROPERTY;

void CAN0_Init(void);
void CAN1_Init(void);
void CAN0_Transmit(Msg* message, uint32_t index);
void CAN1_Receive(Msg* message);
void CAN_Message_Table_Init(Msg* msg);
void CAN0_TX_Setup(Msg* msg);

#endif