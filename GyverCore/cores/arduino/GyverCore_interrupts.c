/********************************************
*  		GyverCore external interrupts 		*
********************************************/

#include <avr/interrupt.h>
#include "Arduino.h"

extern void (*ext_isr_0)();		// Functions pointers
extern void (*ext_isr_1)();
void (*ext_isr_0)();
void (*ext_isr_1)();

ISR(INT0_vect) {				// External interrupt vectors
  (*ext_isr_0)();
}

ISR(INT1_vect) {
  (*ext_isr_1)();
}


void attachInterrupt(uint8_t num, void (*handler)(), uint8_t type) {
  switch (num) {					
    case 0:
      ext_isr_0 = *handler;				// Pass a handler to the pointer
      EICRA = (EICRA & 0x0C) | type;	// Setup interrupt type
      bitSet(EIMSK, INT0);				// Enable external interrupt
      return;
    case 1:
      ext_isr_1 = *handler;
      EICRA = (EICRA & 0x03) | (type << 2);
      bitSet(EIMSK, INT1);
      return;
  }
}


void detachInterrupt(uint8_t num) {
  bitClear(EIMSK, num);					// Disable external interrupt
}