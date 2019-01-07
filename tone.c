#include <STC15.h>
#include "defs.h"

void tone(uint16_t freq) {
	uint16_t t;

#define P(X, Y) P##X##M0
	CBI(PIN_BEEP, 3);
	SBI(PIN_BEEP, 3);  	//P3推挽输出模式，手册P401
#undef P

	t = 65535 - FOSC/12/freq;

	AUXR &= ~0x04;		//定时器时钟12T模式
	T2L = t;			//设置定时初值
	T2H = t >> 8;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04;		//定时器2中断开启

}
#define P(X, Y)	P##X##Y
void no_tone(void){
	IE2 &= ~0x04;
	PIN_BEEP = 0;
}

void tone_isr() interrupt 12 using 1{
	PIN_BEEP = ~ PIN_BEEP;
}
#undef P
