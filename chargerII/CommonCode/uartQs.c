
/*********************************** MODULE INFO ****************************
	
   File name    : uartQs.c
   Ver nr.      : 1.0
   Description  : Module that handles the receiving of data over the uart. It is a high-level abstraction
   Compiler     : AVR GCC
   Company      : Fairbridge Technologies
   Author       : Jaco Raubenheimer
 
   Change log   : 2005-06-02  Initial 
 
****************************************************************************/
#define UARTQS_C
#include "uartQs.h"


//****************************************************************************************
// Added By Nis
// Functions to handle strings containing any characters
//****************************************************************************************

//*************************************************
// Check if the message contains all valid 
// characters
//*************************************************
u08 Check_Valid_CommandMsg(u08 *src_ptr,u08 len)
{
u08 i;

	for(i = 0;i < len;i++)
	{
		if(*src_ptr < 47 || (*src_ptr > 57 && (*src_ptr < 65)) || (*src_ptr > 90 && *src_ptr < 97) || *src_ptr > 122)
		{
			if((*src_ptr != 95) && (*src_ptr != 13)) // underscore and carraige return characters
			{
				return FALSE;
			}
		}
		src_ptr++;
	}
  // Message contained All valid characters
	return TRUE;
}


//*************************************************
// Check if there is a message in the Q waiting
// to be processed
//*************************************************
u08 Q_Check(void)
{
  return(MESSAGE_Q.New);
}

//*************************************************
// Get the Message from the Q
//*************************************************
sMESSAGE_Q *Q_GetMsg(void)
{
	// If there is no msg return Null
	if(!MESSAGE_Q.New)
		return NULL;
	else
	{			
    MESSAGE_Q.New = 0;
		return(&MESSAGE_Q);		// return the pointer to the message
	}
}

//*************************************************
// Add a new WEATHER message to the MESSAGE_Q
//*************************************************
u08 Q_AddMsg(u08 *ptr,u08 len)
{
u08 i; 
	
	if (len > MSG_BUFFER_LENGTH)
  {
		rprintf_rom(PSTR("ERROR Q_AddMsg"));
    return(1);
	}
	else
	{
		MESSAGE_Q.Len = len; // save the length
		MESSAGE_Q.New = 1;     
		for(i = 0;i< len; i++)
    {
				MESSAGE_Q.Msg[i] = *(ptr++);
        //rprintfChar(MESSAGE_Q.Msg[i]);
    }
    return(0);
	}
}







