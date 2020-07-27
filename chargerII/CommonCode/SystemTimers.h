
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : SystemTimers.h
   Ver nr.     : 1.0
   Description : Header file for the Timers
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#ifndef SYSTEMTIMERS_H
#define SYSTEMTIMERS_H

#include <avr/wdt.h>

#include "global.h"
#include "time.h"
#include "SystemTimers.h"




// structure that stores the current time of the Charger
typedef struct
{
  u08 year;
  u08 month;
  u08 day;
  u08 hour;
  u08 min;
  u08 sec;
}sSystemTime;

// structure that stores the timer values or the system
typedef struct
{	
  volatile  u16 us_tick;
}sSystemTimers;	


void delay(unsigned short us);
void Wait(u16 waittime);


#ifdef SYSTEMTIMERS_C
  volatile	sSystemTimers SystemTimers;
  volatile sSystemTime 		SystemTime;  
#else
  extern volatile	sSystemTimers SystemTimers;
  extern volatile sSystemTime 		SystemTime;  
#endif

#endif
