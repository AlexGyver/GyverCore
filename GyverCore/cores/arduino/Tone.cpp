#include <avr/interrupt.h>
#include "Arduino.h"
/* оптимизированный вариант реализации tone */
uint8_t tone_pin; // глобальная переменная для пина
uint32_t toggle_counter;  // счетчик toggl'ов
uint32_t toggle_top;  // предел счета для toggl'ов
bool willStop; // флаг позволяющий генерировать сигнал бесконечно долго

void tone(uint8_t pin , uint16_t freq, uint32_t duration = 0) {

  tone_pin = pin;  // присвоили номер пина для прерывания
  willStop = duration;// если введено время генерации - будет ненулевым
  toggle_top = (freq * duration) / 500; // расчет кол-ва toggl'ов за указанное время генерации

  uint8_t oldSREG = SREG; // запомнинаем были ли включены прерывания
  cli();//выключаем прерывания

  TCCR2A = 0x2; // вкл CTC
  TIMSK2 |= (1 << OCIE2A); //Вкл прерывание

  if (freq < (F_CPU >> 18)) {  // ( < 240 Hz)
    TCCR2B = 0x7; // prescaler = 1024
    OCR2A = ((F_CPU >> 11) / freq) - 1;
  }
  else if (freq < (F_CPU >> 15)) { // ( 240 - 480 Hz)
    TCCR2B = 0x6; // prescaler = 256
    OCR2A = ((F_CPU >> 9) / freq) - 1;
  }
  else if (freq <= (F_CPU >> 14)) { // ( 480 - 970 Hz)
    TCCR2B = 0x5; // prescaler = 128
    OCR2A = ((F_CPU >> 8) / freq) - 1;
  }
  else if (freq <= (F_CPU >> 13)) { // ( 970 - 1900 Hz)
    TCCR2B = 0x4; // prescaler = 64
    OCR2A = ((F_CPU >> 7) / freq) - 1;
  }
  else if (freq <= (F_CPU >> 11)) { // ( 1.9 - 7.8 kHz)
    TCCR2B = 0x3; // prescaler = 32
    OCR2A = ((F_CPU >> 6) / freq) - 1;
  }
  else if (freq <= (F_CPU >> 8)) { // ( 7.8 - 62 kHz)
    TCCR2B = 0x2; // prescaler = 8
    OCR2A = ((F_CPU >> 4) / freq) - 1;
  }
  else { // ( > 62 kHz)
    TCCR2B = 0x1; // prescaler = 1
    OCR2A = ((F_CPU >> 1) / freq) - 1;
  }

  toggle_counter = 0; // обнулили счетчик toggl'ов
  SREG = oldSREG;

}

void noTone(uint8_t pin) { // если вызван noTone
  digitalWrite(pin, 0); // устанавливаем 0 на выходе
  uint8_t oldSREG = SREG; // запомнинаем были ли включены прерывания
  cli();//выключаем прерывания
  TIMSK2 &= ~ (1 << OCIE2A); // остановить прерывания
  TCCR2A = 0x3;  // установить стандартные настройки для таймера 2
  TCCR2B = 0x4;
  OCR2A = 0; //обнуляем регистр сравнения
  SREG = oldSREG;
}


ISR(TIMER2_COMPA_vect) { // прерывание генерации
  if (!willStop || toggle_counter < toggle_top ) { // если количество toggl'ов не достигло предела или процесс бесконечен - генерация
    digitalToggle(tone_pin); // иневертируем состоние на ноге
    toggle_counter++; // икремент счетчика
  }
}