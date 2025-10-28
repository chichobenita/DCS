
#ifndef HEADER_SERVO_H_
#define HEADER_SERVO_H_
#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include <stdint.h>
#include  "../header/app.h"
#include  "../header/radar_protocol.h"



//===================================================================
//             Servo Motor PWM (TA1.1 on P2.1)
//===================================================================
#define SERVO_PWM_PORT_DIR    P2DIR
#define SERVO_PWM_PORT_SEL    P2SEL
#define SERVO_PWM_BIT         BIT1

#define SERVO_PWM_TIMER_CAPTURE_VALUE       TA1CCR1
#define SERVO_PWM_TIMER_CONTROL             TA1CTL
#define SERVO_PWM_TIMER_CHANNEL_CONTROL     TA1CCTL1
#define SERVO_PWM_TIMER_TOP                 TA1CCR0

#define SERVO_PWM_TIMER_SRC   TASSEL_2 // SMCLK
#define SERVO_PWM_TIMER_MODE  MC_1     // up mode
#define SERVO_PWM_OUTMODE     OUTMOD_7

#define SERVO_PWM_PERIOD      20000    // 20ms for 50Hz
#define SERVO_PWM_MIN_PULSE   400
#define SERVO_PWM_MAX_PULSE   2200

#define SERVO_MOVE_DURATION_MS 300     // ׳–׳�׳� ׳—׳™׳›׳•׳™ ׳�׳�׳¢׳‘׳¨ ׳–׳•׳•׳™׳×

volatile uint8_t servo_action_done ;

void servo_init(uint16_t pulse , uint16_t degree);

void moveServoTo(uint8_t degre);

void moveServoToInRate(unsigned char degre ,unsigned char Rate_degre);

void stopScanning();

void continueScanning();

void startFullScanning();


/* --- ׳¡׳¨׳™׳§׳” ׳—׳“-׳₪׳¢׳�׳™׳× ׳‘׳™׳� ׳©׳×׳™ ׳–׳•׳•׳™׳•׳× (׳�׳� ׳—׳•׳¡׳�) --- */
void startSweepOnce(uint8_t from_deg, uint8_t to_deg, uint8_t step_deg);
/* ׳�׳§׳¨׳•׳� ׳�׳—׳–׳•׳¨׳™׳× ׳�׳×׳•׳� ׳”-FSM/׳˜׳™׳™׳�׳¨ */
void continueSweepOnce(void);
/* ׳¡׳˜׳˜׳•׳¡ ׳ ׳•׳— ׳�׳©׳™׳�׳•׳© ׳‘׳�׳•׳�׳�׳” */
uint8_t sweep_isRunning(void);
uint8_t sweep_isDone(void);
/* ׳‘׳™׳˜׳•׳� ׳�׳™׳™׳“׳™ (׳¢׳¦׳™׳¨׳”) */
void sweep_abort(void);



#endif /* HEADER_SERVO_H_ */
