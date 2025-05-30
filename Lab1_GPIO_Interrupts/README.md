# System Programming Lab 1 – GPIO, LPM & Interrupts

## Overview

In Lab 1 we explore the MSP430x2xx/x4xx’s basic peripherals and low‑power features by:

* **GPIO Configuration**: setting pin direction, pull-up/down resistors, and I/O modes.
* **Low‑Power Modes (LPM0–LPM4)**: entering/exiting low‑power states to minimize energy consumption.
* **Interrupt Handling**: configuring GPIO interrupts for button presses and implementing ISR routines.
* **Software Layering**: structuring firmware into BSP, HAL, API, and Application layers with a Finite State Machine (FSM).

We will implement an interrupt-driven FSM, demonstrating robust hardware‑software integration and power‑aware design.

## Directory Structure

```text
Lab1_Preparation/              # root directory for Lab 1 sources and docs
├── DOCsource/                 # lab assignment and reports
│   └── lab1_task.pdf          # assignment specification & block diagrams
├── header/                    # Public headers for each firmware layer
│   ├── bsp.h                  # BSP: clock setup, GPIO and peripheral initialization
│   ├── halGPIO.h              # HAL: GPIO, TimerA, ADC/DAC driver prototypes and ISR declarations
│   ├── api.h                  # API: LED display, PWM generation, delay and utility functions
│   └── app.h                  # Application: FSM state enums and high-level routines
├── source/                    # C source implementations
│   ├── bsp.c                  # BSP: system clock configuration and GPIO init
│   ├── halGPIO.c              # HAL: sysConfig(), interrupt handlers, TimerA and ADC setup
│   ├── api.c                  # API: printSWs2LEDs(), createPWM(), display helpers
│   └── main.c                 # Application: FSM control loop and state transitions
└── README.md                  # This overview document
```

## Pre‑Lab Materials

Before coding, review:

1. **GPIO** – MSP430x4xx User Guide pp. 407–441; Tutorial Video 3.1
2. **Low‑Power Modes & GPIO Interrupts** – User Guide pp. 411–412, 445–450; MSP430xG461x datasheet pp. 34–36; Tutorial Videos 4.1–4.3
3. **Example Projects** – MSP430xG46x sample code for GPIO and LPM modes

## Lab Tasks & FSM States

Implement an interrupt‐driven FSM with these states (triggered by PB0–PB3 on P2.0–P2.3):

| State              | Trigger | Behavior                                                                                               |
| :----------------- | :------ | :----------------------------------------------------------------------------------------------------- |
| **0 Idle**         | RESET   | Enter LPM0, turn off LEDs, clear display; enable GPIO interrupts and wait for button event             |
| **1 Binary Count** | PB0     | Increment or decrement a 4‑bit counter on LEDs; 0.5 s delay per change; run for 10 s total             |
| **2 Blink Shift**  | PB1     | Shift a single LED right→left at 0.5 s intervals; preserve last position; run for 7 s total            |
| **3 PWM Tone**     | PB2     | Generate PWM on P2.7: 4 kHz @75% duty then 2 kHz @25% duty on alternate presses; continuous until exit |
| **4 Sleep**        | Exit    | Turn off LEDs, return to LPM0 (Idle)                                                                   |

> **Note**: Each state completes its full sequence before allowing a new interrupt to transition states.

## Software Architecture

Firmware is organized into four layers:

1. **BSP** (`bsp.c`, `bsp.h`):

   * Configure system clocks and GPIO ports for buttons and LEDs.
2. **HAL** (`halGPIO.c`, `halGPIO.h`):

   * Implement low-level drivers and ISRs for GPIO, TimerA, ADC, and DAC.
3. **API** (`api.c`, `api.h`):

   * Provide utility functions: LED display routines, PWM generation, delays, and formatting.
4. **Application** (`main.c`, `app.h`):

   * Define the FSM, handle state transitions in the main loop, and manage power modes.

## Build & Flash

Use your preferred toolchain (MSP430 GCC, IAR, or CCS):

```bash
cd Lab1_Preparation
make build    # compile BSP, HAL, API, and Application layers
make flash    # program the MSP430 board with the generated image
```

Or import the `header/` and `source/` files into an IDE project structured by layers.

*Prepared by Ron Benita*
*Date: May 2025*
