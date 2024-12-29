#include <io.h>
#include <mega32.h>
#include <delay.h>

#define F_CPU 16000000UL // 16 MHz clock frequency
#define BAUD 9600
#define MYUBRR ((F_CPU / 16 / BAUD) - 1)
void USART_init(unsigned int ubrr);
void USART_Transmit(char data);
