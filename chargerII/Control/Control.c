/*********************************** MODULE INFO ****************************
	
   Device       : ATMega8
   File name    : Control.c
   Ver nr.      : 1.0
   Description  : Standalone Control Module with ATMega48 (main program)                 
   Compiler     : GCC-AVR  
   Author       : Jason van Aardt iTribe Design
 
   Change log   : 2004-05-15  Initial 
 
****************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/wdt.h>
  
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

# define ENABLE_BIT_DEFINITIONS

#include "control.h"
#include <avr/io.h>
#include <avr/signal.h>	
#include <avr/interrupt.h>	

#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
//#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "vt100.h"		// include VT100 terminal support
#include "i2c.h"		// include i2c support
#include "lcd.h"




/* Typedefs */
typedef struct{ 
  unsigned int  tick;
  unsigned char second;   //enter the current time, date, month, and year
  unsigned char minute;
  unsigned char hour;                                     
  unsigned char date;       
  unsigned char month;
  unsigned int year;      
}sTime;

typedef struct 
{     
  u16 volta;
  u16 currenta;
  u16 batteryvolta;
  u08  PWM;
  u08  ChargeState;
}sBattery_info;
 
enum{
  IDLE,
  CALC_CHARGE_VOLTAGE,
  FAST,
  TRICKLE,
  ERROR
};

enum{
  VOLTA,
  CURRENTA,
  BATTERYVOLTAGEA
  };
  
enum{
  NO_KEY = 0,
  UP_KEY,
  DOWN_KEY,
  LEFT_KEY,
  RIGHT_KEY,
  ENTER_KEY
  };  

//-----------------------------------------------------------------
// Global Declarations
//-----------------------------------------------------------------

volatile sTime time;
volatile sBattery_info Battery_info[12];
volatile u08 gFlash, gMinute_flag, gSecond_Flag;

void Led_on(void)
{
    PORTD = PORTD | 0x04;
}

void Led_off(void)
{
    PORTD = PORTD & 0xFB;
}


//****************************************
// InitHW()  Initialize Hardware Registers
//****************************************
void InitHW(void)
{
  PORTC   = 0xFF;         // Prepare Port C as analog input
  DDRC    = 0x00;         // with pull-ups
  
  PORTB   = 0x00;         // Turn of Leds on Port B
  DDRB    = 0xFF;         // Set PORTB as output                    

  PORTD   = 0x00;         // Turn of Leds on Port D
  DDRD    = 0xFF;         // Set PORTD as output                    
 
  ADCSRA   = 0x96;         // Single A/D conversion, fCK/64 
}


/*************************************
 *  Initialize Timer1
 *************************************/
void InitTimers(void) 
{

  TCCR1A  = 0x0;         // Normal Port operation
//  TCCR1B  = 0x01;        // Clock x1 select           

/*
  TCCR1B  = 0x09;        // Clock x1 select           
  
  TCNT1   = 0x00;         // set counter1 to zero  
  OCR1A    = 0x7FFF;    

//  TIFR1 = (1<<TOV1);      // Counter1 overflow Flag
  TIMSK = (1<<OCIE1A);
//  TIMSK1   = (1<<TOIE1);  // enable counter1 overflow interrupt
*/

//    TCCR1B = (1<<CTC1) | (1<<CS13) | (1<<CS11) | (1<<CS10); // | _BV(CS10);
    /* set counter value to 0 */
    TCCR1B  = 0x09;        // Clock x1 select           	
    TCNT1 = 0;	
//	OCR1C = 1; // Compare Value  
	OCR1A    = 0x7FFF;
    TIMSK |= (1<<TOIE1);

}


//**************************************************
// KeyPad Routines
//**************************************************

unsigned char Scan_Keypad(void)
{
  unsigned char key,KeyCode;

  KeyCode = NO_KEY;  
  key = (PINC & 0x3E) >> 1;
  if(key != 0x1F)
  {
    // Now check which key it is
    if((key & 0x10) == 0)
      KeyCode = ENTER_KEY;
    if((key & 0x08) == 0)
      KeyCode = LEFT_KEY;
    if((key & 0x04) == 0)
      KeyCode = RIGHT_KEY;
    if((key & 0x02) == 0)
      KeyCode = UP_KEY;
    if((key & 0x01) == 0)
      KeyCode = DOWN_KEY;
   }
   return(KeyCode);
}   
      

//*************************************************
// LCD _Init
//*************************************************
void LCD_Init(void)
{
	lcdInit();
	lcdHome();
	lcdClear();
}


	  
//**************************************************
// The Main Entry Point into the program
//**************************************************
int main(void)
{
unsigned char key;
//unsigned int i;

    InitHW();     
    InitTimers();
    LCD_Init();

	uartInit();
	uartSetBaudRate(9600);		
//	timerInit();
	rprintfInit(uartSendByte); 
		
	rprintfInit(lcdDataWrite);	
	
	lcdClear();	
	rprintf("Powered by ");
	lcdGotoXY(0,1);	
	rprintf("the Sun(TM) \r\n");
	

	//lcdClear();
	//rprintf("Test %d",42); 
/*	
	lcdPrint("Welcome to BC Control");
	lcdGotoXY(0,1);
	lcdPrint("--June 2004--");	
	lcdProgressBar(67, 100, 10);
*/
	

    sei();
    
    while(Scan_Keypad()== NO_KEY);
    time.year = 2004;
    time.month = 5;
    time.date = 30;
    time.hour = 14;
    time.minute = 50;
    time.second = 0;    
	
	lcdClear();
    
    while(1)
    {
	  wdt_reset();
	  lcdGotoXY(0,0);
// 	  rprintf("%d:%d:%d",time.hour,time.minute,time.second);
	  
	  rprintf("%d",time.hour);
	  rprintf(":%d",time.minute);
	  rprintf(":%d",time.second);
	  

	while(gSecond_Flag == 0);
      gSecond_Flag = 0;
    }
      
/*             
    while(1)
    {
      key = Scan_Keypad();
      if( key != NO_KEY)
      {
		lcdClear();		
        switch(key)
        {
          case UP_KEY:            
            lcdPrint("UP_KEY");
            break;
          case DOWN_KEY:            
            lcdPrint("DOWN_KEY");
            break;
          case LEFT_KEY:            
            lcdPrint("LEFT_KEY");
            break;
          case RIGHT_KEY:            
            lcdPrint("RIGHT_KEY");
            break;
          case ENTER_KEY:            
            lcdPrint("ENTER_KEY");
            break;
        }
      }
    }
*/	
}



/***************************
 * Check for Leap Year     *
 **************************/
unsigned char not_leap(void)      //check for leap year
{
    if (!(time.year%100))
        return (unsigned char)(time.year%400);
    else
        return (unsigned char)(time.year%4);
}         
           		  
/*************************************************************************
**************************************************************************/
void TimeKeeper(void) 
{     
//	 if (++time.tick == 2)
	 {
	   if(gFlash)
	   {  
	    gFlash = 0;
  	  Led_off();
  	 }
  	 else
  	 {
  	  gFlash = 1;
  	  Led_on();
  	 }
//		 time.tick = 0;
//		time.second++;
    gSecond_Flag = 1;
    if (++time.second==60)        //keep track of time, date, month, and year
    {
		gMinute_flag = 1;
        time.second=0;		
        if (++time.minute==60) 
        {			
            time.minute=0;
            if (++time.hour==24)
            {
                time.hour=0;
                if (++time.date==32)
                {
                    time.month++;
                    time.date=1;
                }
                else if (time.date==31) 
                {                    
                    if ((time.month==4) || (time.month==6) || (time.month==9) || (time.month==11)) 
                    {
                        time.month++;
                        time.date=1;
                    }
                }
                else if (time.date==30)
                {
                    if(time.month==2)
                    {
                       time.month++;
                       time.date=1;
                    }
                }              
                else if (time.date==29) 
                {
                    if((time.month==2) && (not_leap()))
                    {
                        time.month++;
                        time.date=1;
                    }                
                }                          
                if (time.month==13)
                {
                    time.month=1;
                    time.year++;
                }
            }
        }
    }  
	}
}  



/*************************************
 *  Interrupt Timer1 Overflow
 *************************************/
 
INTERRUPT (SIG_OVERFLOW1) 
{
	cli();
	TimeKeeper();
	sei();
}





//***************************************************
// Timer1 Interrupt Overflow vector
//***************************************************
/*
#pragma vector = TIMER1_OVF_vect
__interrupt void T1Ovf()
{  
  __disable_interrupt();
  
//  TimeKeeper();  
  __enable_interrupt();
}


#pragma vector = TIMER1_COMPA_vect
__interrupt void T1CompA()
{  
  __disable_interrupt();
  
  TimeKeeper();  
  __enable_interrupt();
}
*/
 

// ************************[ End of CONTROL.C ]*******************************
