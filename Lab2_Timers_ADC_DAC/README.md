# Lab2 – MSP430 TimerA, ADC & DAC 

## Overview

This lab deepens our understanding of the MSP430x4xx peripherals:

* **TimerA**: input capture, output compare, PWM generation, and low-power timer modes.
* **ADC12/ADC10**: configuring sample-and-hold, clock sources, data conversion, and calibration.
* **DAC12**: analog output generation and interfacing.
* **GPIO**: pin configuration for switches, buttons, LEDs, and multiplexed peripherals.

We will implement a layered firmware architecture in C, comprising:

1. **BSP (Board Support Package)** – low-level clock and power configuration.
2. **HAL (Hardware Abstraction Layer)** – peripheral initialization and control.
3. **API Layer** – higher‑level functions for timer capture, ADC measurement, and LCD output.
4. **Application** – finite-state machine coordinating user inputs and display.

## Directory Structure

```text
Lab2_Preparation/
├── DOCsource/            # Lab assignment & theoretical report
│   └── lab2_task.pdf     # Original PDF: task description & block diagrams 
├── header/               # Public include files for each firmware layer
│   ├── main.h            # FSM state enums, system mode definitions & entry point prototype
│   ├── api.h             # API prototypes for TimerA, ADC/DAC, and display functions
│   ├── halGPIO.h         # HAL definitions: GPIO, TimerA, ADC10/12, DAC12 registers and masks
│   ├── bsp.h             # BSP prototypes: system clock setup, power modes, interrupt enable
│   └── LCD.h             # LCD driver interface: command/data functions and pin mappings
├── source/               # C source implementations
│   ├── main.c            # Main loop & FSM logic: state handling and power management
│   ├── api.c             # API layer: frequency capture, time update, display updates
│   ├── halGPIO.c         # HAL layer: sysConfig(), delay(), interrupt handlers for GPIO/TimerA/ADC
│   ├── bsp.c             # BSP layer: GPIOconfig(), TIMERconfig(), ADCconfig()
│   └── LCD.c             # LCD driver: init, command, data writes, string handling
└── README.md             # This document
```

## Lab Procedure & FSM States

Our firmware implements a finite‑state machine with five distinct states, triggered by push buttons PB0–PB3 or on reset:

* **Idle (state0)**: Default after reset. The MCU enters Low‑Power Mode 0 (LPM0), clearing the LCD and enabling interrupts until a button press awakens the system.

* **Frequency Measurement (state1)**: Triggered by PB0 (P2.4 input from signal generator). The system arms TimerA1 in input‑capture mode to timestamp two rising edges, computes the external signal frequency `f_in`, then displays the result dynamically on the LCD at row 0, column 6.

* **Stopwatch Mode (state2)**: Triggered by PB1. On entry, the LCD shows `00:00` and TimerA0 is configured for a 1 s tick. Pressing SW0 toggles start/stop of the up/down stopwatch. Each second, the minutes and seconds are updated in place on the LCD to minimize redraw time and power.

* **Tone Generation (state3)**: Triggered by PB2. Initially displays `Start state 3`, then continuously samples from ADC10, maps the ADC value `N_adc` to a tone frequency `f_out = m·N_adc + n`, and updates TimerA1 CCR registers to generate a PWM tone on the buzzer pin (P2.2).

* **Real‑Time Display (state4)**: Triggered by PB3. Animates a moving character (`'b'`) across the LCD by updating one position at a time, using TimerA0 with a 900 ms interval between moves, and blanks the previous position to create a scrolling effect.

## Getting Started

1. **Build & Flash** (using MSP430 GCC or TI CCS project):

   ```bash
   cd Lab2_Preparation
   make all      # compiles BSP, HAL, API, and main
   make flash    # programs the MSP430 board
   ```
2. **Run the Lab**:

   * Observe button presses, LED state changes, and LCD output.
   * Use an oscilloscope to verify TimerA capture and PWM outputs.
   * Use ADC readings to measure V<sub>CC</sub> and external analog signals.
3. **Review Results**:

   * Compare observed behavior against expected waveforms in `lab2_task.pdf`.
   * Document findings and waveform snapshots in `pre2_report.pdf` or a new report.

---

*Prepared by Ron Benita*
*Date: May 2025*
