#include "Arduino.h"
#include <util/delay.h>

/* функции времени и инициализация таймеров , АЦП*/

void init() {       
	cli();

	/*************** ADC ***************/

#ifndef ADC_PRESCALER 
#define ADC_PRESCALER 0x02
#endif

	/******************************************************************
* ADC prescaler: [ /2 ]-[ /4 ]-[ /8 ]-[ /16]-[ /32]-[ /64]-[/128] *
* ADC_PRESCALER: [0x01]-[0x02]-[0x03]-[0x04]-[0x05]-[0x06]-[0x07] *
******************************************************************/
	ADCSRA = (1 << ADEN) | ADC_PRESCALER;



	/************ Timers ************/

	TCCR0A = (1 << WGM01) | (1 << WGM00);
	TCCR0B = (1 << CS01) | (1 << CS00);
#ifndef _GYVERCORE_NOMILLIS
	TIMSK0 |= (1 << TOIE0);
#endif 

	TCCR1A = (1 << WGM10);
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);

	TCCR2A = (1 << WGM20);
	TCCR2B = (1 << CS22);


	/************** USART **************/
	UCSR0B = 0x00;

	sei();
}


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
	uint8_t oldSREG = SREG; 			// запомнинаем были ли включены прерывания
	cli();								// выключаем прерывания
	unsigned long m = timer0_millis; 	// перехватить значение
	SREG = oldSREG; 					// если прерывания не были включены - не включаем и наоборот
	return m;						    // вернуть миллисекунды
}

unsigned long micros() {
	uint8_t oldSREG = SREG; 						// запомнинаем были ли включены прерывания
	cli();											// выключаем прерывания
	unsigned long m = timer0_overflow_count; 		// счет переполнений
	uint8_t t = TCNT0; 								// считать содержимое счетного регистра
	if ((TIFR0 & _BV(TOV0)) && (t < 255)) 			// инкремент по переполнению
	m++;
	SREG = oldSREG; 								// если прерывания не были включены - не включаем и наоборот
	return (long)(((m << 8) + t) * MICROS_MULT);  	// вернуть микросекунды
}
#endif

void delay(unsigned long ms) {
	
#ifdef _GYVERCORE_NOMILLIS
	while(ms){
		_delay_ms(ms);	
		ms--;
	}	
#else	
	uint32_t start = micros(); 
	while (ms > 0) { // ведем отсчет
		while ( ms > 0 && (micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
	
#endif
}

void delayMicroseconds(unsigned int us) {
	
#if F_CPU < 1000000L
	_delay_us(us);
#else
	

#if F_CPU >= 24000000L
	if (!us) return; //  = 3 cycles, (4 when true)
	us *= 6; // x6 us, = 7 cycles
	us -= 5; //=2 cycles
#elif F_CPU >= 20000000L
	__asm__ __volatile__ (
	"nop" "\n\t"
	"nop" "\n\t"
	"nop" "\n\t"
	"nop"); //just waiting 4 cycles
	if (us <= 1) return; //  = 3 cycles, (4 when true)
	us = (us << 2) + us; // x5 us, = 7 cycles
	us -= 7; // 2 cycles
#elif F_CPU >= 16000000L
	if (us <= 1) return; 	//  = 3 cycles, (4 when true)
	us <<= 2; 			// x4 us, = 4 cycles
	us -= 5;
#elif F_CPU >= 8000000L
	if (us <= 2) return; 	//  = 3 cycles, (4 when true)
	us <<= 1; 			//x2 us, = 2 cycles
	us -= 4; 				// = 2 cycles
#elif F_CPU >= 1000000L
	if (us <= 16) return; //= 3 cycles, (4 when true)
	if (us <= 25) return; //= 3 cycles, (4 when true)
	us -= 22; 			// = 2 cycles
	us >>= 2; 			// us div 4, = 4 cycles
#endif

	asm volatile
	(
	"loop_%=: 				\n\t"	
	"sbiw %[counter],1  \n\t"  // 2 cycles
	"brne loop_%=  		\n\t" 
	: [counter]"=w" (us) 
	: "0" (us) 				   // 2 cycles
	);

	// return = 4 cycles
#endif
}

