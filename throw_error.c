#include "throw_error.h"
#include "tm4c1294ncpdt.h"
#include <stdint.h>
#include "msp432e401y.h" //this is the file which includes a reference to __disable_irq()

void OS_ERROR(void) {
	
		__disable_irq(); //Disable all interrupts
		volatile int i; //volatile counter variable
		while (1) {
			GPIO_PORTN_DATA_R ^= 0x02; //Toggle Second Bit for LED1
			for (i = 0; i < 1000000; i++) {} 
		}
	
}