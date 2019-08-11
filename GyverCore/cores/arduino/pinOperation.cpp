/*
Нормальный ввод/вывод
*/
#define ARDUINO_MAIN
#include "Arduino.h"
#include <avr/io.h>
uint8_t a_ref = DEFAULT;  // глобальная переменная для хранения опорного напряжения АЦП
// ============= DIGITAL =============

void pinMode(uint8_t pin, uint8_t mode)
{
	switch (mode) {
	case 0: // input
		if (pin < 8) bitWrite(DDRD, pin, 0);   // расставляем нули в DDRn
		else if (pin < 14) bitWrite(DDRB, (pin - 8), 0);
		else if (pin < 20) bitWrite(DDRC, (pin - 14), 0);
		break;
	case 1: // output
		if (pin < 8) bitWrite(DDRD, pin, 1);   // расставляем еденицы в DDRn
		else if (pin < 14) bitWrite(DDRB, (pin - 8), 1);
		else if (pin < 20) bitWrite(DDRC, (pin - 14), 1);
		break;
	case 2: // in_pullup
		if (pin < 8) {
			bitWrite(DDRD, pin, 0);  // настраиваем как вх
			bitWrite(PORTD, pin, 0); // вкл подтяжку
		}
		else if (pin < 14) {
			bitWrite(DDRB, (pin - 8), 0);
			bitWrite(PORTB, (pin - 8), 1);
		}
		else if (pin < 20) {
			bitWrite(DDRC, (pin - 14), 0);
			bitWrite(PORTC, (pin - 14), 1);
		}
		break;
	}
}

void digitalWrite(uint8_t pin, uint8_t x) {
	switch (pin) { // откл pwm
	case 3:  // 2B
		TCCR2A &= ~(1 << COM2B1);
		break;
	case 5: // 0B
		TCCR0A &= ~(1 << COM0B1);
		break;
	case 6: // 0A
		TCCR0A &= ~(1 << COM0A1);
		break;
	case 9: // 1A
		TCCR1A &= ~(1 << COM1A1);
		break;
	case 10: // 1B
		TCCR1A &= ~(1 << COM1B1);
		break;
	case 11: // 2A
		TCCR2A &= ~(1 << COM2A1);
		break;
	}
	if (pin < 8) bitWrite(PORTD, pin, x);
	else if (pin < 14) bitWrite(PORTB, (pin - 8), x);
	else if (pin < 20) bitWrite(PORTC, (pin - 14), x);
}

int digitalRead (uint8_t pin) {
	if (pin < 8) return bitRead(PIND, pin);
	else if (pin < 14) return bitRead(PINB, pin - 8);
	else if (pin < 20) return bitRead(PINC, pin - 14);
}


void digitalToggle(uint8_t pin){
	if (pin < 8) bitToggle(PORTD, pin);
	else if (pin < 14) bitToggle(PORTB, pin - 8);
	else if (pin < 20) bitToggle(PORTC, pin - 14);
	/*
	if (pin < 8) {
		bitWrite(PORTD, pin, !bitRead(PORTD,pin));
	}
	else if (pin < 14){
		bitWrite(PORTB, (pin - 8),!bitRead(PORTB,(pin-8)));
	}
	else if (pin < 20){ 
		bitWrite(PORTC, (pin - 14), !bitRead(PORTC,(pin-14)));
	}
	*/
}
// ============= ANALOG =============
void analogPrescaler (uint8_t prescl) { 
  cli();
  switch (prescl) {
    case 2: 
      ADCSRA &= ~((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
      break;
    case 4: // (defalut)
      ADCSRA &= ~((1 << ADPS2) | (1 << ADPS0));
      ADCSRA |= (1 << ADPS1);
      break;
    case 8:
      ADCSRA &= ~ (1 << ADPS2);
      ADCSRA |= ((1 << ADPS1) | (1 << ADPS0));
      break;
    case 16:
      ADCSRA &= ~((1 << ADPS1) | (1 << ADPS0));
      ADCSRA |= (1 << ADPS2);
      break;
    case 32:
      ADCSRA &= ~ (1 << ADPS1);
      ADCSRA |= ((1 << ADPS2) | (1 << ADPS0));
      break;
    case 64:
      ADCSRA &= ~ (1 << ADPS0);
      ADCSRA |= ((1 << ADPS2) | (1 << ADPS1));
      break;
    case 128:
      ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
      break;
  }
  sei();
}

void analogReference(uint8_t mode)
{
a_ref  = mode; // изменения будут приняты в силу при следующем analogRead() / analogStartConvert()
}

int analogRead(uint8_t pin)
{    
    if(a_ref == INTERNAL) a_ref = 3; // для удобства задвигания 
    uint8_t oldSREG = SREG; // запомнинаем были ли включены прерывания
	cli();//выключаем прерывания
	pin = (pin < 14) ? (pin) : (pin - 14);		// совместимость между A0, A1.. A7 и 0, 1.. 7
	ADMUX = 0; // обнуляем опорное и мультиплексор 
	ADMUX = (a_ref << 6) | pin; // задвигаем опорное и номер входа
	SREG = oldSREG; // если прерывания не были включены - не включаем и наоборот
	if (pin == 8 || pin == 14) delay(2); // Wait for Vref to settle для VCC и термометра
	ADCSRA |= (1 << ADSC); // начинаем преобразование
	while (ADCSRA & (1 << ADSC)); // ждем окончания
	return ADCL | (ADCH << 8); // склеить и вернуть результат
}

void analogStartConvert(byte pin) {
	 if(a_ref == INTERNAL) a_ref = 3; // для удобства задвигания 
    uint8_t oldSREG = SREG; // запомнинаем были ли включены прерывания
	cli();//выключаем прерывания
	pin = (pin < 14) ? (pin) : (pin - 14);		// совместимость между A0, A1.. A7 и 0, 1.. 7
	ADMUX = 0; // обнуляем опорное и мультиплексор 
	ADMUX = (a_ref << 6) | pin; // задвигаем опорное и номер входа
	SREG = oldSREG; // если прерывания не были включены - не включаем и наоборот
	if (pin == 8 || pin == 14) delay(2); // Wait for Vref to settle для VCC и термометра
	ADCSRA |= (1 << ADSC); // начинаем преобразование
}

int analogGet() {
	while (ADCSRA & (1 << ADSC)); // ждем окончания
	return ADCL | (ADCH << 8); // склеить и вернуть результат
}

// ============= PWM =============

boolean _TMR0_HF_PWM = false;
boolean _TMR1_HF_PWM = false;
boolean _TMR2_HF_PWM = false;
boolean _TMR1_HR_PWM = false;

void analogWrite(uint8_t pin, int val)
{
	cli();
	switch (val) {
	case 0:
		digitalWrite(pin, 0);
		break;
		/*case 255:
		digitalWrite(pin, 1);
		break;*/
	default:
		switch (pin) { // вкл pwm
		case 3:  // 2B
			if (!_TMR2_HF_PWM) {
				TCCR2A |= (1 << COM2B1);
				OCR2A = val;
			} else {
				if (val == 0) {
					bitClear(TCCR2A, COM2B1);					
					bitClear(PORTD, 3);
				} else {
					bitSet(TCCR2A, COM2B1);
					OCR2B = map(val, 0, 255, 0, 99);
				}
			}			
			break;
		case 5: // 0B
			if (!_TMR0_HF_PWM) {
				TCCR0A |= (1 << COM0B1);
				OCR0B = val;
			} else {
				if (val == 0) {
					bitClear(TCCR0A, COM0B1);
					bitClear(PORTD, 5);
				} else {
					bitSet(TCCR0A, COM0B1);
					OCR0B = map(val, 0, 255, 0, 99);
				}
			}
			
			break;
		case 6: // 0A
			TCCR0A |= (1 << COM0A1);
			OCR0A = val;
			break;
		case 9: // 1A
			if (!_TMR1_HF_PWM) {				
				TCCR1A |= (1 << COM1A1);
				OCR1A = val;
			} else {
				if (!_TMR1_HR_PWM) val *= 3.13;
				else val *= 0.78;
				OCR1AH = highByte(val);
				OCR1AL = lowByte(val);
			}			
			break;
		case 10: // 1B
			if (!_TMR1_HF_PWM) {
				TCCR1A |= (1 << COM1B1);
				OCR1B = val;
			} else {
				if (!_TMR1_HR_PWM) val *= 3.13;
				else val *= 0.78;
				OCR1BH = highByte(val);
				OCR1BL = lowByte(val);
			}			
			break;
		case 11: // 2A
			TCCR2A |= (1 << COM2A1);
			OCR2A = val;
			break;
		}
		break;
	}
	sei();
}

/*
void setPWM_20kHz(byte pin);		// установить частоту ШИМ 20 кГц (8 бит) на 3, 5, 9, 10
void setPWM_9_10_resolution(boolean resolution); // разрешение ШИМ на пинах 9 и 10 для 20 кГц: 0 - 8 бит, 1 - 10 бит
void setPwmFreqnuency(pin, freq);  	// установить частоту ШИМ (8 бит) на 3, 5, 6, 9, 10, 11: default, 8KHZ, 31KHZ
void setPWM_default(byte pin);		// настроить ШИМ по умолчанию
*/

void setPWM_20kHz(byte pin) {
	cli();
	switch (pin) { // вкл pwm
	case 3:  // 2B
		TCCR2A = 0b10100011;
		TCCR2B = 0b00001010;
		OCR2A = 99;
		_TMR2_HF_PWM = true;
		break;
	case 5: // 0B
		TCCR0A = 0b10100011;
		TCCR0B = 0b00001010;
		OCR0A = 99;
		_TMR0_HF_PWM = true;
		break;
	case 9: // 1A
		TCCR1A = 0b10100010;
		TCCR1B = 0b00011001;
		ICR1H = 3;		// highByte(799)
		ICR1L = 31;		// lowByte(799)	
		_TMR1_HF_PWM = true;
		break;
	case 10: // 1B
		TCCR1A = 0b10100010;
		TCCR1B = 0b00011001;
		ICR1H = 3;		// highByte(799)
		ICR1L = 31;		// lowByte(799)
		_TMR1_HF_PWM = true;
		break;
	}
	sei();
}

// 0 - 8 бит, 1 - 10 бит
void setPWM_9_10_resolution(boolean resolution) {
	if (resolution) _TMR1_HR_PWM = true;
	else _TMR1_HR_PWM = false;	
}

void setPwmFreqnuency(byte pin, byte freq) {	//default, 8KHZ, 31KHZ
	cli();
	switch (pin) {
	case 5:
	case 6: //Timer0
		switch (freq) {
		case 0:
			TCCR0B = 0b00000011; // x64
			TCCR0A = 0b00000011; // fast pwm
			break;
		case 1:
			TCCR0B = 0b00000010; // x8
			TCCR0A = 0b00000011; // fast pwm
			break;
		case 2:
			TCCR0B = 0b00000001; // x1
			TCCR0A = 0b00000001; // phase correct
			break;			
		}
		break;
		
	case 9:
	case 10: //Timer1
		_TMR1_HR_PWM = false;
		switch (freq) {
		case 0:
			TCCR1A = 0b00000001; // 8bit
			TCCR1B = 0b00000011; // x64 phase correct
			break;
		case 1:
			TCCR1A = 0b00000001; // 8bit
			TCCR1B = 0b00001010; // x8 fast pwm
			break;
		case 2:
			TCCR1A = 0b00000001; // 8bit
			TCCR1B = 0b00000001; // x1 phase correct
			break;			
		}
		break;
	case 3:	
	case 11: //Timer2
		switch (freq) {
		case 0:
			TCCR2B = 0b00000100; // x64
			TCCR2A = 0b00000001; // phase correct
			break;
		case 1:
			// Пины D3 и D11 - 8 кГц
			TCCR2B = 0b00000010; // x8
			TCCR2A = 0b00000011; // fast pwm
			break;
		case 2:
			TCCR2B = 0b00000001; // x1
			TCCR2A = 0b00000001; // phase correct
			break;			
		}
		break;
	}
	sei();
}

void setPWM_default(byte pin) {
	cli();
	switch (pin) {
	case 3: //Timer2_B  // не хочешь /64 можешь сделать /1
		TCCR2A = 0b10100001;  //default pwm 8 bit phaseCorrect
		TCCR2B = 0b00000100;  // prescaler /64  // крайние левые 3 бита поменять на 001 для делителя 1
		_TMR2_HF_PWM = false;
		break;
	case 5: //Timer0_B
		TCCR0A = 0b10100011; //default pwm 8 bit FastPWM
		TCCR0B = 0b00000011; // prescaler /64
		_TMR0_HF_PWM = false;
		break;
	case 6: //Timer0_A
		TCCR0A = 0b10100011; //default pwm 8 bit FastPWM
		TCCR0B = 0b00000011; // prescaler /64
		_TMR0_HF_PWM = false;
		break;
	case 9: //Timer1_A
		TCCR1A = 0b10100001; // default pwm 8 bit phaseCorrect
		TCCR1B = 0b00000011; // prescaler /64 // крайние левые 3 бита в 001 для делителя 1
		_TMR1_HF_PWM = false;
		break;
	case 10: //Timer1_B
		TCCR1A = 0b10100001; // default pwm 8 bit phaseCorrect
		TCCR1B = 0b00000011; // prescaler /64
		_TMR1_HF_PWM = false;
		break;
	case 11: //Timer2_A
		TCCR2A = 0b10100011; //default pwm 8 bit FastPWM
		TCCR2B = 0b00000011; // prescaler /64
		_TMR2_HF_PWM = false;
		break;
	}
	sei();
}

