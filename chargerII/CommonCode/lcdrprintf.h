
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : Menu.h
   Ver nr.     : 1.0
   Description : Header file for the Menu's 
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#ifndef LCDRPRINTF_H
#define LCDRPRINTF_H

#include "global.h"
#include "lcd.h"
#include "rprintf.h"



void lcdprintChar(u08 data);
void lcdprintat(u08 x,u08 y, u08 * str);
void lcdprint(u08 * str);
void lcdprint_hex_u08(u08 data);
void lcdprint_hex_u16(u16 data);
void lcdprint_dec_u08(u08 data,u08 leading_zero_suppresion);
//void lcdprint_dec_u08(u08 data);
void lcdprint_dec_u16(u16 data);







#ifdef LCDRPRINTF_C
#else
#endif

#endif
