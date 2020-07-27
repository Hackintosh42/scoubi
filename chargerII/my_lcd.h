

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
#include "lcdconf.h"


//****************************************************


// HD44780 LCD controller command set (do not modify these)
// writing:
#define LCD_CLR             0      // DB0: clear display
#define LCD_HOME            1      // DB1: return to home position
#define LCD_ENTRY_MODE      2      // DB2: set entry mode
#define LCD_ENTRY_INC       1      //   DB1: increment
#define LCD_ENTRY_SHIFT     0      //   DB2: shift
#define LCD_ON_CTRL         3      // DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY      2      //   DB2: turn display on
#define LCD_ON_CURSOR       1      //   DB1: turn cursor on
#define LCD_ON_BLINK        0      //   DB0: blinking cursor
#define LCD_MOVE            4      // DB4: move cursor/display
#define LCD_MOVE_DISP       3      //   DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT      2      //   DB2: move right (0-> left)
#define LCD_FUNCTION        5      // DB5: function set
#define LCD_FUNCTION_8BIT   4      //   DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 3      //   DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS 2      //   DB2: 5x10 font (0->5x7 font)
#define LCD_CGRAM           6      // DB6: set CG RAM address
#define LCD_DDRAM           7      // DB7: set DD RAM address
// reading:
#define LCD_BUSY            7      // DB7: LCD is busy

// Default LCD setup
// this default setup is loaded on LCD initialization
#ifdef LCD_DATA_4BIT
	#define LCD_FDEF_1			(0<<LCD_FUNCTION_8BIT)
#else
	#define LCD_FDEF_1			(1<<LCD_FUNCTION_8BIT)
#endif
#define LCD_FDEF_2				(1<<LCD_FUNCTION_2LINES)
#define LCD_FUNCTION_DEFAULT	((1<<LCD_FUNCTION) | LCD_FDEF_1 | LCD_FDEF_2)
#define LCD_MODE_DEFAULT		((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC))

// custom LCD characters
#define LCDCHAR_PROGRESS05		0	// 0/5 full progress block
#define LCDCHAR_PROGRESS15		1	// 1/5 full progress block
#define LCDCHAR_PROGRESS25		2	// 2/5 full progress block
#define LCDCHAR_PROGRESS35		3	// 3/5 full progress block
#define LCDCHAR_PROGRESS45		4	// 4/5 full progress block
#define LCDCHAR_PROGRESS55		5	// 5/5 full progress block
#define LCDCHAR_REWINDARROW		6	// rewind arrow
#define LCDCHAR_STOPBLOCK		7	// stop block
#define LCDCHAR_PAUSEBARS		8	// pause bars
#define LCDCHAR_FORWARDARROW	9	// fast-forward arrow
#define LCDCHAR_SCROLLUPARROW	10	// scroll up arrow
#define LCDCHAR_SCROLLDNARROW	11	// scroll down arrow
#define LCDCHAR_BLANK			12	// scroll down arrow
#define LCDCHAR_ANIPLAYICON0	13	// animated play icon frame 0
#define LCDCHAR_ANIPLAYICON1	14	// animated play icon frame 1
#define LCDCHAR_ANIPLAYICON2	15	// animated play icon frame 2
#define LCDCHAR_ANIPLAYICON3	16	// animated play icon frame 3

// progress bar defines
#define PROGRESSPIXELS_PER_CHAR	6

//****************************************************


#define LCD_SCREEN_WIDTH 16


void lcdDataWrite(u08 data);
void lcdControlWrite(u08 data);
void lcdClear(void);
void lcdGotoXY(u08 row, u08 col);
void lcdInitHW(void);
void lcdInit(void);
void lcdHome(void);



#endif


