#ifndef __DEFS_H__
#define __DEFS_H__

typedef unsigned char 	uint8_t;
typedef signed char 	int8_t;
typedef unsigned int	uint16_t;
typedef signed int		int16_t;
typedef unsigned long	uint32_t;
typedef signed long		int32_t;

#define SBI(X, n) do{(X) |= 1<<(n);}while(0)
#define CBI(X, n) do{(X) &= ~(1<<(n));}while(0)

#define FOSC 			11059200L

/* when use it, define P as a form your needed */
#define PIN_BEEP 		P(3, 3)

#endif