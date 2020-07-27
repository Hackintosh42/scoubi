
/*********************************** MODULE INFO ****************************
	
   File name    : Sleep.h
   Ver nr.      : 1.0
   Description  : 
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt
 
   Change log   : 2005-05-03  Initial 
 
****************************************************************************/
#ifndef SLEEP_H
#define SLEEP_H
#include "avrheaders.h"
#include "global.h"	
#include "led.h"
#include "io.h"
#include "modem.h"
#include "melexis.h"
#include "hardwareinterrupts.h"
#include "timers.h"






void Sleep_start(u08 sleeptime);

#endif // WIND_H

