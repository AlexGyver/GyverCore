#include <avr/interrupt.h>
#include "Arduino.h"
/* оптимизированный вариант реализации tone */
uint8_t tone_pin; // глобальная переменная для пина
uint32_t toggle_counter;  // счетчик toggl'ов
uint32_t toggle_top;  // предел счета для toggl'ов
bool willStop; // флаг позволяющий генерировать сигнал бесконечно долго

void tone(uint8_t pin , uint16_t freq, uint32_t duration = 0){	
	tone_pin = pin;  // присвоили номер пина для прерывания
	willStop = duration;// если введено время генерации - будет ненулевым 
	toggle_top = (freq * duration)/500; // расчет кол-ва toggl'ов за указанное время генерации
	uint8_t oldSREG = SREG; // запомнинаем были ли включены прерывания
	cli();//выключаем прерывания
	TCCR2A = (1<<WGM21); // вкл CTC
	TIMSK2 |= (1<<OCIE2A);	//Вкл прерывание
	if(freq < 122){ // максимальный делитель
		TCCR2B = 0x7; // присвоили делитель
		OCR2A = (15625/freq)-1; // рассчитали top для CTC
	}
	if(freq > 244){ // минимальный делитель
		TCCR2B = 0x5; // по аналогии см выше
		OCR2A = (62500/freq)-1; // по аналогии см выше
	}
	else { // средний делитель
		TCCR2B = 0x6;  // по аналогии см выше
		OCR2A = (125000/freq)-1; // по аналогии см выше
	}
	toggle_counter = 0; // обнулили счетчик toggl'ов
	SREG = oldSREG;
}

void noTone(uint8_t pin){ // если вызван noTone
	digitalWrite(pin,0);// устанавливаем 0 на выходе
	uint8_t oldSREG = SREG; // запомнинаем были ли включены прерывания
	cli();//выключаем прерывания
	TIMSK2 &=~ (1<<OCIE2A); // остановить прерывания
	TCCR2A = 0b00000011;  // установить стандартные настройки для таймера 2
	TCCR2B = 0b00000100;  
	OCR2A = 0; //обнуляем регистр сравнения
	TCCR2A &=~ ((1<<COM2A1)|(1<<COM2B1)); // откл регистры сравнения (подстраховка)
	SREG = oldSREG;
}


ISR(TIMER2_COMPA_vect){ // прерывание генерации
	if(toggle_counter < toggle_top || !willStop ){ // если количество toggl'ов не достигло предела или процесс бесконечен - генерация
		digitalToggle(tone_pin); // иневертируем состоние на ноге
		toggle_counter++; // икремент счетчика
	}
}

