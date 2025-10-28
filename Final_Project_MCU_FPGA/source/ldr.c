#include  "../header/ldr.h"
// source/calib.c
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

/* --- עזרים לגישה לפלאש --- */
static inline uint16_t seg_align_64(uint16_t addr) { return addr & (uint16_t)~0x003F; }

static void flash_erase_64(uint16_t anyAddr)
{
    uint16_t seg = seg_align_64(anyAddr);
    __disable_interrupt();
    FCTL2 = FWKEY | FSSEL_1 | FN0;   // SMCLK / 1 (התאם אם צריך)
    FCTL3 = FWKEY;                   // פתח LOCK
    FCTL1 = FWKEY | ERASE;           // מצב מחיקה
    *(volatile uint16_t*)seg = 0;    // כתיבה “דמי” – מפעילה מחיקה
    while (FCTL3 & BUSY) ;
    FCTL1 = FWKEY;
    FCTL3 = FWKEY | LOCK;            // נעל
    __enable_interrupt();
}

static void flash_write16(uint16_t addr, uint16_t w)
{
    __disable_interrupt();
    FCTL2 = FWKEY | FSSEL_1 | FN0;   // SMCLK / 1
    FCTL3 = FWKEY;                   // פתח LOCK
    FCTL1 = FWKEY | WRT;             // מצב כתיבה
    *(volatile uint16_t*)addr = w;   // כתיבת מילה אחת
    while (FCTL3 & BUSY) ;
    FCTL1 = FWKEY;                   // סגור WRT
    FCTL3 = FWKEY | LOCK;            // נעל
    __enable_interrupt();
}


void ldr_init()
{
    LDR_PortSel    &=~LDR_Leg1;
    LDR_PortSel    &=~LDR_Leg2;
    LDR_PortDir    &=~LDR_Leg1;
    LDR_PortDir    &=~LDR_Leg2;

    TimerA1_control = TimerMode_UP+TimerSource_SMCLK;

    ADC_control=ADC_on+ADC_enable_Interrupt+ADC_sample_hold+ADC_source_ref;
    ADC_TimeControl=ADC_input_channel0+ADC_clock_source;
    ADC_analog_enable &= ~LDR_Leg2;
    ADC_analog_enable |= LDR_Leg1;

}
void enable_LDR_sample()
{
    ADC_control |=ADC_on +ADC_conversion+ ADC_start_conversion ;
    ADC_control |=ADC_enable_Interrupt;
}

void disable_LDR_sample()
{
    ADC_control &=~ ADC_conversion;
    ADC_control &=~ ADC_on;
    ADC_control &=~ ADC_start_conversion;
    ADC_control &=~ADC_enable_Interrupt;
}
uint16_t getSampleOfLDR()
{
    return (uint16_t)ADC_value;
}

void ldr1_configuration()
{
    ADC_TimeControl= ADC_input_channel0 +ADC_clock_source;
}
void ldr2_configuration()
{
    ADC_TimeControl= ADC_input_channel3 +ADC_clock_source;
}


void getFullSampleOfLDR(uint16_t* ldr1 ,uint16_t* ldr2)
{
    ldr1_configuration();
    enable_LDR_sample();
    __bis_SR_register(LPM0_bits + GIE);
    disable_LDR_sample();
    *ldr1=getSampleOfLDR();


    //Sample from LDR2
    ldr2_configuration();
    enable_LDR_sample();
    __bis_SR_register(LPM0_bits + GIE);
    disable_LDR_sample();
    *ldr2=getSampleOfLDR();
}

/* --- כתיבת דפוסי כיול “דמה” --- */
bool ldr_calib_write_dummy(uint16_t LDR1[LDR_CALIB_COUNT],uint16_t LDR2[LDR_CALIB_COUNT])
{
    //uint16_t LDR1[LDR_CALIB_COUNT];
    //uint16_t LDR2[LDR_CALIB_COUNT];
    /* הגנה: אל תמחק בטעות את Seg B/A */
    /* כאן מוחקים רק C ו-D: */
    flash_erase_64(LDR0_CALIB_ADDR);   // Seg C
    flash_erase_64(LDR1_CALIB_ADDR);   // Seg D

    /* דפוסי בדיקה:
       LDR0: 0x0100, 0x0200, ... 0x0A00
       LDR1: 0x0111, 0x0222, ... 0x0AAA
       (סתם תבניות ברורות לקריאה בזיכרון) */
    uint16_t base0 = LDR0_CALIB_ADDR;
    uint16_t base1 = LDR1_CALIB_ADDR;
    uint8_t i;
    for (i = 0; i < LDR_CALIB_COUNT; ++i) {
        uint16_t v0 = LDR1[i];//(uint16_t)((i+1) << 8);            // 0x0100, 0x0200, ...
        uint16_t v1 = LDR2[i]; //(uint16_t)((i+1) * 0x0111u);       // 0x0111, 0x0222, ...

        flash_write16((uint16_t)(base0 + i*2u), v0);
        flash_write16((uint16_t)(base1 + i*2u), v1);
    }

    /* אופציונלי: כתוב “MAGIC” בתחילת כל סגמנט כדי לסמן שהכיול קיים
       (לא חובה כרגע; אם תרצה – אפשר לשים 0xC10L/0xC11L במילה הראשונה). */

    return true;
}


