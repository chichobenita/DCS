#include  "../header/halGPIO.h"     // private library - HAL layer


static volatile int _capture_count = 0;
static volatile unsigned char  _capture_done = 0;
static volatile unsigned int frequency_cal = 0;
volatile uint8_t rx_idx = 0;
volatile uint16_t X = 500;
volatile uint16_t delayMs = 500;
volatile uint8_t x_ready = 0;
volatile uint8_t flag=0;
volatile int head = 0;
volatile int tail = 0;
volatile char    tx_buf[TX_BUF_SZ];
volatile char     rx_buf[];


//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMERconfig();
	ADCconfig();
	UART_Init(9600);
	ultrasonic_init();
	servo_init(400, 0);
	moveServoTo(0);
	ldr_init();
	//servo_init();

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
    uint16_t adc_val= ADC_value;

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
        if(state!= Servo_moving)
        {
            if (!waiting_for_echo)

            {
            __bic_SR_register_on_exit(LPM0_bits);
            break;
            }

                    if (P2IN & BIT4) {
                        echo_start = TA1CCR2; // rising edge

                    } else {
                        echo_duration = TA1CCR2 - echo_start; // falling edge
                        measurement_done = 1;
                        waiting_for_echo = 0;
                        __bic_SR_register_on_exit(LPM0_bits);
                    }
                    break;
        }
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
           if (state==Increment_LCD_State || state == Decrement_LCD_State || state==Rorate_Right_LCD_State )
           {
           tickCounter++; // 1 sec pass
           __bic_SR_register_on_exit(LPM0_bits); // wake the cpu up
           }
//           if (state== ReadText_state)
//           {
//               stopTimerA0();              // כיבוי הטיימר
//
//               __bic_SR_register_on_exit(LPM0_bits);
//           }
           if (state==Servo_moving) {
               stopScanning(); // כלומר: כיבוי פלט PWM
               state=poststate;
               servo_status.running = 0;
               servo_action_done = 1;      // מסמן ל-continueSweepOnce()/moveServoTo()
               __bic_SR_register_on_exit(LPM0_bits);  // wake CPU
           }

           if( (waiting_for_echo)&&(state!= Servo_moving)){
                   waiting_for_echo = 0;
                   measurement_done = 0;
                   __bic_SR_register_on_exit(LPM0_bits);
               }

}



//*********************************************************************
//            UART Interrupt Service Rotine
//*********************************************************************
volatile uint8_t  msg_idx = 0;
volatile uint8_t  msg_in_progress = 0;
volatile uint8_t  msg_expected = 0;   /* 2+LEN+1 כאשר ידוע */

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    uint8_t c = UCA0RXBUF;
    static radar_message_t msg_out;
    int8_t result = radar_parse_char(&msg_out, c);

    if (result == 1) {
        // we get a full message
        api_handle_msg(&msg_out);

        // wake the cpu up
    }
    else if (result==-1)
    {
        __bic_SR_register_on_exit(LPM0_bits);
    }


    __bic_SR_register_on_exit(LPM0_bits);

    /*
     * //התחלת פריים – רק אם *לא* באמצע פריים
    if (!msg_in_progress) {
        if (c == '@') {
            msg_idx = 0;
            msg_expected = 0;       // נחשב אחרי שנקבל LEN
            msg_in_progress = 1;
        }
        __bic_SR_register_on_exit(LPM0_bits);
        return;
    }
    else
    {
            // בתוך פריים
        if (c == '#')
        {
            if (msg_expected && msg_idx == msg_expected)
            {
                api_handle_msg((const char*)msg_buf, msg_idx);
                msg_in_progress = 0; msg_idx = 0; msg_expected = 0;
                __bic_SR_register_on_exit(LPM0_bits);
            }
            else
            {
                if (msg_idx < MSG_BUF_SIZE) msg_buf[msg_idx++] = c;  // '#' כחלק מהמידע
            }
            return;
        }
        else
        {
            // תו רגיל – הוסף לבאפר
            if (msg_idx < MSG_BUF_SIZE)
            {
                msg_buf[msg_idx++] = c;
                if (msg_idx == 2)
                {
                    uint8_t len = msg_buf[1];
                    msg_expected = (uint8_t)(2 + len + 1);  // cmd + len + payload + crc
                    if (msg_expected > MSG_BUF_SIZE)
                    {
                        // פיילוד גדול מדי  זריקה
                        msg_in_progress = 0;
                        msg_idx = 0;
                        msg_expected = 0;
                    }
                }
            }
            else
                 {
                    // Overflow  זריקה
                    msg_in_progress = 0;
                    msg_idx = 0;
                    msg_expected = 0;
                 }
        }

    }

    __bic_SR_register_on_exit(LPM0_bits);*/
}






/* ------- ISR TX  ------- */
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
    if (head != tail) {                     /* יש תו לשלוח */
        UCA0TXBUF = tx_buf[tail];
        tail = (tail + 1U) % TX_BUF_SZ;
    } else {                                /* תור ריק  סוף שידור */
        IE2 &= ~UCA0TXIE;                   /* כבה פסיקה          */
        __bic_SR_register_on_exit(LPM0_bits); /* העיר את LPM0      */
    }
}


//*********************************************************************
//            Port1 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void PBs_handler(void){
   
	delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------

	if(PBsArrIntPend & PB0 )//check who push the button
	  {

	        nextstate = ReadFiles_state;
	        //currentFile++;
	        PBsArrIntPend &= ~PB0; // reset the bits of the PBsArrIntPend
      }

     if(PBsArrIntPend & PB1)
     {
         nextstate = ReadText_state;
         PBsArrIntPend &= ~PB1;
     }




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





