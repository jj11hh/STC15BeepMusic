#include <STC15.h>
#include "defs.h"

void tone(uint16_t freq) {
	uint16_t t;

#define P(X, Y) P##X##M0
	CBI(PIN_BEEP, 3);
	SBI(PIN_BEEP, 3);  	//P3�������ģʽ���ֲ�P401
#undef P

	t = 65535 - FOSC/12/freq;

	AUXR &= ~0x04;		//��ʱ��ʱ��12Tģʽ
	T2L = t;			//���ö�ʱ��ֵ
	T2H = t >> 8;		//���ö�ʱ��ֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
	IE2 |= 0x04;		//��ʱ��2�жϿ���

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
