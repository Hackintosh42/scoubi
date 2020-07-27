

/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : MyLCD.h
   Ver nr.     : 1.0
   Description : Header file for Keyboard Control
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#ifndef MYLCD_H
#define MYLCD_H

//#include "global.h"
#include "C:\work\BatteryCharger\software\CommonCode\global.h"

#include <avr/io.h>



		#define LCD_CTRL_RW_PORT	PORTB
		#define LCD_CTRL_RW_DDR	  DDRB
		#define LCD_CTRL_RW		    0 

		#define LCD_CTRL_E_PORT	  PORTB
		#define LCD_CTRL_E_DDR	  DDRB		
		#define LCD_CTRL_E		    2   

		#define LCD_CTRL_RS_PORT	PORTC
		#define LCD_CTRL_RS_DDR	  DDRC		
		#define LCD_CTRL_RS		    5   

		#define LCD_DATA_POUT	PORTD
		#define LCD_DATA_PIN	PIND
		#define LCD_DATA_DDR	DDRD


    #define LCD_SCREEN_WIDTH 16

void LCD_ClrScr(void);
void LCD_Addr(unsigned char data);
void LCD_Goto(unsigned char y, unsigned char x);
void LCD_Put_char(unsigned char data);
void LCD_print(unsigned char *string);
void LCD_print_int(int uart_data);
void LCD_print_char(unsigned char uart_data);
void LCD_Init(void);


#endif


