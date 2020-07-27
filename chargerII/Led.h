
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : KBD.h
   Ver nr.     : 1.0
   Description : Header file for Led's
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#ifndef LED_H
#define LED_H

#include ".\CommonCode\global.h"

//#include <avr/io.h>

  #define LED_PORT PORTB
  #define LED_PORT_DDR DDRB
  
  #define LED_GREEN_PIN     6
  #define LED_YELLOW_PIN    7

  #define LED_ON_GREEN LED_PORT = LED_PORT | (1<<LED_GREEN_PIN)  
  #define LED_OFF_GREEN LED_PORT = LED_PORT & ~(1<<LED_GREEN_PIN)       

  #define LED_ON_YELLOW LED_PORT = LED_PORT | (1<<LED_YELLOW_PIN)  
  #define LED_OFF_YELLOW LED_PORT = LED_PORT & ~(1<<LED_YELLOW_PIN)       

  #define LED_ON_ALL  LED_PORT = LED_PORT | ((1<<LED_GREEN_PIN)|(1<<LED_YELLOW_PIN))
  #define LED_OFF_ALL LED_PORT = LED_PORT & ~((1<<LED_GREEN_PIN)|(1<<LED_YELLOW_PIN)) 
         
#endif
