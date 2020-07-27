

/*********************************** MODULE INFO ****************************
	
   File name    : I2C.h
   Ver nr.      : 2.0
   Description  : I2C communications
				   
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/

#ifndef I2C_H
#define I2C_H

#include "avrheaders.h"
#include "global.h"		
//#include "modem.h"
 
#define RTC_I2C_ADDR 0x68
 
//void i2c_init(void);
u08 i2c_start(u08 adr);
u08 i2c_write(u08 data);
void i2c_stop(void);
void i2c_test(void);


//void i2c_init ();
u08 i2c_transmit (u08 adress, u08 subadress, u08 data);
u08 i2c_transmit_addr (u08 adress, u08 subadress);
u08 i2c_receive (u08 adress, u08 subadress, u08* data);




// defines and constants
#define TWCR_CMD_MASK		0x0F
#define TWSR_STATUS_MASK	0xF8

// return values
#define I2C_OK				0x00
#define I2C_ERROR_NODEV		0x01

// functions

//! Initialize I2C (TWI) interface
void i2cInit(void);

//! Set the I2C transaction bitrate (in KHz)
void i2cSetBitrate(u16 bitrateKHz);

// I2C setup and configurations commands
//! Set the local (AVR processor's) I2C device address
void i2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn);

// Low-level I2C transaction commands 
//! Send an I2C start condition in Master mode
void i2cSendStart(void);
//! Send an I2C stop condition in Master mode
void i2cSendStop(void);
//! Wait for current I2C operation to complete
void i2cWaitForComplete(void);
//! Send an (address|R/W) combination or a data byte over I2C
void i2cSendByte(u08 data);
//! Receive a data byte over I2C  
// ackFlag = TRUE if recevied data should be ACK'ed
// ackFlag = FALSE if recevied data should be NACK'ed
void i2cReceiveByte(u08 ackFlag);
//! Pick up the data that was received with i2cReceiveByte()
u08 i2cGetReceivedByte(void);
//! Get current I2c bus status from TWSR
u08 i2cGetStatus(void);

//! send I2C data to a device on the bus (non-interrupt based)
u08 i2cMasterSendNI(u08 deviceAddr, u08 length, u08* data);
//! receive I2C data from a device on the bus (non-interrupt based)
u08 i2cMasterReceiveNI(u08 deviceAddr, u08 length, u08 *data);


#ifndef I2C_C
	volatile u08 i2c_buf[5];
#else
	extern u08 i2c_buf[];
#endif




#endif

