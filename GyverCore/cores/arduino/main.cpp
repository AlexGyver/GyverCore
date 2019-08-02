/* Главный цикл программы */
#include <Arduino.h>
int main(void)
{
	WDTCSR = 0; // Первым делом отключаем ватчдог
	init();  // инициализация таймеров и ацп
	setup(); // функция setup
	while (1) {  // бесконечный цикл
		loop(); // функция loop
		if (serialEventRun) serialEventRun(); // обслуживание serial
	}  
	return 0;
}

