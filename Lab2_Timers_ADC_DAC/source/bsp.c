#include  "../header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // Switches Setup
  SWsArrPortSel &= ~(SW0+PB3);
  SWsArrPortDir &= ~(SW0+PB3);
  SWsArrIntEn |= (SW0+PB3);
  SWsArrIntEdgeSel &= ~PB3;
  SWsArrIntPend &= ~0xFF; // clear pending interrupts

  
  //pin for freq
  FreqInPortDir &= ~FreqInMask; // set to be input
  FreqInPortSel |=  FreqInMask; // set to be GPIO

  // PushButtons Setup
  PBsArrPortSel &= ~(PB0+PB1+PB2);
  PBsArrPortDir &= ~(PB0+PB1+PB2);  //0X07 PB0+PB1+PB2;          // 0xF0 - all the 4PB activate
  //PBsArrPortDir |=  BIT7;            // set p2.7 to be output for the PWM
  PBsArrIntEdgeSel |= 0x30;  	     // pull-up mode
  PBsArrIntEdgeSel &= ~0xC0;         // pull-down mode
  PBsArrIntEn |= 0x07;               // all pin can do interrupts
  PBsArrIntEn &= ~FreqInMask;        // disable interrupts for the freq input
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts
  

  FreqOutPortDir |= FreqOutToBuzzerOn;


  /////////////////////////////////
  P2SEL  |= BIT2;     // select peripheral function on P2.2
  P2SEL2 &= ~BIT2;    // ensure it’s the TA1.1 function, not some other
  P2DIR  |= BIT2;     // make it an output
  /////////////////////////////////



   lcd_init();
  _BIS_SR(GIE);                     // enable interrupts globally
}                             
//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
	
	//write here timers congiguration code
    // configuration of Timer A1
    TimerA1_control =TimerSource_SMCLK | TimerMode_STOP |TimerClear ; //TimerMode_CONT
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
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    //write here ADC congiguration code

    ADC_control =ADC_sample_hold|ADC_on|ADC_enable_Interrupt; //ADC_source_ref |
	
    ADC_TimeControl =ADC_input_channel| ADC_clock_source | ADC_clock_DIV ;

    ADC_analog_enable|=BIT3;

}              

           
             

 
             
             
            
  

