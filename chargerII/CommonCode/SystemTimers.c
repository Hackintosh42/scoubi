
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : Menu.c
   Ver nr.     : 1.0
   Description : C file for the SYstem Timers
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#define SYSTEMTIMERS_C
#include "SYSTEMTIMERS.h"


void delay(unsigned short us) 
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (us+3)/6*CYCLES_PER_US; // +3 for rounding up (dirty) 

	// one loop takes 5 cpu cycles 
	for (i=0; i < delay_loops; i++){
  wdt_reset();
  };
} 

//**************************************************
// Wait routine. Wait Time in Milliseconds.
//**************************************************
void Wait(u16 waittime)
{
u16 i;

  for(i=0;i< waittime;i++)
   delay(100);
}


