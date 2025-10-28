#ifndef _app_H_
#define _app_H_

#include "../header/servo.h"
#include "../header/q14_math.h"
#include  "../header/radar_protocol.h"
#include "stdio.h"


extern enum FSMstate {state0,
               Increment_LCD_State,
               Decrement_LCD_State,
               Rorate_Right_LCD_State,
               Set_Delay_State,
               Clear_LCD_State,
               Servo_Degree_State,
               Servo_Scan_State,
               Sleep_State,
               state9,
               GET_DELAY,
               LDR_Conf_State,
               Get_all_LDR_calibration,
               Full_Scan_Ultasonic_State,
               Full_Scan_LDR_State,
               Full_Scan_Mix_State,
               Servo_moving,
               Script_state,
               ReadFiles_state,
               ReadText_state
}; // global variable


enum SYSmode{mode0,mode1,mode2,mode3,mode4}; // global variable
extern  enum FSMstate nextstate , poststate;
extern char start1;
extern uint8_t last_capture;

extern unsigned char timeSec;
extern uint16_t num;
extern uint8_t tickCounter;
extern uint8_t new_state6;
extern uint8_t new_state7;
extern uint16_t LDR1[LDR_CALIB_COUNT];
extern uint16_t LDR2[LDR_CALIB_COUNT];
extern uint8_t stepConfigurtionLDR;
extern int8_t currentFile;

extern servo_sweep_t servo_status;



#endif







