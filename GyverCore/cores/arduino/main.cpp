/* Главный цикл программы */
#pragma message "Нас тут заперли, вызовите 911!"
#pragma message "GyverCore v2.0.3 inside. Enjoy"

#include <Arduino.h>

int main(void) {
#ifndef _GYVERCORE_NOINIT
	init();		// инициализация таймеров и ацп	
#endif
	setup();		// функция setup
	for(;;) {		// бесконечный цикл
		loop();		// функция loop
#ifndef _GYVERCORE_GYVERUART
		if (serialEventRun) serialEventRun(); // обслуживание serial
#endif
	}  
	return 0;
}