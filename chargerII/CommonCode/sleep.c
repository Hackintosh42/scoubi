
/*********************************** MODULE INFO ****************************
	
   File name    : sleep.c
   Ver nr.      : 1.0
   Description  : 
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2005-05-03  Initial 
 
****************************************************************************/

#define SLEEP_C
#include "sleep.h"

void Sleep_start(u08 sleeptime)
{
u08 storedPORTB, storedPORTC, storedPORTD ;

	//rprintf_rom(PSTR("\r\nsleeping...\r\n"));
    //i();
    LED_OFF_RED;
    LED_OFF_GREEN;
		
    // Save the port values
    storedPORTB = PORTB;
    storedPORTC = PORTC;
    storedPORTD = PORTD;
    
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    
/*                  
    Modem_enable();  
    Modem_Switch(MODEM_STANDBY);
    while((volatile u08)Modem.State != MODEM_STANDBY);
    Modem_disable();    
*/ 
    Mel_Mode(MEL_STANDBY);
    
    // Switch off all onchip peripherals
    PRR = 0xFF;
    ACSR = (1<<ACD); // switch off analog comparitor
    u08 sleep_tmp = sleeptime;
    MODEM_PAUSE;
    while(sleep_tmp != 0)
    {
      SMCR = (1<<SM1) |(1<<SM0) | (1<<SE); // Power down mode and enable sleep interrupt	
      cli();
      DDRD = DDRD & ~(1<<3);   // Make it an input
      PORTD   = PORTD  | (1<<3);         // Turn of Leds on Port D          
      PRR = 0x00;      
      sei();
      __asm__ __volatile__ ("sleep" "\n\t" :: );\
      SMCR &= ~(1<<SE); // disable sleep  
      //wait until the interrupt is handled
      //while(PCIFR & (1<<PCIF2));
      //debug_rom(PSTR(">"),1);
      // Check if it was a UART receive interrupt that woke us up      
      sleep_tmp--;
    }
    PRR = 0x00;
    
//    PinChange_RTC_INTA_enable();
    PinChange_RTC_INTB_enable();
    
    // Enable the RX interrupt      
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);   // enable UART transmitter
    UCSR0B |= (1<<RXCIE0);      

    PORTB = storedPORTB;
    PORTC = storedPORTC;
    PORTD = storedPORTD;
  
    MODEM_RESTART;
  
    sei();      
	
    //rprintf_rom(PSTR("\r\n....slept\r\n"));
}

