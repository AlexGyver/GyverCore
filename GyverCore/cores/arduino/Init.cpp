#include "Arduino.h"

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