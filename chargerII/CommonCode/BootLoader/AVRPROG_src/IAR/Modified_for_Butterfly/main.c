/*****************************************************************************
*
* Atmel Corporation
*
* File              : main.c
* Compiler          : IAR C 3.10C Kickstart
* Revision          : $Revision: 1.3 $
* Date              : $Date: Wednesday, January 26, 2005 10:20:44 UTC $
* Updated by        : $Author: jllassen $
*
* Support mail      : avr@atmel.com
*
* Target platform   : All AVRs with bootloader support
*
* AppNote           : AVR109 - Self-programming
*
* Description   : This Program allows an AVR with bootloader capabilities to 
*                 Read/write its own Flash/EEprom. To enter Programming mode   
*                 an input pin is checked. If this pin is pulled low, programming mode  
*                 is entered. If not, normal execution is done from $0000 
*                 "reset" vector in Application area.
*
* Preparations  : Use the preprocessor.xls file for obtaining a customized
*                 defines.h file and linker-file code-segment definition for
*                 the device you are compiling for.
****************************************************************************/
#include <inavr.h>
#include "defines.h"
#include "serial.h"
#include "flash.h"



/* Uncomment the following to save code space */
//#define REMOVE_AVRPROG_SUPPORT
//#define REMOVE_FUSE_AND_LOCK_BIT_SUPPORT
//#define REMOVE_BLOCK_SUPPORT
//#define REMOVE_EEPROM_BYTE_SUPPORT
//#define REMOVE_FLASH_BYTE_SUPPORT

// Remove support for all but Flash block operations to
// fit into 512 words with full optimization.
// Next line added for BUTTERFLY specific bootloader.
//#define REMOVE_BLOCK_EEPROM_SUPPORT



#ifndef REMOVE_BLOCK_SUPPORT
unsigned char BlockLoad(unsigned int size, unsigned char mem, long *address);
void BlockRead(unsigned int size, unsigned char mem, long *address);

/* BLOCKSIZE should be chosen so that the following holds: BLOCKSIZE*n = PAGESIZE,  where n=1,2,3... */
#define BLOCKSIZE PAGESIZE

#endif



// Prototype needed for BUTTERFLY specific bootloader.
void OSCCAL_calibration(void);


// Even if BUTTERFLY is ATmega169, AVRprog only supports ATmega16.
#undef PARTCODE
#define	PARTCODE 0x75


__C_task void main(void)
{
    long address;
    unsigned int temp_int;
    unsigned char val;


    /* Initialization */
    void (*funcptr)( void ) = 0x0000; // Set up function pointer to RESET vector.
    
// The next two lines are removed for the BUTTERFLY bootloader.
//    PROGPORT |= (1<<PROG_NO); // Enable pull-up on PROG_NO line on PROGPORT.
//    initbootuart(); // Initialize UART.


    /**********************************************/
    /*** START: BUTTERFLY specific code section ***/
    /**********************************************/

    OSCCAL_calibration();   // Calibrate the OSCCAL byte

    ACSR = (1<<ACD);

    // Disable Digital input on PF0-2 (power save)
    DIDR1 = (7<<ADC0D);

    // Enable pullups.
    PORTB = 0xFF;
    PORTE = 0xFF;
    
    // boost IntRC to 2Mhz to achieve 19200 baudrate
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 4, Inter RC 8Mhz / 4 = 2Mhz
    CLKPR = (1<<CLKPS1);    

    UBRR0H = 0;//(unsigned char)(baudrate>>8);
    UBRR0L = BRREG_VALUE;//(unsigned char)baudrate;

    // Enable 2x speed
    UCSR0A = (1<<U2X0);

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(0<<RXCIE0)|(0<<UDRIE0);

    // Async. mode, 8N1
    UCSR0C = (0<<UMSEL0)|(0<<UPM00)|(0<<USBS0)|(3<<UCSZ00)|(0<<UCPOL0);
        
    MCUCR = (1<<IVCE);       
    MCUCR = (1<<IVSEL);             //move interruptvectors to the Boot sector    

    __enable_interrupt();
    
    PCMSK1 = (1<<PORTB6) | (1<<PORTB4);
  	EIFR = (1<<PCIF1);
	EIMSK = (1<<PCIE1);

	for(;;)
	{
	
        if(!(PINB & (1<<PORTB6))) // If joystick in UP-position
        {
            MCUCR = (1<<IVCE); 
            MCUCR = (0<<IVSEL);             //move interruptvectors to the Application sector
            __disable_interrupt();
            funcptr();      // Jump to application sector   
        }

        else if(!(PINB & (1<<PORTB4))) // If joystick in the ENTER-position
        {
            val = recchar();
   
            if( val == 0x1B)
         	{
/*          		while (val != 'S')				// Wait for signon.
          		{
          		  val = recchar();
          		}

                sendchar('A');					// Report signon ID.
                sendchar('V');
                sendchar('R');
                sendchar('B');
                sendchar('O');
                sendchar('O');
                sendchar('T');
*/                
                __disable_interrupt();
                
                break; // out of for-loop
            } else {
	    		sendchar('?');
	    	}
        }

        else
        {	
            UCSR0A = 0x02;
            SMCR = (3<<SM0) | (1<<SE);      // Enable Power-save mode
            __sleep();                      // Go to sleep
            SMCR = 0;                       // Just woke, disable sleep
        }
	}			

    /**********************************************/
    /*** END: BUTTERFLY specific code section ***/
    /**********************************************/


    /* Branch to bootloader or application code? */
// The next line is removed for the BUTTERFLY bootloader.
//    if( !(PROGPIN & (1<<PROG_NO)) ) // If PROGPIN is pulled low, enter programmingmode.
    {
        /* Main loop */
        for(;;)
        {
            val=recchar(); // Wait for command character.

            // Check autoincrement status.
            if(val=='a')
            {
                sendchar('Y'); // Yes, we do autoincrement.
            }


            // Set address.
            else if(val=='A') // Set address...
            { // NOTE: Flash addresses are given in words, not bytes.                                            
                address=(recchar()<<8) | recchar(); // Read address high and low byte.
                sendchar('\r'); // Send OK back.
            }

            
            // Chip erase.
            else if(val=='e')
            {
                for(address = 0; address < APP_END;address += PAGESIZE)
                { // NOTE: Here we use address as a byte-address, not word-address, for convenience.
                    _WAIT_FOR_SPM();        
                    #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                    _PAGE_ERASE( address );
                    #pragma diag_default=Pe1053 // Back to default.
                }
          
                sendchar('\r'); // Send OK back.
            }
            
#ifndef REMOVE_BLOCK_SUPPORT
            // Check block load support.
            else if(val=='b')
		    {
    			sendchar('Y'); // Report block load supported.
    			sendchar((BLOCKSIZE>>8) & 0xFF); // MSB first.
    			sendchar(BLOCKSIZE&0xFF); // Report BLOCKSIZE (bytes).
    		}


            // Start block load.
    		else if(val=='B')
    		{
	    	    temp_int = (recchar()<<8) | recchar(); // Get block size.
		    	val = recchar(); // Get memtype.
			    sendchar( BlockLoad(temp_int,val,&address) ); // Block load.
		    }
		
		    
		    // Start block read.
    		else if(val=='g')
    		{
	    	    temp_int = (recchar()<<8) | recchar(); // Get block size.
    			val = recchar(); // Get memtype
	    		BlockRead(temp_int,val,&address); // Block read
    		}		
#endif

#ifndef REMOVE_FLASH_BYTE_SUPPORT            
            // Read program memory.
            else if(val=='R')
            {        
                // Send high byte, then low byte of flash word.
                _WAIT_FOR_SPM();        
                _ENABLE_RWW_SECTION();
                #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                sendchar( _LOAD_PROGRAM_MEMORY( (address << 1)+1 ) );
                sendchar( _LOAD_PROGRAM_MEMORY( (address << 1)+0 ) );
                #pragma diag_default=Pe1053 // Back to default.

                address++; // Auto-advance to next Flash word.
            }
        

            // Write program memory, low byte.        
            else if(val=='c')
            { // NOTE: Always use this command before sending high byte.
                temp_int=recchar(); // Get low byte for later _FILL_TEMP_WORD.
                sendchar('\r'); // Send OK back.
            }
            
            
            // Write program memory, high byte.
            else if(val=='C')
            {
                temp_int |= (recchar()<<8); // Get and insert high byte.
                _WAIT_FOR_SPM();
                #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                _FILL_TEMP_WORD( (address << 1), temp_int ); // Convert word-address to byte-address and fill.
                #pragma diag_default=Pe1053 // Back to default.
                address++; // Auto-advance to next Flash word.
                sendchar('\r'); // Send OK back.
            }
        
        
            // Write page.       
            else if(val== 'm')
            {
                if( address >= (APP_END>>1) ) // Protect bootloader area.
                {
                    sendchar('?');
                } else
                {
                    _WAIT_FOR_SPM();        
                    #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                    _PAGE_WRITE( address << 1 ); // Convert word-address to byte-address and write.
                    #pragma diag_default=Pe1053 // Back to default.
                }

                sendchar('\r'); // Send OK back.
            }
#endif

#ifndef REMOVE_EEPROM_BYTE_SUPPORT
            // Write EEPROM memory.
            else if (val == 'D')
            {
                _WAIT_FOR_SPM();        
                EEARL = address; // Setup EEPROM address.
                EEARH = (address >> 8);
                EEDR = recchar(); // Get byte.
                EECR |= (1<<EEMWE); // Write byte.
                EECR |= (1<<EEWE);
                while (EECR & (1<<EEWE)) // Wait for write operation to finish.
                    ;
                    
                address++; // Auto-advance to next EEPROM byte.
                sendchar('\r');// Send OK back.
            }

            
            // Read EEPROM memory.
            else if (val == 'd')
            {
                EEARL = address; // Setup EEPROM address.
                EEARH = (address >> 8);
                EECR |= (1<<EERE); // Read byte...
                sendchar(EEDR); // ...and send it back.
                address++; // Auto-advance to next EEPROM byte.
            }
#endif

#ifndef REMOVE_FUSE_AND_LOCK_BIT_SUPPORT
            // Write lockbits.
            else if(val=='l')
            {
                _WAIT_FOR_SPM();        
                _SET_LOCK_BITS( recchar() ); // Read and set lock bits.
                sendchar('\r'); // Send OK back.
            }
               

            // Read lock bits.
            else if(val=='r')
            {
                _WAIT_FOR_SPM();        
                sendchar( _GET_LOCK_BITS() );
            }


            // Read fuse bits.
            else if(val=='F')
            {
                _WAIT_FOR_SPM();        
                sendchar( _GET_LOW_FUSES() );
            }


            // Read high fuse bits.
            else if(val=='N')
            {
                _WAIT_FOR_SPM();        
                sendchar( _GET_HIGH_FUSES() );
            }


            // Read extended fuse bits.
            else if(val=='Q')
            {
                _WAIT_FOR_SPM();        
                sendchar( _GET_EXTENDED_FUSES() );
            }
#endif

#ifndef REMOVE_AVRPROG_SUPPORT        
            // Enter and leave programming mode.
            else if((val=='P')||(val=='L'))
            {
                sendchar('\r'); // Nothing special to do, just answer OK.
            }
            
            
            // Exit bootloader.
            else if(val=='E')
            {
// The next four lines are removed for the BUTTERFLY bootloader...
//                _WAIT_FOR_SPM();        
//                _ENABLE_RWW_SECTION();
//                sendchar('\r');
//                funcptr(); // Jump to Reset vector 0x0000 in Application Section.
// ...and replaced by these two lines:
                WDTCR = (1<<WDCE) | (1<<WDE); //Enable Watchdog Timer to give reset
                sendchar('\r');
            }

    
            // Get programmer type.        
            else if (val=='p')
            {
                sendchar('S'); // Answer 'SERIAL'.
            }
            
            
            // Return supported device codes.
            else if(val=='t')
            {
                sendchar( PARTCODE ); // Supports only this device, of course.
                sendchar( 0 ); // Send list terminator.
            }
            
            
            // Set LED, clear LED and set device type.
            else if((val=='x')||(val=='y')||(val=='T'))
            {
                recchar(); // Ignore the command and it's parameter.
                sendchar('\r'); // Send OK back.
            }
#endif
       
            // Return programmer identifier.
            else if(val=='S')
            {
                sendchar('A'); // Return 'AVRBOOT'.
                sendchar('V'); // Software identifier (aka programmer signature) is always 7 characters.
                sendchar('R');
                sendchar('B');
                sendchar('O');
                sendchar('O');
                sendchar('T');
            }
        
            
            // Return software version.
            else if(val=='V')
            {
                sendchar('1');
                sendchar('4');
            }        


            // Return signature bytes.
            else if(val=='s')
            {							
                sendchar( SIGNATURE_BYTE_3 );
                sendchar( SIGNATURE_BYTE_2 );
                sendchar( SIGNATURE_BYTE_1 );
            }       


            // The last command to accept is ESC (synchronization).
            else if(val!=0x1b)                  // If not ESC, then it is unrecognized...
            {
                sendchar('?');
            }
        } // end: for(;;)
    }

// The next six lines are removed for the BUTTERFLY bootloader...
//    else
//    {
//        _WAIT_FOR_SPM();        
//        _ENABLE_RWW_SECTION();
//        funcptr(); // Jump to Reset vector 0x0000 in Application Section.
//    }


} // end: main


#ifndef REMOVE_BLOCK_SUPPORT
unsigned char BlockLoad(unsigned int size, unsigned char mem, long *address)
{
// Next line added for BUTTERFLY specific bootloader.
#ifndef REMOVE_BLOCK_EEPROM_SUPPORT
    unsigned char buffer[BLOCKSIZE];
// Next line added for BUTTERFLY specific bootloader.
#endif

	unsigned int data, tempaddress;

	
	// EEPROM memory type.
    if(mem=='E')
    {
// Next line added for BUTTERFLY specific bootloader.
#ifndef REMOVE_BLOCK_EEPROM_SUPPORT
        /* Fill buffer first, as EEPROM is too slow to copy with UART speed */
        for(tempaddress=0;tempaddress<size;tempaddress++)
            buffer[tempaddress] = recchar();
        
        /* Then program the EEPROM */
        _WAIT_FOR_SPM();
    	for( tempaddress=0; tempaddress < size; tempaddress++)
    	{
	        EEARL = *address; // Setup EEPROM address
            EEARH = ((*address) >> 8);
            EEDR = buffer[tempaddress]; // Get byte.
            EECR |= (1<<EEMWE); // Write byte.
            EECR |= (1<<EEWE);
            while (EECR & (1<<EEWE)) // Wait for write operation to finish.
                ;

  			(*address)++; // Select next EEPROM byte
        }
// Next line added for BUTTERFLY specific bootloader.
#endif
        return '\r'; // Report programming OK
    } 
    
    // Flash memory type.
	else if(mem=='F')
    { // NOTE: For flash programming, 'address' is given in words.
        (*address) <<= 1; // Convert address to bytes temporarily.
        tempaddress = (*address);  // Store address in page.
	
        do
		{
            data = recchar();
            data |= (recchar() << 8);
            #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
            _FILL_TEMP_WORD(*address,data);
            #pragma diag_default=Pe1053 // Back to default.
            (*address)+=2; // Select next word in memory.
            size -= 2; // Reduce number of bytes to write by two.
        } while(size); // Loop until all bytes written.

		_PAGE_WRITE(tempaddress);
		_WAIT_FOR_SPM();
		_ENABLE_RWW_SECTION();

        (*address) >>= 1; // Convert address back to Flash words again.
        return '\r'; // Report programming OK
    }
    
    // Invalid memory type?
    else
    {
        return '?';
    }
}


void BlockRead(unsigned int size, unsigned char mem, long *address)
{
    // EEPROM memory type.
    if (mem=='E') // Read EEPROM
    {
// Next line added for BUTTERFLY specific bootloader.
#ifndef REMOVE_BLOCK_EEPROM_SUPPORT
        do
        {
            EEARL = *address; // Setup EEPROM address
            EEARH = ((*address) >> 8);
            (*address)++; // Select next EEPROM byte
            EECR |= (1<<EERE); // Read EEPROM
            sendchar(EEDR); // Transmit EEPROM dat ato PC

            size--; // Decrease number of bytes to read
        } while (size); // Repeat until all block has been read
// Next line added for BUTTERFLY specific bootloader.
#endif
    }
    
    // Flash memory type.
	else if(mem=='F')
	{
        (*address) <<= 1; // Convert address to bytes temporarily.
	
        do
        {
            #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
            sendchar( _LOAD_PROGRAM_MEMORY(*address) );
            sendchar( _LOAD_PROGRAM_MEMORY((*address)+1) );
            #pragma diag_default=Pe1053 // Back to default.
            (*address) += 2; // Select next word in memory.
            size -= 2; // Subtract two bytes from number of bytes to read
        } while (size); // Repeat until all block has been read

        (*address) >>= 1; // Convert address back to Flash words again.
    }
}
#endif


/*** Rest of file is BUTTERFLY specific ***/

/* Interrupt vector for pin change interrupt. No code, just for wakeing up. */
#pragma vector = PCINT1_vect
__interrupt void PCINT1_interrupt(void)
{
//    __no_operation();
}


/*****************************************************************************
*   Function name : Delay
*   Returns :       None
*   Parameters :    unsigned int millisec
*   Purpose :       Delay-loop
*****************************************************************************/
void Delay(unsigned int millisec)
{
    int i;
    
    while (millisec--)
        for (i=0; i<125; i++);
}


/*****************************************************************************
*   Function name : OSCCAL_calibration
*   Returns :       None
*   Parameters :    None
*   Purpose :       Calibrate the internal OSCCAL byte, using the external 
*                   32,768 kHz crystal as reference
*****************************************************************************/
void OSCCAL_calibration(void)
{
    unsigned char calibrate;
    int temp;
    unsigned char tempL;

    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    Delay(1000);    // wait for external crystal to stabilise
    
    calibrate = 0;
    while(!calibrate)
    {
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
        while ( !(TIFR2 & (1<<OCF2A)) );   // wait for timer2 compareflag
    
        TCCR1B = 0; // stop timer1

        if ( (TIFR1 & (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
        {
            calibrate = 1;   // the interRC is correct
        }
    
        TCCR1B = (1<<CS10); // start timer1
    }
}


/* end of file */
