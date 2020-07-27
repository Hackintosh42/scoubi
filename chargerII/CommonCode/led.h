
#ifndef LED_H
#define LED_H
#include "avrheaders.h"
#include "global.h"		

/* VTAG_I
enum{
  GREEN_LED, BLUE_LED,RED_LED,ALL_LED};
*/
// VTAG_II
enum{
  GREEN_LED, RED_LED,ALL_LED};
  
  
#define LED_PORT PORTD
#define LED_GREEN_PIN  7
#define LED_RED_PIN    6


//#define NO_LEDS_PLEASE 

#ifdef NO_LEDS_PLEASE 
  #define LED_ON_GREEN   
  #define LED_OFF_GREEN        

  #define LED_ON_RED 
  #define LED_OFF_RED 

  #define LED_ON_ALL  
  #define LED_OFF_ALL  
#else
  #define LED_ON_GREEN LED_PORT = LED_PORT | (1<<LED_GREEN_PIN)  
  #define LED_OFF_GREEN LED_PORT = LED_PORT & ~(1<<LED_GREEN_PIN)       

  #define LED_ON_RED LED_PORT = LED_PORT | (1<<LED_RED_PIN)  
  #define LED_OFF_RED LED_PORT = LED_PORT & ~(1<<LED_RED_PIN)       

  #define LED_ON_ALL  LED_PORT = LED_PORT | ((1<<LED_GREEN_PIN)|(1<<LED_RED_PIN))
  #define LED_OFF_ALL LED_PORT = LED_PORT & ~((1<<LED_GREEN_PIN)|(1<<LED_RED_PIN)) 
#endif
                  


#endif
