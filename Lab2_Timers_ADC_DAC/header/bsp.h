#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx
// #include  <msp430xG46x.h>  // MSP430x4xx



#define   debounceVal      250
#define   LEDs_SHOW_RATE   0xFFFF  // 62_5ms
#define   HALF_SEC_PERIOD  52429
#define   PWM_PERIOD       1000 // for 1 KHz

#define SMCLK_FREQ 1010000


//==================================================
//              LCD defines
//==================================================

#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

/*------------------------------------------------------------------------
      CONFIG: change values according to your port pin selection
---------------------------------------------------------------------------*/

#define LCD_EN(a)   (!a ? (P2OUT&=~ElcdBIT) : (P2OUT|=ElcdBIT)) // P1.5 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (P2DIR&=~ElcdBIT) : (P2DIR|=ElcdBIT)) // P1.5 pin direction

#define LCD_RS(a)   (!a ? (P2OUT&=~RSlcdBIT) : (P2OUT|=RSlcdBIT)) // P1.6 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (P2DIR&=~RSlcdBIT) : (P2DIR|=RSlcdBIT)) // P1.6 pin direction

#define LCD_RW(a)   (!a ? (P2OUT&=~RWlcdBIT) : (P2OUT|=RWlcdBIT)) // P1.7 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (P2DIR&=~RWlcdBIT) : (P2DIR|=RWlcdBIT)) // P1.7 pin direction


#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset

#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL

#define ElcdBIT            BIT5
#define RSlcdBIT           BIT6
#define RWlcdBIT           BIT7
#define DATA_bits_LCD      0xF0 // the pins we configure


/*------------------------------------------------------------------------
      LEDS PushButtons Switch  and special
---------------------------------------------------------------------------*/
// 4000Hz-> 250usec delay    1 Period 1 usec

// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR // Define which leg will be input or outPut
#define SWsArrPortSel      P2SEL //GPIO or other use
#define SWsArrIntPend      P2IFG  //flag of how made the interrupt
#define SWsArrIntEn        P2IE   // Interrupt Enable Register
#define SWsArrIntEdgeSel   P2IES
#define SWmask             0x0F
#define SW0                0x01

//Pin leg for F_in signal
#define FreqInPortIn       P2IN
#define FreqInPortDir      P2DIR
#define FreqInPortSel      P2SEL
#define FreqInMask         BIT4 //Define which leg we want p2.4
                                //Timer1_A, capture: CCI2A input, compare: Out2 output

//Pin leg for sending PWM to buzzer
#define FreqOutPortOut     P2OUT
#define FreqOutPortDir     P2DIR
#define FreqOutToBuzzerOn  BIT2
#define FreqOutToBuzzerOff (~(BIT2))



// PushButtons abstraction
#define PBsArrPort	       P1IN
#define PBsArrIntPend	   P1IFG  //flag of how made the interrupt
#define PBsArrIntEn	       P1IE   // Interrupt Enable Register
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PB0                BIT0 //usually BIT4
#define PB1                BIT1
#define PB2                BIT2
#define PB3                BIT1 // PB3 in port 2.1




//===================================================================
//              Timers defines
//===================================================================
// ===== Timer0_A0 =====
#define TimerA0_control               TA0CTL
#define TimerA0_channel0_control      TA0CCTL0
#define TimerA0_capture_value         TA0CCR0
#define TimerA0_counter               TA0R

// ===== Timer0_A1 =====
#define TimerA1_control               TA1CTL   //Control Register
#define TimerA1_channel2_control      TA1CCTL2 //Capture/Compare Control Register
#define TimerA1_capture_value         TA1CCR2  // Capture value from P2.4
#define TimerA1_counter               TA1R
#define TimerA1_IntPend               TA1IV

#define TimerSource_TACLK             TASSEL_0   // ׳³ֻ�׳³ג„¢׳³ג„¢׳³ן¿½׳³ֲ¨ ׳³ן¿½׳³ֲ§׳³ג€˜׳³ן¿½ ׳³ֲ§׳³ן¿½׳³ֻ� ׳³ן¿½׳³ג€˜׳³ג€”׳³ג€¢׳³ֲ¥ ׳’ג‚¬ג€� TA0CLK
#define TimerSource_SMCLK             TASSEL_2   // ׳³ֻ�׳³ג„¢׳³ג„¢׳³ן¿½׳³ֲ¨ ׳³ן¿½׳³ֲ§׳³ג€˜׳³ן¿½ ׳³ֲ§׳³ן¿½׳³ֻ� ׳³ן¿½׳³ג€�׳³ֲ©׳³ֲ¢׳³ן¿½ ׳³ג€�׳³ג‚×׳³ֲ ׳³ג„¢׳³ן¿½׳³ג„¢
#define TimerSource_ACLK              TASSEL_1   // 32768 Hz
#define TimerMode_STOP                MC_0       // stop mode
#define TimerMode_UP                  MC_1       // ׳³ן¿½׳³ֲ¦׳³ג€˜ ׳³ֲ¡׳³ג€¢׳³ג‚×׳³ֲ¨ ׳³ֲ¢׳³ג€� CCRx
#define TimerMode_CONT                MC_2       // Continuous Mode׳³
#define TimerMode_hold                MC_3
#define TimerClear                    TACLR      // rest counter
#define TimerInterrupt_enable         CCIE

#define TIMER_MAX                     0x3CE0     // SMCLK/8

#define TimerDividerBy2               ID_1
#define TimerDividerBy4               ID_2
#define TimerDividerBy8               ID_3

// Capture mode flags
#define CAPTURE_MODE_ENABLE  (CM_1 | CCIS_0 | SCS | CAP | CCIE)






//===================================================================
//             ADC defines
//===================================================================

// === ADC and Voltage Scaling ===
//#define ADC_RESOLUTION     1023U         // 10-bit ADC max value
//#define VREF_MV            3300U         // Reference voltage in mV

#define ADC_control             ADC10CTL0
#define ADC_TimeControl         ADC10CTL1
#define ADC_value               ADC10MEM
#define ADC_analog_enable       ADC10AE0

#define ADC_input_channel       INCH_3      // A0 ׳’ג€ ן¿½ P1.0

#define ADC_sample_hold         ADC10SHT_0  // [0 -4 cycles 1 -8 cycles 2 -16 cycles 3 -64 cycles]
#define ADC_enable_Interrupt    ADC10IE
#define ADC_on                  ADC10ON
#define ADC_conversion          ENC
#define ADC_start_conversion    ADC10SC
#define ADC_source_ref          SREF_0
#define ADC_clock_source        ADC10SSEL_3  //0-ADC10OSC 1-ACLK 2-MCLK 3-SMCLK
#define ADC_clock_DIV           ADC10DIV_7   // Divide at 4


extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);

#endif



