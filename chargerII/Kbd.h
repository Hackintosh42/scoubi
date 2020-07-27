
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : KBD.h
   Ver nr.     : 1.0
   Description : Header file for Keyboard Control
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#ifndef KBD_H
#define KBD_H

//#include "global.h"
#include ".\CommonCode\global.h"

#include <avr/io.h>

#define KDB_ENABLE_PORT       PORTC
#define KDB_ENABLE_PORT_PIN   PINC

#define KDB_ENABLE_PORT_DDR   DDRC
#define KDB_ENABLE_PIN        3

#define KDB_ENABLE_1 KDB_ENABLE_PORT = KDB_ENABLE_PORT | (1<<KDB_ENABLE_PIN);
#define KDB_ENABLE_0 KDB_ENABLE_PORT = KDB_ENABLE_PORT & ~(1<<KDB_ENABLE_PIN);

#define TRISTATE 3


#define KBD_PORT        PORTD
#define KBD_PORT_PIN    PIND
#define KBD_PORT_DDR    DDRD


#define KBD_LEFTKEY_PIN     3
#define KBD_RIGHTKEY_PIN    6
#define KBD_UPKEY_PIN       5
#define KBD_DOWNKEY_PIN     4
#define KBD_ENTERKEY_PIN    7

#define KBD_ALLKEY (1<<KBD_LEFTKEY_PIN | 1<<KBD_RIGHTKEY_PIN| 1<<KBD_UPKEY_PIN | 1<<KBD_DOWNKEY_PIN |1<<KBD_ENTERKEY_PIN)
  

  enum{
    NO_KEY = 0,
    LEFT_KEY,
    RIGHT_KEY,
    UP_KEY,
    DOWN_KEY,    
    ENTER_KEY
  };  
  
typedef struct{ 
  u16 RepeatRateDelay;
}sKbd;  
  
  
// Function Prototypes
u08 Kbd_Scan(void);
void Kdb_Enable(u08 state);


#ifdef KBD_C
  sKbd Kbd;
#else
   extern sKbd Kbd;
#endif

#endif
