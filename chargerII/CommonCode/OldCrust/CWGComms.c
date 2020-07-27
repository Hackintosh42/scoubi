/*********************************** MODULE INFO ****************************
	
   File name    : CWGComms.c
   Ver nr.      : 2.0
   Description  : Cooking with Gas Communications protocols for data transmission 
				   and reception over Melexis RF connection
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-12-12  Initial 
 
****************************************************************************/
#define  CWGCOMMS_C
#include "headers.h"


/*

//***************************************************************************
// Copies the next message out 
// 					                                                        
//***************************************************************************
void Comms_AddRxMsg(u08 *src_ptr,u08 len,u08 RSSI)
{	
u08 i;

  //debug_rom(PSTR("AddRxMsg"),1);
	if(++RxMsgQ.wr > (MAX_MSG_Q_SIZE-1)){
		RxMsgQ.wr = 0;
	}
		
    for(i=0;i<len;i++){
      RxMsgQ.Msg[RxMsgQ.wr].buf[i] = *(src_ptr +i);
    }
	RxMsgQ.Msg[RxMsgQ.wr].buf[len] = 0x00; // Complete with NULL character
	RxMsgQ.Msg[RxMsgQ.wr].len = len;
	RxMsgQ.Msg[RxMsgQ.wr].rssi = RSSI;

    #ifdef DEBUG_RECEIVEDMSGBUF
	   debug_nn_ram(&RxMsgQ.Msg[RxMsgQ.wr].buf[0],1);
	   debug_buffer_hex(&RxMsgQ.Msg[RxMsgQ.wr].buf[0], len);
    #endif	
}


//***************************************************************************
// Checks if there is a new message waiting
// 					                                                        
//***************************************************************************
u08 Comms_CheckRxMsg(void)
{
	if(RxMsgQ.rd != RxMsgQ.wr)
		return(1);
	else
		return(0);
}


//***************************************************************************
// Copies the next message out 
// 					                                                        
//***************************************************************************
sRxMsg *Comms_GetNextRxMsg(void)
{
  //debug_rom(PSTR("GetNextRxMsg"),1);

	if(RxMsgQ.rd != RxMsgQ.wr)
	{
		if(++RxMsgQ.rd > (MAX_MSG_Q_SIZE-1))
		{
			RxMsgQ.rd = 0;
		}		
		return(&RxMsgQ.Msg[RxMsgQ.rd]);
	}
	else
		return(0);
}


//***************************************************************************
// CWG *Comms_GetNextWrCWG()
// Passes a pointer to the next CWG structure to use from the interrupt sampling
//*************************************************************************** 
/*
sCWG *Comms_GetNextWrCWG(void)
{

	//debug(PSTR("Comms_GetNextWrCWG()"),TRUE);
	if(++CWGQ.wr > (MAX_CWG_Q_SIZE-1)){
		CWGQ.wr = 0;
	}
	return(&CWGQ.CWG[CWGQ.wr]);
	//return(&CWGQ.CWG[0]);	
}
*/

//*************************************************************************** 
// CWG *Comms_GetNextRdCWG()
// Passes a pointer to the next CWG structure to use from the interrupt sampling
//*************************************************************************** 
/*
sCWG *Comms_GetNextRdCWG(void)
{

	//debug(PSTR("Comms_GetNextRdCWG()"),TRUE);
	if(CWGQ.rd != CWGQ.completedwr)
	{
		if(++CWGQ.rd > (MAX_CWG_Q_SIZE-1))
		{
			CWGQ.rd = 0;
		}		
		return(&CWGQ.CWG[CWGQ.rd]);
		//return(&CWGQ.CWG[0]);			
	}
	else
		return(0);		
}
*/
//***************************************************************************
// Checks if there is a new CWG waiting to be processed
// 					                                                        
//***************************************************************************
/*
u08 Comms_CheckCWGQ(void)
{
	if(CWGQ.rd != CWGQ.completedwr){	
//		debug_u08(PSTR("CWGQ.rd = "),CWGQ.rd,TRUE);	
//		debug_u08(PSTR("CWGQ.wr = "),CWGQ.wr,TRUE);			
		return(1);		
	}
	else{
		return(0);
	}
}
*/

u08 one;
//=================================================
// void Comms_TxBit_interrupt(void)
// Time to transmit the next bit of the packet
// 
//-------------------------------------------------
u08 Comms_TxBit_interrupt(void)
{  
       if(TX.txbyte & TX.bitmask)
		   {
			 if(Comms.sample_cnt == 0)		   
				Comms_Txbit(1);             
			 else
				Comms_Txbit(0);            			 
		   }
           else
		   {
		   	 if(Comms.sample_cnt == 0)		   
				Comms_Txbit(0);             
			 else
				Comms_Txbit(1);            			 
		   }
       
    //---------------------------------------------//
		// Load the next byte into the buffer if 
		// it is time to transmit the next bit
		//---------------------------------------------//
  #ifdef COMMS_8SAMPLES    
	 if(Comms.sample_cnt == 4)		   
  #endif
  #ifdef COMMS_4SAMPLES    
	 if(Comms.sample_cnt == 2)		   
  #endif  
	 {        
          TX.bitmask >>=1;         
          // Preload the registers with the next bit to send
          if(TX.bitmask == 0)
          { 
            // Check if that was the last bit to transmit
            if(TX.bytecnt == 0)
            {
              Comms_Txbit(0);
              TX.Complete = 1;
              return(1);
            }
            else
            {
              TX.bitmask = 0x80;
              TX.bytecnt--;        
              TX.pktbuf_ptr++;        
              TX.txbyte = TX.pktbuf[TX.pktbuf_ptr];			  
            }
          }
     }
     return(0);
}




//***************************************************************************
// Comms_FastStart(void)
// Initialise the variables that will be used by the interrupt routine
// interrupt routine.                                                       
//*************************************************************************** 
void Comms_BlastStart(void)
{  
  // Initialise variables for Sample Decoder 
  SMP.ShiftReg=0;
  SMP.Sample=0x00;
  SMP.GoodBitCount=0x00;
  SMP.Sync=0;
  SMP.Sync_cnt =0;
//  SMP.Synccomplete = 0;
  SMP.Ones_cnt = 0;
  SMP.Ones_cmp =0;
  SMP.Ones_cmp_previous =0;
  SMP.Ones_cmp_sample =0;
  SMP.Ones_cmp_sample_previous = 0xFF;
  SMP.Preamblefound = 0;  
  SMP.preamblebyte[0] = 0x00;
  SMP.preamblebyte[1] = 0x00;
  SMP.preamblebyte[2] = 0x00;
  SMP.Complete = 0;  
  SMP.bitbuf_index = 0;
  SMP.bit_bitmask = 0x80; 
  SMP.len = 0;
 
  
// Get the pointer to the next CWG to process the samples into
  //SMP.CWG = Comms_GetNextWrCWG();
  SMP.CWG = &CWG;

      
  // Initialize the sample count
  Comms.sample_cnt = 0;  
}



//***************************************************************************
// Comms_Sync
// Achieve SYnc then check for the Preamble
// interrupt routine.                                                       
// Return value determined by the signal being received
//*************************************************************************** 

/*
u08 Comms_Sync(void)
{
  unsigned char OldValue;

         
  OldValue=SMP.ShiftReg&0x01;
  
  // The new value is read in as soon as it enters the interrupt
  // routine to try eliminate jitter as much as possible
  
  if ((COMMS_RX_PORT & (1 << COMMS_RX_PIN)) == (1<<COMMS_RX_PIN))
  {
    NewValue=0x01;
    //DEBUG0_1;    
  }
  else
  {
    NewValue=0x00;
    //DEBUG0_0;        
  }  
  
#ifdef COMMS_8SAMPLES  
  // Shift new sample into shift register 
  SMP.ShiftReg=SMP.ShiftReg>>1 | ((NewValue==0)?0x00:0x80);
  // Invert new middle bit (Manchester coding) 
  SMP.ShiftReg^=0x08;
#endif  
#ifdef COMMS_4SAMPLES  
  // Shift new sample into shift register 
  SMP.ShiftReg=SMP.ShiftReg>>1 | ((NewValue==0)?0x00:0x08);
  // Invert new middle bit (Manchester coding) 
  SMP.ShiftReg^=0x02;
#endif  
#ifdef COMMS_2SAMPLES  
  // Shift new sample into shift register 
  SMP.ShiftReg=SMP.ShiftReg>>1 | ((NewValue==0)?0x00:0x02);
  // Invert new middle bit (Manchester coding) 
  SMP.ShiftReg^=0x01;
#endif  


  
  
  // Update A according to values shifted in and out 
  // (We are really counting the number of 0's in the shift register) 
  if ((OldValue==1)&&(NewValue==0))
  {
    if(SMP.Ones_cnt > 0)
	 SMP.Ones_cnt--;
  }
  if ((OldValue==0)&&(NewValue==1))
  { 
    if(SMP.Ones_cnt < 8)		
		SMP.Ones_cnt++;    
  }
#ifdef COMMS_8SAMPLES  
  if ((SMP.ShiftReg&0x08)==0){
#endif
#ifdef COMMS_4SAMPLES  
  if ((SMP.ShiftReg&0x02)==0){
#endif
#ifdef COMMS_2SAMPLES  
  if ((SMP.ShiftReg&0x01)==0){
#endif
    if(SMP.Ones_cnt > 0)  
		SMP.Ones_cnt--;
  }
  else{
    if(SMP.Ones_cnt < 8)		  
		SMP.Ones_cnt++;
  }
   
  if(SMP.Sync)
  {   
    // Time to get the bit out of the shift register
    if(SMP.Sample == SMP.SyncPos)
    {              
      #ifdef COMMS_8SAMPLES  
        #define CWG_ONES_UPPER_THRESH 4    
      #endif    
      #ifdef COMMS_4SAMPLES  
        #define CWG_ONES_UPPER_THRESH 2    
      #endif              
      #ifdef COMMS_2SAMPLES  
        #define CWG_ONES_UPPER_THRESH 1    
      #endif                    
      if(SMP.Ones_cnt > CWG_ONES_UPPER_THRESH)
        SMP.bit = 0x01;      
      else
        SMP.bit = 0x00;      
      // Clock the bits into the buffer
      if(SMP.Preamblefound)
      {               
          // Read in the length of the message
          if(SMP.bit)
          {				  
            SMP.CWG->bitbuf[SMP.bitbuf_index] |= SMP.bit_bitmask;
          }
          else
          {
          }           
          // Update the bitmask
          SMP.bit_bitmask = (SMP.bit_bitmask>>1);    
          if(SMP.bit_bitmask == 0)
          {
            // The length is now in the first byte in the bitbuf
            if(SMP.bitbuf_index == 0x00)
              SMP.len = SMP.CWG->bitbuf[0];
          
            SMP.bit_bitmask =0x80;
            SMP.bitbuf_index++;
            SMP.CWG->bitbuf[SMP.bitbuf_index] = 0x00;                         
            if((SMP.bitbuf_index > SMP.len+3) ||(SMP.bitbuf_index > (MAX_RX_BITBUF_SIZE-1)))            
            //if(SMP.bitbuf_index > (MAX_RX_BITBUF_SIZE-1))                        
            {
              // The end of the message has been achieved
              SMP.Complete = 1; 
              Comms_CheckReceivedMessage(SMP.CWG);                            
            }            
          }
      }
      else // FINDING PREAMBLE 
      {
        // Now Start to check for 0xFF 0xFF 0x42
        SMP.preamblebyte[2] = (SMP.preamblebyte[2] <<1) | (SMP.preamblebyte[1] >>7);
        SMP.preamblebyte[1] = (SMP.preamblebyte[1] <<1) | (SMP.preamblebyte[0] >>7);
        SMP.preamblebyte[0] = (SMP.preamblebyte[0] <<1) | (SMP.bit);
  
        // Now check if we can find the preamble
        if(SMP.preamblebyte[2] == 0xFF)
          if(SMP.preamblebyte[1] == 0xFF)
            if(SMP.preamblebyte[0] == 0x42)
            {
              SMP.Preamblefound = 1;              
              SMP.bit_bitmask =0x80;
              SMP.bitbuf_index = 0;
              SMP.CWG->bitbuf[SMP.bitbuf_index] = 0x00;  
              
              // The next bits comming in are going to be the actual payload
                            
            }
        // Check for the maximum amount of bits, if no preamble is found then go back to checking
        // for sync
        if(++SMP.preamble_cnt > READER_PREAMBLE_LEN*8){          
          SMP.Sync = 0;
          SMP.preamble_cnt = 0;
        }
      }               
    }   	
  }  
  // Keep track of where the greatest bit power is produced, 
  // If this matches for N bits, sync has been achieved.
  if(SMP.Ones_cnt > SMP.Ones_cmp){
    SMP.Ones_cmp = SMP.Ones_cnt;
    SMP.Ones_cmp_sample = SMP.Sample;
  }
	
#ifdef COMMS_8SAMPLES   
  if(++SMP.Sample > 7)
#endif
#ifdef COMMS_4SAMPLES   
  if(++SMP.Sample > 3)
#endif  
#ifdef COMMS_2SAMPLES   
  if(++SMP.Sample > 1)
#endif  
  {           
    // Check if it is the same sample number that gives the maximum output power   
    if(SMP.Sync == 0)
    {
      //LED_OFF_GREEN;        
      //#define CWG_ONES_SYNC_THRESH 4
      //if((SMP.Ones_cmp_sample == SMP.Ones_cmp_sample_previous) && (SMP.Ones_cnt >CWG_ONES_SYNC_THRESH))
      if((SMP.Ones_cmp_sample == SMP.Ones_cmp_sample_previous))      
      {
        //DEBUG0_1;
        #ifdef COMMS_8SAMPLES         
          #define SYNC_THRESHOLD 8
        #endif
        #ifdef COMMS_4SAMPLES         
          #define SYNC_THRESHOLD 15
        #endif        
        #ifdef COMMS_2SAMPLES         
          #define SYNC_THRESHOLD 25
        #endif        
        
        if(++SMP.Sync_cnt == SYNC_THRESHOLD)
        {
            //DEBUG0_1;                          
            SMP.preamble_cnt = 0;
            SMP.Sync = 1;
            SMP.preamblebyte[2] = 0xFF;
            SMP.preamblebyte[1] = 0xFF;
            SMP.preamblebyte[0] = 0xFF;
            
            SMP.SyncPos = SMP.Ones_cmp_sample;  
        }
      }
      else
      {
        SMP.Sync_cnt = 0;
      }
      SMP.Ones_cmp_previous = SMP.Ones_cmp;
      SMP.Ones_cmp_sample_previous = SMP.Ones_cmp_sample;                 
      SMP.Ones_cmp = 0x00;
      SMP.Ones_cmp_sample = 0;            
    }
     SMP.Sample  = 0;  
  }  

  
  if(SMP.Complete)
    return(CWG_COMPLETE);
  else if(SMP.Preamblefound)
    return(CWG_PREAMBLE);
  else if (SMP.Sync)
    return(CWG_SYNC);
  else
    return(CWG_NONE);
}

*/

//***************************************************************************
// Comms_BlastPostProcess(void)
// Post process the bits that were captured from the air if SYnc was detected
//*************************************************************************** 
/*
void Comms_BlastPostProcess(void)
{ 	
sCWG *CWG;

  debug_nn_rom(PSTR("*"),dbg.cwg_CWGPROGRESS);    
  // Get the next CWG to process
  CWG = Comms_GetNextRdCWG();
  //debug_u16(PSTR("CWGrd= 0x"),(u16)(CWG),1);
  Comms_CheckReceivedMessage(CWG);

}
*/


//***************************************************************************
// Comms_CheckReceivedMessage();
// Check the message size, and CRC, and if valid copy to the
// find the preable
//*************************************************************************** 

void Comms_CheckReceivedMessage(sCWG *CWG)
{
u16 crc,reccrc;
u08 crc_h,crc_l,reccrc_h,reccrc_l;
//u08 i;

    // The first byte is the length
    CWG->ReceivedMsgLen = CWG->bitbuf[0];
    #ifdef DEBUG_RECEIVEDMSG
      rprintf_rom(PSTR("["));
      rprintf(&CWG->bitbuf[0]);     
      rprintf_rom(PSTR("]\r\n"));        
      debug_buffer_hex(&CWG->bitbuf[0],CWG->ReceivedMsgLen+4);
    #endif
  
  
  // Calculate the CRC
  crc = Comms_CRC(&CWG->bitbuf[0],CWG->ReceivedMsgLen+1);   // include the message length in the calc
  crc_h = (u08)((crc >>8) & 0x00ff); 
  crc_l = (u08)(crc & 0x00FF);  
  reccrc_h = CWG->bitbuf[1+CWG->ReceivedMsgLen];
  reccrc_l = CWG->bitbuf[2+CWG->ReceivedMsgLen];
  reccrc = (reccrc_h<<8) +reccrc_l;
      
  if(( reccrc_h == crc_h) && ( reccrc_l == crc_l))
  {
    //debug_nn_rom(PSTR("~"),dbg.cwg_CWGPROGRESS);
    CWG->CRCPass = 1;
    Comms_AddRxMsg(&CWG->bitbuf[1],CWG->ReceivedMsgLen,CWG->StartRSSI);	
  }
  else{  
		//debug_nn_rom(PSTR("!~"),dbg.cwg_CWGPROGRESS);
    #define DEBUG_CRC
    #ifdef DEBUG_CRC
      //debug_u16(PSTR("Calc Crc = "),crc,1);
      //debug_u16(PSTR("Recv Crc = "),reccrc,1);
    #endif    
  }       
}


u08 one;

//*************************************************
// u08 Comms_Rxbit(void)
// Read the received bit
//*************************************************
u08 Comms_Rxbit(void)
{
  if ((COMMS_RX_PORT & (1 << COMMS_RX_PIN)) == (1<<COMMS_RX_PIN))
  {
    return(1);
  }
  else
  {
    return(0);
  }
}


//*************************************************
// Comms_Txbit(u08)
// Blast the bit into EtherWaves
//*************************************************
void Comms_Txbit(u08 bit)
{
  if(bit)
  {
	#ifdef DEBUG_TX_BITS
		rprintfChar('1');
    #endif
    COMMS_TX_PORT |= (1<< COMMS_TX_PIN); 	
  }
  else
  {
	#ifdef DEBUG_TX_BITS	
		rprintfChar('0');  
	#endif
    COMMS_TX_PORT &= ~(1<< COMMS_TX_PIN); 
  }        
  
}


//u08 gTXTestbit;
//u08 one;


//**************************************************
// Comms ClearRSSIbuf
//**************************************************
void Comms_ClearRSSIbuf(void)
{
u08 i;

  for(i=0;i<RSSIbuf_SIZE; i++)
    RX.RSSIbuf[i] = 0x01FF;
    
  RX.RSSIbuf_tailptr = 0;
  RX.RSSI = Adc_ReadRSSI_u08(); 
  // Initialise the Noise Accumulator with an RSSI reading
  RX.NoiseAccum = 0; 
}




//*************************************************
// Create a Data Packet from the Message
// Includes the Preable, Payload Length, Payload, and CRC
// Returns : Packet length in bytes
//*************************************************
/*
u08 Comms_SendWakeMsg(void)
{
u08 i;
u08 *pkt;

  pkt = &TX.pktbuf[0];  
  
  // Add the preamble
  for(i=0;i<PREAMBLE_LEN;i++){
	*pkt = PREAMBLE_BYTE; pkt++;
  }
	
  *pkt = MAGIC_BYTE; pkt++;  
  
  // Check if the Q is ready 
  if(Modem.TXBusy)
  {
    debug_rom(PSTR("ERROR: Comms_SendWakeMsg() TX is busy"),1);
	return(FAIL);
  }
  else{  
  	TX.Tx_cnt = 0;
   	Modem.StartTx_flg = 1;
    //Modem.TXBusy = 0;
    TX.bitmask = 0x80;
    TX.bytecnt = PREAMBLE_LEN + 1;
    TX.pktbuf_ptr = 0;    
    TX.txbyte = TX.pktbuf[0];  
    return(TRUE);
  }  
 
}
*/

  


//*************************************************
// Add the message to the Q for transmission
// over the air
// 
//*************************************************
void Comms_SendMsg(u08 *msg, u08 len)
{
   #ifdef COMMS_DEBUG_SENDMSG
		rprintf_rom(PSTR("SendMsg len = "));
		rprintfu08(len);
		rprintf_rom(PSTR(" "));
		rprintf(msg);
		rprintf_rom(PSTR("\r\n"));
   #endif

  TX.pktbuf_len = Comms_CreatePkt(msg,len,&TX.pktbuf[0]);
  // The manchester encoding occurs during the interrupt 
  // that transmits the bits over the air

  Comms_SendPkt();  // Sends the packet in TX.pktbuf   
  // ***********************************************************
  // NOTE INTRODUCE A COUNTER THAT FLAGS AN ERROR HERE PLEASE
  // REBOOT POSSIBLY TO AVOID LOCK UP
  // ***********************************************************
  while((volatile u08)Modem.StartTx_flg);
    //rprintf("*");
  while((volatile u08)Modem.TXBusy);
    //rprintf("&");

  
  // ************************************* 
  
  //rprintf("Here\r\n");
}



//*************************************************
// Start the transmission of the pkt in  TX.pktbuf 
// Returns : OK if transmission could be started
// elese returns FAIL
//*************************************************
u08 Comms_SendPkt()
{
//u08 i;

//#define COMMS_DEBUG_SENDPKT
#ifdef COMMS_DEBUG_SENDPKT
  rprintf_rom(PSTR("TX.pktbuf_len = "));
  rprintfu08(TX.pktbuf_len);
  rprintf_rom(PSTR(" TX.pktbuf = "));
  rprintf(&TX.pktbuf[0]);
  rprintf_rom(PSTR("\r\n"));
#endif  


  // Check That the modem is not busy with a transmission
  if((volatile u08)Modem.TXBusy)
  {
    rprintf_rom(PSTR("ERROR: Comms_SendPkt() TX is busy"));
	return(FAIL);
    //Led_on(RED_LED);
  }
  else{  
  	TX.Tx_cnt = 0;
   	Modem.StartTx_flg = 1;
    TX.Complete = 0;
    TX.bitmask = 0x80;
    TX.bytecnt = TX.pktbuf_len;
    TX.pktbuf_ptr = 0;    
    TX.txbyte = TX.pktbuf[0];  
  }  
  return(OK);
}
 
//*************************************************
// Create a Data Packet from the Message
// Includes the Preable, Payload Length, Payload, and CRC
// Returns : Packet length in bytes
//*************************************************
u08 Comms_CreatePkt(u08 *msg, u08 len,u08 *pkt)
{
u08 i;
u16 crc;
u08 *payload_ptr;
u08   crc_h,crc_l;  
u08 *tmp_ptr;

  tmp_ptr = pkt;
  // Add the preamble
  for(i=0;i<PREAMBLE_LEN;i++){
	*pkt = PREAMBLE_BYTE; pkt++;
  }
	
  //*pkt = ENDPREAMBLE_BYTE; pkt++;  
  *pkt = MAGIC_BYTE; pkt++;  
  
  
  // Payload Length
  *pkt = len;
  payload_ptr = pkt;
  pkt++;
    
  // Copy the message into the packet payload
  for(i=0;i<len;i++)
  {
    *pkt = *msg;
    pkt++; msg++;
  }

  // Calculate the CRC
  crc = Comms_CRC(payload_ptr,len+1);   // include the message length in the calc

  crc_h = (u08)((crc >>8) & 0x00ff); 
  crc_l = (u08)(crc & 0x00FF);  

/*
  #ifdef DEBUG_CRC
    rprintf_rom(PSTR("\r\n[Calulated Crc = "));
    rprintfu08(crc_h );        
    rprintfu08(crc_l );    
	rprintf_rom(PSTR("]\r\n"));
  #endif
*/  

  *pkt = (u08)((crc >>8) & 0x00ff);  pkt++;
  *pkt = (u08)(crc & 0x00FF); pkt++;
  
  *pkt = 0x42;  // For glory at the end 

  //#define DEBUG_CREATEPKT
  #ifdef DEBUG_CREATEPKT
	debug_buffer_hex(tmp_ptr,len +PREAMBLE_LEN+5);	
  #endif  
  
  
  return(len +CWG_PKT_HEADER_LEN); // return the constructed packet length  
}  
  




u16 gCount,gCount1;
//*************************************************
// Comms_CheckSignal(void)
// Check if there is an RF signal present
// 
//*************************************************
u08 Comms_CheckSignal(void)
{
u08 i;

  // Calculate the Noise floor, 
  RX.NoiseAccum = 0;
  for(i= 0;i<RSSIbuf_SIZE ;i++)
    RX.NoiseAccum += RX.RSSIbuf[i];
        
  RX.NoiseFloor = RX.NoiseAccum >> RSSIbuf_SIZE_POW2; 
  RX.RSSI = Adc_ReadRSSI_u08();     
  
    
  // Any signal that is too far over the noise floor do not accumulate  
	// Accumulate and calculate the Noise floor average  
	//if ((RX.RSSI< (RX.NoiseFloor + NOISEACCUM_FACTOR)))	
	{
		RX.RSSIbuf[RX.RSSIbuf_tailptr] = RX.RSSI;   
		RX.RSSIbuf_tailptr += 1;  
		if(RX.RSSIbuf_tailptr >(RSSIbuf_SIZE-1))
			RX.RSSIbuf_tailptr = 0;  
	}
  
  //#define COMMS_DEBUG_RSSI
  #ifdef COMMS_DEBUG_RSSI
    rprintf_rom(PSTR("\r\nComms_CheckSignal() : NoiseFloor = "));
    print_decimal_u16(RX.NoiseFloor);       	
    rprintf_rom(PSTR(" RSSI = "));    
    print_decimal_u16(RX.RSSI);   
    rprintf_rom(PSTR("\r\n"));
  #endif

  //***************************************************//
  if ((RX.RSSI> (RX.NoiseFloor + CommsData.rxLevel_u08)))
  { 
    // Save the RSSI value for comparison later 
    // against the RSSI while receiving the message.
    //RX.RSSIInitRx = RX.RSSI - RSSIPKT_FACTOR;
    RX.RSSIInitRx = RX.RSSI;
    
    return(1);
  }
  else
  {
    return(0);        
  }
}



//*************************************************
// Calculate CRC 
// CRC is calculated over the packet payload
// including the length byte
//*************************************************

//internal use only - puts a byte
void Comms_PutCRC(u08 b)
{    
   unsigned char i;
   CRC_buffer.Part.Data = b;
   for (i=0; i<8; i++)
   {
       CRC_buffer.Whole = CRC_buffer.Whole << 1;
       if (CRC_buffer.Part.Head & 0x01)
          CRC_buffer.Part.Remainder ^= POLYNOMIAL;
  };
}


//call this routine with your own data buffer
// yes! it's really that simple!
u16 Comms_CRC (u08* Data,u08 Length)
{
   CRC_buffer.Part.Remainder = INITIAL_VALUE;
   while (Length--  >  0)
      Comms_PutCRC(*Data++);
   Comms_PutCRC(0);
   Comms_PutCRC(0);
   return CRC_buffer.Part.Remainder;
} 

*/




