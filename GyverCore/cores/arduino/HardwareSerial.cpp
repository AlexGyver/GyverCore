/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/
#include "Arduino.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/atomic.h>

#include "HardwareSerial.h"

static volatile rx_buffer_index_t buffer_head;
static volatile rx_buffer_index_t buffer_tail;
static volatile char buffer[SERIAL_RX_BUFFER_SIZE];

HardwareSerial Serial {};

void serialEvent() __attribute__((weak));
void serialEventRun(void){
    if (serialEvent && Serial.available()) serialEvent();
}

ISR(USART_RX_vect) {
    if (bit_is_set(UCSR0A, UPE0))
        UDR0; // Не сохранять новые данные если parity error
    else {
        unsigned char c = UDR0;
        rx_buffer_index_t i = (unsigned int)(buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;

        // Не сохранять новые данные если нет места
        if (i != buffer_tail) {
            buffer[buffer_head] = c;
            buffer_head = i;
        }
    }
}


void HardwareSerial::begin(unsigned long baud, byte config) {
    uint16_t speed = ((F_CPU / 8) / baud) - 1; // расчет baudrate
    buffer_head = buffer_tail = 0;

    UBRR0H = speed >> 8;
    UBRR0L = speed;
    UCSR0A = (1 << U2X0); // вкл удвоенную скорость
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); // вкл uart
    UCSR0C = config;
}

void HardwareSerial::end() {
    UCSR0B = 0;
}

int HardwareSerial::available(void) {
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + buffer_head - buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}

int HardwareSerial::peek(void) {
    return buffer_head != buffer_tail? buffer[buffer_tail]: -1;
}

int HardwareSerial::read(void) {
    if (buffer_head == buffer_tail)
        return -1;
  
    unsigned char c = buffer[buffer_tail];
    buffer_tail = (rx_buffer_index_t)(buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
    return c;
}

size_t HardwareSerial::write(uint8_t c) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 1;
}
