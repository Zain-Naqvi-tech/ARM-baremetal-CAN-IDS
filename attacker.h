#ifndef ATTACKER_H
#define ATTACKER_H

#include <stdint.h>
#include "can.h"
#include "systick.h"
#include <stdbool.h>

void INJECT_TOO_FAST(volatile Msg* message, uint32_t index, volatile uint32_t* attackerTime);
void INJECT_OVER_RANGE(volatile Msg* message, uint32_t index);
bool INJECT_MISSING(uint32_t startTime, uint32_t stopTime, uint32_t index);

#endif