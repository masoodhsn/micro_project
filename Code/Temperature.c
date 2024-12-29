#include "Temperature.h"

void ADC_init() {

    ADMUX = (1 << REFS0);// Configure ADC: AVCC as reference, ADC7 (PA7) as input, enable ADC
    ADMUX &= ~(1 << REFS1);
    //ADMUX=(ADMUX & 0xf8)|(7 & 0X07);
     ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Enable ADC and set prescaler to 64
}

unsigned int ADC_read() {
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    return ADCW; // Return ADC result
}

float calculate_temperature(unsigned int adc_value) {
    return (((float)adc_value * V_REF*100 ) / 1024); // Convert ADC value to temperature
}