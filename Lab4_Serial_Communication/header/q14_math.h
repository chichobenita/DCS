#ifndef Q14_MATH_H
#define Q14_MATH_H


#include <stdint.h>
#include  "../header/bsp.h"
#define Q14_SHIFT           14
#define Q14_ONE             (1 << Q14_SHIFT)
#define Q14_SCALE(x)        ((x) << Q14_SHIFT)
#define ADC_RESOLUTION      1023U

#define Q14_BASE_FREQ     Q14_SCALE(1000) // 1kHz
#define Q14_FREQ_SPAN     Q14_SCALE(1500) // 1.5kHz

#define Q14_MAX        ((1 << 16) - 1)         // = 65535 (uint16_t)

#define Q14_VREF_SCALE     (Q14_ONE * VREF_MV / 1000U)  // 3.3V * 16384 = 54067

#define VREF_MV            3300U         // Reference voltage in mV

typedef uint16_t q14_t;

static inline q14_t q14_add(q14_t a, q14_t b) {
    return a + b;
}

static inline q14_t q14_sub(q14_t a, q14_t b) {
    return a - b;
}

static inline q14_t q14_mul(q14_t a, q14_t b) {
    return (q14_t)(((int32_t)a * b) >> Q14_SHIFT);
}

static inline q14_t q14_div(q14_t a, q14_t b) {
    return (q14_t)(((int32_t)a << Q14_SHIFT) / b);
}


// === ADC conversion (10-bit ADC, 0-3.3V) ===
// returns Q14 representation of voltage from ADC value
static q14_t adc_to_q14(uint16_t adc_val) {
    return (q14_t)(((uint32_t)adc_val * Q14_VREF_SCALE) / ADC_RESOLUTION);
}

// === Q14 to raw integer (scaled down) ===
static inline uint16_t q14_to_adc(q14_t qval) {
    return (uint16_t)(((uint32_t)qval * ADC_RESOLUTION) / Q14_VREF_SCALE);
}

static q14_t adc_to_freq_q14(uint16_t adc_val) {
    return Q14_BASE_FREQ + ((uint32_t)adc_val * Q14_FREQ_SPAN) / ADC_RESOLUTION;
}

#endif // Q14_MATH_H

