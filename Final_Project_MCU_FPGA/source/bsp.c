#include  "../header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // Switches Setup

  /*SWsArrPortSel &= ~(SW0+PB3);
  SWsArrPortDir &= ~(SW0+PB3);
  SWsArrIntEn |= (SW0+PB3);
  SWsArrIntEdgeSel &= ~PB3;
  SWsArrIntPend &= ~0xFF; // clear pending interrupts
*/
  
  //pin for freq
  FreqInPortDir &= ~FreqInMask; // set to be input
  FreqInPortSel |=  FreqInMask; // set to be GPIO

  // PushButtons Setup
  PBsArrPortSel &= ~(PB1+PB0);
  PBsArrPortDir &= ~(PB1+PB0);  // PB1 connected to P1.0
  //PBsArrPortDir |=  BIT7;            // set p2.7 to be output for the PWM
  PBsArrIntEdgeSel      |=  0x30;  	     // pull-up mode
  PBsArrIntEdgeSel      &=  ~0xC0;         // pull-down mode
  PBsArrIntEn           |=  (PB0 +PB1);          // only P1.3 can interrupt
  PBsArrIntEn           &=  ~FreqInMask;        // disable interrupts for the freq input
  PBsArrIntPend         &=  ~0xFF;            // clear pending interrupts
  P1REN                 |=  BIT0;             /* Pull-up resistor    */

  //FreqOutPortDir |= FreqOutToBuzzerOn;
  //P2SEL |= BIT4;                 /* P2.4 ג†’ TA1.1 */
  //P2DIR |= BIT4;


  //Init Servo

   lcd_init();
  _BIS_SR(GIE);                     // enable interrupts globally
}



//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
	
	//write here timers congiguration code
    // configuration of Timer A1
    //TimerA1_control =TimerSource_SMCLK | TimerMode_STOP |TimerClear ; //TimerMode_CONT
    //TimerA1_channel2_control = CAPTURE_MODE_ENABLE;

    // Route the internal VLO (~12 kHz) to ACLK
    // clear the LFXT1S bits, then set to 10b = VLOCLK
    BCSCTL3 = (BCSCTL3 & ~LFXT1S_3) | LFXT1S_2;


    //configuration of Timer A0
    TimerA0_control=TimerClear;
    TimerA0_capture_value= 0x4000;//0xFFFF;
    TimerA0_channel0_control=TimerInterrupt_enable;
    TimerA0_control=TimerSource_ACLK|TimerMode_STOP|TimerDividerBy2|TimerClear;// TimerSource_SMCLK | TimerMode_STOP |TimerDividerBy8|TimerClear ;

} 
//------------------------------------------------------------------------------------- 
//            ADC configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    //write here ADC configuration code for sampling the Potentiometer

    ADC_control =ADC_sample_hold|ADC_on|ADC_enable_Interrupt; //ADC_source_ref |
	
    ADC_TimeControl =ADC_input_channel0| ADC_clock_source | ADC_clock_DIV ;

    ADC_analog_enable|=BIT3;

}              

//-------------------------------------------------------------------------------------
//            UART congiguration
//-------------------------------------------------------------------------------------
void UARTconfig(void){
    BCSCTL1 = 0x86;
    DCOCTL  = 0xBE;

    P1SEL  |= PERIPHERAL_MODE;   //select peripheral function on P1.1,P1.2
    P1SEL2 |= UART_MODE;

    P1DIR |=  BIT2;          // P1.2 (TX)
    P1DIR &= ~BIT1;          // P1.1 (RX)
 

     UCA0CTL1 |= UART_SMCLK;        // source clock SMCLK
     UCA0BR0 = 104;                 // 1MHz / 9600 Baud
     UCA0BR1 = 0;                   // 1MHz / 9600 Baud
     UCA0MCTL = UCBRS0;
     UCA0CTL1 &= UART_RESET;
     IE2 |= UART_ENABLE;
}
  

void UART_Init(unsigned long baudrate)
{
    BCSCTL1 = 0x86;
    DCOCTL  = 0xBE;
    P1SEL |= PERIPHERAL_MODE;   //select peripheral function on P1.1,P1.2
    P1SEL2 |= UART_MODE;

    UART_Control1 |= UCSWRST;             //Reset
    UART_Control1 = UCSSEL_2 | UCSWRST;   //SMCLK

    UART_Control0 = UART_CHAR_8BIT |
                    UART_PARITY_NONE |
                    UART_STOPBIT_1 |
                    UART_LSB_FIRST |
                    UART_MODE_UART |
                    UART_ASYNC_MODE;


    unsigned int divider = (unsigned int)(1000000UL / baudrate);
    UART_BaudRateLow = divider & 0xFF;
    UART_BaudRateHigh = (divider >> 8) & 0xFF;

    UART_ModulationControl = UCBRS0;

    UART_Control1 &= ~UCSWRST; // end reset
    UART_InterruptEnableReg |= UART_TransmitInterruptEnable | UART_ReceiveInterruptEnable;
    __enable_interrupt();
}



