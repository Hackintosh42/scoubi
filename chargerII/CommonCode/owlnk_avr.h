

#ifndef OWLNK_AVR_H
#define OWLNK_AVR_H
#include "avrheaders.h"
#include "global.h"		
#include "ow.h"


#define OWIREOUTPORT		PORTC
#define OWIREOUTPIN		0

#define OWIREINPORT		PINC
#define OWIREDDR		DDRC
#define OWIREINPIN		OWIREOUTPIN



//#define OW_AVR_PORTPIN_IMPLEMENTATION_ENABLE

#ifdef OW_AVR_PORTPIN_IMPLEMENTATION_ENABLE

  // exportable link-level functions
  u08 owAcquire_avr(u08 portnum, char *port_zstr);
  void owRelease_avr(u08 portnum);
  
  u08 owTouchReset_avr(u08);
  u08 owTouchBit_avr(u08,u08);
  u08 owTouchByte_avr(u08,u08);
  u08 owWriteByte_avr(u08,u08);
  u08 owReadByte_avr(u08);
  u08 owLevel_avr(u08,u08);
#endif



//*******************************************************************
// DELAY ROUTINES
//*******************************************************************

#define XTAL_CPU 8000000

#if (XTAL_CPU == 8000000)
/* 6 usec */
#define DELAY_A	asm volatile(	\
	"ldi r21, 12\n\t"	\
	"L_%=: nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 64 usec */
#define DELAY_B asm volatile(	\
	"ldi r21, 64\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 60 usec */
#define DELAY_C asm volatile(	\
	"ldi r21, 60\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 10 usec */
#define DELAY_D asm volatile(	\
	"ldi r21, 18\n\t"	\
	"L_%=: nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 9 usec */
#define DELAY_E asm volatile(	\
	"ldi r21, 16\n\t"	\
	"L_%=: nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 55 usec */
#define DELAY_F asm volatile(	\
	"ldi r21, 54\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 0 usec */    
#define DELAY_G

/* 480 usec */
#define DELAY_H asm volatile(	\
	"ldi r21, 240\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 70 usec */    
#define DELAY_I asm volatile(	\
	"ldi r21, 70\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")
#else
#error No 1 wire delay routines for selected clock speed
#endif






#endif


