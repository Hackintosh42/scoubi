
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : LCDRPRINTF.c
   Ver nr.     : 1.0
   Description : C file for printing to the LCD
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#define LCDRPRINTF_C
#include "LCDRPRINTF.h"

//***************************************************
// lcdprintat(x,y,string)
//***************************************************
void lcdprintat(u08 x,u08 y, u08 * str)
{
  // Set all output of the rprintf routine to the LCD screen
	rprintfInit(lcdDataWrite);		
  lcdGotoXY(x,y);  
  rprintf(str);    
  

  // Set all output of the rprintf routine to the Uart
	rprintfInit(uartSendChar);		  
}


//***************************************************
// lcdprint(string)
//***************************************************
void lcdprint(u08 * str)
{
  // Set all output of the rprintf routine to the LCD screen
	rprintfInit(lcdDataWrite);		
  rprintf(str);    
  
  // Set all output of the rprintf routine to the Uart
	rprintfInit(uartSendChar);		  
}

void lcdprintChar(u08 data)
{
  rprintfInit(lcdDataWrite);		
  rprintfChar(data);    
  // Set all output of the rprintf routine to the Uart
  rprintfInit(uartSendChar);		  
}

//***************************************************
// void lcdprint_hex_u08(u08x, u08y, u08 data)
//***************************************************
void lcdprint_hex_u08(u08 data)
{
  // Set all output of the rprintf routine to the LCD screen
	rprintfInit(lcdDataWrite);		
  rprintfu08(data);    
  
  // Set all output of the rprintf routine to the Uart
	rprintfInit(uartSendChar);		  
}

//***************************************************
// void lcdprint_hex_u16(u08x, u08y, u16 data)
//***************************************************
void lcdprint_hex_u16(u16 data)
{
  // Set all output of the rprintf routine to the LCD screen
	rprintfInit(lcdDataWrite);		
  rprintfu16(data);    
  
  // Set all output of the rprintf routine to the Uart
	rprintfInit(uartSendChar);		  
}


//***************************************************
// void lcdprint_dec_u08(u08x, u08y, u08 data)
//***************************************************
void lcdprint_dec_u08(u08 data,u08 leading_zero_suppresion)
{
  // Set all output of the rprintf routine to the LCD screen
	rprintfInit(lcdDataWrite);		
  rprintf_dec_u08(data,leading_zero_suppresion);    
  
  // Set all output of the rprintf routine to the Uart
	rprintfInit(uartSendChar);		  
}

//***************************************************
// void lcdprint_dec_u16(u08x, u08y, u16 data)
//***************************************************
void lcdprint_dec_u16(u16 data)
{
  // Set all output of the rprintf routine to the LCD screen
	rprintfInit(lcdDataWrite);		
  rprintf_dec_u16(data);    
  
  // Set all output of the rprintf routine to the Uart
	rprintfInit(uartSendChar);		  
}





