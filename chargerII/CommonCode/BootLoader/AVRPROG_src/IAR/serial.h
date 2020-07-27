/*****************************************************************************
*
* Atmel Corporation
*
* File              : serial.h
* Compiler          : IAR C 3.10C Kickstart
* Revision          : $Revision: 1.6 $
* Date              : $Date: Wednesday, January 26 $
* Updated by        : $Author: jllassen  $
*
* Support mail      : avr@atmel.com
*
* Target platform   : All AVRs with bootloader support
*
* AppNote           : AVR109 - Self-programming
*
* Description       : Header file for serial.c
****************************************************************************/

void initbootuart();
void sendchar( char );
char recchar( void );
