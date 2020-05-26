/*
   GyverCore digital / analog IO functions
*/

#define ARDUINO_MAIN
#include "Arduino.h"
#include <avr/io.h>


/************************************
*  		Digital I/O functions  		*
************************************/

void pinMode(uint8_t pin, uint8_t mode) {
  switch (mode) {
    case INPUT:
      if (pin < 8) {
        bitClear(DDRD, pin);		
        bitClear(PORTD, pin);
      } else if (pin < 14) {
        bitClear(DDRB, (pin - 8));
        bitClear(PORTB, (pin - 8));
      } else if (pin < 20) {
        bitClear(DDRC, (pin - 14));	 // Mode: INPUT
        bitClear(PORTC, (pin - 8));  // State: LOW
      }
      return;
    case OUTPUT:
      if (pin < 8) {
        bitSet(DDRD, pin);
        bitClear(PORTD, pin);
      } else if (pin < 14) {
        bitSet(DDRB, (pin - 8));
        bitClear(PORTB, (pin - 8));
      } else if (pin < 20) {
        bitSet(DDRC, (pin - 14));	// Mode: OUTPUT
        bitClear(PORTC, (pin - 8));	// State: LOW
      }
      return;
    case INPUT_PULLUP:
      if (pin < 8) {
        bitClear(DDRD, pin);
        bitSet(PORTD, pin);
      } else if (pin < 14) {
        bitClear(DDRB, (pin - 8));
        bitSet(PORTB, (pin - 8));
      } else if (pin < 20) {
        bitClear(DDRC, (pin - 14));	// Mode: OUTPUT
        bitSet(PORTC, (pin - 14));	// State: HIGH
      }
      return;
  }
}

void digitalWrite(uint8_t pin, bool x) {
  switch (pin) {						
    case 3: bitClear(TCCR2A, COM2B1);
      break;
    case 5: bitClear(TCCR0A, COM0B1);
      break;
    case 6: bitClear(TCCR0A, COM0A1);
      break;
    case 9: bitClear(TCCR1A, COM1A1);
      break;
    case 10: bitClear(TCCR1A, COM1B1);
      break;
    case 11: bitClear(TCCR2A, COM2A1); 	// PWM disable 
      break;
  }

  if (pin < 8) {
    bitWrite(PORTD, pin, x);
  } else if (pin < 14) {
    bitWrite(PORTB, (pin - 8), x);
  } else if (pin < 20) {
    bitWrite(PORTC, (pin - 14), x);		// Set pin to HIGH / LOW 
  }
}

bool digitalRead (uint8_t pin) {
  if (pin < 8) {
    return bitRead(PIND, pin);
  } else if (pin < 14) {
    return bitRead(PINB, pin - 8);
  } else if (pin < 20) {
    return bitRead(PINC, pin - 14);		// Return pin state
  }
}


void digitalToggle(uint8_t pin) {
  if (pin < 8) {
    bitSet(PIND, pin);
  } else if (pin < 14) {
    bitSet(PINB, (pin - 8));
  } else if (pin < 20) {
    bitSet(PINC, (pin - 14));		// Toggle pin state (for 'tone()')
  }
}

/****************************************
*  		Analog inputs functions  		*
****************************************/

uint8_t analog_reference = DEFAULT;	
		
 /*
   Available prescalers: 2,4,8,16,32,64,128
 */ 
  
void analogPrescaler(uint8_t prescaler) {
  switch (prescaler) {
    case 2: ADCSRA = (ADCSRA & 0xF8) | 0x01;
      break;
    case 4: ADCSRA = (ADCSRA & 0xF8) | 0x02;
      break;
    case 8: ADCSRA = (ADCSRA & 0xF8) | 0x03;
      break;
    case 16: ADCSRA = (ADCSRA & 0xF8) | 0x04;
      break;
    case 32: ADCSRA = (ADCSRA & 0xF8) | 0x05;
      break;
    case 64: ADCSRA = (ADCSRA & 0xF8) | 0x06;
      break;
    case 128: ADCSRA = (ADCSRA & 0xF8) | 0x07;
      break;
  }
}

void analogReference(uint8_t mode) {
	analog_reference = mode; 
}

uint16_t analogRead(uint8_t pin) {
  pin = ((pin < 8) ? pin : pin - 14);		// analogRead(2) = analogRead(A2)
  ADMUX = (analog_reference << 6) | pin;	// Set analog MUX & reference
  bitSet(ADCSRA, ADSC);						// Start 
  while (ADCSRA & (1 << ADSC));				// Wait
  return ADC;								// Return result
}


/****************************
*  		PWM function  		*
****************************/

void analogWrite(uint8_t pin, uint16_t duty) {
	
	if (!duty) {					// If duty = 0
		digitalWrite(pin, LOW);		// Disable PWM and set pin to LOW
		return;						// Skip next code
	}

	switch (pin) {
	case 5:
		bitSet(TCCR0A, COM0B1);		// Enable hardware timer output
		OCR0B = duty;				// Load duty to compare register
		return;
	case 6:
		bitSet(TCCR0A, COM0A1);
		OCR0A = duty;
		return;
	case 10:
		bitSet(TCCR1A, COM1B1);
		OCR1B = duty;
		return;
	case 9:
		bitSet(TCCR1A, COM1A1);
		OCR1A = duty;
		return;
	case 3:
		bitSet(TCCR2A, COM2B1);
		OCR2B = duty;
		return;
	case 11:
		bitSet(TCCR2A, COM2A1);
		OCR2A = duty;
		return;
	}
}