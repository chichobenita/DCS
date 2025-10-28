#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include  "../header/app.h"

extern void count_up();
extern void rec_X();
extern void buzzer_tones();
extern void poten_meas();
extern void send_love();
extern void reset_count();
extern void api_send_msg(const char *msg);
extern void get_string();

extern void handleState3();
extern void real_time_b();
extern void createPWM_4();
extern void createPWM_2();
extern void startCaptureFreq();
extern void updatePWM(float f_out);

extern void handleStateFreq();
unsigned int someApiFunction(void);







extern void updateClock();
extern void updateTimeOnLCD(unsigned int oldMin, unsigned int oldSec, unsigned int newMin, unsigned int newSec);
extern void api_handle_msg(const char* msg);



#endif







