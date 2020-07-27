
/*********************************** MODULE INFO ****************************
	
   File name    : debug.h
   Ver nr.      : 1.0
   Description  : IO on the Vtag
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/
#ifndef DEBUG_H
#define DEBUG_H
#include "headers.h"

typedef struct
{
	u08 comms;
	u08 cwg_RECEIVEDMSGBUF;
	u08 cwg_CWGPROGRESS;
	u08 ris;
	u08 reader;
	u08 tag;
  u08 i2c;
}sdbg;




#ifdef DEBUG_ROUTINES
  void debug_rom(const u08* string,u08 flg);
  void debug_ram(u08* string,u08 flg);
  void debug_nn_rom(const u08* string, u08 flg);
  void debug_nn_ram(u08* string, u08 flg);
  
  void debug_u08(const u08* string,u08 var,u08 flg);
  void debug_u16(const u08* string,u16 var,u08 flg);
  void debug_u32(const u08* string,u32 var,u08 flg);
    
  void debug_buffer_bin(u08 * ptr,u08 len);
#endif
  
void debug_buffer_hex(u08 *ptr,u08 len);



#ifdef DEBUG_C
    sdbg dbg;
#else	
  extern	sdbg dbg;
#endif



#endif //DEBUG_H

