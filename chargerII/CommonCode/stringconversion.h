
/*********************************** MODULE INFO ****************************
	
   File name    : string.h
   Ver nr.      : 1.0
   Description  : String manipulation
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/
#ifndef STRINGCONVERSION_H
#define STRINGCONVERSION_H
#include "avrheaders.h"
#include "global.h"		
#include "uart.h"		


// Function prototypes

u08 ascii_to_u08(u08 *hex_ptr);
u16 ascii_to_u16(u08 *hex_ptr);
u32 ascii_to_u32(u08 *hex_ptr);

//u08 hex_to_decimal_u08(u08 tmp1, u08 tmp2);
u08 tostr_u04(u08 data);
void tostr_u08(u08 data,u08 *str);
void tostr_u16(u16 data,u08 *str);
void tostr_u32(u32 data,u08 *str);




#ifdef STRINGCONVERSION_C
static char __attribute__ ((progmem)) HexChars[] = "0123456789ABCDEF";
#endif


#endif // STRINGCONVERSION_H

