#include "Arduino.h"
#include "timeControl.h"
/* функции времени и инициализация таймеров , АЦП*/
/*
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))	// 1024 на 16 МГц / 2048 на 8 МГц
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)					// 1 	на 16 МГц / 2 	на 8 МГц
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)				// 3 	на 16 МГц / 6 	на 8 МГц
#define FRACT_MAX (1000 >> 3)													// 125 	на 16 МГц / 125 на 8 МГц
*/

#if F_CPU >= 16000000L

#define MILLIS_INC	1
#define FRACT_INC	3
#define FRACT_MAX	125
#define MICROS_MULT	4

#elif F_CPU >= 8000000L

#define MILLIS_INC	2
#define FRACT_INC	6
#define FRACT_MAX	125
#define MICROS_MULT	8

#elif F_CPU >= 1000000L

#define MILLIS_INC	16
#define FRACT_INC	48
#define FRACT_MAX	125
#define MICROS_MULT	64

#endif

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

#ifdef MILLIS_TMRS  // переключатель, отключающий millis(), освобождающий вектор прерывания
ISR(TIMER0_OVF_vect)
{
	timer0_millis += MILLIS_INC;
	timer0_fract += FRACT_INC;
	if (timer0_fract >= FRACT_MAX) {
		timer0_fract -= FRACT_MAX;
		timer0_millis++;
	}	
	timer0_overflow_count++;
}
#endif

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

	// return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());		// default
}

void delay(unsigned long ms)  
{
	uint32_t start = micros(); // запомнили время старта

	while (ms > 0) { // ведем отсчет
		yield();
		while ( ms > 0 && (micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
}

void delayMicroseconds(unsigned int us) // работает на счете тиков 
{
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
}

void init() // функция инициализации
{
cli(); 
/* timer 0 */
TCCR0A = 0b00000011;  // fast pwm 8 bit
TCCR0B = 0b00000011; // делитель 64
TIMSK0 |= (1<<TOIE0); // ovf interrupt вкл
/* timer 1 */
TCCR1A = 0b00000001; // fast pwm 8 bit
TCCR1B = 0b00001011;  // делитель 64
/* timer 2 */
TCCR2A = 0b00000011;  // fast pwm 8 bit
TCCR2B = 0b00000100; // делитель 64
/* adc */
ADMUX = 0b01001111; // reference - vcc , вход ацп припаркован к gnd
ADCSRA = 0b10000010; // делитель  - 4   [0,1,2 bits - делитель]
/* ADC prescalers:  001 >> /2  010 >> /4  011 >> /8  100 >> /16  101 >> /32  110 >> /64  111 >> /128*/
/* UART */
UCSR0B = 0; // пока не вызван Serial.begin / uartBegin выводы 0/1 свободны для работы.
sei();
}
