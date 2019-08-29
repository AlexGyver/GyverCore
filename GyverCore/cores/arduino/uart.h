/* Облегченный Serial */

#ifndef uart_h
#define uart_h

#include "Arduino.h"
#include <avr/io.h>

#define uartBegin Serial.begin
#define uartEnd Serial.end
#define uartAvailable Serial.available
#define uartRead Serial.read
#define uartPeek Serial.peek
#define uartWrite Serial.write
#define uartClear() do{}while(0)

void uartSetTimeout(int timeout);
int32_t uartParseInt();
float uartParseFloat();
String uartReadString();
boolean uartParsePacket(int *intArray);

void uartPrintln(void);

void uartPrint(int8_t data);
void uartPrint(uint8_t data);
void uartPrint(int16_t data);
void uartPrint(uint16_t data);
void uartPrint(int32_t data);
void uartPrint(uint32_t data);
void uartPrint(double data);
void uartPrint(double data, byte decimals);
void uartPrint(String data);
void uartPrint(char data[]);

void uartPrintln(int8_t data);
void uartPrintln(uint8_t data);
void uartPrintln(int16_t data);
void uartPrintln(uint16_t data);
void uartPrintln(int32_t data);
void uartPrintln(uint32_t data);
void uartPrintln(double data);
void uartPrintln(double data, byte decimals);
void uartPrintln(String data);
void uartPrintln(char data[]);

void printHelper(int32_t data);
void printHelper(uint32_t data);
void printBytes(uint32_t data);
void printBytes(uint32_t data, byte amount);

#endif