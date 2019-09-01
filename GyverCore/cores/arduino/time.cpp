#include "Arduino.h"
#include <util/delay.h>

/* функции времени и инициализация таймеров , АЦП*/

#ifndef _GYVERCORE_NOMILLIS  // millis включен
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)
#define MICROS_MULT (64 / clockCyclesPerMicrosecond())

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect){
	timer0_millis += MILLIS_INC;
	timer0_fract += FRACT_INC;
	if (timer0_fract >= FRACT_MAX) {
		timer0_fract -= FRACT_MAX;
		timer0_millis++;
	}	
	timer0_overflow_count++;
}

unsigned long millis() {	
	cli(); // остановить счет
	unsigned long m = timer0_millis; // перехватить значение
	sei(); // продолжить счет
	return m; // вернуть миллисекунды
}

unsigned long micros() {
	cli(); // остановить прерывания
	unsigned long m = timer0_overflow_count; // счет переполнений
	uint8_t t = TCNT0; // считать содержимое счетного регистра
	if ((TIFR0 & _BV(TOV0)) && (t < 255)) //инкремент по переполнению
	m++;
	sei(); // продолжить счет	
	return (long)(((m << 8) + t) * MICROS_MULT);  // вернуть микросекунды
}
#endif

void delay(unsigned long ms) {
#ifdef _GYVERCORE_NOMILLIS
	_delay_ms(ms);
#else
	
	uint32_t start = micros(); // запомнили время старта
	while (ms > 0) { // ведем отсчет
		yield();
		while ( ms > 0 && (micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
#endif
}

void delayMicroseconds(unsigned int us) {
#if defined(_GYVERCORE_NOMILLIS) || F_CPU < 1000000L
	_delay_us(us);
#else
	
	// работает на счете тиков 
#if F_CPU >= 16000000L
	if (us <= 1) return; //  = 3 cycles, (4 when true)
	us <<= 2; // x4 us, = 4 cycles
	us -= 5;
#elif F_CPU >= 8000000L
	if (us <= 2) return; //  = 3 cycles, (4 when true)
	us <<= 1; //x2 us, = 2 cycles
	us -= 4; // = 2 cycles
#elif F_CPU >= 1000000L
	if (us <= 16) return; //= 3 cycles, (4 when true)
	if (us <= 25) return; //= 3 cycles, (4 when true)
	us -= 22; // = 2 cycles
	us >>= 2; // us div 4, = 4 cycles	
#endif

	// busy wait
	__asm__ __volatile__ (
	"1: sbiw %0,1" "\n\t" // 2 cycles
	"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
	// return = 4 cycles
#endif
}