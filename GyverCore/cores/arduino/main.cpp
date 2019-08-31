/* Главный цикл программы */

#pragma message "GyverCore v1.7.2 inside. Enjoy"
#include <Arduino.h>
int main(void)
{   WDTCSR |= (1<<WDCE); // даем разрешение отключить ватчдог
	WDTCSR = 0;	// Первым делом отключаем ватчдог
#if defined (_GYVERCORE_DEF_INIT)
	init();		// инициализация таймеров и ацп	
#elif defined (_GYVERCORE_LIGHT_INIT)
	lightInit();// лёгкая инициализация таймеров и ацп	
#endif
	setup();		// функция setup
	for(;;) {		// бесконечный цикл
		loop();		// функция loop
		if (serialEventRun) serialEventRun(); // обслуживание serial
	}  
	return 0;
}