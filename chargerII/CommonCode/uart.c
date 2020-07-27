#define UART_C
#include "uart.h"

//static char __attribute__ ((progmem)) HexChars[] = "0123456789ABCDEF";



//*************************************************
// Serial UART Receive interrupt
// A problem is that the message cannot be longer than 
// RX_SERIAL_BUFFER_SIZE coz that will result in 
// unpredicatble behaviour
//*************************************************
SIGNAL(SIG_USART_RECV)
{
cli();
	
  //UDR0 = '*';
	RxSerial.charbuffer[RxSerial.wr_index] = UDR0;  // Get_char();
  RxSerial.LastRxSerial_cnt = 0;  // reset the counter that checks how long ago the last message was sent
  
	if(RxSerial.charbuffer[RxSerial.wr_index] == 100)
	{
		RxSerial.CarriageReturn_flag = 1;		
		// Copy the message out
		u08 i;
		for(i= 0; i<RxSerial.wr_index; i++)
			RxSerial.Message[i] = RxSerial.charbuffer[i];
		RxSerial.MessageReady_flag = 1;
		RxSerial.MessageSize= RxSerial.wr_index;			
		RxSerial.wr_index = 0;
	}
	else if(++RxSerial.wr_index >= RX_SERIAL_BUFFER_SIZE)
  {
    RxSerial.wr_index = 0;  
	RxSerial.CarriageReturn_flag = 0;		
  }
	
sei();
}



//*************************************************
// Serial UART Transmit interrupt
//*************************************************
SIGNAL(SIG_USART_TRANS)
{
cli();
	if(!(UCSR0A & (1<<UDRE0)))
		return; // wait for empty transmit buffer
 
	// Check if all the bytes have been read out 
	if(TxSerial.rd_index  != TxSerial.wr_index)
	{
		// Send the next byte to the UART register
		UDR0 = TxSerial.charbuffer[TxSerial.rd_index];
	
		if(++TxSerial.rd_index >= TX_SERIAL_BUFFER_SIZE){
			TxSerial.rd_index = 0;
		}
	}
	else	
	{	
		TxSerial.transmitting = 0;
		// Finished sending all the bytes	
	}	
sei();  
}




//****************************************
// Initialize the UART Baud Rate and enable
//****************************************
void uartInit(void)
{
#ifdef ATMEGA8
kkl
  UCSRB = (1<<RXEN)|(1<<TXEN);                        // enable UART transmitter
	UCSRC = (1<<URSEL)|(3<<UCSZ0);	
#endif  
#ifdef ATMEGA88
	  UCSR0B = (1<<RXEN0)|(1<<TXEN0);                        // enable UART transmitter
	//	UCSR0C = (1<<URSEL)|(3<<UCSZ0);	
	// enable the Uart receive interrupt
	UCSR0B |= (1<<RXCIE0);                       // Enable Rx complete interrupt
	#ifndef UART_STRAIGHT_TX  
	  // enable the Uart Transmit interrupt  
	  UCSR0B |= (1<<TXCIE0);
	#endif  
#endif  

// Clear the circular buffers
	TxSerial.rd_index = 0;
	TxSerial.wr_index = 0;
	TxSerial.transmitting = 0;
  uart_ClearRXbuffers();
}

void uart_ClearRXbuffers(void)
{
	u08 i;
// Clear the circular buffers
	RxSerial.rd_index = 0;
	RxSerial.wr_index = 0;
	RxSerial.CarriageReturn_flag = 0;
  RxSerial.StartDelim_flag = 0;  
  RxSerial.EndDelim_flag = 0;    
	RxSerial.Wakeup_flag = 0;
	
	// fill the array with nulls
	for(i=0;i<RX_SERIAL_BUFFER_SIZE;i++)
	{
		RxSerial.charbuffer[i] = 0x00;
	}
}
//****************************************
// Set the uart baud rate
//****************************************
void uartSetBaudRate(u32 baudrate)
{
	// calculate division factor for requested baud rate, and set it
	u16 bauddiv = ((F_CPU+(baudrate*8L))/(baudrate*16L)-1);
  #ifdef ATMEGA8
    outb(UBRRL, bauddiv);
    outb(UBRRH, bauddiv>>8);    
  #endif
  #ifdef ATMEGA88
    UBRR0L =  bauddiv;
    UBRR0H =  bauddiv>>8;        
  #endif
}




//****************************************
// Send the byte to the Uart, or write to 
// the circular transmit buffer.
//***************************************
void uartSendChar(u08 data)
{
#ifdef UART_STRAIGHT_TX
    #ifdef ATMEGA8
    while (!(UCSRA & (1<<UDRE)));              // wait for empty transmit buffer
    #endif
    #ifdef ATMEGA88
    while (!(UCSR0A & (1<<UDRE0)));              // wait for empty transmit buffer
    #endif
    #ifdef ATMEGA8
      UDR = data;                            
    #endif
    #ifdef ATMEGA88
      UDR0 = data;                            
    #endif    
    #ifdef ATMEGA8
    while (!(UCSRA & (1<<UDRE)));              // wait for empty transmit buffer
    #endif
    #ifdef ATMEGA88
    while (!(UCSR0A & (1<<UDRE0)));              // wait for empty transmit buffer
    #endif    
#else
	// add the byte to the transmit Q, check if there is a transmission taking
	// place if not, then kickstart the transmission
	
	if(!TxSerial.transmitting)
	{
		// Kickstart by writing the first byte into the UART Data register
		UDR0 = data;                            				
		//UDR0 = '+';                            				
		TxSerial.transmitting = 1;
	}
	else
	{
		// Copy the byte into the circular buffer.
		TxSerial.charbuffer[TxSerial.wr_index] = data;			
		
		if(++TxSerial.wr_index >= TX_SERIAL_BUFFER_SIZE){
			TxSerial.wr_index = 0;
		}
		
	}

	
#endif	
}




u08 uartGetChar(void)
{
#ifndef PC
  #ifdef ATMEGA8
    while ( !(UCSRA & (1<<RXC)) );
    return UDR;    
  #endif
  #ifdef ATMEGA88
    while ( !(UCSR0A & (1<<RXC0)) );
    return UDR0;        
  #endif
  
#else
#endif
}


