# ARM-BareMetal-CAN-IDS

**A simulated multi-ECU automotive CAN network featuring a custom firmware-level Intrusion Detection System (IDS).**

## Project Overview
This project simulates a secure, multi-node automotive controller area network (CAN) entirely on a single ARM Cortex-M4 microcontroller (MSP432E401Y) using bare-metal C. 

By leveraging the dual built-in CAN controllers (CAN0 and CAN1) routed through external physical transceivers, this system creates two independent hardware nodes operating on a shared physical bus:
* **Node 1 (Engine ECU):** Broadcasts realistic automotive telemetry (RPM, throttle, speed, and temperature) at standard vehicular message rates.
* **Node 2 (Body Control Module):** Actively listens, processes, and responds to the network traffic.

## Phase 1: Hardware Setup & Bare-Metal Initialization
* **External Hardware:** Utilized SN65HVD230 CAN transceivers for physical communication. The modules include built-in 120Ω termination resistors across the CANH and CANL headers.
* **MCU Configuration:** Located and mapped the specific registers and GPIO pins responsible for CAN communication on the Cortex-M4.
* **Bit Timing:** Researched CAN bit timing architecture and calculated the appropriate Baud Rate Prescaler (BRP) using the standard bit rate formula to establish a stable connection.

Transceiver Used:
<img width="1536" height="2048" alt="WhatsApp Image 2026-05-28 at 00 55 18" src="https://github.com/user-attachments/assets/0836dada-bb5c-497d-9a8c-5f19a5d747bf" />

### Wiring Setup
The physical bus uses a twisted-pair configuration to resolve electromagnetic interference (EMI) and maintain signal integrity.

<img width="1034" height="451" alt="image" src="https://github.com/user-attachments/assets/e6f128bb-95f4-495d-8db6-35752c1f3af1" />

<img width="1600" height="875" alt="WhatsApp Image 2026-05-28 at 00 55 05" src="https://github.com/user-attachments/assets/22873dcd-f75b-4e6b-9416-f17d59599165" />

**The Physical Bus (Backbone)**
* `CANH` on Transceiver 1 <---> `CANH` on Transceiver 2
* `CANL` on Transceiver 1 <---> `CANL` on Transceiver 2
(These two wires are twisted together between the nodes).

**Node 1 (CAN0 - Engine ECU)**
* `VCC`  -> MCU 3.3V
* `GND`  -> MCU Common GND
* `TXD`  -> MCU Pin PA1 (CAN0TX)
* `RXD`  -> MCU Pin PA0 (CAN0RX)

**Node 2 (CAN1 - Body Control Module)**
* `VCC`  -> MCU 3.3V
* `GND`  -> MCU Common GND
* `TXD`  -> MCU Pin PB1 (CAN1TX)
* `RXD`  -> MCU Pin PB0 (CAN1RX)

## Milestone 1 — TX/RX Proof of Life

The first integration milestone: a single CAN frame transmitted from CAN0, received by CAN1 across a physical two-node bus, and parsed out of the receiving message object. No CAN library used

**What this proves**
- Both CAN controllers brought up from raw registers with a bit timing for 500 kbps (BRP=14, TSEG1=12, TSEG2=3) and message-object configuration using the IF1/IF2 indirect interface
- CAN0 transmits a standard 11-bit frame
- CAN1 receives it, and the firmware reads the frame back out of the message object (`WRNRD=0`) and parses ID, DLC, and the 8 payload bytes into a `Msg` struct

**Verified result**: debugger watch window after one TX → RX cycle:

![alt text](image.png)

Milestone 1: Received CAN frame parsed into the Msg struct, shown in the debugger watch window

**Goal:** Stream live CAN traffic to a host PC using UART

**Why UART2:** Enabling CAN0 (jumpers JP4/JP5) reassigns PA0/PA1 away from
UART0, so per the board user guide UART2 (PD4/PD5) becomes the XDS-110
backchannel.

**Config:** 115200 8-N-1, PIOSC clock (IBRD 8 / FBRD 44); PD4/PD5 muxed to
UART2 alternate function, Port D clock enabled.

**Trace format** — `Message_Object_UART_Print()` emits fixed CSV
`DIR,ID,DLC,payload…,timestamp`:

```
RX,0x100,8,0x11,0x00,0x22,0x00,0x33,0x00,0x44,0x00,0
```

![UART2 CAN trace in RealTerm](![alt text](image-1.png))

## The Intrusion Detection System (IDS)
(Planned)
Running concurrently on the same Cortex-M4 core is a lightweight, custom-built Intrusion Detection System. It actively monitors the raw bus traffic at the hardware level to detect and flag common automotive cybersecurity threats, including:
* **Timing Violations:** Detecting messages sent outside their expected frequency windows.
* **Unexpected Message IDs:** Flagging unauthorized or unknown device IDs attempting to transmit on the bus.
* **Value Spoofing:** Identifying physically impossible data jumps (e.g., an instant spike from 0 to 100% throttle).

## Traffic Visualization
(Planned)
The firmware will interface with a custom Python dashboard that visualizes the live bus traffic over a serial connection. It will log standard telemetry while instantly flagging network anomalies and spoofing attempts in real time.
