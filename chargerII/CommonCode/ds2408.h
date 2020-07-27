/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : ds2408.h
   Ver nr.     : 1.0
   Description : Dallas DS2408 1-wire 8 IO
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-11    JvA     
                 
*************************************************************************************/

#ifndef DS2408_H
#define DS2408_H
#include "avrheaders.h"
#include "global.h"		




//**************************************************
// Prototypes
//**************************************************
void DS2408_init(void);
void DS2408_readMemory(u08* OW_IDENTITY, u08 startAddr, u08* readBuf, u08 offset, u08 len, u08 startPhysicalAddr);
void DS2408_writeMemory(u08* OW_IDENTITY, u08 startAddr, u08* writeBuf, u08 offset, u08 len, u08 startPhysicalAddress);
void DS2408_readRegister(u08* OW_IDENTITY, u08* REGISTER);
void DS2408_setResetMode(u08* OW_IDENTITY, u08 set);
void DS2408_readDevice(u08* OW_IDENTITY, u08* STATE);
u08 DS2408_getLevel(u08* OW_IDENTITY, u08 channel);
u08 DS2408_getAllLatchLevel(u08* OW_IDENTITY);




#ifdef DS2408_C
#else
#endif

#endif //DS2408

