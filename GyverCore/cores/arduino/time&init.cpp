#include "Arduino.h"
#include "timeControl.h"

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))	// 1024
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)					// 1
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)				// 3
#define FRACT_MAX (1000 >> 3)													// 125

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

#ifdef MILLIS_TMRS
ISR(TIMER0_OVF_vect)
{
	timer0_millis++;
	timer0_fract += 3;
	if (timer0_fract >= 128) {
		timer0_fract -= 125;
		timer0_millis++;
	}	
	timer0_overflow_count++;
}
#endif

unsigned long millis() {	
	cli();
	unsigned long m = timer0_millis;
	sei();
	return m;
}

unsigned long micros() {
	cli();
	unsigned long m = timer0_overflow_count;
	uint8_t t = TCNT0;
	if ((TIFR0 & _BV(TOV0)) && (t < 255))
	m++;
	sei();
	return (long)(((m << 8) + t) * 4); 
}

/*
void delayMicroseconds(unsigned int us)
{
	us *= 4; //1us = 4 цикла
	__asm__ volatile 
		  (	
			"1: sbiw %0,1" "\n\t" //; вычесть из регистра значение N
			"brne 1b"				
			: "=w" (us)
			: "0" (us)
		  );
}

void delay(unsigned long ms)
{
	while(ms)
	{
		delayMicroseconds(999);
		ms--;
	}
}
*/

/*
#include <util/delay.h>
void delay(unsigned long ms)
{
	_delay_ms(ms);
}

void delayMicroseconds(unsigned int us)
{
	_delay_us(us);
}
*/


void delay(unsigned long ms)
{
	uint32_t start = micros();

	while (ms > 0) {
		yield();
		while ( ms > 0 && (micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
}


void delayMicroseconds(unsigned int us)
{
	
#if F_CPU >= 16000000L
	if (us <= 1) return; //  = 3 cycles, (4 when true)
	us <<= 2; // x4 us, = 4 cycles
	us -= 5; 
#endif

	// busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
	// return = 4 cycles
}


void init()
{
cli();
/* timer 0 */
TCCR0A = 0b00000011;  // fast pwm 8 bit
TCCR0B = 0b00000011; // prescaler 64
TIMSK0 |= (1<<TOIE0); // ovf interrupt enable
/* timer 1 */
TCCR1A = 0b00000001; // fast pwm 8 bit
TCCR1B = 0b00001011;  // prescaler 64
/* timer 2 */
TCCR2A = 0b00000011;  // fast pwm 8 bit
TCCR2B = 0b00000100; // prescaler 64
/* adc */
ADMUX = 0b01001111; // reference - vcc , adc parked to gnd
ADCSRA = 0b10000010; // prescaler - 4   [0,1,2 bits - prescaler]
/* ADC prescalers:  001 >> /2  010 >> /4  011 >> /8  100 >> /16  101 >> /32  110 >> /64  111 >> /128*/
/* UART */
UCSR0B = 0;
sei();
}
