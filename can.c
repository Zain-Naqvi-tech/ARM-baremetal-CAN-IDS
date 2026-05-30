#include "can.h"
#include <stdint.h>
#include "tm4c1294ncpdt.h"

void CAN0_Init(void) {

    //RCGCCAN Register setup. It is used to let software enable and disable the clock to the CAN module
    SYSCTL_RCGCCAN_R |= SYSCTL_RCGCCAN_R0; //enables clock for CAN0
    while ((SYSCTL_PRCAN_R&SYSCTL_PRCAN_R0) == 0) {} //waits until bit 0 is set

    CAN0_CTL_R |= CAN_CTL_INIT; //INIT bit set to 1
    CAN0_CTL_R |= CAN_CTL_CCE; //CCE bit set to 1

    //Calculated values for the CANBIT Register
    uint32_t brp = 14 - 1; //13
    uint32_t sjw = 1 - 1; //0
    uint32_t tseg1 = 12 - 1; //11
    uint32_t tseg2 = 3 - 1; //2

    //CAN0_BIT_R is the CAN Bit Timing Register. [0:5] is the BRP field, [6:7] is the SJW field, [8:11] is the TSEG1 field, and [12:14] is the TSEG2 field
    CAN0_BIT_R = (brp << CAN_BRPE_BRPE_S) | (sjw << CAN_BIT_SJW_S) | (tseg1 << CAN_BIT_TSEG1_S) | (tseg2 << CAN_BIT_TSEG2_S);
    //CAN0_BIT_R = (brp << 0) || (sjw << 6) || (tseg1 << 8) || (tseg2 << 12);



}

void CAN1_Init(void) {

    //RCGCCAN Register setup. It is used to let software enable and disable the clock to the CAN module
    SYSCTL_RCGCCAN_R |= SYSCTL_RCGCCAN_R1; //enables clock for CAN1
    while ((SYSCTL_PRCAN_R&SYSCTL_PRCAN_R1) == 0) {} //waits until bit 1 is set

    CAN1_CTL_R |= CAN_CTL_INIT; //INIT bit set to 1
    CAN1_CTL_R |= CAN_CTL_CCE; //CCE bit set to 1

    //Calculated values for the CANBIT Register
    uint32_t brp = 14 - 1; //13
    uint32_t sjw = 1 - 1; //0
    uint32_t tseg1 = 12 - 1; //11
    uint32_t tseg2 = 3 - 1; //2

    //CAN1_BIT_R is the CAN Bit Timing Register. [0:5] is the BRP field, [6:7] is the SJW field, [8:11] is the TSEG1 field, and [12:14] is the TSEG2 field
    CAN1_BIT_R = (brp << CAN_BRPE_BRPE_S) | (sjw << CAN_BIT_SJW_S) | (tseg1 << CAN_BIT_TSEG1_S) | (tseg2 << CAN_BIT_TSEG2_S);
    //CAN1_BIT_R = (brp << 0) || (sjw << 6) || (tseg1 << 8) || (tseg2 << 12);

}