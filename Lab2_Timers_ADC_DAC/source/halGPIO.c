#include  "../header/halGPIO.h"     // private library - HAL layer


static volatile int _capture_count = 0;
static volatile unsigned char  _capture_done = 0;
static volatile unsigned int frequency_cal = 0;

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMERconfig();
	ADCconfig();

}

//--------------------------------------------------------------------
//				Read value of 4-bit SWs array 
//--------------------------------------------------------------------
unsigned char readSWs(void){
	unsigned char ch;
	
	ch = PBsArrPort;
	ch &= SWmask;     // mask the least 4-bit
	return ch;
}
//---------------------------------------------------------------------
//             Increment / decrement LEDs shown value 
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}

//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

void disable_interrupts_modified(char PB_pin)
// the function disable interrupts from all pins except the PB_pin
{
        PBsArrIntEn |= PB_pin;
}





//*********************************************************************
//            ADC10 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_handler (void)
#else
#error Compiler not supported!
#endif
{

    //int mv= val*3300/1023;

   // f_out=//adc_to_freq_q14(ADC_value);
    unsigned int adc_val= ADC_value;
    f_out = 1.466*adc_val+1000;

    __bic_SR_register_on_exit(LPM0_bits);

}


//*********************************************************************
//            Timer A1 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer_A1(void)
#elif defined(__GNUC__)

void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A1 (void)
#else
#error Compiler not supported!
#endif
{
    switch(TimerA1_IntPend)
    {
    case TA1IV_NONE:
    {
        break;
    }
    case TA1IV_TACCR1: //interrupt from CCR1
    {
        break;
    }
    case TA1IV_TACCR2:

    {
        unsigned int curr = TimerA1_capture_value;

         if (_capture_count == 0) {
             // first edge: just remember it
             last_capture = curr;
             _capture_count = 1;
         } else {
             // second edge: compute period & frequency
             if (curr >= last_capture)
                 period = curr - last_capture;
             else
                 period = (0xFFFF - last_capture) + curr + 1;

             frequency_cal = (period ? (1070000 / period) : 0); //protects you from dividing by zero

             // mark done & disable further CCR2 interrupts
             _capture_done = 1;
             TimerA1_channel2_control = 0;

             // optionally stop the timer if you want to reset TA1R
             TimerA1_control = TimerSource_SMCLK | TimerMode_STOP | TimerClear;

             _capture_count = 0;
             }
         __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
       break;
    }
    case TA1IV_TAIFG: // overflow of the timer
        {
            break;
        }
    }

}
//*********************************************************************
//            Timer A0 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0(void)
#elif defined(__GNUC__)

void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A0 (void)
#else
#error Compiler not supported!
#endif
{
    // write the code of the timer mm:ss
       //LPM0_EXIT;

           tickCounter++; // 1 sec pass
           __bic_SR_register_on_exit(LPM0_bits); // wake the cpu up
}





//*********************************************************************
//            Port1 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){
   
	delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------

	if(PBsArrIntPend & PB0 )//check who push the button
	  {

	        state = state1;
	        PBsArrIntPend &= ~PB0; // reset the bits of the PBsArrIntPend
      }


     else if(PBsArrIntPend & PB1)
     {
         state = state2;

         if(SWsArrPort&SW0) // if (P2IN.0 == 1)
         {
             SWsArrIntEdgeSel|=SW0; // change the interupt to down
         }
         PBsArrIntPend &= ~PB1;
     }


	else if(PBsArrIntPend & PB2)
	  {
	  state = state3;
	  Flag_PB_status[state3]=1 ;
	  PBsArrIntPend &= ~PB2;
      }



	/*else if(PBsArrIntPend & PB3)
	    {
	      state = state4;
	      //Flag_PB_status[state4]^=1 ;
	      PBsArrIntPend &= ~PB3;
	    }*/


//---------------------------------------------------------------------
//            Exit from a given LPM 
//---------------------------------------------------------------------	
        switch(lpm_mode){
		case mode0:
		 LPM0_EXIT; // must be called from ISR only
		 break;
		 
		case mode1:
		 LPM1_EXIT; // must be called from ISR only
		 break;
		 
		case mode2:
		 LPM2_EXIT; // must be called from ISR only
		 break;
                 
                case mode3:
		 LPM3_EXIT; // must be called from ISR only
		 break;
                 
                case mode4:
		 LPM4_EXIT; // must be called from ISR only
		 break;
	}
        
}
 

#pragma vector=PORT2_VECTOR
  __interrupt void Switch_handler(void){

    delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------

    if(SWsArrIntPend & SW0) //check who push the button
    {
      if (state==state2)  // relevent only for state 2
      {
          if(SWsArrPort&SW0) // if (P2IN.0 == 1)
          {

              Flag_PB_status[state2]=1 ;
              SWsArrIntEdgeSel|=SW0;
              startTimerA0();

          }
          else
          {

              stopTimerA0();
              Flag_PB_status[state2]=0;
              SWsArrIntEdgeSel &= ~SW0;
          }
      }
      SWsArrIntPend &= ~SW0; // reset the bits of the PBsArrIntPend
     }
    if(SWsArrIntPend & PB3)
    {
        state = state4;
        SWsArrIntPend &= ~PB3;
    }

    /*else if(PBsArrIntPend & PB1){
      state = state2;

      PBsArrIntPend &= ~PB1;
        }
    else if(PBsArrIntPend & PB2)
      {
      state = state3;

      PBsArrIntPend &= ~PB2;
      }
    else if(PBsArrIntPend & PB3)
        {
          state = state4;
          PBsArrIntPend &= ~PB3;
        }
        */
//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
        switch(lpm_mode){
        case mode0:
         LPM0_EXIT; // must be called from ISR only
         break;

        case mode1:
         LPM1_EXIT; // must be called from ISR only
         break;

        case mode2:
         LPM2_EXIT; // must be called from ISR only
         break;

                case mode3:
         LPM3_EXIT; // must be called from ISR only
         break;

                case mode4:
         LPM4_EXIT; // must be called from ISR only
         break;
    }

}


void setupTimerA0Ms(unsigned int delay_ms)
{
    unsigned int ticks= (10000 / (2*1000)) * delay_ms; //calculate the value of the capture
    TimerA0_control=TimerSource_ACLK|TimerMode_STOP|TimerDividerBy2|TimerClear;
    TimerA0_capture_value = ticks - 1;
    TimerA0_channel0_control = TimerInterrupt_enable;
    TimerA0_control |= TimerMode_UP;
}


 void startTimerA0()
  {
      tickCounter = 0;
      TimerA0_control &= ~TimerMode_hold;
      TimerA0_control |= TimerSource_ACLK | TimerMode_UP;

  }

 void stopTimerA0()
 {
     TimerA0_control &= ~TimerMode_hold;
 }



 // chicho staff


  void startTimerA1Capture(void) {
      _capture_count = 0;
      _capture_done  = 0;

      // reset the timer counter
      TimerA1_counter = 0;

      // Clear any pending flag first
      TimerA1_channel2_control &= ~CCIFG;

      // start Timer A1 in continuous mode (SMCLK source)
      TimerA1_control = TimerSource_SMCLK // select SMCLK as the timer clock
                      | TimerMode_CONT    // put the timer into continuous mode
                      | TimerClear;       // clear TAR to zero
      while (_capture_done == 0){
      // Then enable rising edge capture, synchronous, capture mode + interrupt
      TimerA1_channel2_control  = CM_1   // capture on rising edge
                                | CCIS_0 // select CCI2A input (default)
                                | SCS    // synchronous capture
                                | CAP    // enable capture mode
                                | CCIE;  // enable interrupt

      __bis_SR_register(LPM0_bits + GIE); // sleep until interraput
      }

  }

  void stopTimerA1Capture(void) {
      // halt Timer A1 (stop mode + clear)
      TimerA1_control = TimerSource_SMCLK | TimerMode_STOP | TimerClear;
  }

  unsigned int getCapturedFrequency(void) {
      // return the last computed frequency from the ISR
      return frequency_cal;
  }


