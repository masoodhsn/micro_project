#define F_CPU 16000000UL // Clock speed for atmega32
#include <mega32.h>       // Header for atmega32
#include <delay.h>        // To use for delay function
#include <stdint.h>
#include <stdio.h>
#include <eeprom.h>
#include <stdbool.h>

// Port definitions for easier reference
#define PC0 0  // Pin 0 of PORTC
#define PC1 1  // Pin 1 of PORTC
#define PB0 0  // Pin 0 of PORTB (Trigger)
#define PB1 1  // Pin 1 of PORTB (Echo)
#define PD0 0
#define PD1 1
#define PD2 2


// LCD Definitions
#define lcd_port PORTD    // LCD connected to PORTD
#define lcd_data_dir DDRD // LCD data direction register
#define rs PD0            // RS pin connected to PD0
#define en PD1            // Enable pin connected to PD1

// Ultrasonic Sensor Definitions
#define US_PORT PORTB     // Ultrasonic sensor connected to PORTB
#define US_PIN PINB       // Ultrasonic PIN register
#define US_DDR DDRB       // Ultrasonic data direction register

#define US_TRIG_POS PB0   // Trigger pin connected to PB0
#define US_ECHO_POS PB1   // Echo pin connected to PB1

// Error indicators
#define US_ERROR -1       // Error indicator
#define US_NO_OBSTACLE -2 // No obstacle indicator
extern unsigned char passedStu;
extern bool isObjectDetected;
void HCSR04Init();
void HCSR04Trigger();
uint16_t GetPulseWidth();
