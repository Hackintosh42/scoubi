/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : ds620.c
   Ver nr.     : 1.0
   Description : Dallas DS620 temperature sensor                
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-05-12    JvA  
               
 
*************************************************************************************/

#define DS620_C
#include "ds620.h"


void DS620_init(void)
{
  i2c_buf[0] = DS620_START_CONVERT;  // initiate one shot conversion  
  i2cMasterSendNI(DS620_I2C_ADDR,1,(u08*)&i2c_buf[0]); 
  
  // Wait a while for the converstion to complete
 

  i2c_buf[0]=0xAC;  // configuration register address
  i2c_buf[1]=0x0D;  // 1shot mode  
  i2cMasterSendNI(DS620_I2C_ADDR,2,(u08*)&i2c_buf[0]); 

  i2c_buf[0]= DS620_COPY_DATA;  // Copy data
  i2cMasterSendNI(DS620_I2C_ADDR,1,(u08*)&i2c_buf[0]); 

  Wait(10);
  i2c_buf[0] = DS620_STOP_CONVERT;  // initiate one shot conversion  
  i2cMasterSendNI(DS620_I2C_ADDR,1,(u08*)&i2c_buf[0]); 
  
}

s16 DS620_readtemperature(void)
{
  // Read the two temperature bytes 
  
  i2c_buf[0] = DS620_START_CONVERT;  // initiate one shot conversion  
  i2cMasterSendNI(DS620_I2C_ADDR,1,(u08*)&i2c_buf[0]); 
  //Wait(1000);
  
  i2c_buf[0] = 0xAA;  // Temperature MSB
  i2cMasterSendNI(DS620_I2C_ADDR,1,(u08*)&i2c_buf[0]);   
  i2cMasterReceiveNI(DS620_I2C_ADDR, 1, (u08*)&i2c_buf[0]);
  u08 MSB = i2c_buf[0];
  
  i2c_buf[0] = 0xAB;  // Temperature LSB
  i2cMasterSendNI(DS620_I2C_ADDR,1,(u08*)&i2c_buf[0]);   
  i2cMasterReceiveNI(DS620_I2C_ADDR, 1, (u08*)&i2c_buf[0]);
  u08 LSB = i2c_buf[0];

  return((s16)((MSB <<8) + LSB));
}


s16 DS620_ConvertDegrees(s16 temp)
{
    temp = temp >>7; // 1 degree accuracy
    return(temp);
}



