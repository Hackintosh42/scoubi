//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------
//
//  ownet.C - Network functions for 1-Wire net devices.
//
//  Version: 3.00
//
//  History: 1.00 -> 1.01  Change to owFamilySearchSetup, LastDiscrepancy[portnum]
//                         was set to 64 instead of 8 to enable devices with
//                         early contention to go in the '0' direction first.
//           1.02 -> 1.03  Initialized goodbits in  owVerify
//           1.03 -> 2.00  Changed 'MLan' to 'ow'. Added support for
//                         multiple ports.
//           2.00 -> 2.01  Added support for owError library
//           2.01 -> 3.00  Make search functions consistent with AN187
//

//#include <stdio.h>
//#include "ownet.h"

#define OWNET_C
#include "ownet.h"


//--------------------------------------------------------------------------
// The 'owFirst' finds the first device on the 1-Wire Net  This function
// contains one parameter 'alarm_only'.  When
// 'alarm_only' is TRUE (1) the find alarm command 0xEC is
// sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                        Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): There are no devices on the 1-Wire Net.
//
u08 owFirst(u08 portnum, u08 do_reset, u08 alarm_only)
{
   // reset the search state
   LastDiscrepancy[portnum] = 0;
   LastDevice[portnum] = FALSE;
   LastFamilyDiscrepancy[portnum] = 0;

   return owNext(portnum,do_reset,alarm_only);
}


//--------------------------------------------------------------------------
// The 'owNext' function does a general search.  This function
// continues from the previos search state. The search state
// can be reset by using the 'owFirst' function.
// This function contains one parameter 'alarm_only'.
// When 'alarm_only' is TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
//
u08 owNext(u08 portnum, u08 do_reset, u08 alarm_only)
{
   u08 bit_test, search_direction, bit_number;
   u08 last_zero, serial_byte_number, next_result;
   u08 serial_byte_mask;
   u08 lastcrc8 =0;

   // initialize for search
   bit_number = 1;
   last_zero = 0;
   serial_byte_number = 0;
   serial_byte_mask = 1;
   next_result = 0;
   setcrc8(portnum,0);

   // if the last call was not the last one
   if (!LastDevice[portnum])
   {
      // check if reset first is requested
      if (do_reset)
      {
         // reset the 1-wire
         // if there are no parts on 1-wire, return FALSE
         if (!owTouchReset(portnum))
         {
            // printf("owTouchReset failed\r\n");
            // reset the search
            LastDiscrepancy[portnum] = 0;
            LastFamilyDiscrepancy[portnum] = 0;
            //OWERROR(OWERROR_NO_DEVICES_ON_NET);
            return FALSE;
         }
      }

      // If finding alarming devices issue a different command
      if (alarm_only)
         owWriteByte(portnum,0xEC);  // issue the alarming search command
      else
         owWriteByte(portnum,0xF0);  // issue the search command

      //pause before beginning the search
      //usDelay(100);
      owDelay();

      // loop to do the search
      do
      {
         // read a bit and its compliment
         bit_test = owTouchBit(portnum,1) << 1;
         bit_test |= owTouchBit(portnum,1);

         // check for no devices on 1-wire
         if (bit_test == 3)
            break;
         else
         {
            // all devices coupled have 0 or 1
            if (bit_test > 0)
              search_direction = !(bit_test & 0x01);  // bit write value for search
            else
            {
               // if this discrepancy if before the Last Discrepancy
               // on a previous next then pick the same as last time
               if (bit_number < LastDiscrepancy[portnum])
                  search_direction = ((SerialNum[portnum][serial_byte_number] & serial_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (bit_number == LastDiscrepancy[portnum]);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     LastFamilyDiscrepancy[portnum] = last_zero;
               }
            }

            // set or clear the bit in the SerialNum[portnum] byte serial_byte_number
            // with mask serial_byte_mask
            if (search_direction == 1)
              SerialNum[portnum][serial_byte_number] |= serial_byte_mask;
            else
              SerialNum[portnum][serial_byte_number] &= ~serial_byte_mask;

            // serial number search direction write bit
            owTouchBit(portnum,search_direction);

            // increment the byte counter bit_number
            // and shift the mask serial_byte_mask
            bit_number++;
            serial_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum[portnum] byte serial_byte_number
            // and reset mask
            if (serial_byte_mask == 0)
            {
                lastcrc8 = docrc8(portnum,SerialNum[portnum][serial_byte_number]);  // accumulate the CRC
                serial_byte_number++;
                serial_byte_mask = 1;
            }
         }
      }
      while(serial_byte_number < 8);  // loop until through all SerialNum[portnum] bytes 0-7

      // if the search was successful then
      if (!((bit_number < 65) || lastcrc8))
      {
         // search successful so set LastDiscrepancy[portnum],LastDevice[portnum],next_result
         LastDiscrepancy[portnum] = last_zero;
         LastDevice[portnum] = (LastDiscrepancy[portnum] == 0);
         next_result = TRUE;
      }
   }

   // if no device found then reset counters so next 'next' will be
   // like a first
   if (!next_result || !SerialNum[portnum][0])
   {
      LastDiscrepancy[portnum] = 0;
      LastDevice[portnum] = FALSE;
      LastFamilyDiscrepancy[portnum] = 0;
      next_result = FALSE;
   }

   return next_result;
}

//--------------------------------------------------------------------------
// The 'owSerialNum' function either reads or sets the SerialNum buffer
// that is used in the search functions 'owFirst' and 'owNext'.
// This function contains two parameters, 'serialnum_buf' is a pointer
// to a buffer provided by the caller.  'serialnum_buf' should point to
// an array of 8 unsigned chars.  The second parameter is a flag called
// 'do_read' that is TRUE (1) if the operation is to read and FALSE
// (0) if the operation is to set the internal SerialNum buffer from
// the data in the provided buffer.
//
// 'portnum'       - number 0 to MAX_PORTNUM-1.  This number is provided to
//                   indicate the symbolic port number.
// 'serialnum_buf' - buffer to that contains the serial number to set
//                   when do_read = FALSE (0) and buffer to get the serial
//                   number when do_read = TRUE (1).
// 'do_read'       - flag to indicate reading (1) or setting (0) the current
//                   serial number.
//
void owSerialNum(u08 portnum, u08 *serialnum_buf, u08 do_read)
{
   u08 i;

   // read the internal buffer and place in 'serialnum_buf'
   if (do_read)
   {
      for (i = 0; i < 8; i++)
         serialnum_buf[i] = SerialNum[portnum][i];
   }
   // set the internal buffer from the data in 'serialnum_buf'
   else
   {
      for (i = 0; i < 8; i++)
         SerialNum[portnum][i] = serialnum_buf[i];
   }
}

//--------------------------------------------------------------------------
// Setup the search algorithm to find a certain family of devices
// the next time a search function is called 'owNext'.
//
// 'portnum'       - number 0 to MAX_PORTNUM-1.  This number was provided to
//                   OpenCOM to indicate the port number.
// 'search_family' - family code type to set the search algorithm to find
//                   next.
//
void owFamilySearchSetup(u08 portnum, u08 search_family)
{
   u08 i;

   // set the search state to find SearchFamily type devices
   SerialNum[portnum][0] = search_family;
   for (i = 1; i < 8; i++)
      SerialNum[portnum][i] = 0;
   LastDiscrepancy[portnum] = 64;
   LastDevice[portnum] = FALSE;
}

//--------------------------------------------------------------------------
// Set the current search state to skip the current family code.
//
// 'portnum'     - number 0 to MAX_PORTNUM-1.  This number is provided to
//                 indicate the symbolic port number.
//
void owSkipFamily(u08 portnum)
{
   // set the Last discrepancy to last family discrepancy
   LastDiscrepancy[portnum] = LastFamilyDiscrepancy[portnum];
   LastFamilyDiscrepancy[portnum] = 0;

   // check for end of list
   if (LastDiscrepancy[portnum] == 0)
      LastDevice[portnum] = TRUE;
}


