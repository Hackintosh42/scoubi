

/*********************************** MODULE INFO ****************************
	
   File name    : Melexis.c
   Ver nr.      : 1.0
   Description  : Routines to set the internal registers of the Melexis TH7122
   Compiler     : 
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-08-14  Initial 
 
****************************************************************************/
#define MELEXIS_C
#include "Melexis.h"


//#define MELEXIS_DEBUGBITS
//#define MELEXIS_DEBUG


/******************************/
/* Toggle SDA Pin			  */
/******************************/
void Mel_SDA(u08 val)
{
	if(val)
		SPI_PORT = SPI_PORT | (1<<SDA_PIN);
	else
		SPI_PORT = SPI_PORT & ~(1<<SDA_PIN);
}

/******************************/
/* Toggle SCL Pin             */
/******************************/
void Mel_SCL(u08 val)
{
	if(val)
		SPI_PORT = SPI_PORT | (1<<SCL_PIN);
	else
		SPI_PORT = SPI_PORT & ~(1<<SCL_PIN);
}


/******************************/
/* Toggle SDEN Pin             */
/******************************/
void Mel_SDEN(u08 val)
{
	if(val)
		SDEN_PORT = SDEN_PORT | (1<<SDEN_PIN);
	else
		SDEN_PORT = SDEN_PORT & ~(1<<SDEN_PIN);
}


/***********************************/
/* Wait for the wind and sunshine  */
/***********************************/
void Mel_wait(void)
{
  //delay(1);
}



/******************************/
/* Write to Serial register  */
/******************************/
void Mel_Wrreg(u32 data)
{
u08 i;
u32 dataword;
	dataword = (0x00FFFFFF & data); // | ((addr &0x03)<<22);	
	dataword = dataword <<8; // Left align, MSB bit shifted out first.
		
	for(i=0; i< 24; i++)
	{
    if( (dataword & 0x80000000))
    {
      Mel_SDA(1);
      //rprintf("1");      
    }
    else
    {
      Mel_SDA(0);    
      //rprintf("0");
    }

    Mel_wait();
		Mel_SCL(1);
		Mel_wait();Mel_wait();
		Mel_SCL(0);
		Mel_wait();
		dataword = dataword <<1;				
	}
	Mel_SDEN(1);
	Mel_wait();
	Mel_SDEN(0);	
  Mel_wait();  
  Mel_SDA(0);
  
  Mel_wait();
  
}


/******************************************/
/* Switch between RX/TX IDLE and Standby  */
/*****************************************/
void Mel_Mode(u08 data)
{
  switch(data)
  {
    case MEL_STANDBY:
		#ifdef MELEXIS_DEBUG 
			rprintf_rom(PSTR("\r\nMEL_STANDBY\r\n ")); 
		#endif   	  
		Mel_Reg.A = MEL_REGA_LOADVAL_STANDBY; Mel_Wrreg(Mel_Reg.A);  // A   
    Mel.Mode = MEL_STANDBY;
    #ifdef TAG_DEBUG_MELRX
      DEBUG0_0;
    #endif
		break;
  
    case MEL_TX :
		#ifdef MELEXIS_DEBUG 
			rprintf_rom(PSTR("\r\nMEL_TX\r\n "));  
		#endif
		// Hack , change this later please
		switch(Mel.txLevel_u08)
		{
			case 0:			
				Mel_Reg.A = MEL_REGA_LOADVAL_TX_LVL0; Mel_Wrreg(Mel_Reg.A);  // A	  
				break;
			case 1:
				Mel_Reg.A = MEL_REGA_LOADVAL_TX_LVL1; Mel_Wrreg(Mel_Reg.A);  // A	  
				break;
			case 2:
				Mel_Reg.A = MEL_REGA_LOADVAL_TX_LVL2; Mel_Wrreg(Mel_Reg.A);  // A	  
				break;
			case 3:
				Mel_Reg.A = MEL_REGA_LOADVAL_TX_LVL3; Mel_Wrreg(Mel_Reg.A);  // A	  
				break;
			default:
				Mel_Reg.A = MEL_REGA_LOADVAL_TX_LVL0; Mel_Wrreg(Mel_Reg.A);  // A	  
				break;
		}
			
		Mel.Mode = MEL_TX; 
    // nOW SET THE vco CURRENT
    switch(Mel.RxFreq)
    {
      case  FREQ_RX_403:
        Mel_Reg.C = MEL_REGC_LOADVAL_TX_403;      
      break;
      
      case  FREQ_RX_433:
        Mel_Reg.C = MEL_REGC_LOADVAL_TX_433;
      break;
    }
    Mel_Wrreg(Mel_Reg.C);  	
    
    #ifdef TAG_DEBUG_MELRX
      DEBUG0_0;
    #endif        
		break;
    
    case MEL_RX :
		#ifdef MELEXIS_DEBUG    
			rprintf_rom(PSTR("\r\nMEL_RX\r\n ")); 
		#endif
		Mel_Reg.A = MEL_REGA_LOADVAL_RX; Mel_Wrreg(Mel_Reg.A);  // A
    Mel.Mode = MEL_RX; 
    // nOW SET THE vco CURRENT
    switch(Mel.RxFreq)
    {
      case  FREQ_RX_403:
        Mel_Reg.C = MEL_REGC_LOADVAL_RX_403;
      break;
      case  FREQ_RX_433:
        Mel_Reg.C = MEL_REGC_LOADVAL_RX_433;
      break;
    }
    Mel_Wrreg(Mel_Reg.C);  	
    
     
    #ifdef TAG_DEBUG_MELRX
      DEBUG0_1;
    #endif       
		break;
    
    case MEL_IDLE:
		#ifdef MELEXIS_DEBUG 
			rprintf_rom(PSTR("\r\nMEL_IDLE\r\n ")); 
		#endif	
		Mel_Reg.A = MEL_REGA_LOADVAL_STANDBY; Mel_Wrreg(Mel_Reg.A);  // A
    Mel.Mode = MEL_IDLE;    
    #ifdef TAG_DEBUG_MELRX
      DEBUG0_0;
    #endif       
    
		break;    
   } 
}

//******************************************
// Select the TX Frequency
//*****************************************
void Mel_SetTxFreq(u08 freq)
{  
  switch(freq)
  {
    case  FREQ_TX_403:
      Mel.TxFreq = freq;    
      Mel_Reg.D = MEL_REGD_LOADVAL_403;
    break;
    case  FREQ_TX_433:
      Mel.TxFreq = freq;        
      Mel_Reg.D = MEL_REGD_LOADVAL_433;
    break;
  }
 Mel_Wrreg(Mel_Reg.D);  	
}



//****************************************
// Select the RX Frequency
// The naming is based on the frequency that will
// be received ie A reader must receive on the TAG frequency
//*****************************************
void Mel_SetRxFreq(u08 freq)
{  
  switch(freq)
  {
    case  FREQ_RX_403:
      Mel.RxFreq = freq;
      Mel_Reg.C = MEL_REGC_LOADVAL_RX_403;
    break;
    case  FREQ_RX_433:
      Mel.RxFreq = freq;    
      Mel_Reg.C = MEL_REGC_LOADVAL_RX_433;
    break;
  }
 Mel_Wrreg(Mel_Reg.C);  	
}






void Mel_Init(void)
{

  Mel_SCL(0);
  Mel_SDA(0);
  Mel_SDEN(0);
  Mel_wait();Mel_wait();
  
  // Clear out the melexis registers
  Mel_Wrreg(0x00000000);
    

/*
	Mel_Reg.D = MEL_REGD_LOADVAL_TAG; Mel_Wrreg(Mel_Reg.D);  	
	Mel_Reg.C = MEL_REGC_LOADVAL_READER; Mel_Wrreg(Mel_Reg.C);  
*/	
// Remeber to uncomment.

	Mel_Reg.A = MEL_REGA_LOADVAL_STANDBY; Mel_Wrreg(Mel_Reg.A);  
	Mel_Reg.B = MEL_REGB_LOADVAL; Mel_Wrreg(Mel_Reg.B);  
  
  Mel_SetRxFreq(FREQ_RX_433);
  Mel_SetTxFreq(FREQ_TX_433);   

#ifdef MELEXIS_DEBUGBITS
  rprintf("\r\n A = ");
  rprintfu08((Mel_Reg.A>>24) & 0xFF);rprintfu08((Mel_Reg.A>>16) & 0xFF);rprintfu08((Mel_Reg.A>>8) & 0xFF);rprintfu08((Mel_Reg.A) & 0xFF);  
  rprintf(" ");
  Mel_Wrreg(Mel_Reg.A);

  rprintf("\r\n B = ");
  rprintfu08((Mel_Reg.B>>24) & 0xFF);rprintfu08((Mel_Reg.B>>16) & 0xFF);rprintfu08((Mel_Reg.B>>8) & 0xFF);rprintfu08((Mel_Reg.B) & 0xFF);  
  rprintf(" ");
  Mel_Wrreg(Mel_Reg.B);

  rprintf("\r\n C = ");
  rprintfu08((Mel_Reg.C>>24) & 0xFF);rprintfu08((Mel_Reg.C>>16) & 0xFF);rprintfu08((Mel_Reg.C>>8) & 0xFF);rprintfu08((Mel_Reg.C) & 0xFF);  
  rprintf(" ");
  Mel_Wrreg(Mel_Reg.C);

  rprintf("\r\n D = ");
  rprintfu08((Mel_Reg.D>>24) & 0xFF);rprintfu08((Mel_Reg.D>>16) & 0xFF);rprintfu08((Mel_Reg.D>>8) & 0xFF);rprintfu08((Mel_Reg.D) & 0xFF);  
  rprintf(" ");
  Mel_Wrreg(Mel_Reg.D);
#endif

  Mel_SCL(0);
  Mel_SDA(0);

}

