/*********************************** MODULE INFO ****************************
	
   File name    : Eeprom.c
   Ver nr.      : 2.0
   Description  : I2C communications
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-12-12  Initial 
 
****************************************************************************/
#define  EEPROM_C
#include "eeprom.h"


//************************************************************************************************************

  void EEPROM_INT_write(u16 uiAddress, u08 ucData)
  {
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE))
    ;
    /* Set up address and Data Registers */
    EEAR = uiAddress;
    EEDR = ucData;
    /* Write logical one to EEMPE */
    EECR |= (1<<EEMPE);
    /* Start eeprom write by setting EEPE */
    EECR |= (1<<EEPE);
  }
  
  
  u08 EEPROM_INT_read(u16 uiAddress)
  {
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE))
    ;
    /* Set up address register */
    EEAR = uiAddress;
    /* Start eeprom read by writing EERE */
    EECR |= (1<<EERE);
    /* Return data from Data Register */
  return EEDR;
  }
  
  
#ifdef EXTERNAL_EEPROM  
#define AT24C512_I2C_ADDR 0x50

  void EEPROM_EXT_write(u16 memAddr, u08 data)
  {
    u08 packet[3];
    // prepare address + data
    packet[0] = (memAddr>>8);
    packet[1] = (memAddr&0x00FF);
    packet[2] = data;
    // send memory address we wish to access to the memory chip
    // along with the data we wish to write
    i2cMasterSendNI(AT24C512_I2C_ADDR, 3, packet);    
    
    // Wait the write cycle time as specified for AT25C512A 5ms
     Wait_ms(5);
  }
  
  
  u08 EEPROM_EXT_read(u16 memAddr)
  {
    u08 packet[2];
    // prepare address
    packet[0] = (memAddr>>8);
    packet[1] = (memAddr&0x00FF);
    // send memory address we wish to access to the memory chip
    i2cMasterSendNI(AT24C512_I2C_ADDR, 2, packet);
    // retrieve the data at this memory address
    i2cMasterReceiveNI(AT24C512_I2C_ADDR, 1, packet);
    // return data
    return packet[0];  
  }

#endif









