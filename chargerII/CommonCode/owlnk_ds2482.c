//*********************************** MODULE INFO ************************************
	
//   Device      : ATMega168
//   File name   : owlnk_ds2482.c
//   Ver nr.     : 1.0
//   Description : Dallas ds2482 i2C one wire driver   
//   Compiler    : GCC
//   Author      : Jason van Aardtu08
 
//   Change log  : 2005-05-12    JvA  
               
 

#define OWLNK_DS2482_C
#include "owlnk_ds2482.h"


#ifdef OW_AVR_DS2482_IMPLEMENTATION_ENABLE


//****************************************
// DS2482_DeviceReset
//****************************************
void DS2482_DeviceReset(u08 addr) 
{

  i2c_buf[0] = DS2482_DEVICE_RESET;  
  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,1,(u08*)&i2c_buf[0]);    
}

u08 ds2482_configbyte;
//****************************************
// DS2482_WriteConfiguration
//****************************************
void DS2482_WriteConfiguration(u08 addr, u08 configbyte) 
{
  i2c_buf[0] = DS2482_WRITE_CONFIGURATION;  
  i2c_buf[1] = configbyte;
  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,2,(u08*)&i2c_buf[0]);    
}

//****************************************
// DS2482_SetReadPointer
//****************************************
void DS2482_SetReadPointer(u08 addr, u08 addrreg) 
{
  i2c_buf[0] = DS2482_SET_READ_POINTER;  
  i2c_buf[1] = addrreg;
  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,2,(u08*)&i2c_buf[0]);    
}


//****************************************
// u08 owTriplet(u08 portnum)
//****************************************
u08 owTriplet_ds2482(u08 portnum)
{
  u08 addr = portnum;

  i2c_buf[0] = DS2482_1WIRE_TRIPLET;  
  
  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,1,(u08*)&i2c_buf[0]);    
  
  // Wait for the 1-wire to complete
  owBusyWait_ds2482(portnum);

    
  // Receive the status of the one wire triplet bit
  i2cMasterReceiveNI(DS2482_I2C_ADDR0 + addr + addr, 1, (u08*)&i2c_buf[0]);
  
  // Check the Single Bit Result in the Status register
  if((i2c_buf[0] & (1<<DS2482_STATUSBIT_DIR)) == (1<<DS2482_STATUSBIT_DIR))  //  ~(1 << DS2482_STATUSBIT_DIR))
    return (1);  
  else
    return (0);    
}


//****************************************
// Wait until the OW is not Busy
//****************************************
void owBusyWait_ds2482(u08 portnum)
{
u08 addr = portnum;
  do
  {
    // Receive the status of the one wire reset
    i2cMasterReceiveNI(DS2482_I2C_ADDR0 + addr, 1, (u08*)&i2c_buf[0]);
  }while((i2c_buf[0] & (1<<DS2482_STATUSBIT_1WB)) == (1<<DS2482_STATUSBIT_1WB));  
}



//****************************************
// Acquire and initialize the port
//****************************************
u08 owAcquire_ds2482(u08 portnum, char *port_zstr)
{  
  //DS2482_DeviceReset(portnum);
  ds2482_configbyte  = (1<<DS2482_CONFIGBIT_APU); // Active pull up to driver longer distance!

  ds2482_configbyte = (((~(ds2482_configbyte & 0x0F))<<4)& 0xF0) | (ds2482_configbyte & 0x0F);
  
  //DS2482_WriteConfiguration(portnum, ds2482_configbyte);
  return TRUE;
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
u08 owTouchReset_ds2482(u08 portnum)
{
u08 addr = portnum;

  i2c_buf[0] = DS2482_1WIRE_RESET;  
  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,1,(u08*)&i2c_buf[0]);    
  
  i2c_buf[0]  =0;

  // Wait for the 1-wire to complete
  owBusyWait_ds2482(portnum);        
  
//  if((i2c_buf[0] & (1<<DS2482_STATUSBIT_PPD)) == (1<<DS2482_STATUSBIT_PPD))
  if((i2c_buf[0] & (u08)(1<<DS2482_STATUSBIT_PPD)))
    return(1);
  else
    return(0);
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

u08 owTouchBit_ds2482(u08 portnum, u08 sendbit)
{
   // add platform specific code here

  u08 addr = portnum;

  i2c_buf[0] = DS2482_1WIRE_SINGLE_BIT;  
  
  if(sendbit > 0)
    i2c_buf[1] = 0x80;   // MSB sets 1 is a write one TimeSlot
  else
    i2c_buf[1] = 0x00;   // MSB sets 0 is a write zero TimeSlot
  

  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,2,(u08*)&i2c_buf[0]);    
  
  // Wait for the 1-wire to complete
  owBusyWait_ds2482(portnum);
        
  // Check the Single Bit Result in the Status register
  if(i2c_buf[0] & (u08)(1 << DS2482_STATUSBIT_SBR))
    return (1);  
  else
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

u08 owTouchByte_ds2482(u08 portnum, u08 sendbyte)
{
#ifdef OWTOUCHBYTE_BITMETHOD
uint8_t i, result = 0;    

    for (i = 0; i < 8; i++) 
    {
      result >>= 1;	
      // If sending a 1 then read a bit, otherwise write a 0 
      if (sendbyte & 0x01) 
      {
          if (owTouchBit_ds2482(portnum,1))
            result |= 0x80;
      }
      else
        owTouchBit_ds2482(portnum,0);	
        
      sendbyte >>= 1;
    }
   return(result);

#else
  u08 addr = portnum;

  i2c_buf[0] = DS2482_1WIRE_WRITE_BYTE;  
  i2c_buf[1] = sendbyte;  

  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,2,(u08*)&i2c_buf[0]);    
  
  // Wait for the 1-wire to complete
  owBusyWait_ds2482(portnum);

  DS2482_SetReadPointer(portnum, DS2482_DATAREGISTER);

  i2cMasterReceiveNI(DS2482_I2C_ADDR0 + addr, 1, (u08*)&i2c_buf[0]);
   
  return(i2c_buf[0]); 
#endif

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
u08 owWriteByte_ds2482(u08 portnum, u08 sendbyte)
{
/*
u08 addr = portnum;

  i2c_buf[0] = DS2482_1WIRE_WRITE_BYTE;  
  i2c_buf[1] = sendbyte;  

  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,2,(u08*)&i2c_buf[0]);    
  
  // Wait for the 1-wire to complete
  owBusyWait(portnum);
  
  // Need to change this later to acurately indicate that the
  // write was successful
*/
  if(owTouchByte_ds2482(portnum,sendbyte) == sendbyte)
    return(1);
  else
    return(0);
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
u08 owReadByte_ds2482(u08 portnum)
{
u08 addr = portnum;

  i2c_buf[0] = DS2482_1WIRE_READ_BYTE;  
  i2cMasterSendNI(DS2482_I2C_ADDR0 + addr,1,(u08*)&i2c_buf[0]);  
  
  // Wait for the 1-wire to complete
  owBusyWait_ds2482(portnum);
  
  // Set the ReadDataregister
  DS2482_SetReadPointer((u08)portnum, DS2482_DATAREGISTER);
    
  // Receive the byte
  i2cMasterReceiveNI(DS2482_I2C_ADDR0 + addr, 1, (u08*)&i2c_buf[0]);

   return(i2c_buf[0]);
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

u08 owLevel_ds2482(u08 portnum, u08 new_level)
{
   // add platform specific code here
   switch(new_level)
   {
    case MODE_NORMAL:
      ds2482_configbyte  = (1<<DS2482_CONFIGBIT_APU); // Active pull up to driver longer distance!
      break;
    
    case MODE_STRONG5:
      ds2482_configbyte  = (1<<DS2482_CONFIGBIT_APU) | (1<<DS2482_CONFIGBIT_SPU); // Active pull up to driver longer distance, and strong pull up
      break;

    default:
      ds2482_configbyte  = (1<<DS2482_CONFIGBIT_APU); // Active pull up to driver longer distance!
      break;    
   }
   
   ds2482_configbyte = (((~(ds2482_configbyte & 0x0F))<<4)& 0xF0) | (ds2482_configbyte & 0x0F);
   DS2482_WriteConfiguration(portnum, ds2482_configbyte);
   
   return TRUE;
}

#endif //OW_AVR_DS2482_IMPLEMENTATION_ENABLE


