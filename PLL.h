#ifndef PLL_H
#define PLL_H

#define PSYSDIV 3 //Not personal clock speed anymore. It is 120MHz

// bus frequency is 480MHz/(PSYSDIV+1) = 480MHz/(3+1) = 120 MHz
// IMPORTANT: See Step 6) of PLL_Init().  If you change something, change 480 MHz.
// IMPORTANT: You can use the 10-bit PSYSDIV value to generate an extremely slow
// clock, but this may cause the debugger to be unable to connect to the processor
// and return it to full speed.  If this happens, you will need to erase the whole
// flash or somehow slow down the debugger.  You should also insert some mechanism
// to return the processor to full speed such as a key wakeup or a button pressed
// during reset.  The debugger starts to have problems connecting when the system
// clock is about 600,000 Hz or slower; although inserting a failsafe mechanism
// will completely prevent this problem.
// IMPORTANT: Several peripherals (including but not necessarily limited to ADC,
// USB, Ethernet, etc.) require a particular minimum clock speed.  You may need to
// use the 16 MHz PIOSC as the clock for these modules instead of the PLL.

// configure the system to get its clock from the PLL
void PLL_Init(void);

/*
PSYSDIV  SysClk (Hz)
  3     120,000,000
  4      96,000,000
  5      80,000,000
  7      60,000,000
  9      48,000,000
 15      30,000,000
 19      24,000,000
 29      16,000,000
 39      12,000,000
 79       6,000,000
*/

#endif