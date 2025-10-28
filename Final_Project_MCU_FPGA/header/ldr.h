
#ifndef HEADER_LDR_H_
#define HEADER_LDR_H_
#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include  "../header/app.h"

#include <stdint.h>
#include <stdbool.h>

/* MSP430G2553 – Info Memory
   Seg D: 0x1000..0x103F
   Seg C: 0x1040..0x107F
   Seg B: 0x1080..0x10BF (משמש אצלך ל-FAT)
   Seg A: 0x10C0..0x10FF (אסור לשימוש)
*/
#define INFO_SEG_D   ((uint16_t)0x1060)   // LDR1
#define INFO_SEG_C   ((uint16_t)0x1040)   // LDR0

#define LDR0_CALIB_ADDR  INFO_SEG_C
#define LDR1_CALIB_ADDR  INFO_SEG_D
//#define LDR_CALIB_COUNT  10               // 10 דגימות לכל חיישן


#define LDR_PortDir     P1OUT
#define LDR_PortOut     P1DIR
#define LDR_PortSel     P1SEL
#define LDR_Leg1        BIT0
#define LDR_Leg2        BIT3

/* כותב דפוס כיול “דמה” לשני החיישנים:
   - מוחק Seg C ו-Seg D
   - כותב 10 מילים (uint16_t) לכל חיישן
   מחזיר true אם הצליח. */

void ldr_init();

void enable_LDR_sample();

void disable_LDR_sample();

uint16_t getSampleOfLDR();

void getFullSampleOfLDR();




void ldr1_configuration();
void ldr2_configuration();
bool ldr_calib_write_dummy(uint16_t LDR1[LDR_CALIB_COUNT], uint16_t LDR2[LDR_CALIB_COUNT]);






#endif /* HEADER_LDR_H_ */
