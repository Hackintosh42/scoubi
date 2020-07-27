/*********************************** MODULE INFO ****************************
	
   File name    : Debug.c
   Ver nr.      : 1.0
   Description  : Debug functions
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-08-14  Initial 
 
****************************************************************************/
#define DEBUG_C
#include "debug.h"

#ifdef DEBUG_ROUTINES
//*************************************************
// Print the string out depending on the debug level
// 
//*************************************************
void debug_rom(const u08* string, u08 flg)
{
	if(flg){
		rprintf_rom(PSTR("["));
		rprintf_rom(string);
		rprintf_rom(PSTR("]\r\n"));	
	}
}

void debug_ram(u08* string, u08 flg)
{
	if(flg){
		rprintf_rom(PSTR("["));
		rprintf(string);
		rprintf_rom(PSTR("]\r\n"));	
	}
}



//*************************************************
// Print the string out depending on the debug level
// No newline
//*************************************************
void debug_nn_rom(const u08* string, u08 flg)
{
	if(flg){
		rprintf_rom(PSTR("["));
		rprintf_rom(string);
		rprintf_rom(PSTR("]"));	
	}
}

//*************************************************
// Print the string out depending on the debug level
// No newline
//*************************************************
void debug_nn_ram(u08* string, u08 flg)
{
	if(flg){
		rprintf_rom(PSTR("["));
		rprintf(string);
		rprintf_rom(PSTR("]"));	
	}
}




//*************************************************
// Print the string and a variable               // 
//*************************************************
void debug_u08(const u08* string,u08 var, u08 flg)
{
	if(flg){
		rprintf_rom(PSTR("["));
		rprintf_rom(string);
		rprintfu08(var);
		rprintf_rom(PSTR("]\r\n"));	
	}
}



//*************************************************
// Print the string and a variable               // 
//*************************************************
void debug_u16(const u08* string,u16 var, u08 flg)
{
	if(flg){
		rprintf_rom(PSTR("["));
		rprintf_rom(string);
		rprintfu16(var);
		rprintf_rom(PSTR("]\r\n"));	
	}
}





//*************************************************
// Print the string and a variable               // 
//*************************************************
void debug_u32(const u08* string,u32 var, u08 flg)
{
	if(flg){
		rprintf_rom(PSTR("["));
		rprintf_rom(string);
		rprintfu32(var);
		rprintf_rom(PSTR("]\r\n"));	
	}
}





//*************************************************
// Print the buffer out as binary data
// 
//*************************************************
void debug_buffer_bin(u08 *ptr,u08 len)
{
u08 i,n,bitmask;

  for(i=0;i<len;i++)
  { 
      if((i%4) == 0)
        rprintf_rom(PSTR("\r\n"));
      rprintf_rom(PSTR("[0x"));
      rprintfu08(*ptr);
      rprintf_rom(PSTR("] "));

      bitmask = 0x80;
    for(n=0;n<8;n++)
    {
      if((*ptr & bitmask) == bitmask)
        rprintf_rom(PSTR("1"));
      else
        rprintf_rom(PSTR("0"));
      
      bitmask = bitmask>>1;
    }
    ptr++;
    rprintf_rom(PSTR(" "));
      
  }

  rprintf_rom(PSTR("\r\n")); 
}

#endif


//*************************************************
// Print the buffer out as binary data
// 
//*************************************************
void debug_buffer_hex(u08 *ptr,u08 len)
{
u08 i;

  rprintf_rom(PSTR("[ ")); 
	
  for(i=0;i<len;i++)
  { 
      //if(((i%8) == 0) && (i!= 0))
      //  rprintf_rom(PSTR("\r\n"));
      rprintf_rom(PSTR("0x"));
      rprintfu08(*ptr++);
      rprintf_rom(PSTR(" "));      
  }

  rprintf_rom(PSTR("]\r\n")); 
}
