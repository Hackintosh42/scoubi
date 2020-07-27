
/*********************************** MODULE INFO ****************************
	
   File name    : stringconversion.c
   Ver nr.      : 1.0
   Description  : String manipulation functions
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/
#define STRINGCONVERSION_C
#include "stringconversion.h"



//**************************************************
// Convert from two hex character numbers to u08
//**************************************************
u08 ascii_to_u08(u08 *hex_ptr)
{
u08 returnbyte,i;

returnbyte = 0;

for(i=0;i<2;i++)
{
	 // Check that it is a valid Hex number,
	 if((*hex_ptr >= '0') && (*hex_ptr <= '9'))
	 {
		returnbyte += (*hex_ptr - '0');
		if(i == 0)
			returnbyte = returnbyte<<4;
	 }	
	 else
	 {
		if((*hex_ptr >= 'A') && (*hex_ptr <= 'F'))
		{
			returnbyte += *hex_ptr - 'A';// the char b4 'A'
			returnbyte = returnbyte + 10;
			if(i == 0)
				returnbyte = returnbyte<<4;			
		}
		else
		{
			//rprintf_rom(PSTR("[Not a valid hex number]\r\n"));
			return(0);		
		}
	}
	hex_ptr++;
}
return (returnbyte);
}

//**************************************************
// Convert from 4 hex character numbers to integer
//**************************************************
u16 ascii_to_u16(u08 *hex_ptr)
{
u16 returnbyte;

	returnbyte = (ascii_to_u08(hex_ptr)) << 8;
	returnbyte += ascii_to_u08(hex_ptr+2);
return(returnbyte);
}


//**************************************************
// Convert from 8 hex characters to integer
//**************************************************
u32 ascii_to_u32(u08 *hex_ptr)
{
u32 return_u32;
	return_u32 = ascii_to_u16(hex_ptr);	
	return_u32 = return_u32 <<16;
	return_u32 += ascii_to_u16(hex_ptr+4);	
return(return_u32);
}


//**************************************************
// Convert from u04 to abyte string representation
// 
//**************************************************
u08 tostr_u04(u08 data)
{
//	return(PRG_RDB( HexChars+(data&0x0f) ));
	return(pgm_read_byte( HexChars+(data&0x0f) ));
}



//**************************************************
// Convert from u08 to a two byte string representation
// 
//**************************************************
void tostr_u08(u08 data,u08 *str)
{
	// construct the 2 byte string for the byte
  *str = tostr_u04(data>>4);
  str++;
	*str = tostr_u04(data);
}


//**************************************************
// Convert from u16 to a four byte string representation
// 
//**************************************************
void tostr_u16(u16 data,u08 *str)
{
	// construct the 4 byte string for the 16 bit word

  *str = tostr_u04(data>>12);
  str++;
	*str = tostr_u04(data>>8);
  str++;
	*str = tostr_u04(data>>4);
  str++;
	*str = tostr_u04(data);  
}


//**************************************************
// Convert from u32 to a eight byte string representation
// 
//**************************************************
void tostr_u32(u32 data,u08 *str)
{
  *str = tostr_u04(data>>28);
  str++;
	*str = tostr_u04(data>>24);
  str++;
	*str = tostr_u04(data>>20);
  str++;
	*str = tostr_u04(data>>16);  
  str++;
  *str = tostr_u04(data>>12);
  str++;
	*str = tostr_u04(data>>8);
  str++;
	*str = tostr_u04(data>>4);
  str++;
	*str = tostr_u04(data);  
}



