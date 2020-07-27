/*****************************************************************************
*
* Atmel Corporation
*
* File              : ErrorMsg.cpp
* Compiler          : Dev-C++ 4.9.8.0 - http://bloodshed.net/dev/
* Revision          : $Revision: 1.8 $
* Date              : $Date: Friday, July 09, 2004 06:35:28 UTC $
* Updated by        : $Author: raapeland $
*
* Support mail      : avr@atmel.com
*
* Target platform   : Win32
*
* AppNote           : AVR911 - AVR Open-source Programmer
*
* Description       : A class providing a container for general error messages. This
*                     class can be thrown as an exception.
*
* 
****************************************************************************/
#include "ErrorMsg.hpp"


ErrorMsg::ErrorMsg( const string & _message ) :
	message( _message )
{
	// No code here.
}


/* Destructor */
ErrorMsg::~ErrorMsg()
{
	// No code here.
}


/* Get message */
const string & ErrorMsg::What()
{
	return message;
}

/* end of file */

