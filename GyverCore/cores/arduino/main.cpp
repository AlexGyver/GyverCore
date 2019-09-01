/* Главный цикл программы */

#pragma message "GyverCore v1.7.3 inside. Enjoy"

#include <Arduino.h>
int main(void) {   
#ifndef _GYVERCORE_NOINIT
	init();		// инициализация таймеров и ацп	
#endif
	setup();		// функция setup
	for(;;) {		// бесконечный цикл
		loop();		// функция loop
		if (serialEventRun) serialEventRun(); // обслуживание serial
	}  
	return 0;
}