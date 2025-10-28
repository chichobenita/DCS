# Lab4 – MSP430 UART Communication System

## Overview

This project implements a **bi‑directional asynchronous UART link** between an **MSP430G2553** and a **PC host**. The firmware integrates **UART (USCI_A0)**, **TimerA0/A1**, **ADC10**, and an **HD44780 LCD (4‑bit)** within a **layered C architecture** (BSP → HAL → API → Application) coordinated by an **interrupt‑driven FSM**. A lightweight **Tkinter GUI** on the PC side sends framed commands and logs MCU responses.

### Learning Objectives

* Configure UART A0 for **9600 bps, 8N1** and use **interrupt‑driven RX/TX** with a circular TX buffer.
* Interface LCD (4‑bit), buzzer (PWM via TA1.1), and potentiometer (ADC10 on P1.3).
* Apply a clean separation of concerns across **BSP / HAL / API / APP**.
* Use **low‑power modes (LPM0)** between interrupts.
* Build a small **Python GUI** to exercise the MCU menu.

---

## Directory Structure

```text
Lab4_UART/
├─ DOCsource/
│  └─ Preparation_report_LAB4.pdf    # Lab specification & diagrams
├─ header/                            # Public headers for each layer
│  ├─ api.h        # High‑level APIs (counting, tones, ADC, UART TX, parser)
│  ├─ app.h        # FSM states, system modes, shared globals
│  ├─ bsp.h        # Clocks, GPIO, TimerA, ADC10, UART macros & prototypes
│  ├─ halGPIO.h    # ISRs (ADC10/TimerA/UART/Ports) + timer helpers
│  ├─ LCD.h        # LCD macros (4‑bit), commands, helpers
│  └─ q14_math.h   # Q14 utilities (ADC→freq mapping, fixed‑point helpers)
├─ source/                            # MCU firmware implementation
│  ├─ main.c       # Application FSM loop & state dispatch
│  ├─ api.c        # High‑level behaviors + UART TX queue + message parser
│  ├─ halGPIO.c    # ISRs: ADC10, TimerA0/1, UART RX/TX, Port1; LPM exits
│  ├─ bsp.c        # GPIO/Clock init, TimerA0/A1 setup, ADC10, UART init
│  ├─ LCD.c        # HD44780 4‑bit driver (init/cmd/data/cursor/delay)
│  └─ ...
├─ PC_side/
│  ├─ pcside.py    # Tkinter GUI: COM selection, @N# / @3X# sender, RX log
│  ├─ requirements.txt
│  └─ README.md
└─ README.md                        # This document
```

> **Note:** If actual file names differ, update the tree to match your repo before publishing.

---

## Firmware Architecture

### BSP (Board Support Package)

Responsible for **low‑level setup**:

* `GPIOconfig()` — Ports, pull‑ups, LCD lines, PB1 configuration.
* `TIMERconfig()` — **TimerA0** tick generator (ACLK/VLO) and **TimerA1** (PWM/capture).
* `ADCconfig()` — **ADC10** on **P1.3** for potentiometer sampling.
* `UARTconfig()` / `UART_Init(9600)` — UART A0 @ **9600 bps (SMCLK≈1 MHz)**.

### HAL (Hardware Abstraction Layer)

Implements **ISRs and hardware helpers**:

* **TimerA0 ISR** — stopwatch tick / tone step wakeups.
* **TimerA1 ISR** — PWM tone sequencing and capture‑to‑frequency conversion.
* **ADC10 ISR** — converts ADC to `f_out` (e.g., ~`1.466·ADC + 1000`).
* **USCI_A0 RX ISR** — reconstructs framed messages `@...#` and calls `api_handle_msg()`.
* **USCI_A0 TX ISR** — drains the circular TX buffer (`tx_buf`).
* **Port1 ISR** — PB1 → triggers state 5 (send fixed string).
* LPM helpers: `enterLPM()`, `enable_interrupts()`, `disable_interrupts()`.

### API Layer

High‑level, register‑free functions the APP calls:

* `count_up()` — Stopwatch on LCD (`MM:SS`) using TimerA0 tick; partial‑update prints.
* `buzzer_tones()` — Cyclic tone series **1–2.5 kHz** on TA1.1 (50% duty).
* `poten_meas()` — ADC10 sample → compute voltage → print as `x.xx V`.
* `rec_X()` — Display the current delay **X [ms]**.
* `send_love()` — Transmit `"I love my Negev"` via UART (ISR‑driven TX).
* `reset_count()` — Clear stopwatch counters.
* `get_string()` — Print buffered text to LCD.
* `api_handle_msg(const char* msg)` — Parse framed commands and set `nextstate`.

### Application (main.c)

Main **FSM loop** and state dispatch. Each state runs until an interrupt or UART command updates `nextstate`, with **LPM0** used between events.

---

## UART Protocol (PC ↔ MCU)

Framed commands from PC use `@` and `#` delimiters:

```text
@1#      → Counting on LCD with delay X
@2#      → Circular tone series via buzzer
@3X#     → Set delay X (ms), e.g., @3500#
@4#      → Display potentiometer voltage on LCD
@5#      → On PB1 press, send fixed string to PC
@6#      → Clear LCD
@7#      → Reprint text/menu
@8#      → Enter sleep (LPM0)
```

All UART TX is **non‑blocking** and **interrupt‑driven** (circular buffer).

---

## FSM States (Summary)

| State | Purpose                                          |
| :---: | :----------------------------------------------- |
|   1   | `count_up()` — Stopwatch on LCD with delay **X** |
|   2   | `buzzer_tones()` — PWM tones (1–2.5 kHz)         |
|   3   | `rec_X()` — Show/update delay **X [ms]**         |
|   4   | `poten_meas()` — ADC10 read → print `x.xx V`     |
|   5   | `send_love()` — On PB1, TX "I love my Negev"     |
|   6   | `reset_count()` — Clear stopwatch counters       |
|   7   | `get_string()` — Print buffered string to LCD    |
|   8   | Sleep — enter **LPM0** until UART/PB wake        |

---

## Hardware Connections

| Peripheral    | MSP430 Pin(s)                              | Notes                     |
| ------------- | ------------------------------------------ | ------------------------- |
| UART          | **P1.1 (RX), P1.2 (TX)**                   | LaunchPad USB‑UART bridge |
| LCD (4‑bit)   | **P1.4–P1.7 (D7–D4), P2.5–P2.7 (RS,RW,E)** | HD44780                   |
| Buzzer        | **P2.4 (TA1.1 PWM)**                       | 50% duty by CCR1          |
| Potentiometer | **P1.3 (ADC10 A3)**                        | 0–3.3 V                   |
| PB1           | **P1.0**                                   | Triggers state 5          |

---

## Build & Flash (MCU)

Use **TI CCS** or your chosen makeflow. Typical steps:

```bash
# Open the CCS project in source/ and build
# or, if using Makefiles (example):
make all
make flash
```

* UART default: **9600 bps, 8N1**, clocked from **SMCLK≈1 MHz** (ACLK/VLO used for TimerA0 ticks).

---

## PC Application (Tkinter GUI)

### Features

* Auto‑detect **COM** ports, one‑click **Connect**.
* Buttons **1–8** send `@N#`; button **3** prompts for **X** then sends `@3X#`.
* Background **RX thread** shows MCU responses in a scrolling log.

### Usage

```bash
python pcside.py
```

Select the COM port → **Connect** → press **1–8** to interact.

---

## Testing & Verification

1. Open a serial terminal (or the GUI) at **9600 8N1** and verify the **menu**.
2. Exercise states **1–8** and observe **LCD**, **buzzer**, and **serial** output.
3. Probe **P1.1/P1.2** on a scope to verify frames/baud.
4. Turn the potentiometer and confirm live **x.xx V** updates on the LCD.
5. Validate PWM frequencies on **P2.4** across the tone sequence.

---

## Credits

Prepared by **Ron Benita** · May 2025
