#ifndef _app_H_
#define _app_H_


enum FSMstate {state0,
               state1,  // val ++ or val --
               state2,  // single led each time
               state3,  // pwm
               state4}; // global variable


enum SYSmode{mode0,mode1,mode2,mode3,mode4}; // global variable

extern  unsigned char LEDs_status_state[5];
extern char Flag_PB_status[5];
extern char stopPWM;
#endif







