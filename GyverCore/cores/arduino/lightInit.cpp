#include "Arduino.h"


/* Легкий вариант инцииализации периферии ATmega328 */
void lightInit() {
  /* timer 0*/
  TCCR0A = 0b00000011;
  TCCR0B = 0b00000011;
#ifndef _GYVERCORE_NOMILLIS
	TIMSK0 |= (1<<TOIE0); // ovf interrupt вкл
#endif
  /* timer 1 */
  TCCR1A = 0b00000001;
  TCCR1B = 0b00000011;
  /*timer 2*/
  TCCR2A = 0b00000001;
  TCCR2B = 0b00000100;
  /* adc */
  ADCSRA = 0b10000010;
}