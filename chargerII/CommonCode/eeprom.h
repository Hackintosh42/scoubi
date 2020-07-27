

/*********************************** MODULE INFO ****************************
	
   File name    : I2C.h
   Ver nr.      : 2.0
   Description  : I2C communications
				   
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/

#ifndef EEPROM_H
#define EEPROM_H

#include "avrheaders.h"
#include "global.h"		
#include "i2c.h"
 
void EEPROM_INT_write(u16 uiAddress, u08 ucData);
u08 EEPROM_INT_read(u16 uiAddress);

void EEPROM_EXT_write(u16 memAddr, u08 data);
u08 EEPROM_EXT_read(u16 memAddr);
 

#endif

