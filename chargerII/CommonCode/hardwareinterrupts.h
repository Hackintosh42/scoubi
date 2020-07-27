
/*********************************** MODULE INFO ****************************
	
   File name    : Hardwareinterrupts.h
   Ver nr.      : 1.0
   Description  : Hardware interrupt vectors
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/


#ifndef HARDWAREINTERRUPTS_H
#define HARDWAREINTERRUPT_H

#include "avrheaders.h"
#include "global.h"		
#include "timers.h"
#include "led.h"
#include "io.h"

void Interrupt1_enable(void);
void PinChange_RTC_INTB_enable(void);


#endif // HARDWAREINTERRUPTS_H

