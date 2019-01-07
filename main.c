#include <STC15.h>

#include "defs.h"
#include "tone.h"
#include "delay.h"
			  
void main(){
	int i = 0;
	EA = 1;
	
	for (i = 100; i < 20000; i ++){
		tone(i);
		delayMs(2);
	}
	no_tone();
}