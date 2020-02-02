#include <avr/interrupt.h>
#include "Arduino.h"

/**********************************************
* GyverCore tone() by Egor 'Nich1con' Zaharov *
**********************************************/


uint8_t tone_pin;       	// Global pin varible
uint32_t toggle_counter;    // Toggles counter
uint32_t toggle_top;      	// Toggles top
bool willStop;          	// Infinity generation flag


void tone(uint8_t pin , uint16_t freq, uint32_t duration = 0) {

  tone_pin = pin;               			// Global pin = local pin
  willStop = duration;            			// If the generation time is entered , it will not be infinite
  toggle_top = (freq * duration) / 500;   	// Calulation of the number of toggles

  uint8_t oldSREG = SREG;           		// Save global interupts settings
  cli();                 					// Disable global interrupts

  TCCR2A = 0x02;              				// Enable timer CTC mode
  TIMSK2 |= (1 << OCIE2A);            		// Enable COMPA interrupt

  if (freq < (F_CPU >> 18)) {         		// (< 240 Hz)
	  
    TCCR2B = 0x07;              			// Prescaler = /1024
    OCR2A = ((F_CPU >> 11) / freq) - 1;
	
  } else if (freq < (F_CPU >> 15)) {        // (240 - 480 Hz)
	  
    TCCR2B = 0x06;              			// Prescaler = /256
    OCR2A = ((F_CPU >> 9) / freq) - 1;
	
  } else if (freq <= (F_CPU >> 14)) {     	// (480 - 970 Hz)
	  
    TCCR2B = 0x05;              			// Prescaler = /128
    OCR2A = ((F_CPU >> 8) / freq) - 1;
	
  } else if (freq <= (F_CPU >> 13)) {     	// (970 - 1900 Hz)
	  
    TCCR2B = 0x04;               		    // Prescaler = /64
    OCR2A = ((F_CPU >> 7) / freq) - 1;
	
  } else if (freq <= (F_CPU >> 11)) {       // (1.9 - 7.8 kHz)
	  
    TCCR2B = 0x03;              			// Prescaler = /32
    OCR2A = ((F_CPU >> 6) / freq) - 1;
	
  } else if (freq <= (F_CPU >> 8)) {        // (7.8 - 62 kHz)
	  
    TCCR2B = 0x02;              			// Prescaler = /8
    OCR2A = ((F_CPU >> 4) / freq) - 1;
	
  } else {                  				// (> 62 kHz)
  
    TCCR2B = 0x01;                			// Prescaler = /1
    OCR2A = ((F_CPU >> 1) / freq) - 1;
	
  }

  toggle_counter = 0;             			// Reset toggle counter
  SREG = oldSREG;             				// Return interrupt settings

}

void noTone(uint8_t pin) {

  digitalWrite(pin, LOW);     // Set tone pin to LOW

  uint8_t oldSREG = SREG;     // Save global interupts settings
  cli();            		  // Disable global interrupts

  TIMSK2 &= ~ (1 << OCIE2A);  // Disable COMPA interrupt
  TCCR2A = (1 << WGM20);      // Load default timer settings
  TCCR2B = (1 << CS22);
  OCR2A = 0x00;

  SREG = oldSREG;             // Return interrupt settings

}


ISR(TIMER2_COMPA_vect) {
  if (!willStop || toggle_counter < toggle_top ) {     	// Generate until the top is reached
    digitalToggle(tone_pin);               				// Toggle pin
    toggle_counter++;                 					// Toggle counter increment
  }
}