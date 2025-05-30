#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"

extern void handleState2();
extern void handleState3();
extern void real_time_b();
extern void createPWM_4();
extern void createPWM_2();
extern void startCaptureFreq();
extern void updatePWM(float f_out);

extern void handleStateFreq();
unsigned int someApiFunction(void);

extern void tone_generator();

extern void updateClock();
extern void updateTimeOnLCD(unsigned int oldMin, unsigned int oldSec, unsigned int newMin, unsigned int newSec);




#endif







