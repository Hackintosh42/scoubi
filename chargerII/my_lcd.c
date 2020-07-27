


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

#define cbi(x,y) x=x&~(1<<y)
#define sbi(x,y) x=x|(1<<y)
 
#define outp(x,y) y = x
#define inp(x) x
#define PRG_RDB pgm_read_byte 
#ifndef outb
	#define	outb(addr, data)	outp(data, addr)
#endif
#ifndef inb
	#define	inb(addr)			inp(addr)
#endif


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

/*
void lcd_delay(void)
{

  unsigned int i;
 
  for(i=0;i<10;i++)
  {
      asm volatile(
        "NOP\n\t"
      );  
  }  
}
*/


void lcd_delay(void)
{
  LCD_DELAY;
  LCD_DELAY;
  LCD_DELAY;  
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
  lcd_delay();
  LCD_ENABLE(1);
  lcd_delay();
  LCD_ENABLE(0);
  
  LCD_DATA(0x0F &data);
  lcd_delay();
  LCD_ENABLE(1);
  lcd_delay();  
  LCD_ENABLE(0);
}

void lcdDataWrite(u08 data)
//void LCD_WrData(unsigned char data)
{
  LCD_RS(1);
  LCD_Wr(data);    
}

void lcdControlWrite(u08 data)
//void LCD_WrCmd(unsigned char data)
{  
  LCD_RS(0);
  LCD_Wr(data);
}

void lcdClear(void)
//void LCD_ClrScr(void)
{
  lcdControlWrite(0x02);
  lcdControlWrite(0x01);
}

void LCD_Addr(unsigned char data)
{
  lcdControlWrite(0x80 | (data & 0x7F));
}

void lcdGotoXY(u08 row, u08 col)
//void LCD_Goto(unsigned char y, unsigned char x)
{
  LCD_Addr(col*LCD_SCREEN_WIDTH + row);
}


void lcdInitHW(void)
{
	// initialize I/O ports
	// if I/O interface is in use
#ifdef LCD_PORT_INTERFACE
	// initialize LCD control lines
	cbi(LCD_CTRL_RS_PORT, LCD_CTRL_RS);
	cbi(LCD_CTRL_RW_PORT, LCD_CTRL_RW);
	cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);
	// initialize LCD control lines to output
	sbi(LCD_CTRL_RS_DDR, LCD_CTRL_RS);
	sbi(LCD_CTRL_RW_DDR, LCD_CTRL_RW);
	sbi(LCD_CTRL_E_DDR, LCD_CTRL_E);
	// initialize LCD data port to input
	// initialize LCD data lines to pull-up
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);						// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);						// set pull-ups to on (8bit)
	#endif
#endif
}

//void LCD_Init(void)
/*
void lcdInitHW(void)
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
    lcd_delay();
  
  // Select 4 bit mode
  LCD_RS(0);
  LCD_RW(0);
  LCD_ENABLE(1);
  LCD_DATA(0x02);
  lcd_delay();
  LCD_ENABLE(0);
  lcd_delay();

  lcdControlWrite(0x0D);  // Display on No Underline  
  lcdControlWrite(0x06);  //Character Entry Mode Increment/ Display shift off
  lcdControlWrite(0x2C);  // Function Set 4 bit/2 line mode 5x7 dot format
//  lcdControlWrite(0x20);  // Function Set 4 bit/2 line mode 5x7 dot format

//  lcdControlWrite(0x10);
  lcdClear();  
}
*/


void lcdInit(void)
{
  lcdInitHW();
	// LCD function set
	lcdControlWrite(LCD_FUNCTION_DEFAULT);
	// clear LCD
	lcdControlWrite(1<<LCD_CLR);
	delay(60000);	// wait 60ms
	// set entry mode
	lcdControlWrite(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC);
	// set display to on
	//lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 1<<LCD_ON_BLINK);
	lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY );
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
	// set data address to 0
	//lcdControlWrite(1<<LCD_DDRAM | 0x00);

	// load the first 8 custom characters
/*  
	lcdLoadCustomChar((u08*)LcdCustomChar,0,0);
	lcdLoadCustomChar((u08*)LcdCustomChar,1,1);
	lcdLoadCustomChar((u08*)LcdCustomChar,2,2);
	lcdLoadCustomChar((u08*)LcdCustomChar,3,3);
	lcdLoadCustomChar((u08*)LcdCustomChar,4,4);
	lcdLoadCustomChar((u08*)LcdCustomChar,5,5);
	lcdLoadCustomChar((u08*)LcdCustomChar,6,6);
	lcdLoadCustomChar((u08*)LcdCustomChar,7,7);
*/  
  
}


void lcdHome(void)
{
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
}



