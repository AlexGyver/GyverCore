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
	uint8_t oldSREG = SREG; // запомнинаем были ли включены прерывания
	cli();//выключаем прерывания
	unsigned long m = timer0_millis; // перехватить значение
	SREG = oldSREG; // если прерывания не были включены - не включаем и наоборот
	return m; // вернуть миллисекунды
}

unsigned long micros() {
	uint8_t oldSREG = SREG; // запомнинаем были ли включены прерывания
	cli();//выключаем прерывания
	unsigned long m = timer0_overflow_count; // счет переполнений
	uint8_t t = TCNT0; // считать содержимое счетного регистра
	if ((TIFR0 & _BV(TOV0)) && (t < 255)) //инкремент по переполнению
	m++;
	SREG = oldSREG; // если прерывания не были включены - не включаем и наоборот
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

void init() { // функция инициализации
	cli(); 
	/* timer 0 */
	TCCR0A = 0b00000011;  // fast pwm 8 bit
	TCCR0B = 0b00000011; // делитель 64
#ifndef _GYVERCORE_NOMILLIS
	TIMSK0 |= (1<<TOIE0); // ovf interrupt вкл
#endif
	/* timer 1 */
	TCCR1A = 0b00000001; // phasecorrect pwm 8 bit
	TCCR1B = 0b00001011;  // делитель 64
	/* timer 2 */
	TCCR2A = 0b00000001;  // phasecorrect pwm 8 bit
	TCCR2B = 0b00000100; // делитель 64
	/* adc */
	ADCSRA = 0b10000010; // делитель  - 4   [0,1,2 bits - делитель]
	/* ADC prescalers:  001 >> /2  010 >> /4  011 >> /8  100 >> /16  101 >> /32  110 >> /64  111 >> /128*/
	/* UART */
	UCSR0B = 0; // пока не вызван Serial.begin / uartBegin выводы 0/1 свободны для работы.
	sei();
}