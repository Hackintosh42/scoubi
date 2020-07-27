/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : ds1920.h
   Ver nr.     : 1.0
   Description : Dallas DS1920 Temperature device 
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-11    JvA     
                 
*************************************************************************************/

#ifndef DS1920_H
#define DS1920_H
#include "avrheaders.h"
#include "global.h"	
#include "ow.h"
#include "owcrc.h"
#include "ownet.h"
#include "owtran.h"



// *******************************************
// Function Prototypes
// *******************************************

void DS1920_readDevice(u08* OW_IDENITY);
void DS1920_owSendCmd(uint8_t *ROM, uint8_t cmd);
void DS1920_readScratch(u08* OW_IDENITY);
void DS1920_doTemperatureConvert(u08 * OW_IDENITY);
u16 DS1920_readTemperature(u08 * OW_IDENITY);





#ifdef DS1920_C
#else
#endif

#endif //DS1920

