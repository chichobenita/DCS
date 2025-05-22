#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer




void startCaptureFreq()
{
    char* start="fin = ";
    char* end ="Hz";
    char strFreq[6] = {'\0'};
    if (start1==0)
    {
        lcd_puts(start);

        start1=1;
        TimerA1_control |= TimerMode_CONT;


    }
    sprintf(strFreq, "%d", frequency);
    lcd_puts(strFreq);
}
 
void intToStr(unsigned int num , char* str)
{

}


void updateClock()
{
    /*
     * this function check if the tickCounter change by the timer and update the current time
     */
    unsigned char lastMin = timeMinute;
    unsigned char lastSec = timeSec;
    if (tickCounter > 0)
        {
        tickCounter--;

        timeSec++;
        if (timeSec == 60)
            {
                timeSec = 0;
                timeMinute++;
            }
        updateTimeOnLCD(lastMin, lastSec, timeMinute, timeSec);
        /*lcd_cursor(0);
        char timeStr[6] = {0};
        sprintf(timeStr, "%d:%d", timeMinute, timeSec);
        lcd_puts(timeStr);*/
    }
   // __bis_SR_register(LPM0_bits + GIE); //turn off the cpu untill inturprte
}



void updateTimeOnLCD(unsigned int oldMin, unsigned int oldSec, unsigned int newMin, unsigned int newSec)
{
    /*
     * this function check what are the digits that change and print only them for saving time and power
     */
       unsigned char oldMinTens = oldMin / 10;
       unsigned char oldMinUnits = oldMin % 10;
       unsigned char oldSecTens = oldSec / 10;
       unsigned char oldSecUnits = oldSec % 10;

       unsigned char newMinTens = newMin / 10;
       unsigned char newMinUnits = newMin % 10;
       unsigned char newSecTens = newSec / 10;
       unsigned char newSecUnits = newSec % 10;

       //
       if (oldMinTens != newMinTens)
       {
           lcd_cursor(0); // 10 minutes pass
           lcd_putchar('0' + newMinTens);
       }
       if (oldMinUnits != newMinUnits)
       {
           lcd_cursor(1); // 1 minutes pass
           lcd_putchar('0' + newMinUnits);
       }

       //

       if (oldSecTens != newSecTens)
       {
           lcd_cursor(3); // 10 sec pass
           lcd_putchar('0' + newSecTens);
       }
       if (oldSecUnits != newSecUnits)
       {
           lcd_cursor(4); // 1 sec pass
           lcd_putchar('0' + newSecUnits);
       }
}


void handleState2()
{
    static unsigned char firstTimeInState2 = 1;
    static unsigned char firstTimeInState2_2 = 1;

    disable_interrupts();
        lcd_clear();
        lcd_puts("00:00");
        firstTimeInState2_2=0;
        setupTimerA0Ms(1000);
    while(state == state2){
    if (SWsArrPort&SW0)
          {
              Flag_PB_status[state2]=1 ;
              if (firstTimeInState2)
              {
                  firstTimeInState2=0;
                  startTimerA0();
              }

          }
    if(Flag_PB_status[state2])
    {

        updateClock();
    }
     enable_interrupts();
      __bis_SR_register(LPM0_bits + GIE); // sleep until interraput
}
}



void handleState3()
{
    static unsigned char firstTimeInState3 = 1;
    disable_interrupts();
    if(firstTimeInState3)
    {
        lcd_clear();
        lcd_puts("Start state 3");
        firstTimeInState3=0;

    }
    if(Flag_PB_status[state3])
    {
        updatePWM(f_out);
        ADC_control|=ADC_conversion +ADC_start_conversion ;





    }

    enable_interrupts();
    __bis_SR_register(LPM0_bits + GIE); // sleep until interraput

}

void updatePWM(float  f_out)
{
    uint16_t pwm_period = (uint16_t)(SMCLK_FREQ / f_out);



   /* //int freq_hz = f_out >> Q14_SHIFT;                         // äçì÷ äùìí
    //int frac = ((f_out & (Q14_ONE - 1)) * 100) >> Q14_SHIFT;
    char str[16];
    sprintf(str, "f = %d Hz", f_out);
    lcd_clear();
    lcd_cursor(0);
    lcd_puts(str);*/


}


// state 1
void handleStateFreq(void) {
    disable_interrupts();
    lcd_clear();
    lcd_puts("fin =       Hz");
    while(state == state1){
    // 1) arm the capture
    startTimerA1Capture();

    // 2) spin until the ISR tells us we have two edges
    unsigned int f = someApiFunction();

    // 3) stop the timer
    char numbuf[5];               // 4 digits + null
    sprintf(numbuf, "%d", f); // right-justify in 4
    lcd_goto_xy(0, 6);            // move to the first space
    lcd_puts(numbuf);             // overwrite old digits
    DelayMs(5000);

    // 4) display the result
    stopTimerA1Capture();
    enable_interrupts();
}
}
unsigned int someApiFunction(void) {
    // just read the global—no wrapper function needed
    return getCapturedFrequency();
    // …use f…
}



void tone_generator(){
    lcd_clear();
    lcd_puts("Start state 3");
    float n = 1000;
        float m = 1.466;  // m = 1500 / 1023;
        TA1CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
        TA1CCTL1 =  OUTMOD_7;
        while(state == state3){
            ADC10CTL0 |= ENC + ADC10SC;             // Start sampling
            __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
            ADC10CTL0 &= ~ADC10ON; // turns adc off after sample
            unsigned int N_adc = ADC10MEM;
            float f_out = m * N_adc + n;  // Choose Linear Mapping
            float SMCLK_FREQ_hz = 1048576L; // SMCLK freq ~ 2^20
            unsigned int period_to_pwm = SMCLK_FREQ_hz/f_out;
            TA1CCR0 = period_to_pwm;   //set
            TA1CCR1 = (int) period_to_pwm/2; //reset
        }
        TA1CTL &= ~MC_0;
        TA1CCTL1 =  OUTMOD_0;
    }



////////////real time lab//////////////
void real_time_b(){
    int row = 0;
    int col = 0;
    int last_row = 0xFF;
    int last_col = 0xFF;
    while(state == state4){
        if (last_row != 0xFF){
            lcd_goto_xy(last_row,last_col);
            lcd_puts(" ");
        }
        lcd_goto_xy(row,col);
        lcd_puts("b");
        last_row = row;
        last_col = col;
        col++;
        if (col ==16)
        {
            col=0;
            row++;
        }
        if (row==2)
        {
            row=0;
        }
        disable_interrupts();
        setupTimerA0Ms(900);
        startTimerA0();

        //TA0CTL |= TACLR;
        //TA0CCR0 = 0xE666;
        //TA0CTL  = TASSEL_2 + ID_3 + MC_3 + TAIE;
        __bis_SR_register(LPM0_bits + GIE);
        }
        //TA0CTL &= ~(MC_3 + TAIE);
        //TA0CTL  |= TACLR;
        stopTimerA0();
        }

//lcd_goto_xy(0, 6);            // move to the first space
    //lcd_puts(numbuf);             // overwrite

