/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : ds1923.h
   Ver nr.     : 1.0
   Description : Dallas DS1923 HydroChron
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-11    JvA     
                 
*************************************************************************************/

#ifndef DS1923_H
#define DS1923_H
#include "avrheaders.h"
#include "global.h"	
#include "ow.h"
#include "owcrc.h"
#include "ownet.h"
#include "owtran.h"



// *******************************************
// Function Prototypes
// *******************************************

void DS1923_clearMemory(u08* OW_IDENTITY);
void DS1923_startMission(u08* OW_IDENTITY);
void DS1923_stopMission(u08* OW_IDENTITY);
u08  DS1923_readByte(u08* OW_IDENTITY, u16 memAddress);

u16 DS1923_readTemperature(u08* OW_IDENTITY);
u16 DS1923_readHumidity(u08* OW_IDENTITY);




#ifdef DS1923_C
#else
#endif

#endif //DS1923

