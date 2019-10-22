#include "Arduino.h"
#include "pins_arduino.h"

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse.
 *
 * This function performs better with short pulses in noInterrupt() context
 */
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout)
{
	return pulseInLong(pin, state, timeout);
	/*
	// cache the port and bit of the pin in order to speed up the
	// pulse width measuring loop and achieve finer resolution.  calling
	// digitalRead() instead yields much coarser resolution.
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	uint8_t stateMask = (state ? bit : 0);

	// convert the timeout from microseconds to a number of times through
	// the initial loop; it takes approximately 16 clock cycles per iteration
	unsigned long maxloops = microsecondsToClockCycles(timeout)/16;

	unsigned long width = countPulseASM(portInputRegister(port), bit, stateMask, maxloops);

	// prevent clockCyclesToMicroseconds to return bogus values if countPulseASM timed out
	if (width)
		return clockCyclesToMicroseconds(width * 16 + 16);
	else
		return 0;
	*/
}

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse.
 *
 * ATTENTION:
 * this function relies on micros() so cannot be used in noInterrupt() context
 */
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout)
{
	// cache the port and bit of the pin in order to speed up the
	// pulse width measuring loop and achieve finer resolution.  calling
	// digitalRead() instead yields much coarser resolution.
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	uint8_t stateMask = (state ? bit : 0);

	unsigned long startMicros = micros();

	// wait for any previous pulse to end
	while ((*portInputRegister(port) & bit) == stateMask) {
		if (micros() - startMicros > timeout)
			return 0;
	}

	// wait for the pulse to start
	while ((*portInputRegister(port) & bit) != stateMask) {
		if (micros() - startMicros > timeout)
			return 0;
	}

	unsigned long start = micros();
	// wait for the pulse to stop
	while ((*portInputRegister(port) & bit) == stateMask) {
		if (micros() - startMicros > timeout)
			return 0;
	}
	return micros() - start;
}
