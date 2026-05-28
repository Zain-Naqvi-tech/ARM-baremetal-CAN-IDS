# ARM-baremetal-CAN-IDS
A bare-metal multi-ECU automotive CAN network simulation and Intrusion Detection System (IDS) built on the ARM Cortex-M4 (MSP432E401Y)

First Phase:
SN65HVD230 CAN Transceiver was used as the external hardware for CAN communication. They include the CANH and CANL pin headers which are connected using a 120ohm resistor.
Found the registers and pins responsible for CAN communication on the MCU
Researched bit timing and its use in CAN communication. Also, calculated the BRP using the bit rate formula. 
