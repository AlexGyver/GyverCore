/* внешние прерывания*/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "Arduino.h"

extern void (*ext_isr0)();
extern void (*ext_isr1)();
void (*ext_isr0)();
void (*ext_isr1)();

ISR(INT0_vect){
(*ext_isr0)();	
}
ISR(INT1_vect){
(*ext_isr1)();	
}


void attachInterrupt(uint8_t num,void (*isr)(), uint8_t type){
switch(num){
case 0:
ext_isr0 = *isr;
EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (type << ISC00);
EIMSK |= (1 << INT0);
break;
case 1:
ext_isr1 = *isr;
EICRA = (EICRA & ~((1 << ISC10) | (1 << ISC11))) | (type << ISC10);
EIMSK |= (1 << INT1);
break;
}
}


void detachInterrupt(uint8_t num){
switch(num){
case 0:
EIMSK &=~ (1<<INT0);
break;
case 1:
EIMSK &=~  (1<<INT1);
break;
}
}