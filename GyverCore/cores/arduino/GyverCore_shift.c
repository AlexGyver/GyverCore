/* Програмный SPI */
#include "Arduino.h"

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
	
	uint8_t data = 0;

	for (uint8_t i = 0; i < 8; ++i) {
		
		digitalWrite(clockPin, HIGH);
		
		if (bitOrder == LSBFIRST) data |= digitalRead(dataPin) << i;	
		else data |= digitalRead(dataPin) << (7 - i);
			
		digitalWrite(clockPin, LOW);
	}
	
	return data;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{

	for (uint8_t i = 0; i < 8; i++)  {
		
		if (bitOrder == LSBFIRST) digitalWrite(dataPin, !!(val & (1 << i)));		
		else digitalWrite(dataPin, !!(val & (1 << (7 - i))));	
						
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);		
	}
}
