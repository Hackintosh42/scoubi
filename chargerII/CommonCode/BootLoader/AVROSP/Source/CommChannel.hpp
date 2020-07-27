/*****************************************************************************
*
* Atmel Corporation
*
* File              : CommChannel.hpp
* Compiler          : Dev-C++ 4.9.8.0 - http://bloodshed.net/dev/
* Revision          : $Revision: 1.8 $
* Date              : $Date: Friday, July 09, 2004 06:35:30 UTC $
* Updated by        : $Author: raapeland $
*
* Support mail      : avr@atmel.com
*
* Target platform   : Win32
*
* AppNote           : AVR911 - AVR Open-source Programmer
*
* Description       : An abstract class for general byte-by-byte communication.
*                     Serialport, USB, TCP/IP or similar implementations can be derived
*                     from this class to create a technology-independent
*                     communication interface.
*
*                     This abstract class does not provide any constructor as it is
*                     too specific for this generalized class. Derived classes should
*                     implement their own constructors for specific communication devices.
*
* 
****************************************************************************/
#ifndef COMMCHANNEL_HPP
#define COMMCHANNEL_HPP

using namespace std;

class CommChannel
{
public:
	// Destructor
	virtual ~CommChannel() = 0;

    // Open the communication channel.
    virtual void openChannel() = 0;
    
    // Close the communication channel.
    virtual void closeChannel() = 0;
    
    // Transmit a single byte.
    virtual void sendByte( long data ) = 0;
    
    // Receive a single byte.
    virtual long getByte() = 0;
    
    // Flush the transmit buffer.
    virtual void flushTX() = 0;
    
    // Flush the receive buffer.
    virtual void flushRX() = 0;
    
    // Transmit multiple bytes.
    virtual void sendMultiple( unsigned char * data, long bufsize ) = 0;    
};

#endif
