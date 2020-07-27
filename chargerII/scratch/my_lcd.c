


/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : MyLCD.c
   Ver nr.     : 1.0
   Description : Header file for LCD Control
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#define MYLCD_C
#include "my_lcd.h"



//**************************************************
// LCD_Wr(unsigned char data)
//**************************************************

// PB4
void LCD_RS(unsigned char data)
{
  if(data)
    LCD_CTRL_RS_PORT = LCD_CTRL_RS_PORT |(1<<LCD_CTRL_RS);
  else
    LCD_CTRL_RS_PORT = LCD_CTRL_RS_PORT & ~(1<<LCD_CTRL_RS);
}

// PD3
void LCD_ENABLE(unsigned char data)
{
  if(data)
  {
    LCD_CTRL_E_PORT = LCD_CTRL_E_PORT |(1<<LCD_CTRL_E);
  }
  else
  {
    LCD_CTRL_E_PORT = LCD_CTRL_E_PORT & ~(1<<LCD_CTRL_E);
  }
}

//PB5
void LCD_RW(unsigned char data)
{
  if(data)
    LCD_CTRL_RW_PORT = LCD_CTRL_RW_PORT |(1<<LCD_CTRL_RW);    
  else
    LCD_CTRL_RW_PORT = LCD_CTRL_RW_PORT & ~(1<<LCD_CTRL_RW);    
}

void LCD_DELAY(void)
{

  unsigned int i;
 
  for(i=0;i<10;i++)
  {
      asm volatile(
        "NOP\n\t"
      );  
  }
  
}

void LCD_DATA(unsigned char data)
{
  LCD_DATA_POUT  = (LCD_DATA_POUT &0x0F) |((data & 0x0F)<<4);
}

void LCD_Wr(unsigned char data)
{
  LCD_RW(0);
  LCD_ENABLE(0);
  LCD_DATA(data>>4);
  LCD_DELAY();
  LCD_ENABLE(1);
  LCD_DELAY();
  LCD_ENABLE(0);
  
  LCD_DATA(0x0F &data);
  LCD_DELAY();
  LCD_ENABLE(1);
  LCD_DELAY();  
  LCD_ENABLE(0);
}

void LCD_WrData(unsigned char data)
{
  LCD_RS(1);
  LCD_Wr(data);    
}

void LCD_WrCmd(unsigned char data)
{  
  LCD_RS(0);
  LCD_Wr(data);
}

void LCD_ClrScr(void)
{
  LCD_WrCmd(0x02);
  LCD_WrCmd(0x01);
}

void LCD_Addr(unsigned char data)
{
  LCD_WrCmd(0x80 | (data & 0x7F));
}

void LCD_Goto(unsigned char y, unsigned char x)
{
  LCD_Addr(y*LCD_SCREEN_WIDTH + x);
}

//****************************************
// LCD_Put_char
//****************************************
void LCD_Put_char(unsigned char data)
{
  LCD_WrData(data);
}

void LCD_print(unsigned char *string)
{
  while(*(string) != 0x00)
  {
    LCD_Put_char(*string++);    
  }  
}


//****************************************
// Print decimal value of int
//****************************************
void LCD_print_int(int uart_data)
{
    int temp,middle;
    temp = uart_data;
    middle = (temp/1000);                   // first digit of int uart_data
    LCD_Put_char(middle + 0x30);
    temp = (temp-(middle*1000));
    middle = (temp/100);                    // second digit of int uart_data
    LCD_Put_char(middle + 0x30);
    temp = (temp-(middle*100));
    middle = (temp/10);                     // third digit of int uart_data
    LCD_Put_char(middle + 0x30);
    temp = (temp-(middle*10));
    middle = temp;                          
    LCD_Put_char(middle + 0x30);                    // fourth digit of int uart_data
}

//****************************************
// print_char() prints the decimal value of 
//****************************************
void LCD_print_char(u08 uart_data)
{
    u08 temp,middle;
    temp = uart_data;
    middle = (temp/10);                     // first digit of char uart_data
    LCD_Put_char(middle + 0x30);
    temp = (temp-(middle*10));
    middle = temp;                          // second digit of char uart_data
    LCD_Put_char(middle + 0x30);
}


void LCD_Init(void)
{
unsigned int i;

  // Make the port all output
		LCD_CTRL_RW_DDR	  = LCD_CTRL_RW_DDR	| (1<<LCD_CTRL_RW);
		LCD_CTRL_E_DDR	  = LCD_CTRL_E_DDR	| (1<<LCD_CTRL_E);
		LCD_CTRL_RS_DDR	  = LCD_CTRL_RS_DDR	| (1<<LCD_CTRL_RS);
    LCD_DATA_DDR      = LCD_DATA_DDR    | (0xF0);


  LCD_RS(0);
  LCD_RW(0);
  LCD_ENABLE(0);
  for (i=0;i<100;i++)
    LCD_DELAY();
  
  // Select 4 bit mode
  LCD_RS(0);
  LCD_RW(0);
  LCD_ENABLE(1);
  LCD_DATA(0x02);
  LCD_DELAY();
  LCD_ENABLE(0);
  LCD_DELAY();

  LCD_WrCmd(0x0D);  // Display on No Underline  
  LCD_WrCmd(0x06);  //Character Entry Mode Increment/ Display shift off
  LCD_WrCmd(0x2C);  // Function Set 4 bit/2 line mode 5x7 dot format
//  LCD_WrCmd(0x20);  // Function Set 4 bit/2 line mode 5x7 dot format

//  LCD_WrCmd(0x10);
  LCD_ClrScr();
}

