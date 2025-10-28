# Light Sources & Objects Detector System — Final Project (DCS EE 2025)

## Overview

Embedded system on **MSP430G2553** that scans **0°–180°** with a **servo**, measures distance with an **ultrasonic sensor**, detects **light sources** using **two LDRs**, persists small files in on‑chip **Flash**, and communicates with a **PC GUI** via a **binary UART protocol** (9600 8N1). The firmware follows a layered design (**BSP → HAL → API → Application**) and an **interrupt‑driven FSM**.

---

## Directory Structure

```text
dcs-final-detector/
├─ DOCsource/
│  └─ DCS EE Final Project 2025.pdf
├─ header/                    # Public headers (MCU abstractions & APIs)
│  ├─ api.h
│  ├─ app.h
│  ├─ bsp.h
│  ├─ fs.h
│  ├─ halGPIO.h
│  ├─ LCD.h
│  ├─ ldr.h
│  ├─ radar_protocol.h
│  ├─ servo.h
│  ├─ ultrasonic.h
│  └─ script.h
├─ source/                    # MCU firmware (CCS)
│  ├─ api.c
│  ├─ bsp.c
│  ├─ fs.c
│  ├─ halGPIO.c
│  ├─ LCD.c
│  ├─ ldr.c
│  ├─ main.c
│  ├─ radar_protocol.c
│  ├─ script.c
│  ├─ servo.c
│  └─ ultrasonic.c
└─ README.md                  # This document
```

> Update names/paths if your repo uses different capitalization.

---

## Firmware Architecture

### BSP (Board Support Package)

Clocks, GPIO mapping, **TimerA0/A1**, **ADC10**, and **UART (9600 8N1)** setup; LCD/UART pin macros and timer sources used by higher layers.

### HAL (Hardware Abstraction Layer)

System bring‑up (`sysConfig`) and **all ISRs**: **USCI_A0 RX/TX**, **TimerA0/A1**, **ADC10**, **PORT** buttons. Provides low‑power helpers (enter/exit **LPM0**) and shared RX/TX buffers.

### API Layer

High‑level operations (no direct register access): LCD utilities, delay handling, **servo degree/scan**, **ultrasonic/LDR sampling**, **mixed scans**, **file mode** actions, and protocol **ACK/ERROR**.

### Application (FSM)

`main.c` dispatches states for LCD ops, Set‑Delay, Servo degree/scan & telemeter, **Ultrasonic / LDR / Mixed scans**, File read/list, and **Script mode**; transitions are driven by UART/ISR events.

---

## Communication Protocol (MCU ↔ PC)

**Binary framing** with `MAGIC`, `cmd`, `len`, `payload[0..n‑1]` (bounded), and **CRC**; helpers build/parse frames and validate length/CRC before handing commands to the API.

**Typical flow:** RX ISR → parser → `api_handle_msg()` → state update → **ACK/ERROR** reply (echoing the command ID).

---

## Sensors & Actuators

* **Servo** — TA1.1 PWM; degree→pulse mapping and timed moves (TimerA0) for single‑angle and sweep.
* **Ultrasonic** — trigger + **TA1.2 echo capture**; distance (cm) computation with timeout.
* **LDR ×2** — dual ADC inputs, enable/disable, calibration stored in **Info Flash** with helpers.
* **LCD (HD44780, 4‑bit)** — init, cmd/data, cursor, clear, and XY helpers.

---

## File System (Flash) & Scripts

* **Mini‑FS** — fixed Flash layout (~2 KB file area, FAT in Info B); **create/open/read/delete/list**, `fs_format`, `fs_repack`, simple upload engine. Stores text, scripts, and calibration.
* **Script runner** — executes a script file from Flash: `script_init()`, `script_start(id)`, `script_step()`, `script_running()`, `script_abort()`; parses tokens and dispatches internally (no UART).

---

## Hardware Notes (Key Pins)

| Module         | Pins / Peripherals                            | Notes                                     |
| -------------- | --------------------------------------------- | ----------------------------------------- |
| **UART**       | P1.1 (RX), P1.2 (TX)                          | 9600 bps, 8N1 (USCI_A0)                   |
| **Servo**      | TA1.1 PWM                                     | Degree control + sweep timing via TimerA0 |
| **Ultrasonic** | Trigger (GPIO), Echo **TA1.2 capture**        | Continuous‑mode timing + timeout          |
| **LDR x2**     | ADC10 channels (muxed)                        | Calibration to Info Flash                 |
| **LCD**        | 4‑bit: P1.4–P1.7 (D7–D4), P2.5–P2.7 (RS,RW,E) | HD44780 driver                            |

---

## Build & Run

### MCU (Code Composer Studio)

1. Import the project for **MSP430G2553**.
2. Verify pin/timer routing in `bsp.h`/`bsp.c` before flashing.
3. Build & flash. The system idles in **LPM0** between interrupts.

### PC Side

Use a simple GUI/CLI to send protocol commands and visualize scans over **9600‑8N1**. Follow the frame spec in `radar_protocol.h/.c`.

---

## Source/Headers Quick Map

* `main.c` — FSM loop & state dispatch (servo/ultrasonic/LDR scans, files, scripts).
* `radar_protocol.c/.h` — frame structs, CRC, build/parse, command IDs.
* `servo.c/.h` — TA1.1 PWM, single move and one‑pass sweep helpers.
* `ultrasonic.c/.h` — trigger, **TA1.2** echo capture, single/multi‑sample, timeout globals.
* `ldr.c/.h` — dual sampling, Info‑Flash calibration addresses & routines.
* `fs.c/.h` — ~2 KB file area, FAT in Info B, upload engine, list/CRUD/repack.
* `script.c/.h` — start/step/running/abort APIs; runs script files from Flash.
* `halGPIO.c` — ISRs (UART/Timers/ADC/Port), shared buffers, LPM exits.
* `bsp.c/.h` — clocks, GPIO, timers, ADC, UART, LCD pin macros.
* `LCD.c/.h` — HD44780 4‑bit driver functions/macros.

---

## Credits

Prepared for **DCS – Digital Computer Structure** (Final Project 2025). Specification and interfaces aligned with the provided headers and sources.
