/*! \file lcd.c \brief Character LCD driver for HD44780/SED1278 displays. */
//*****************************************************************************
//
// File Name	: 'lcd.c'
// Title		: Character LCD driver for HD44780/SED1278 displays
//					(usable in mem-mapped, or I/O mode)
// Author		: Pascal Stang
// Created		: 11/22/2000
// Revised		: 4/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "global.h"
//#include "timer.h"

#include "lcd.h"

/*
// custom LCD characters
static unsigned char __attribute__ ((progmem)) LcdCustomChar[] =
{
	0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, // 0. 0/5 full progress block
	0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, // 1. 1/5 full progress block
	0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, // 2. 2/5 full progress block
	0x00, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00, // 3. 3/5 full progress block
	0x00, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00, // 4. 4/5 full progress block
	0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 5. 5/5 full progress block
	0x03, 0x07, 0x0F, 0x1F, 0x0F, 0x07, 0x03, 0x00, // 6. rewind arrow
	0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 7. stop block
	0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x00, // 8. pause bars
	0x18, 0x1C, 0x1E, 0x1F, 0x1E, 0x1C, 0x18, 0x00, // 9. fast-forward arrow
	0x00, 0x04, 0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x00, // 10. scroll up arrow
	0x00, 0x1F, 0x1F, 0x0E, 0x0E, 0x04, 0x04, 0x00, // 11. scroll down arrow
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 12. blank character
	0x00, 0x0E, 0x19, 0x15, 0x13, 0x0E, 0x00, 0x00,	// 13. animated play icon frame 0
	0x00, 0x0E, 0x15, 0x15, 0x15, 0x0E, 0x00, 0x00,	// 14. animated play icon frame 1
	0x00, 0x0E, 0x13, 0x15, 0x19, 0x0E, 0x00, 0x00,	// 15. animated play icon frame 2
	0x00, 0x0E, 0x11, 0x1F, 0x11, 0x0E, 0x00, 0x00,	// 16. animated play icon frame 3
};
*/

/*************************************************************/
/********************** LOCAL FUNCTIONS **********************/
/*************************************************************/

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


/*
void lcd_delay(void)
{
  LCD_DELAY;
  LCD_DELAY;
  LCD_DELAY;  
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


void lcdBusyWait(void)
{
	return;
	// wait until LCD busy bit goes to zero
	// do a read from control register
#ifdef LCD_PORT_INTERFACE
	cbi(LCD_CTRL_RS_PORT, LCD_CTRL_RS);				// set RS to "control"
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);	// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);					// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);					// set pull-ups to on (8bit)
	#endif
	sbi(LCD_CTRL_RW_PORT, LCD_CTRL_RW);				// set R/W to "read"
	sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);					// set "E" line
	lcd_delay();								// wait
	while(inp(LCD_DATA_PIN) & 1<<LCD_BUSY)
	{
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);		// clear "E" line
		lcd_delay();									// wait
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);		// set "E" line
		lcd_delay();									// wait
		#ifdef LCD_DATA_4BIT						// do an extra clock for 4 bit reads
			cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
			lcd_delay();								// wait
			sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
			lcd_delay();								// wait
		#endif
	}
	cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);			// clear "E" line
	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	// memory bus read
	// sbi(MCUCR, SRW);			// enable RAM waitstate
	// wait until LCD busy bit goes to zero
	while( (*((volatile unsigned char *) (LCD_CTRL_ADDR))) & (1<<LCD_BUSY) );
	// cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
}

void lcdControlWrite(u08 data) 
{
// write the control byte to the display controller
#ifdef LCD_PORT_INTERFACE
//	lcdBusyWait();							// wait until LCD not busy
	cbi(LCD_CTRL_RS_PORT, LCD_CTRL_RS);			// set RS to "control"
	cbi(LCD_CTRL_RW_PORT, LCD_CTRL_RW);			// set R/W to "write"
	#ifdef LCD_DATA_4BIT
		// 4 bit write
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)|0xF0);	// set data I/O lines to output (4bit)
		outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data&0xF0) );	// output data, high 4 bits
		lcd_delay();								// wait
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
		lcd_delay();								// wait    
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data<<4) );	// output data, low 4 bits
		lcd_delay();								// wait
    
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit write
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_DDR, 0xFF);				// set data I/O lines to output (8bit)
		outb(LCD_DATA_POUT, data);				// output data, 8bits
		lcd_delay();								// wait
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
#else
	// memory bus write
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	*((volatile unsigned char *) (LCD_CTRL_ADDR)) = data;
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
}
*/

/*
u08 lcdControlRead(void)
{
// read the control byte from the display controller
	register u08 data;
#ifdef LCD_PORT_INTERFACE
	lcdBusyWait();				// wait until LCD not busy
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
	cbi(LCD_CTRL_RS_PORT, LCD_CTRL_RS);		// set RS to "control"
	sbi(LCD_CTRL_RW_PORT, LCD_CTRL_RW);		// set R/W to "read"
	#ifdef LCD_DATA_4BIT
		// 4 bit read
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		lcd_delay();						// wait
		data = inb(LCD_DATA_PIN)&0xF0;	// input data, high 4 bits
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
		lcd_delay();						// wait
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		lcd_delay();						// wait
		data |= inb(LCD_DATA_PIN)>>4;	// input data, low 4 bits
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit read
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		lcd_delay();						// wait
		data = inb(LCD_DATA_PIN);		// input data, 8bits
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	data = *((volatile unsigned char *) (LCD_CTRL_ADDR));
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
	return data;
}

*/
/*
void lcdDataWrite(u08 data) 
{
// write a data byte to the display
#ifdef LCD_PORT_INTERFACE
	//lcdBusyWait();							// wait until LCD not busy
	sbi(LCD_CTRL_RS_PORT, LCD_CTRL_RS);		// set RS to "data"
	cbi(LCD_CTRL_RW_PORT, LCD_CTRL_RW);		// set R/W to "write"
	#ifdef LCD_DATA_4BIT
		// 4 bit write
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)|0xF0);	// set data I/O lines to output (4bit)
		outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data&0xF0) );	// output data, high 4 bits
		lcd_delay();								// wait
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
		lcd_delay();								// wait
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data<<4) );	// output data, low 4 bits
		lcd_delay();								// wait
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit write
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_DDR, 0xFF);			// set data I/O lines to output (8bit)
		outb(LCD_DATA_POUT, data);			// output data, 8bits
		lcd_delay();								// wait
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
#else
	// memory bus write
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	*((volatile unsigned char *) (LCD_DATA_ADDR)) = data;
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
}
*/
/*
u08 lcdDataRead(void)
{
// read a data byte from the display
	register u08 data;
#ifdef LCD_PORT_INTERFACE
	lcdBusyWait();				// wait until LCD not busy
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
	sbi(LCD_CTRL_RS_PORT, LCD_CTRL_RS);		// set RS to "data"
	sbi(LCD_CTRL_RW_PORT, LCD_CTRL_RW);		// set R/W to "read"
	#ifdef LCD_DATA_4BIT
		// 4 bit read
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		lcd_delay();								// wait
		data = inb(LCD_DATA_PIN)&0xF0;	// input data, high 4 bits
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
		lcd_delay();								// wait
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		lcd_delay();								// wait
		data |= inb(LCD_DATA_PIN)>>4;			// input data, low 4 bits
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit read
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
		lcd_delay();								// wait
		data = inb(LCD_DATA_PIN);			// input data, 8bits
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	// memory bus read
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	data = *((volatile unsigned char *) (LCD_DATA_ADDR));
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
	return data;
}
*/


/*************************************************************/
/********************* PUBLIC FUNCTIONS **********************/
/*************************************************************/
/*
void lcdInit(void)
{
	// initialize hardware
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
	lcdControlWrite(1<<LCD_DDRAM | 0x00);
}

void lcdHome(void)
{
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
}

void lcdClear(void)
{
	// clear LCD
	lcdControlWrite(1<<LCD_CLR);
}

void lcdGotoXY(u08 x, u08 y)
{
	register u08 DDRAMAddr;

	// remap lines into proper order
	switch(y)
	{
	case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
	case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
	case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
	case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
	default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}

	// set data address
	lcdControlWrite(1<<LCD_DDRAM | DDRAMAddr);
}
*/
/*
void lcdLoadCustomChar(u08* lcdCustomCharArray, u08 romCharNum, u08 lcdCharNum)
{
	register u08 i;
	u08 saveDDRAMAddr;

	// backup the current cursor position
	saveDDRAMAddr = lcdControlRead() & 0x7F;

	// multiply the character index by 8
	lcdCharNum = (lcdCharNum<<3);	// each character occupies 8 bytes
	romCharNum = (romCharNum<<3);	// each character occupies 8 bytes

	// copy the 8 bytes into CG (character generator) RAM
	for(i=0; i<8; i++)
	{
		// set CG RAM address
		lcdControlWrite((1<<LCD_CGRAM) | (lcdCharNum+i));
		// write character data
		lcdDataWrite( PRG_RDB(lcdCustomCharArray+romCharNum+i) );
	}

	// restore the previous cursor position
	lcdControlWrite(1<<LCD_DDRAM | saveDDRAMAddr);

}
*/

/*
void lcdPrintData(char* data, u08 nBytes)
{
	register u08 i;

	// check to make sure we have a good pointer
	if (!data) return;

	// print data
	for(i=0; i<nBytes; i++)
	{
		lcdDataWrite(data[i]);
	}
}
*/
/*
void lcdProgressBar(u16 progress, u16 maxprogress, u08 length)
{
	u08 i;
	u32 pixelprogress;
	u08 c;

	// draw a progress bar displaying (progress / maxprogress)
	// starting from the current cursor position
	// with a total length of "length" characters
	// ***note, LCD chars 0-5 must be programmed as the bar characters
	// char 0 = empty ... char 5 = full

	// total pixel length of bargraph equals length*PROGRESSPIXELS_PER_CHAR;
	// pixel length of bar itself is
	pixelprogress = ((progress*(length*PROGRESSPIXELS_PER_CHAR))/maxprogress);
	
	// print exactly "length" characters
	for(i=0; i<length; i++)
	{
		// check if this is a full block, or partial or empty
		// (u16) cast is needed to avoid sign comparison warning
		if( ((i*(u16)PROGRESSPIXELS_PER_CHAR)+5) > pixelprogress )
		{
			// this is a partial or empty block
			if( ((i*(u16)PROGRESSPIXELS_PER_CHAR)) > pixelprogress )
			{
				// this is an empty block
				// use space character?
				c = 0;
			}
			else
			{
				// this is a partial block
				c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
			}
		}
		else
		{
			// this is a full block
			c = 5;
		}
		
		// write character to display
		lcdDataWrite(c);
	}

}
*/


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

#define LCD_DELAY_LOOP 10
#define DELAY_A	asm volatile(	\
	"ldi r21, 10\n\t"	\
	"L_%=: nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

void LCD_ADELAY(void)
{
DELAY_A;
/*
  unsigned int i; 
  for(i=0;i<LCD_DELAY_LOOP;i++)
  {
      asm volatile(
        "NOP\n\t"
      );  
  } 
*/ 
}

/*
void LCD_DATA(unsigned char data)
{
  LCD_DATA_POUT  = (LCD_DATA_POUT &0x0F) |((data & 0x0F)<<4);
}
*/
#define DELAY_B asm volatile(	\
	"ldi r21, 64\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

void LCD_Wr(unsigned char data)
{
  LCD_RW(0);
  LCD_ENABLE(0);	
  //LCD_DATA(data>>4);
  LCD_ADELAY();
	outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)|0xF0);	// set data I/O lines to output (4bit)
	outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data&0xF0) );	// output data, high 4 bits
	
  LCD_ENABLE(1);
	//DELAY_B;
  LCD_ADELAY();
  LCD_ENABLE(0);
	//DELAY_B;
  //LCD_DATA(0x0F &data);	
  //LCD_ADELAY();
	outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data<<4) );	// output data, low 4 bits		
  LCD_ENABLE(1);
  LCD_ADELAY();  
  LCD_ENABLE(0);
  LCD_ADELAY();
	
/*	
  unsigned int i; 
  for(i=0;i<10;i++)
  {
      asm volatile(
        "NOP\n\t"
      );  
  } 	
*/	
}


/*
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
*/
/*
void LCD_ClrScr(void)
{
  LCD_WrCmd(0x02);
  LCD_WrCmd(0x01);
  unsigned int i; 
  for(i=0;i<255;i++)
  {
      asm volatile(
        "NOP\n\t"
      );  
  } 		
}
*/
/*
void LCD_Addr(unsigned char data)
{
  LCD_WrCmd(0x80 | (data & 0x7F));
}
    #define LCD_SCREEN_WIDTH 16
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
*/

void lcdInit(void)
{
	// initialize hardware
	lcdInitHW();
	// LCD function set
	lcdClear();
	delay(60000);	// wait 60ms	
	lcdHome();
	delay(60000);	// wait 60ms	
}


void lcdInitHW(void)
{
unsigned int i;

  // Make the port all output
/*	
		LCD_CTRL_RW_DDR	  = LCD_CTRL_RW_DDR	| (1<<LCD_CTRL_RW);
		LCD_CTRL_E_DDR	  = LCD_CTRL_E_DDR	| (1<<LCD_CTRL_E);
		LCD_CTRL_RS_DDR	  = LCD_CTRL_RS_DDR	| (1<<LCD_CTRL_RS);
    LCD_DATA_DDR      = LCD_DATA_DDR    | (0xF0);
*/		
		// initialize LCD control lines
		cbi(LCD_CTRL_RS_PORT, LCD_CTRL_RS);
		cbi(LCD_CTRL_RW_PORT, LCD_CTRL_RW);
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);
		// initialize LCD control lines to output
		sbi(LCD_CTRL_RS_DDR, LCD_CTRL_RS);
		sbi(LCD_CTRL_RW_DDR, LCD_CTRL_RW);
		sbi(LCD_CTRL_E_DDR, LCD_CTRL_E);
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
		
		

/*
  LCD_RS(0);
  LCD_RW(0);
  LCD_ENABLE(0);
  for (i=0;i<100;i++)
    LCD_ADELAY();
  
  // Select 4 bit mode
  LCD_RS(0);
  LCD_RW(0);
  LCD_ENABLE(1);
  LCD_DATA(0x02);
  LCD_ADELAY();
  LCD_ENABLE(0);
  LCD_ADELAY();
*/

	// LCD function set
	lcdControlWrite(LCD_FUNCTION_DEFAULT);
	// clear LCD
	//lcdControlWrite(1<<LCD_CLR);
	delay(60000);	// wait 60ms
	// set entry mode
	lcdControlWrite(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC);
	//lcdControlWrite(1<<LCD_ENTRY_INC);
	// set display to on
	//lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 1<<LCD_ON_BLINK);
	lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY );
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
	// set data address to 0
	lcdControlWrite(1<<LCD_DDRAM | 0x00);
}

//***************************************************************
//***************************************************************
//***************************************************************

void lcdControlWrite(u08 data) 
{
  LCD_ENABLE(0);
  LCD_RS(0);
	LCD_ADELAY();
  LCD_Wr(data);    
//	LCD_WrCmd(data);
  LCD_ADELAY();
  LCD_ADELAY();
  LCD_ADELAY();
  LCD_ENABLE(0);	

}
void lcdDataWrite(u08 data) 
{
  LCD_ENABLE(0);
  LCD_RS(1);
	LCD_ADELAY();	
  LCD_Wr(data);   
   LCD_ADELAY();
  LCD_ADELAY();
  LCD_ADELAY();
  LCD_ENABLE(0);

}
void lcdHome(void)
{
	// move cursor to home
	//lcdControlWrite(1<<LCD_HOME);
	//delay(60000);	// wait 60ms
	lcdGotoXY(0,0);
	
	//u08 i;
  //for(i=0;i<5;i++)
//		LCD_ADELAY();
}

u08 i;
void lcdClear(void)
{
	// clear LCD
	//lcdControlWrite(1<<LCD_CLR);
	//delay(60000);	// wait 60ms
	lcdprintat(0,0,"                    ");         
	lcdprintat(0,1,"                    ");         
	
	lcdGotoXY(0,0);
	//delay(60000);	// wait 60ms	
	
	//lcdControlWrite(1<<LCD_HOME);	
/*	
	u08 i;
  for(i=0;i<45;i++)
		LCD_ADELAY();
*/		
}

void lcdGotoXY(u08 x, u08 y)
{
	register u08 DDRAMAddr;

	// remap lines into proper order
	switch(y)
	{
	case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
	case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
	case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
	case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
	default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}

	// set data address
	lcdControlWrite(1<<LCD_DDRAM | DDRAMAddr);
	u08 i;
  for(i=0;i<25;i++)
		LCD_ADELAY();


}


