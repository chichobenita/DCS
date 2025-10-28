#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/bsp.h"    		// private library - BSP layer
#include  "../header/LCD.h"
#include  "../header/servo.h"
#include  "../header/ultrasonic.h"
#include  "../header/app.h"    		// private library - APP layer
#include  "../header/q14_math.h"
#include  "../header/ldr.h"
#include  "../header/fs.h"

#include  "stdio.h"

extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable

extern void sysConfig(void);
extern unsigned char readSWs(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void disable_interrupts_modified(char);

extern void startTimerA1Capture();
extern void stopTimerA1Capture();
unsigned int getCapturedFrequency(void);


unsigned int ultrasonic_get_last_duration_us(void);
uint8_t ultrasonic_get_ultrasonic_success(void);
//extern void  ultrasonic_start_measurement();

extern void setupTimerA0Ms(unsigned int delay_ms);
extern void startTimerA0();
extern void stopTimerA0();

//PORT 1
extern __interrupt void PBs_handler(void);

//PORT 2
extern __interrupt void Switch_handler(void);

//Timers
extern __interrupt void Timer_A0(void);
extern __interrupt void Timer_A1(void);


//ADC
extern __interrupt void ADC10_handler(void);

extern __interrupt void USCI0RX_ISR(void);

extern __interrupt void USCI0TX_ISR(void);

extern void api_handle_msg(const radar_message_t* msg);


#define MAX_DIGITS   5      // נניח שתומכים במספר עד 65535ms
#define TX_BUF_SZ     40
extern volatile char     rx_buf[];
extern volatile uint8_t  rx_index;
extern volatile uint16_t X;
extern volatile uint16_t delayMs;
extern volatile uint8_t x_ready;
extern volatile uint8_t flag;
extern volatile  head, tail;
extern volatile char    tx_buf[TX_BUF_SZ];


#endif
