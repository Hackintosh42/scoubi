/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : ds620.h
   Ver nr.     : 1.0
   Description : Dallas DS620 temperature sensor                
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-05-12    JvA     
                 
*************************************************************************************/

#ifndef DS620_H
#define DS620_H
#include "avrheaders.h"
#include "global.h"		
#include "i2c.h"		


#define DS620_I2C_ADDR 0x48

#define DS620_START_CONVERT 0x51
#define DS620_STOP_CONVERT 0x22
#define DS620_RECALL_DATA 0xB8
#define DS620_COPY_DATA 0x48



void DS620_init(void);
s16 DS620_readtemperature(void);
s16 DS620_ConvertDegrees(s16 temp);





#ifndef DS620_C
#else
#endif

#endif //DS620

