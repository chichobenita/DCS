# **Lab4 – MSP430 UART Communication System**

## **Overview**
This lab implements a **bi-directional asynchronous serial communication system** between the **MSP430G2553** microcontroller and a **PC host** using the **UART (Universal Asynchronous Receiver/Transmitter)** peripheral.  
The project demonstrates the integration of multiple peripherals — UART, TimerA, ADC10, and LCD — within a **layered firmware architecture** in C, supported by a **finite-state machine (FSM)** and a **Python GUI application** on the PC side.

### **Learning Objectives**
- Configure and operate the MSP430 UART (USCI_A0) in 8N1 mode at 9600 bps.  
- Implement **interrupt-driven** serial communication (RX/TX ISRs, circular TX buffer).  
- Interface and synchronize LCD (HD44780, 4-bit), buzzer (PWM), and potentiometer (ADC10).  
- Apply a modular software design using **BSP, HAL, API, and Application layers**.  
- Develop a **PC GUI (Tkinter)** to send framed commands and display responses dynamically.

---

## **Directory Structure**
Lab4_UART/
├── DOCsource/
│ └── Preparation_report_LAB4.pdf # Lab specification and assignment details
├── header/
│ ├── api.h # API layer: high-level control (counting, buzzer, poten, TX)
│ ├── app.h # FSM states, system modes, and global variable declarations
│ ├── bsp.h # Low-level hardware abstraction (clocks, GPIO, timers, UART)
│ ├── halGPIO.h # Interrupt declarations, timers, UART RX/TX, PBs handlers
│ ├── LCD.h # LCD driver macros, 4-bit commands and helpers
│ └── q14_math.h # Fixed-point arithmetic (Q14) utilities for ADC-to-frequency mapping
├── source/
│ ├── main.c # FSM core: transitions between 8 system states (1–8)
│ ├── api.c # Application interface: UART messaging, ADC reading, PWM tones
│ ├── halGPIO.c # ISRs for ADC10, TimerA0/A1, UART RX/TX, Port interrupts
│ ├── bsp.c # System setup: GPIO, clock, ADC, timers, UART init
│ ├── LCD.c # LCD driver (HD44780): 4-bit mode, init, cmd, data, delay
│ └── ...
├── PC_side/
│ ├── pcside.py # Tkinter GUI – COM selection, command buttons, serial logging
│ ├── requirements.txt
│ └── README.md
└── README.md # This document

markdown
Copy code

---

## **Firmware Architecture**

### **BSP – Board Support Package**
Responsible for low-level peripheral configuration:
- `GPIOconfig()` sets up I/O pins, LCD control lines, and PB1 with pull-up.  
- `TIMERconfig()` initializes TimerA0 for delay ticks and TimerA1 for PWM/capture.  
- `ADCconfig()` enables ADC10 on P1.3 for potentiometer sampling.  
- `UARTconfig()` / `UART_Init()` configure UART A0 at 9600 bps (SMCLK ≈ 1 MHz).  
- Provides clock routing via VLO/ACLK for power-efficient timing.

---

### **HAL – Hardware Abstraction Layer**
Implements all **hardware ISRs** and peripheral helpers:

- **TimerA0 ISR** – generates 1 s ticks for counting and updates the stopwatch.  
- **TimerA1 ISR** – handles PWM tone progression and frequency capture.  
- **ADC10 ISR** – converts potentiometer ADC value to frequency (`f_out = 1.466·ADC + 1000`).  
- **UART RX ISR** – reconstructs messages framed as `@...#` and invokes `api_handle_msg()`.  
- **UART TX ISR** – sends queued bytes from a circular buffer (`tx_buf[TX_BUF_SZ]`).  
- **Port1 ISR** – handles PB1 input (state 5 → “I love my Negev”).  

Includes helpers for low-power modes:
```c
enterLPM();
enable_interrupts();
disable_interrupts();
API Layer
Provides high-level functionality exposed to the FSM:

count_up() – Stopwatch on LCD (“MM:SS”) updated via TimerA0 tick.

buzzer_tones() – Plays tone sequence (1 kHz → 2.5 kHz) using TA1CCR1 PWM.

poten_meas() – Samples ADC10, computes voltage, displays as “x.xx V”.

rec_X() – Displays delay parameter X received from PC.

send_love() – Sends “I love my Negev” over UART (ISR-driven TX).

reset_count() – Resets stopwatch counters.

api_handle_msg(const char *msg) – Parses UART commands and updates nextstate.

UART Command Frames
less
Copy code
@1#     → start count
@2#     → play tones
@3XXX#  → set delay X (ms)
@4#     → measure potentiometer
@5#     → PB1 message trigger
@6#     → clear LCD
@7#     → reprint text
@8#     → sleep mode
All UART transmission is non-blocking and interrupt-driven.

Application Layer (main.c)
Implements the Finite-State Machine controlling the system:

c
Copy code
while (1) {
    switch (state) {
        case state1: count_up();      break;
        case state2: buzzer_tones();  break;
        case state3: rec_X();         break;
        case state4: poten_meas();    break;
        case state5: send_love();     break;
        case state6: reset_count();   break;
        case state7: get_string();    break;
        case state8: /* sleep */      break;
    }
    state = nextstate;
}
Each state runs until a UART command or interrupt changes nextstate.
Low-power modes (LPM0) are used between interrupts to minimize energy consumption.

PC Application (Python GUI)
The host GUI (pcside.py) is built with Tkinter and PySerial to enable easy communication and testing.

Features
Auto-detects available COM ports.

Sends UART frames @N# (for 1–8) via buttons.

Button 3 opens a dialog to enter delay X (ms) → sends @3X#.

A background RX thread displays all MCU responses in a live log area.

Usage
bash
Copy code
python pcside.py
Then select COM port → click Connect → press buttons 1–8 to interact with the MCU.

Example serial log:

vbnet
Copy code
Connected to COM3
Response: Menu printed
Response: I love my Negev
Response: Voltage = 2.73 V
Hardware Connections
Peripheral	MSP430 Pin(s)	Description
UART	P1.1 (RX), P1.2 (TX)	Connected via LaunchPad USB-UART
LCD (4-bit)	P1.4–P1.7 (D7–D4), P2.5–P2.7 (RS,RW,E)	Standard HD44780 interface
Buzzer	P2.4 (TA1.1 PWM)	Audio tone output
Potentiometer	P1.3 (ADC10 A3)	Analog input (0–3.3 V)
Push Button PB1	P1.0	Sends “I love my Negev” message

System Operation
Command	Functionality
@1#	Counting on LCD with delay X (ms)
@2#	Circular tone series via buzzer
@3X#	Set new delay X (ms)
@4#	Display potentiometer voltage
@5#	On PB1 press, send “I love my Negev”
@6#	Clear LCD display
@7#	Reprint text/menu
@8#	Enter sleep (LPM0) mode

Testing & Verification
Flash the firmware using Code Composer Studio.

Run pcside.py and connect to the correct COM port (9600 8N1).

Press buttons 1–8 and observe LCD, buzzer, and serial output.

Use an oscilloscope on P1.1/P1.2 to verify UART frames (9600 bps).

Adjust potentiometer and confirm live ADC-to-voltage updates on LCD.

Validate PWM tone generation on P2.4 for all frequencies.

Prepared by
Ron Benita
Date: May 2025
Course: Embedded Systems Lab – MSP430G2553

yaml
Copy code
