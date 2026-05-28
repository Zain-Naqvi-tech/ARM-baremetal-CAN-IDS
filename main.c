#include <stdint.h>
#include "PLL.h"
#include "tm4c1294ncpdt.h"
#include "uart.h"
#include "gpio.h"

int main(void) {

	PLL_Init(); //Sets the clock speed at 120MHz
	UART_Init();
	PortN_Init(); //Initializes Port N
	PortF_Init(); //Initializes Port F
	
}