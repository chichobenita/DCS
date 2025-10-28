# Source Files Overview

## `pcside.py`
A small Python (Tkinter) GUI for the PC side: select a COM port, send commands to the microcontroller (`1–8` or `3XXX`), and display text responses. Uses a background RX thread so the window remains responsive.

## `bsp.c`
Board Support Package. Initializes system clocks, general GPIO configuration (including disconnecting unused pins to save power), and enables the core modules — `Timer0_A`, `Timer1_A`, `ADC10`, and `UART`.

## `halGPIO.c`
HAL (Hardware Abstraction Layer). Implements all hardware ISRs (PORT1 – PB1, USCI RX/TX, TIMER0_A0, TIMER1_A1, ADC10). Manages global flags (`tickCounter`, `rx_buf`, `tx_buf`, `tone_idx`) and starts/stops timers as needed.

## `LCD.c`
Full 4-bit driver for an HD44780 LCD: `lcd_cmd` / `lcd_data`, string printing, cursor positioning, and display clear. Maps P1.4–P1.7 (D7–D4) and P2.5–P2.7 (RS, RW, E).

## `api.c`
API layer with high-level logic called by `main.c` (`send_love`, `count_up`, `buzzer_tones`, `poten_meas`, `reset_count`). Includes an interrupt-driven transmit ring buffer via `api_send_msg()` and parsing of received commands in `api_handle_msg()`. Does **not** touch registers directly.

## `main.c`
Main application FSM. The `state` variable selects the current system mode (menu options 1–8). On each transition it calls API functions, enters/exits LPM0, and handles printing the menu or accepting the `X` delay value (ms). No direct register access — only API services.
