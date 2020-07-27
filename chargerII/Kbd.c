
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : KBD.c
   Ver nr.     : 1.0
   Description : Header file for Keyboard Control
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#define KBD_C
#include "kbd.h"
#include <avr/wdt.h>



//**************************************************
// void Kbd_Enable(u08 state)
// Set's the keyboard enable pin condition 
//**************************************************
void Kbd_Enable(u08 state)
{
  switch(state)
  {
    case 0:
      KDB_ENABLE_PORT_DDR = KDB_ENABLE_PORT_DDR | (1<<KDB_ENABLE_PIN);
      KDB_ENABLE_PORT     = KDB_ENABLE_PORT &~(1<<KDB_ENABLE_PIN);
    break;
    
    case 1:
      KDB_ENABLE_PORT_DDR = KDB_ENABLE_PORT_DDR | (1<<KDB_ENABLE_PIN);
      KDB_ENABLE_PORT     = KDB_ENABLE_PORT |(1<<KDB_ENABLE_PIN);    
    break;
    
    case TRISTATE:
      KDB_ENABLE_PORT_DDR = KDB_ENABLE_PORT_DDR & ~(1<<KDB_ENABLE_PIN);
      KDB_ENABLE_PORT     = KDB_ENABLE_PORT |(1<<KDB_ENABLE_PIN);        
    break;
  }

}

//**************************************************
// u08 Kbd_Scan(void)
// Scans the Keyboard and returns the keycode
//**************************************************

u08 Kbd_Scan(void)
{
  u08  databus, KeyCode;
  u08 stored_ddr, stored_port;
  
  wdt_reset();
  // Save the current state
  stored_ddr = KBD_PORT_DDR;
  stored_port = KBD_PORT;
  

  // Set the data bus as input with pull ups
  KBD_PORT_DDR  = KBD_PORT_DDR &~(KBD_ALLKEY);
  KBD_PORT      = KBD_PORT | (KBD_ALLKEY); 
    
  Kbd_Enable(0);        // Enable the Keyboard
  
  KeyCode = NO_KEY;  
  
  databus = KBD_PORT_PIN & (KBD_ALLKEY);
  if(databus!= KBD_ALLKEY)
  {
    // Now check which key it is
    if((databus & (1<<KBD_LEFTKEY_PIN)) == 0)
      KeyCode = LEFT_KEY;
    if((databus & (1<<KBD_RIGHTKEY_PIN)) == 0)
      KeyCode = RIGHT_KEY;
    if((databus & (1<<KBD_UPKEY_PIN)) == 0)
      KeyCode = DOWN_KEY;
    if((databus & (1<<KBD_DOWNKEY_PIN)) == 0)
      KeyCode = UP_KEY;
    if((databus & (1<<KBD_ENTERKEY_PIN)) == 0)
      KeyCode = ENTER_KEY;  

    Kbd.RepeatRateDelay = 75;
    // Pause now at the repeat Rate
    u08 i;
    for(i=0;i<Kbd.RepeatRateDelay;i++)
      delay(4000);
    
   }
   
   
  KBD_PORT_DDR  = stored_ddr;
  KBD_PORT      = stored_port;   
   
  Kbd_Enable(TRISTATE);        // Enable the Keyboard
   
   return(KeyCode);
}   
  
