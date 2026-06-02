#ifndef CAN_H
#define CAN_H

#include <stdint.h>

typedef struct {
    uint16_t canID;
    uint8_t DLC;
    uint8_t payload[8];
    uint32_t overrunFlag;
    uint32_t timeStamp;
} Msg;

void CAN0_Init(void);
void CAN1_Init(void);
void CAN0_Transmit(void);
void CAN1_Receive(Msg* message);

#endif