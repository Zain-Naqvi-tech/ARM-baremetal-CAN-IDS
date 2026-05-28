#ifndef UART_H
#define UART_H

#include <stdint.h>

// Packet Format: [31:24] event type (top 8 bits) | [23:16] task index (Next 8 bits) | [15:0] tick counter (Bottom 16 bits)
#define UART_PACKET(event, index, ticks) (((uint32_t)(event) << 24) | ((uint32_t)(index) << 16) | ((uint16_t)(ticks)))

#define EVT_TASK_SWITCH   0x01
#define EVT_SEM_WAIT      0x02
#define EVT_SEM_SIGNAL    0x03
#define EVT_MUTEX_LOCK    0x04
#define EVT_MUTEX_UNLOCK  0x05
#define EVT_MALLOC        0x06
#define EVT_FREE          0x07

void UART_Init(void);

// Wait for new input, then return ASCII code 
char UART_InChar(void);
// Wait for buffer to be not full, then output 
void UART_OutChar(char data);
void UART_printf(const char* array);
void Status_Check(char* array, int status);

void UART_Trace(uint32_t Packet);

#endif