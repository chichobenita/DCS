
#ifndef HEADER_ULTRASONIC_H_
#define HEADER_ULTRASONIC_H_

#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include  "../header/app.h"

#define ULTRASONIC_NUM_SAMPLES 10

void ultrasonic_init(void);
unsigned int ultrasonic_measure_cm(void);

unsigned int ultrasonic_get_average_cm(void);
unsigned int ultrasonic_get_max_cm(void);

void ultrasonic_get_samples_cm(void);
extern volatile uint16_t echo_start;
extern volatile uint16_t echo_duration;
extern volatile uint8_t measurement_done;
extern volatile unsigned char waiting_for_echo;

#endif /* HEADER_ULTRASONIC_H_ */
