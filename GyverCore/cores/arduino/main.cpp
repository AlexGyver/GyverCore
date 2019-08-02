#include <Arduino.h>
int main(void)
{
	WDTCSR = 0; // wdt disable
	init();  
	setup();
	while (1) {
		loop();
		if (serialEventRun) serialEventRun();
	}  
	return 0;
}

