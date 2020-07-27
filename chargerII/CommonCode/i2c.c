/*********************************** MODULE INFO ****************************
	
   File name    : I2C.c
   Ver nr.      : 2.0
   Description  : I2C communications
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-12-12  Initial 
 
****************************************************************************/
#define  I2C_C
#include "i2c.h"


//************************************************************************************************************


void i2cInit(void)
{
	// set pull-up resistors on I2C bus pins
	// TODO: should #ifdef these
//	PORTC |= (1<<4);	// i2c SCL on ATmega163,323,16,32,etc
//	PORTC |= (1<<5);	// i2c SDA on ATmega163,323,16,32,etc

	PORTC |= (1<<4);	// i2c SCL on ATmega163,323,16,32,etc
	PORTC |= (1<<5);	// i2c SDA on ATmega163,323,16,32,etc

	// set i2c bit rate to 100KHz
	i2cSetBitrate(100);
	// enable TWI (two-wire interface)
	TWCR|= (1<<TWEN);
	// set state
	// enable TWI interrupt and slave address ACK
//	sbi(TWCR, TWIE);
	TWCR |= (1<<TWEA);
	//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	// enable interrupts
  i2cSetLocalDeviceAddr(0x01,1);  
  //TWCR = 0; 
}

void i2cSetBitrate(u16 bitrateKHz)
{
	u08 bitrate_div;
	// set i2c bitrate
	// SCL freq = F_CPU/(16+2*TWBR))
	#ifdef TWPS0
		// for processors with additional bitrate division (mega128)
		// SCL freq = F_CPU/(16+2*TWBR*4^TWPS)
		// set TWPS to zero
		TWSR &= ~(1<<TWPS0);
		TWSR &= ~(1<< TWPS1);
	#endif
	// calculate bitrate division	
	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div-16)/2;
	TWBR = bitrate_div;  
}

void i2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn)
{
	// set local device address (used in slave mode only)
	TWAR =  ((deviceAddr&0xFE) | (genCallEn?1:0));
}

inline void i2cSendStart(void)
{
	// send start condition
	TWCR =  ((TWCR)&TWCR_CMD_MASK)|(1<<TWINT)|(1<<TWSTA);
}

inline void i2cSendStop(void)
{
	// transmit stop condition
	// leave with TWEA on for slave receiving
	TWCR = ((TWCR)&TWCR_CMD_MASK)|(1<<TWINT)|(1<<TWEA)|(1<<TWSTO);
}

inline void i2cWaitForComplete(void)
{
	// wait for i2c interface to complete operation
	while( !((TWCR) & (1<<TWINT)) );
}

inline void i2cSendByte(u08 data)
{
u08 error;
u08 d;

  d = 20;
  error = 0;
	// save data to the TWDR
	TWDR = data;
	// begin send
	TWCR = ((TWCR)&TWCR_CMD_MASK)|(1<<TWINT);

  while (!(TWCR & (1<<TWINT)) && d) 
  {
    d--;
  }

/*  
  if ((TWSR & TW_STATUS_MASK) != TW_MR_DATA_ACK) 
  {
      debug_rom(PSTR("ERROR TW_MR_DATA_ACK"),1);
      error=1;
  }
*/  

/*
	error=((TWSR&0xF8)!=SLA_W_ACK);    
  if(error)
    rprintf("error");
*/    
}

inline void i2cReceiveByte(u08 ackFlag)
{
	// begin receive over i2c
	if( ackFlag )
	{
		// ackFlag = TRUE: ACK the recevied data
		TWCR = ((TWCR)&TWCR_CMD_MASK)|(1<<TWINT)|(1<<TWEA);
	}
	else
	{
		// ackFlag = FALSE: NACK the recevied data
		TWCR = ((TWCR)&TWCR_CMD_MASK)|(1<<TWINT);
	}
}

inline u08 i2cGetReceivedByte(void)
{
	// retieve received data byte from i2c TWDR
	return( TWDR );
}

inline u08 i2cGetStatus(void)
{
	// retieve current i2c status from i2c TWSR
	return(TWSR);
}

u08 i2cMasterSendNI(u08 deviceAddr, u08 length, u08* data)
{
  // *************************************
  //     MODEM FSSSSUP the I2C Bus
//         MODEM_PAUSE;
  // *************************************

	u08 retval = I2C_OK;

	// disable TWI interrupt
	TWCR &= ~(1<<TWIE);

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();

	// send device address with write
	i2cSendByte( (deviceAddr<<1) & 0xFE );
	i2cWaitForComplete();

	// check if device is present and live
	if(TWSR == TW_MT_SLA_ACK)
	{
		// send data
		while(length)
		{
			i2cSendByte( *data++ );
			i2cWaitForComplete();
			length--;
		}
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
    #ifdef DEBUG_I2C
      debug_rom(PSTR("I2C_ERROR_NODEV"),1);                
    #endif
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();
	while( !(TWCR & (1<<TWSTO)));

	// enable TWI interrupt
	TWCR |= (1<<TWIE);

  // *************************************
  //     MODEM FSSSSUP the I2C Bus
  //MODEM_RESTART;
  // *************************************


	return retval;
}

u08 i2cMasterReceiveNI(u08 deviceAddr, u08 length, u08 *data)
{

  // *************************************
  //     MODEM FSSSSUP the I2C Bus
//         MODEM_PAUSE;
  // *************************************

	u08 retval = I2C_OK;

	// disable TWI interrupt
	TWCR &= ~(1<<TWIE);

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();

	// send device address with read
	i2cSendByte( (deviceAddr<<1) | 0x01 );
	i2cWaitForComplete();

	// check if device is present and live
	if(TWSR == TW_MR_SLA_ACK)
	{
		// accept receive data and ack it
		while(length > 1)
		{
			i2cReceiveByte(TRUE);
			i2cWaitForComplete();
			*data++ = i2cGetReceivedByte();
			// decrement length
			length--;
		}

		// accept receive data and nack it (last-byte signal)
		i2cReceiveByte(FALSE);
		i2cWaitForComplete();
		*data++ = i2cGetReceivedByte();
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
    #ifdef DEBUG_I2C
      debug_rom(PSTR("I2C_ERROR_NODEV"),1);                
    #endif    
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();

	// enable TWI interrupt
	TWCR |= (1<<TWIE);

  // *************************************
  //     MODEM FSSSSUP the I2C Bus
  //       MODEM_RESTART;
  // *************************************

	return retval;
}






