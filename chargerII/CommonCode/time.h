/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : time.h
   Ver nr.     : 1.0
   Description : Header file for timekeeping                
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2004-06-06    JvA  
                 2004-11-29    Update for VtagII
 
*************************************************************************************/
#ifndef TIME_H
#define TIME_H
#include "avrheaders.h"
#include "global.h"		
#include "i2c.h"

typedef struct{ 
  u08 seconds;   //enter the current time, date, month, and year
  u08 minutes;
  u08 hours;                                     
}sTimeInfo;


/* Typedefs */
typedef struct{ 
  u16 ticks;
  u08 seconds;   //enter the current time, date, month, and year
  u08 minutes;
  u08 hours;                                     
  u08 day;
  u08 date;       
  u08 month;
  u16 year;      
}sTime;


#ifdef TIME_C
	volatile sTime Time;
#else
	extern sTime Time;
#endif

void Time_settime(u08 hours,u08 minutes, u08 seconds);
void Time_readtime(u08* hours,u08* minutes, u08* seconds);
void Time_setdate(u08 year,u08 month, u08 date, u08 day);
void Time_readdate(u08 *year,u08 *month, u08 *date, u08 *day);
void Time_setalarm1_maskbits(void);
void Time_setalarm1_seconds(u08 seconds);
void Time_InitRTC(u08 type);


#endif //TIME_H

