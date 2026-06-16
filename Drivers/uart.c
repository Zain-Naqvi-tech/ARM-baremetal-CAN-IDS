#include "uart.h"
#include "tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

static char printf_buffer[1023];


//We are using UART2 because PA1/PA2 are now being used for CAN communication
void UART_Init(void) {
	SYSCTL_RCGCUART_R |= 0x0004; // activate UART2 - Bit 2
	SYSCTL_RCGCGPIO_R |= 0x0008; // activate port D clock
	
	while((SYSCTL_PRUART_R&SYSCTL_PRUART_R2) == 0){};
		
  UART2_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
  UART2_IBRD_R = 8;                     // IBRD = int(16,000,000 / (16 * 115,200)) = int(8.681) -> uses the PIOSC (16MHz)
  UART2_FBRD_R = 44;                    // FBRD = round(0.6806 * 64) = 44
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
                                        // UART gets its clock from the alternate clock source as defined by SYSCTL_ALTCLKCFG_R
  UART2_CC_R = (UART2_CC_R&~UART_CC_CS_M)+UART_CC_CS_PIOSC;
                                        // the alternate clock source is the PIOSC (default)
  SYSCTL_ALTCLKCFG_R = (SYSCTL_ALTCLKCFG_R&~SYSCTL_ALTCLKCFG_ALTCLK_M)+SYSCTL_ALTCLKCFG_ALTCLK_PIOSC;
  UART2_CTL_R &= ~UART_CTL_HSE;         // high-speed disable; divide clock by 16 rather than 8 (default)

	UART2_LCRH_R = 0x0070;		// 8-bit word length, enable FIFO 
	UART2_CTL_R = UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN; // enable RXE, TXE and UART. Comes out to be 0x0301
		
	//Ports PD4 and PD5 are needed
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFF00FFFF)+0x00110000; // UART   
  GPIO_PORTD_AMSEL_R &= ~0x30;    // disable analog function on PD4-5   
  GPIO_PORTD_AFSEL_R |= 0x30;        // enable alt funct on PD4-5 
  GPIO_PORTD_DEN_R |= 0x30;            // enable digital I/O on PD4-5
}

// Wait for new input, then return ASCII code 
	char UART_InChar(void) {
		if((UART2_FR_R&0x0010) == 0) { // check if RXE is 0 
			RXFlag = 1;
			return((char)(UART2_DR_R&0xFF));
		}	
		return -1;
	} 
	
	// Wait for buffer to be not full, then output 
	void UART_OutChar(char data){
		while((UART2_FR_R&0x0020) != 0);	// wait until TXFF is 0 
		UART2_DR_R = data;
	} 
	void UART_printf(const char* array){
		int ptr=0;
		while(array[ptr]){
			UART_OutChar(array[ptr]);
			ptr++;
		}
	}
	
	void Status_Check(char* array, int status){
			if (status != 0){
				UART_printf(array);
				sprintf(printf_buffer," failed with (%d)\r\n",status);
				UART_printf(printf_buffer);
			}else
			{
				UART_printf(array);
				UART_printf(" Successful.\r\n");
			}
	}
	
	void UART_numeric_print(uint32_t num) {
	
		sprintf(printf_buffer, "%u \r\n", num);
		UART_printf(printf_buffer);
	
	}
	
	
	//Prints out the message object elements for better CAN demo and debugging
	//RX/TX,CANID,DLC,payload bits,timestamp
	void Message_Object_UART_Print(uint32_t direction, volatile Msg* message) {
		//Direction==1 is TX and Direction==0 is RX
		if (direction) {UART_printf("TX,");} else {UART_printf("RX,");}
		sprintf(printf_buffer, "0x%X,%d,", message->canID, message->DLC);
		UART_printf(printf_buffer);
		for (int i = 0; i < message->DLC; i++) {
			sprintf(printf_buffer, "0x%02X,", message->payload[i]);
			UART_printf(printf_buffer);
		}
		switch (message->status)
		{
		case OK:
			UART_printf("OK,");
			break;
		
		case MISSING:
			UART_printf("MISSING,");
			break;

		case TOO_FAST:
			UART_printf("TOO_FAST,");
			break;

		case OVER_RANGE:
			UART_printf("OVER_RANGE,");
			break;
		
		case UNKNOWN_ID:
			UART_printf("UNKNOWN_ID,");
			break;
		
		case SPIKE:
			UART_printf("SPIKE,");
			break;
		
		default:
			UART_printf("OK,");
			break;
			
		}
		sprintf(printf_buffer, "%d\r\n", message->timeStamp);
		UART_printf(printf_buffer);
		
	}