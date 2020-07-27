
/*********************************** MODULE INFO ****************************
	
   File name    : rprintf.c
   Ver nr.      : 2.0
   Description  : Print routines
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-12-12  Initial 
 
****************************************************************************/
#define RPRINTF_C
#include "rprintf.h"

static char __attribute__ ((progmem)) HexChars[] = "0123456789ABCDEF";
static void (*rputchar)(unsigned char c);

//**************************************************
// void rprintfInit(void (*putchar_func)(unsigned char c))
//**************************************************
void rprintfInit(void (*putchar_func)(unsigned char c))
{
	rputchar = putchar_func;
}


//**************************************************
// void rprintf(u08 *string)
//**************************************************
void rprintf(u08 *string)
{
  while(*(string) != 0x00)
  {
    rputchar(*string++);    
  }
}


//**************************************************
// void rprintf_rom(const u08 *string)
//**************************************************
void rprintf_rom(const u08 *string)
{
  while(pgm_read_byte(string) != 0x00)
  {
    rputchar(pgm_read_byte(string));    
    string++;
  }  
}

//**************************************************
// void rprintfu04(unsigned char data)
// prints an unsigned 4-bit number in hex (1 digit)
//**************************************************
void rprintfu04(unsigned char data)
{
	rputchar(pgm_read_byte( HexChars+(data&0x0f) ));
}

void rprintfChar(unsigned char data)
{
	rputchar(data);
}


//**************************************************
// void rprintfu08(u08 data)
// prints an unsigned 8-bit number in hex (2 digits)
//**************************************************
void rprintfu08(u08 data)
{
	// print 8-bit hex value
	rprintfu04(data>>4);
	rprintfu04(data);
}

void rprintfu16(u16 data)
{
	// print 8-bit hex value
	rprintfu08(data>>8);
	rprintfu08(data&0xFF);
}

#ifdef RPRINTF_U32
void rprintfu32(u32 data)
{
	// print 8-bit hex value
	rprintfu08(data>>24);
	rprintfu08(data>>16);
	rprintfu08(data>>8);
	rprintfu08(data);  
}
#endif





#ifdef PRINTFBITS
//*************************************************
// rprintfBits_u08(u08)
// Print out the bit sequence
//*************************************************
void rprintfBits_u08(u08 data)
{
u08 bitmask,n;

      bitmask = 0x80;
      for(n=0;n<8;n++)
      {
        if ((data & bitmask) == bitmask)        
          rprintf("1");
        else
          rprintf("0");
        bitmask = bitmask >>1;        
      }
}


//*************************************************
// rprintfBits_u08(u08)
// Print out the bit sequence
//*************************************************

void rprintfBits_u16(u16 data)
{
u16 bitmask,n;

      bitmask = 0x8000;
      for(n=0;n<16;n++)
      {
        if ((data & bitmask) == bitmask)        
          rprintf("1");
        else
          rprintf("0");
        bitmask = bitmask >>1;        
      }
}

//*************************************************
// Comms_ByteToBits_dbg(u08)
// Print out the bit sequence
//*************************************************

void rprintfBits_u32(u32 data)
{
u32 bitmask;
u08 n;

      bitmask = 0x80000000;
      for(n=0;n<32;n++)
      {
        if(n%8 == 0)
          rprintf(" ");      
        if ((data & bitmask) == bitmask)        
          rprintf("1");
        else
          rprintf("0");
        bitmask = bitmask >>1;        
      }
}
#endif

#define RPRINT_DECIMAL
#ifdef RPRINT_DECIMAL

  //****************************************
  // Print decimal value of int
  //****************************************
  void rprintf_dec_u16(u16 uart_data)
  {
      u16 temp,middle;
      temp = uart_data;
      middle = (temp/1000);                   // first digit of int uart_data
      if(middle == 0)
        rputchar(' ');
      else      
        rputchar(middle + 0x30);
      temp = (temp-(middle*1000));
      middle = (temp/100);                    // second digit of int uart_data
      rputchar(middle + 0x30);
      temp = (temp-(middle*100));
      middle = (temp/10);                     // third digit of int uart_data
      rputchar(middle + 0x30);
      temp = (temp-(middle*10));
      middle = temp;                          
      rputchar(middle + 0x30);                    // fourth digit of int uart_data
  }

  //****************************************
  // print_char() prints the decimal value of 
  //****************************************
  void rprintf_dec_u08(u08 uart_data,u08 leading_zero_suppression)
  {
      u08 temp,middle;
      temp = uart_data;
      middle = (temp/100);                    // second digit of int uart_data
      if(middle == 0)
      {
        if(!leading_zero_suppression)
          rputchar(' ');
      }
      else
        rputchar(middle + 0x30);    
      temp = (temp-(middle*100));
      middle = (temp/10);                     // first digit of char uart_data
      rputchar(middle + 0x30);
      temp = (temp-(middle*10));
      middle = temp;                          // second digit of char uart_data
      rputchar(middle + 0x30);
  }
#endif // PRINT_DECIMAL  

