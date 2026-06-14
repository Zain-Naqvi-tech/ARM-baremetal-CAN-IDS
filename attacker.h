#ifndef ATTACKER_H
#define ATTACKER_H

#include <stdint.h>
#include "can.h"
#include "systick.h"
#include <stdbool.h>

void INJECT_TOO_FAST(volatile Msg* message, uint32_t index);
void INJECT_OVER_RANGE(volatile Msg* message, uint32_t index);
bool INJECT_MISSING(uint32_t startTime, uint32_t stopTime, uint32_t index);
void INJECT_UNKNOWN_ID(volatile Msg* message, uint32_t index);

#endif