#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx
// #include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal      250
#define   LEDs_SHOW_RATE   0xFFFF  // 62_5ms
#define   HALF_SEC_PERIOD  52429
#define   PWM_PERIOD       1000 // for 1 KHz

// 4000Hz-> 250usec delay    1 Period 1 usec

// LEDs abstraction
#define LEDsArrPort        P1OUT
#define LEDsArrPortDir     P1DIR
#define LEDsArrPortSel     P1SEL

// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0x0F

#define PinOutForPwm       P2OUT
#define PWMon              BIT7
#define PWMoff             (~(BIT7))
// PushButtons abstraction
#define PBsArrPort	       P2IN
#define PBsArrIntPend	   P2IFG 
#define PBsArrIntEn	       P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL 
#define PBsArrPortDir      P2DIR 
#define PB0                0x10
#define PB1                0x20
#define PB2                0x40
#define PB3                0x80


extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);

#endif



