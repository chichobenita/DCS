#include  "../header/ultrasonic.h"


 volatile uint16_t echo_start = 0;
 volatile uint16_t echo_duration = 0;
 volatile uint8_t measurement_done = 0;
 volatile unsigned char waiting_for_echo = 0;


void ultrasonic_init(void)
{
    // Trigger: P2.2
    P2DIR |= BIT2;
    P2OUT &= ~BIT2;

    // Echo: P2.4 as TA1.2 input
    P2DIR &= ~BIT4;
    P2SEL |= BIT4;

    TA1CCTL2 = 0;
}



static void send_trigger_pulse(void)
{
    P2OUT |= BIT2;
    __delay_cycles(15); // 10ֲµs
    P2OUT &= ~BIT2;
}

unsigned int ultrasonic_measure_cm(void)
{
    waiting_for_echo = 1;
    measurement_done = 0;

    TA1CTL = TASSEL_2 + MC_2 + TACLR; // SMCLK, Continuous mode
    TA1CCTL2 = CM_3 | CCIS_0 | CAP | CCIE ;//CM_3 + CCIS_0 + SCS + CAP + CCIE; // both edges

    send_trigger_pulse();

    // Timeout: 30ms using Timer A0
    //TA0CCTL0 = CCIE;
    //TA0CCR0 = 32768; // ג‰ˆ30ms for ACLK @ ~1kHz
    //TA0CTL = TASSEL_1 + MC_1 + TACLR;
    setupTimerA0Ms(30);
    startTimerA0();

    __bis_SR_register(LPM0_bits + GIE);

    TA0CTL = MC_0;
    TA1CTL = MC_0;
    TA1CCTL2 = 0;

    if (!measurement_done)
        return 0; // timeout or failure

    return echo_duration / 58; // convert to cm
}


void ultrasonic_get_samples_cm(void)
{
    uint16_t* buffer = get_write_buffer();
    uint8_t i;
    for (i = 0; i < ULTRASONIC_NUM_SAMPLES; i++) {
        buffer[i] = ultrasonic_measure_cm();
        __delay_cycles(10000); // ׳”׳�׳×׳ ׳” ׳§׳˜׳ ׳” ׳‘׳™׳� ׳“׳’׳™׳�׳•׳×
    }
}
