#include<STC15.h>
#include<intrins.h>

#include "defs.h"

void delayMs(uint16_t ms){
	unsigned char i, j;
	while(ms --){
		_nop_();
		_nop_();
		_nop_();
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}