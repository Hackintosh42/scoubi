/*****************************************************************************
*
* Atmel Corporation
*
* File              : serial.c
* Compiler          : IAR C 3.10C Kickstart
* Revision          : $Revision: 1.3 $
* Date              : $Date: Wednesday, January 26, 2005 10:20:44 UTC $
* Updated by        : $Author: jllassen $
*
* Support mail      : avr@atmel.com
*
* Target platform   : All AVRs with bootloader support
*
* AppNote           : AVR109 - Self-programming
*
* Description       : UART communication routines
****************************************************************************/
#include "defines.h"


void initbootuart()
{
  BAUD_RATE_LOW_REG = BRREG_VALUE;
  UART_CONTROL_REG = (1 << ENABLE_RECEIVER_BIT) |
                     (1 << ENABLE_TRANSMITTER_BIT); // enable receive and transmit 
}


void sendchar(char c)
{
  UART_DATA_REG = c;                                   // prepare transmission
  while (!(UART_STATUS_REG & (1 << TRANSMIT_COMPLETE_BIT)));// wait until byte sendt
  UART_STATUS_REG |= (1 << TRANSMIT_COMPLETE_BIT);          // delete TXCflag
}


char recchar(void)
{
    int i = 0;

	// This block is added for BUTTERFLY specific code, so that the recchar
	// function times out if nothing happens after ENTER is pressed. The
	// timeout is only checked if interrupts are enabled, i.e. still not
	// entered the main bootloader code.
    if(SREG & 0x80) {
		while(!(UART_STATUS_REG & (1 << RECEIVE_COMPLETE_BIT)) &&
		      (i<10000)) {
			i++;
		}
    } else {
		while(!(UART_STATUS_REG & (1 << RECEIVE_COMPLETE_BIT)));  // wait for data
    }
 

  return UART_DATA_REG;
}
