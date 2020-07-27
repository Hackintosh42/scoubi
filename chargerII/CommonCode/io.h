/*********************************** MODULE INFO ****************************
	
   File name    : IO.h
   Ver nr.      : 1.0
   Description  : IO on the Vtag
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/
#ifndef IO_H
#define IO_H
#include "avrheaders.h"
#include "global.h"		

#define IOPORT PORTC
#define IODDR DDRC
#define IOPIN_0 0
#define IOPIN_1 1
#define IOPIN_2 2

#define DEBUG0_SETOUTPUT      IODDR = IODDR | (1<< IOPIN_0);   // Set IO1 bit          
#define DEBUG1_SETOUTPUT      IODDR = IODDR | (1<< IOPIN_1);   // Set IO1 bit                  
#define DEBUG2_SETOUTPUT      IODDR = IODDR | (1<< IOPIN_2);   // Set IO1 bit                  

#define DEBUG0_SETINPUT      IODDR = IODDR & ~(1<< IOPIN_0);   // Clear IO bit                  
#define DEBUG1_SETINPUT      IODDR = IODDR & ~(1<< IOPIN_1);   // Clear IO bit                  
#define DEBUG2_SETINPUT      IODDR = IODDR & ~(1<< IOPIN_2);   // Clear IO bit                  
        

#define DEBUG0_1      IOPORT = IOPORT | (1<< IOPIN_0)                 
#define DEBUG0_0      IOPORT = IOPORT & ~(1<< IOPIN_0)

#define DEBUG1_1      IOPORT = IOPORT | (1<< IOPIN_1);   // Set IO1 bit                  
#define DEBUG1_0      IOPORT = IOPORT & ~(1<< IOPIN_1);   // Clear IO1 bit                  

#define DEBUG2_1      IOPORT = IOPORT | (1<< IOPIN_2);   
#define DEBUG2_0      IOPORT = IOPORT & ~(1<< IOPIN_2);   


void Wind_powerpin(u08 pwr);

void IO_SetRSSI_dbg(u08 val);
void IO_SetIO0_dbg(u08 val);
void IO_SetIO1_dbg(u08 val);
void IO_SetIO2_dbg(u08 val);

void Init_Switch(void);
u08 Read_MISO(void);
u08 Read_Switch(void);
u08 IO_Read_SleepJumper(void);


#endif // IO_H


