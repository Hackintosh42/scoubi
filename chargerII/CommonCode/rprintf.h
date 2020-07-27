/*********************************** MODULE INFO ****************************
	
   File name    : RPRINTF.h
   Ver nr.      : 2.0
   Description  : Print routines
				   
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/

#ifndef RPRINTF_H
#define RPRINTF_H

#include "avrheaders.h"
#include "global.h"		
#include "uart.h"

void rprintfInit(void (*putchar_func)(unsigned char c));

void rprintf_dec_u16(u16 uart_data);
//void rprintf_dec_u08(u08 uart_data);
void rprintf_dec_u08(u08 uart_data,u08 leading_zero_suppression);


void rprintfChar(u08 data);
u08 Get_char(void);
void rprintfu08(u08 data);
void rprintfu16(u16 data);
void rprintfu32(u32 data);
void rprintfBits_u08(u08 data);
void rprintfBits_u32(u32 data);

void rprintf(u08* string);
void rprintf_rom(const u08 *string);
void rprintfBits_u16(u16 data);


#endif // RPRINTF_H






