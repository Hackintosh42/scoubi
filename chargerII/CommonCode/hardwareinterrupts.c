
/*********************************** MODULE INFO ****************************
	
   File name    : hardwareinterrupts.c
   Ver nr.      : 1.0
   Description  : hardware interrrupt vectors
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/

#define HARDWAREINTERRUPTS_C
#include "hardwareinterrupts.h"

u08 ticktoggle;


//***************************************************************************************
// Interrupt Vectors
//***************************************************************************************


//******************************************
// Enable external interrupt pin 
//******************************************
void Interrupt1_enable(void)
{
  // Enable the external interrupt 
  EICRA = (1<< ISC11) | (1<<ISC10); //rising edge triggered 
  //EICRA = (1<<ISC10); //CHANGE IN LEVEL triggered
  EIMSK = (1<<INT1);  
}


//******************************************
// Enable external interrupt pin from RTC
//******************************************
void PinChange_RTC_INTB_enable(void)
{
  // Enable the pinchange on PD3  
		PCICR |= (1<< PCIE2);
		PCMSK2 |= (1<<PCINT19);   
    DDRD = DDRD & ~(1<<PD3);  // Make it an input
    PORTD = PORTD | (1<<PD3);
}


INTERRUPT(SIG_PIN_CHANGE2)
{
//cli();
//sei();
//	if(PCMSK2 &(1 <<PCINT11))
	{
    if(ticktoggle)
    {
      ticktoggle = 0;
      LED_ON_GREEN;
    }
    else
    {
      ticktoggle = 1;
      LED_OFF_GREEN;

      (u16)SystemTimers.logtick++;
      (u16)SystemTimers.windtick++;
      if(SystemTimers.Wake >0)
        SystemTimers.Wake--;
    }      
	}    
//sei();	
}


//******************************************
//******************************************
/*
INTERRUPT(SIG_INTERRUPT1)
{
}
*/

//******************************************
// Timer1 Overflow interrupt
// Used for timing of the tag wakeup, etc.
//******************************************
/*
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW1)
{
}
*/

//******************************************
// Timer1 OCR1A compare interrupt
// Used for timing of the tag wakeup, etc.
//******************************************
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1A)
{
}


//******************************************
// Timer1 OCR1A compare interrupt
// Used for timing of the tag wakeup, etc.
//******************************************
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1B)
{
}









//*********************************************




