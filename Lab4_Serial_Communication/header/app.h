#ifndef _app_H_
#define _app_H_

#include "../header/q14_math.h"
extern enum FSMstate {state0,
               state1,  // val ++ or val --
               state2,  // single led each time
               state3,  // pwm
               state4,
               state5,
               state6,  // val ++ or val --
               state7,  // single led each time
               state8,  // pwm
               state9,
               GET_DELAY
}; // global variable


enum SYSmode{mode0,mode1,mode2,mode3,mode4}; // global variable
extern  enum FSMstate nextstate;
extern  unsigned char LEDs_status_state[5];
extern char Flag_PB_status[5];
extern char start1;
extern unsigned int last_capture;
extern unsigned int period ;
extern unsigned int frequency ;
extern unsigned char timeSec;
extern unsigned char timeMinute;
extern unsigned int tickCounter;
extern float f_out;
extern unsigned int tone_idx;
extern unsigned int NUM_TONES ;
extern unsigned int tone_series_hz[7];
extern char dataString[MSG_BUF_SIZE];

extern char msg_buf[MSG_BUF_SIZE];
extern unsigned char msg_idx ;
extern unsigned char msg_in_progress;
#endif







