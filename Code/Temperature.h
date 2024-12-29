#include <mega32.h>
#include <delay.h>

#define V_REF 5.0 // Reference voltage for ADC in volts

void ADC_init();
unsigned int ADC_read();
float calculate_temperature(unsigned int adc_value);
