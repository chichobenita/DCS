#include  "../header/servo.h"


void servo_init(uint16_t pulse , uint16_t degree) {
        //__disable_interrupt();

    if (CALBC1_1MHZ != 0xFF) {
            DCOCTL = 0;
            BCSCTL1 = CALBC1_1MHZ;
            DCOCTL = CALDCO_1MHZ;
        }
        BCSCTL2 &= ~(DIVS_3); // SMCLK /1

        // ׳”׳’׳“׳¨׳× ׳₪׳™׳� P2.1 ׳�ײ¾PWM
        SERVO_PWM_PORT_DIR |= SERVO_PWM_BIT;
        SERVO_PWM_PORT_SEL |= SERVO_PWM_BIT;
        P2SEL2 &= ~SERVO_PWM_BIT;

        // ׳˜׳™׳™׳�׳¨ A1: PWM ׳‘׳×׳“׳¨ 50Hz
        SERVO_PWM_TIMER_CONTROL = SERVO_PWM_TIMER_SRC | SERVO_PWM_TIMER_MODE | TACLR; // SMCLK, up mode
        SERVO_PWM_TIMER_TOP = 25000 - 1; // 20ms period
        SERVO_PWM_TIMER_CHANNEL_CONTROL = CM_0;
        servo_status.step=5;
    }


void moveServoTo(uint8_t degree)
{

        static uint8_t last_degree = 180;   // ׳‘׳¨׳™׳¨׳× ׳�׳—׳“׳� ׳”׳×׳—׳�׳×׳™׳×
        uint8_t delta = (degree > last_degree) ? (degree - last_degree) : (last_degree - degree);
        last_degree = degree;

        // convert degree to pulse
        uint16_t pulse = SERVO_PWM_MIN_PULSE + degree *10;  // approx mapping for 1ms-2ms over 0-180

        poststate=state;
        state=Servo_moving;
        // pwm on
         // SMCLK, up mode
        SERVO_PWM_TIMER_TOP = 25000 - 1; // 25ms period
        SERVO_PWM_TIMER_CAPTURE_VALUE = pulse;
        SERVO_PWM_TIMER_CHANNEL_CONTROL = OUTMOD_7;
        SERVO_PWM_TIMER_CONTROL = SERVO_PWM_TIMER_SRC | SERVO_PWM_TIMER_MODE | TACLR;

        uint16_t delay_ms;
        if (poststate==Servo_Degree_State)
        {
        // calculate the time for delay
            delay_ms = 500 + delta*10 ;
        }
        else
        {
            delay_ms = 200 + delta ;
        }


        setupTimerA0Ms(delay_ms);
        servo_status.running = 1;
        startTimerA0();
        __bis_SR_register(LPM0_bits + GIE);  // ׳�׳—׳›׳” ׳©ײ¾TimerA0 ׳™׳¡׳×׳™׳™׳�

        //PWM off

}


void stopScanning(){
    SERVO_PWM_TIMER_CHANNEL_CONTROL = 0  ; // stop
    SERVO_PWM_TIMER_CONTROL=MC_0;
}





//static servo_sweep_t sweep = { 0, 0, 90, 90, 0 };

/*tatic uint8_t clamp180(uint8_t d)
{
    if (d > 180) d = 180;
    return d;
}*/

/* ן¿½ן¿½ן¿½ן¿½ן¿½ ן¿½ן¿½ן¿½ן¿½ן¿½ ן¿½ן¿½-ן¿½ן¿½ן¿½ן¿½ן¿½: ן¿½ן¿½ן¿½ן¿½ן¿½ן¿½ן¿½ from_deg ן¿½ן¿½ to_deg, ן¿½ן¿½ן¿½ן¿½ן¿½ן¿½ ן¿½ן¿½ step_deg */
/*void startSweepOnce(uint8_t from_deg, uint8_t to_deg, uint8_t step_deg)
{
    int8_t s;

    from_deg = clamp180(from_deg);
    to_deg   = clamp180(to_deg);

    if (from_deg == to_deg) {

        moveServoTo(to_deg);
        sweep.running = 0;
        sweep.done    = 1;
        sweep.cur     = to_deg;
        sweep.target  = to_deg;
        sweep.step    = 0;
        return;
    }

    if (step_deg == 0) step_deg = 1;


    s = (from_deg < to_deg) ? (int8_t)step_deg : -(int8_t)step_deg;

    sweep.running = 1;
    sweep.done    = 0;
    sweep.cur     = from_deg;
    sweep.target  = to_deg;
    sweep.step    = s;


    moveServoTo(sweep.cur);
}*/


/*void continueSweepOnce(void)
{
    uint8_t finished;
    int next;

    if (!sweep.running) return;


    if (!servo_action_done) return;


    next = (int)sweep.cur + (int)sweep.step;

    finished = 0;
    if (sweep.step > 0) {
        if (next >= (int)sweep.target) { next = (int)sweep.target; finished = 1; }
    } else {
        if (next <= (int)sweep.target) { next = (int)sweep.target; finished = 1; }
    }

    sweep.cur = (uint8_t)next;
    moveServoTo(sweep.cur);

    if (finished) {

        sweep.running = 0;
        sweep.done    = 1;
    }
}*/
/*
uint8_t sweep_isRunning(void)
{
    return sweep.running;
}

uint8_t sweep_isDone(void)
{
    return sweep.done;
}

void sweep_abort(void)
{

    sweep.running = 0;
    sweep.done    = 0;

       SERVO_PWM_TIMER_CCTL = 0;

}*/

