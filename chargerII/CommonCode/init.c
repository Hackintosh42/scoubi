/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : Init.c
   Ver nr.     : 1.0
   Description : Initializes the Tag hardware               
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-05    JvA  
               
 
*************************************************************************************/

#define INIT_C
#include "Init.h"

//**************************************************
// Top Level Fuction called to Initialize the Tag
//**************************************************
void InitTag(void)
{
  InitPorts();
  
  // ****** RS232 UART ******
  uartInit();
  uartSetBaudRate(19200);

  // ****** I2C ******  
  i2cInit(); 
  Time_InitRTC(0);	
  TWCR = 0; 
  
  // ****** Real Time Clock interrupt *****
  PinChange_RTC_INTB_enable();  
		
	
	i2cInit();  // has to be initialized twice, not too sure what the prob is!!
  
  // *********************************
  // **** Initialize the Wireless ****
  InitWireless();
  
 	DS620_init();

	sei(); // enable all interrupts 
  
  // Intialize the random seed generator from the realtime clock time
  u08 hour,minute,second;
  Time_readtime(&hour,&minute,&second);
  u16 rseed = minute<<8|second;
  srand(rseed); 
}





//**************************************************
// Initialize the Wireless capability
//**************************************************
void InitWireless(void)
{
  Modem_Init();
	Init_timer0();	// Enable the timer that handles the Modem    
  
  Modem_SetRxFreq(FREQ_RX_433);     
  Modem_SetTxFreq(FREQ_TX_433);     
	
  Init_LinkType(MODEM_FULL_DUPLEX,LINK_PHY_RF_AND_RS232);
  
  Modem_Set_BaseTxLevel(0); 
	Modem_SetTxLevel(0);      
     
  // Enable the Modem and start sampling airwaves
  Modem.Enable = 1;  
  MODEM_TIMER_ENABLE;
	//********************************************
	// Initialize the link and network layers
	//********************************************    
	Link.MessageFormat = LINK_CLIMATAG_FORMAT;   
	net_init();
  
  rprintf_rom(PSTR("MI!\r\n"));
}


// *****************************************************
// Initialize the AVR Ports for input and output
// *****************************************************
void InitPorts(void)
{
  PORTB   = 0x10;         // Turn off Leds on Port B
  DDRB    = 0xEF;         // Set PORTB as output  MISO as input
	PORTC   = 0x34;         // IO0 and IO1 are output
	DDRC    = 0x35;         // SDA and SCL output
  PORTD   = 0x09;         // Turn off Leds on Port D
  DDRD    = 0xC0;         

	// Initialize pins for the melexis
	MELEXIS_DATA_IN_DDR |= (1<<MELEXIS_DATA_IN_PORTPIN);
  MELEXIS_DATA_IN_PORT |= (1<<MELEXIS_DATA_IN_PORTPIN);
  MELEXIS_DATA_OUT_DDR &= ~(1<<MELEXIS_DATA_OUT_PORTPIN);
  MELEXIS_DATA_OUT_PORT &= ~(1<<MELEXIS_DATA_OUT_PORTPIN);
  
 	LED_OFF_ALL;	  
}

// *****************************************************
// Initialize the Link used to send messages
// *****************************************************
void Init_LinkType(u08 modemlinktype,u08 phylayer)
{
  Modem.LinkType = modemlinktype;
  switch(Modem.LinkType)
	{
    case MODEM_FULL_DUPLEX:
      Modem_Set_BaseMode(MODEM_RX);    
      Modem_Switch(MODEM_RX); 
      link_setphy(phylayer);    
    break;

    case MODEM_TX_THEN_RX:
      Modem_Set_BaseMode(MODEM_IDLE);    
      Modem_Switch(MODEM_IDLE);
      link_setphy(phylayer);        
    break;            
                    
    case MODEM_TX_ONLY:
      Modem_Set_BaseMode(MODEM_IDLE);    
      Modem_Switch(MODEM_IDLE);
      link_setphy(phylayer);        
    break;
                              
    case MODEM_NONE:
      Modem_Switch(MODEM_IDLE);
      Modem.Enable = 0;      
      while((volatile u08)Modem.State != MODEM_IDLE);
      link_setphy(phylayer); 
    break;
	}
}


