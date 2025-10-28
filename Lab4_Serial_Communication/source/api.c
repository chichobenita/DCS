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


void count_up()
{
        lcd_clear();


        if (Flag_PB_status[1]==0)
        {
            lcd_puts("00:00");
            Flag_PB_status[1]=1;
        }
        else
        {

        char time_buf[6];

        time_buf[0] = (timeMinute / 10) + '0';
        time_buf[1] = (timeMinute % 10) + '0';
        time_buf[2] = ':';
        time_buf[3] = (timeSec / 10) + '0';
        time_buf[4] = (timeSec % 10) + '0';
        time_buf[5] = '\0';

        lcd_puts(time_buf);
        }
        setupTimerA0Ms(X);
        startTimerA0();
    while((state == state1)&&(nextstate==state1)){
        {
               // __bis_SR_register(LPM0_bits + GIE);   /* CPU ׳™׳©׳� ׳¢׳“ ISR   */

                /* ׳�׳� ׳”-ISR ׳”׳’׳“׳™׳� tickCounter ג€“ ׳¢׳“׳›׳� ׳©׳¢׳•׳� */
                while (tickCounter)                   /* ׳™׳™׳×׳›׳� ׳™׳•׳×׳¨ ׳�׳˜׳™׳§ ׳�׳—׳“ */
                {
                    tickCounter--;                    /* ׳¦׳•׳¨׳›׳™׳� ׳˜׳™׳§        */
                    updateClock();                    /* ׳�׳•׳¡׳™׳£ sec + LCD   */
                }
            }
      __bis_SR_register(LPM0_bits + GIE); // sleep until interraput
        }
    }

void updateClock()
{
    /*
     * this function check if the tickCounter change by the timer and update the current time
     */
    unsigned char lastMin = timeMinute;
    unsigned char lastSec = timeSec;

        timeSec++;
        if (timeSec == 60)
            {
                timeSec = 0;
                timeMinute++;
            }
        updateTimeOnLCD(lastMin, lastSec, timeMinute, timeSec);
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



   /* //int freq_hz = f_out >> Q14_SHIFT;                         // ׳”׳—׳�׳§ ׳”׳©׳�׳�
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
    // just read the globalג€”no wrapper function needed
    return getCapturedFrequency();
    // ג€¦use fג€¦
}



void buzzer_tones(){
    lcd_clear();
    lcd_puts("Start state 2");


    P2SEL |= BIT2;                 /* P2.4 ג†’ TA1.1 */
    P2DIR |= BIT2;
       TA1CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
       TA1CCTL1 = OUTMOD_7;

    TA1CCR0 = 1048576/1000;        /* 1 kHz */
    TA1CCR1 = TA1CCR0/2;

    setupTimerA0Ms(X);
    startTimerA0();
    while((state == state2)&&(nextstate==state2)){
        //float SMCLK_FREQ_hz = 1048576L; // SMCLK freq ~ 2^20
        //unsigned int period_to_pwm = SMCLK_FREQ_hz/1250;  //tone_series_hz[0];
        //TA1CCR0 = period_to_pwm;   //set
        //TA1CCR1 =  (int) period_to_pwm/2; //reset
        //tone_idx = (tone_idx + 1) % NUM_TONES;    // ׳�׳¢׳‘׳¨ ׳�׳¢׳’׳�׳™ ׳�׳˜׳•׳� ׳”׳‘׳�

        //enable_interrupts();
        __bis_SR_register(LPM0_bits + GIE); // sleep until interraput
    }
    stopTimerA0();
    TA0CTL  = MC_0;                /* Halt TA0 */
    TA1CTL  = MC_0;                /* Halt TA1 */
    TA1CCTL1 = OUTMOD_0;           /* ׳₪׳™׳� ׳‘׳�׳¦׳‘ GPIO */
    TA1CCR0 = period;              /* תקופה */
    TA1CCR1 = period / 2;          /* 50 % Duty */

    }




void poten_meas(){
    disable_interrupts();
    lcd_clear();
    lcd_puts("v =       V");
    float m = 3.3/1023;
    TA1CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
    TA1CCTL1 =  OUTMOD_7;
    while((state == state4)&&(nextstate==state4)){
        ADC10CTL0 |= ENC + ADC10SC;             // Start sampling
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
        ADC10CTL0 &= ~ADC10ON; // turns adc off after sample
        unsigned int N_adc = ADC10MEM;
       // float v_out = m * N_adc;  // Choose Linear Mapping
        //char numbuf[5];               // 4 digits + null
        //sprintf(numbuf, "%.3f", v_out); // right-justify in 4
        uint16_t mv = (uint32_t)N_adc * 3300 / 1023;   /* 0-3300 mV */

        /* ׳”׳“׳₪׳¡ ׳›-"3.30" V */
        char numbuf[6];                                /* "x.xx\0" */
        uint16_t whole = mv / 1000;                    /* ׳•׳•׳�׳˜ ׳©׳�׳� */
        uint16_t frac  = (mv % 1000) / 10;             /* ׳©׳×׳™ ׳¡׳₪׳¨׳•׳× ׳�׳—׳¨׳™ ׳”׳ ׳§׳•׳“׳” */

        sprintf(numbuf, "%d.%d", whole, frac);      /* 0.00-3.30 */
        lcd_goto_xy(0, 6);            // move to the first space
        lcd_puts(numbuf);             // overwrite old digits
        DelayMs(5000);
    }
    }

////////////receive X  value///////////
void rec_X(){

        lcd_puts("X=");
        char numbuf[10];
        sprintf(numbuf, "%d", X);
        lcd_puts(numbuf);       //

        DelayMs(2000);
        state = state0;


}


////////////send i love my negev///////////
void send_love(){
    static const char msg[] ="I love my Negev!\r\n";
    api_send_msg(msg);
    nextstate=state0;

}




void api_send_msg(const char *msg)
{
    while (*msg)                     /* ׳¢׳“ ׳©׳›׳� ׳”׳×׳•׳•׳™׳� ׳”׳•׳–׳ ׳• */
    {
        /* ---------- ׳©׳�׳‘ ׳”׳–׳ ׳” ׳�׳˜׳‘׳¢׳×, ׳›׳� ׳¢׳•׳“ ׳™׳© ׳�׳§׳•׳� ---------- */
        while (*msg && ((head + 1U) % TX_BUF_SZ) != tail)
        {
            tx_buf[head] = *msg++;           /* ׳“׳—׳•׳£ ׳×׳• */
            head = (head + 1U) % TX_BUF_SZ;
        }

        /* ---------- ׳”׳₪׳¢׳� ׳©׳™׳“׳•׳¨ ׳�׳� ׳›׳‘׳•׳™ ---------- */
        __disable_interrupt();
        if (!(IE2 & UCA0TXIE) && (head != tail)) {
            UCA0TXBUF = tx_buf[tail];        /* ׳×׳• ׳¨׳�׳©׳•׳� / ׳”׳�׳©׳� */
            tail = (tail + 1U) % TX_BUF_SZ;
            IE2 |= UCA0TXIE;
        }
        __enable_interrupt();

        /* ---------- ׳�׳� ׳ ׳©׳�׳¨׳• ׳×׳•׳•׳™׳� ׳‘׳�׳—׳¨׳•׳–׳×, ׳�׳� ׳”׳˜׳‘׳¢׳× ׳�׳�׳�׳” ג€“ ׳”׳�׳×׳� ---------- */
        if (*msg)                            /* ׳¢׳•׳“ ׳�׳� ׳¡׳™׳™׳�׳ ׳• ׳�׳”׳–׳™׳� */
            __bis_SR_register(LPM0_bits + GIE);  /* ISR ׳™׳¢׳™׳¨ ׳›׳©׳ ׳•׳¦׳¨ ׳�׳§׳•׳� */
    }

    /* ---------- ׳›׳� ׳”׳×׳•׳•׳™׳� ׳”׳•׳–׳ ׳•; ׳—׳›׳” ׳¢׳“ ׳©׳”׳˜׳‘׳¢׳× ׳×׳×׳¨׳•׳§׳� ׳�׳’׳�׳¨׳™ ---------- */
    __bis_SR_register(LPM0_bits + GIE);      /* ISR ׳™׳¢׳™׳¨ ׳‘׳¡׳™׳•׳� ׳”׳©׳™׳“׳•׳¨ */
}

void reset_count()
{

    timeSec    = 0;
    timeMinute = 0;
    tickCounter = 0;
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

        __bis_SR_register(LPM0_bits + GIE);
        }
        stopTimerA0();
}




void api_handle_msg(const char* msg)
{
    char temp;

        temp = (msg[0]);
        switch(temp)
        {
        case '1':
            nextstate = state1;
            break;

        case '2':
            nextstate = state2;
            break;

        case '3':
        {
            nextstate = state3;
            X = (int)atoi(&msg[1]);
            break;
        }
        case '4':
        {
            nextstate = state4;
            break;
        }
        case '6':
            nextstate = state6;
            break;

        case '7':
        {
            nextstate = state7;
            strcpy(dataString, msg+1);

            break;
        }
        case '8':
            nextstate = state8;

            break;
        case '9':
            nextstate = state0;
            break;
        }
}

void get_string()
{

    lcd_puts(dataString);       //

    DelayMs(2000);
    nextstate = state0;

}




