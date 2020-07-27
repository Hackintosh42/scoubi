//
//  TODO.C - Link Layer functions required by general 1-Wire drive
//           implimentation.  Fill in the platform specific code.
//
//  Version: 3.00
//
//  History: 1.00 -> 1.01  Added function msDelay.
//           1.02 -> 1.03  Added function msGettick.
//           1.03 -> 2.00  Changed 'MLan' to 'ow'. Added support for
//                         multiple ports.
//           2.10 -> 3.00  Added owReadBitPower and owWriteBytePower
//

#define OWLNK_AVR_C
#include "owlnk_avr.h"


#ifdef OW_AVR_PORTPIN_IMPLEMENTATION_ENABLE

u08 owAcquire_avr(u08 portnum, char *port_zstr)
{
  // Initialize the  PORT pins
  OWIREDDR = OWIREDDR & ~(1<<OWIREOUTPIN);
  OWIREOUTPORT = OWIREOUTPORT & ~(1<<OWIREOUTPIN);  
  
  return TRUE;
}


void owRelease_avr(u08 portnum)
{
  // make the pin an input with no pull up
  OWIREDDR = OWIREDDR & ~(1<<OWIREOUTPIN);
  OWIREOUTPORT = OWIREOUTPORT & ~(1<<OWIREOUTPIN);  
}




/*-----------------------------------------------------------------------------
 * Send a 1-wire write bit.
 */
void
owWriteBit_avr(uint8_t bit) {

  // ***** Needs to be Moved to a higher Layer ******
                    MODEM_PAUSE;

  owDelay();
  if (bit) {
	OWIREOUTPORT &= ~(_BV(OWIREOUTPIN));
	OWIREDDR |= _BV(OWIREOUTPIN);
	DELAY_A;
	OWIREDDR &= ~(_BV(OWIREOUTPIN));
	DELAY_B;
    } else {
	OWIREOUTPORT &= ~(_BV(OWIREOUTPIN));
	OWIREDDR |= _BV(OWIREOUTPIN);
	DELAY_C;
	OWIREDDR &= ~(_BV(OWIREOUTPIN));
	DELAY_D;
    }
    
  // ***** Needs to be Moved to a higher Layer ******
                  MODEM_RESTART;
    
}

/*-----------------------------------------------------------------------------
 * Read a bit from the 1-wire bus and return it.
 */
uint8_t
owReadBit_avr(void) {
    // ***** Needs to be Moved to a higher Layer ******
                      MODEM_PAUSE;

    owDelay();

    OWIREOUTPORT &= ~(_BV(OWIREOUTPIN));
    OWIREDDR |= _BV(OWIREOUTPIN);
    DELAY_A;
    OWIREDDR &= ~(_BV(OWIREOUTPIN));
    DELAY_E;
    
    // ***** Needs to be Moved to a higher Layer ******
                  MODEM_RESTART;     
    return(OWIREINPORT & _BV(OWIREINPIN) ? 1 : 0);
}


//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
u08 owTouchReset_avr(u08 portnum)
{
    // ***** Needs to be Moved to a higher Layer ******
                      MODEM_PAUSE;   
    DELAY_G;
    OWIREOUTPORT &= ~(_BV(OWIREOUTPIN));
    OWIREDDR |= _BV(OWIREOUTPIN);
    DELAY_H;
    OWIREDDR &= ~(_BV(OWIREOUTPIN));
    DELAY_I;

    // ***** Needs to be Moved to a higher Layer ******
                  MODEM_RESTART;     

    return(OWIREINPORT & _BV(OWIREINPIN) ? 0 : 1);   
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbit'    - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//
u08 owTouchBit_avr(u08 portnum, u08 sendbit)
{
   // add platform specific code here
   
	/* If sending a 1 then read a bit, otherwise write a 0 */
	if (sendbit & 0x01) 
  {
    if (owReadBit_avr())
       return(1);
	} 
  else
    owWriteBit_avr(0);   
            
  return (0);
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  8 bytes read from sendbyte
//
u08 owTouchByte_avr(u08 portnum, u08 sendbyte)
{
uint8_t i, result = 0;    

    for (i = 0; i < 8; i++) 
    {
      result >>= 1;	
      /* If sending a 1 then read a bit, otherwise write a 0 */
      if (sendbyte & 0x01) 
      {
          if (owReadBit_avr())
            result |= 0x80;
      }
      else
        owWriteBit_avr(0);	
        
      sendbyte >>= 1;
    }
   return(result);
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
u08 owWriteByte_avr(u08 portnum, u08 sendbyte)
{
   return (owTouchByte(portnum,sendbyte) == sendbyte) ? TRUE : FALSE;
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  8 bytes read from 1-Wire Net
//
u08 owReadByte_avr(u08 portnum)
{
   return owTouchByte_avr(portnum,0xFF);
}


//--------------------------------------------------------------------------
// Set the 1-Wire Net line level.  The values for NewLevel are
// as follows:
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_level'  - new level defined as
//                MODE_NORMAL     0x00
//                MODE_STRONG5    0x02
//                MODE_PROGRAM    0x04
//                MODE_BREAK      0x08
//
// Returns:  current 1-Wire Net level
//
u08 owLevel_avr(u08 portnum, u08 new_level)
{
   // add platform specific code here
   return FALSE;
}

#endif //OW_AVR_PORTPIN_IMPLEMENTATION_ENABLE


