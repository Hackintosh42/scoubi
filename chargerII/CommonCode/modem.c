  
#define MODEM_C
#include "modem.h"
#include "network.h"



// PACKET STRUCTURE LOOKS AS FOLLOWS
//                                                      bitbuf[0]    1     2      3      4       5        6               bitbuf[PACKET LEN+1]  
// [PREAMBLE][PREAMBLE][PREAMBLE][PREAMBLE][MAGIC 0x42][PACKET LEN][LEN][SRC_H][SRC_L][DEST_H][DEST_L][NETWORK ID][][][][][CRC H][CRC L]
//*************************************************************************** 


//******************************************
// Timer0 Overflow interrupt
// Use to power the Modem state machionery
//******************************************
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW0)
{
  u08 NewValue,OldValue;

  // The new value is read in as soon as it enters the interrupt
  // routine to try eliminate jitter as much as possible   
  NewValue = MODEM_RXBIT;

  // *********** Temporary for Debug **************
  //DEBUG1_1;    

/* 
  if(NewValue)
    DEBUG0_1;    
  else
    DEBUG0_0;
*/    
    
  switch(Modem.State)
  {
		case MODEM_START_TX:
      LED_ON_RED;            
			Modem.TXBusy = 1;
      Modem.StartTx_flg = 0;          
      Modem.RXBusy = 0;            
			Mel_Mode(MEL_TX);
      MODEM_TXBIT_1;
      Modem.NextState = MODEM_TX_VCOWAIT;  
      Modem.VCO_cnt = 16;
      // Make sure that the data_in pin to the melexis is an output
      MELEXIS_DATA_IN_DDR |= (1<<MELEXIS_DATA_IN_PORTPIN);  
      // reset the ack received variable
      Modem.AckTimer = 0;      
      Modem.AckReceived = 0;
		break;
    
    case MODEM_TX_VCOWAIT:
      // WAIT for the PLL to Lock
      if((--Modem.VCO_cnt) == 0)
        Modem.NextState = MODEM_TX_BITS;    
    break;
    

    case MODEM_TX_BITS:     
      if (((volatile u08)Modem.sample_cnt == 0) || ((volatile u08)Modem.sample_cnt == (MODEM_SAMPLE_CNT_TOP/2)))
      {              
             if(TX.txbyte & TX.bitmask)
             {
               if(Modem.sample_cnt == 0)	
                  MODEM_TXBIT_1;               
               else
                  MODEM_TXBIT_0;                              
             }
             else
             {
               if(Modem.sample_cnt == 0)		   
                  MODEM_TXBIT_0;                                             
               else
                  MODEM_TXBIT_1;                              
             }
      }
          //---------------------------------------------//
          // Load the next byte into the buffer if 
          // it is time to transmit the next bit
          //---------------------------------------------//       
         if(Modem.sample_cnt == (MODEM_SAMPLE_CNT_TOP-1))		           
         {        
                TX.bitmask >>=1;         
                // Preload the registers with the next bit to send
                if(TX.bitmask == 0)
                { 
                  // Check if that was the last bit to transmit
                  if(TX.bytecnt == 0)
                  {
                    MODEM_TXBIT_1;
                    Modem.NextState = MODEM_IDLE;
                  }
                  else
                  {
                    TX.bitmask = 0x80;
                    TX.bytecnt--;        
                    TX.pktbuf_ptr++;        
                    TX.txbyte = TX.pktbuf[TX.pktbuf_ptr];	
                    //rprintfu08(TX.txbyte);
                  }
                }
          }              		
  		Modem.TXBusy = 1;
      Modem.RXBusy = 0;      
    break;
    
    case MODEM_TX_FF:
      if((volatile u08)Mel.Mode != MEL_TX)
         Mel_Mode(MEL_TX);
         
      if(Modem.CW)
         MODEM_TXBIT_1;
      else
      {      
        if ((volatile u08)Modem.sample_cnt == 0) 
          MODEM_TXBIT_1;     
        if(Modem.sample_cnt == (MODEM_SAMPLE_CNT_TOP/2))        
          MODEM_TXBIT_0;        
      }
      
      if((volatile u08)Modem.Switch_IDLE_flg)
      {
        //Mel_Mode(MEL_RX);	                 // Change this to standby or better still remove
        Modem.Switch_IDLE_flg =0;
        Modem.NextState = MODEM_IDLE;                                
      }
    break;


		case MODEM_RX: 
      //#define MODEM_DEBUG_RX_TOGGLEPIN
      #ifdef MODEM_DEBUG_RX_TOGGLEPIN
        DEBUG2_1;
      #endif

      Modem.RXBusy = 1;      
      Modem.TXBusy = 0;      
      //DEBUG0_1;
      //if((volatile u08)Mel.Mode != MEL_RX)
      //  Mel_Mode(MEL_RX);	           
               
      SMP.ShiftReg=SMP.ShiftReg>>1 | ((NewValue==0)?0x00:MODEM_SHIFTREG_MSB);
      // Invert new middle bit (Manchester coding) 
      SMP.ShiftReg^=MODEM_SHIFTREG_XOR;
        
 
//#define MODEM_WEIGHTED_SAMPLE_METHOD 
#ifdef MODEM_WEIGHTED_SAMPLE_METHOD  

          #ifdef MODEM_8SAMPLES  
            #define CWG_ONES_UPPER_THRESH 4    
          #endif    
          #ifdef MODEM_4SAMPLES 
              no no no
          #endif              

          // Calculated the Accumulated Value in A
          SMP.Shifty = (SMP.ShiftReg &0xF0) >>4;
          SMP.Shifty += (SMP.ShiftReg &0x0F);
          SMP.Ones_cnt = SMP.Shifty;                   
          
          
/*
          SMP.Shifty = SMP.ShiftReg;
          #define SAMPLE_WEIGHT 1
          SMP.Ones_cnt = SMP.Shifty &0x01;       
          SMP.Shifty = SMP.Shifty>>1;
          SMP.Ones_cnt += (SMP.Shifty &0x01);//<<SAMPLE_WEIGHT;
          SMP.Shifty = SMP.Shifty>>1; 
          SMP.Ones_cnt += (SMP.Shifty &0x01);//<<SAMPLE_WEIGHT;
          SMP.Shifty = SMP.Shifty>>1;  
          SMP.Ones_cnt += (SMP.Shifty &0x01); 
          SMP.Shifty = SMP.Shifty>>1; 
                   
          SMP.Ones_cnt += (SMP.Shifty &0x01); 
          SMP.Shifty = SMP.Shifty>>1; 
          SMP.Ones_cnt += (SMP.Shifty &0x01); //<<SAMPLE_WEIGHT; 
          SMP.Shifty = SMP.Shifty>>1; 
          SMP.Ones_cnt += (SMP.Shifty &0x01); // <<SAMPLE_WEIGHT; 
          SMP.Shifty = SMP.Shifty>>1; 
          SMP.Ones_cnt += (SMP.Shifty &0x01);                   
*/          
          
#else         
          OldValue=SMP.ShiftReg&0x01;    
                                     
          // Update A according to values shifted in and out 
          // (We are really counting the number of 1's in the shift register) 
          if ((OldValue==1)&&(NewValue==0))
          {
            if(SMP.Ones_cnt > 0)
           SMP.Ones_cnt--;
          }
          if ((OldValue==0)&&(NewValue==1))
          { 
            if(SMP.Ones_cnt < MODEM_SAMPLE_CNT_TOP)		
            SMP.Ones_cnt++;    
          }         
          if ((SMP.ShiftReg&MODEM_SHIFTREG_XOR)==0){
            if(SMP.Ones_cnt > 0)  
            SMP.Ones_cnt--;
          }
          else{
            if(SMP.Ones_cnt < MODEM_SAMPLE_CNT_TOP)		  
            SMP.Ones_cnt++;
          }          
#endif          
                                                       
          if(SMP.Sync)
          {   
            // Time to get the bit out of the shift register
            //if(SMP.Sample == SMP.SyncPos) 
            if(Modem.sample_cnt == SMP.SyncPos)            
            {              
              if(SMP.Ones_cnt >= CWG_ONES_UPPER_THRESH)
                SMP.bit = 0x01;      
              else
                SMP.bit = 0x00;      
              // Clock the bits into the buffer
              if(SMP.Preamblefound)
              {     
                  // Store the Bit in the buffer
                  if(SMP.bit)
                    SMP.CWG->bitbuf[SMP.bitbuf_index] |= SMP.bit_bitmask;
                    
                  // Update the bitmask
                  SMP.bit_bitmask = (SMP.bit_bitmask>>1);    
                  if(SMP.bit_bitmask == 0)
                  {
                    // The length is now in the first byte in the bitbuf
                    if(SMP.bitbuf_index == 0x00)
                    {
                      SMP.len = SMP.CWG->bitbuf[0];
                      //Modem_CRC_Reset();                      
                    }
                  
                    // *******************
                    // Calculate the CRC
                    // *******************
                    //if(SMP.bitbuf_index < (SMP.len))
                      //Modem_PutCRC(SMP.CWG->bitbuf[SMP.bitbuf_index]);
                      
                  
                    SMP.bit_bitmask =0x80;
                    SMP.bitbuf_index++;
                    SMP.CWG->bitbuf[SMP.bitbuf_index] = 0x00;                         
                    if((SMP.bitbuf_index > SMP.len+4) ||(SMP.bitbuf_index > (MAX_RX_BITBUF_SIZE-1)))            
                    //if(SMP.bitbuf_index > (MAX_RX_BITBUF_SIZE-1))                        
                    {
                      // *********************************************
                      // ******* Check the Packet that was received
                      // *********************************************
                      u16 crc;
                      u08 crc_h,crc_l,reccrc_h,reccrc_l;
                      
                      // The first byte is the packet length
                      SMP.CWG->ReceivedPacketLen = SMP.CWG->bitbuf[0]; 
                          
                          //#define DEBUG_RECEIVEDPACKET
                          #ifdef DEBUG_RECEIVEDPACKET
                            rprintf_rom(PSTR("rxPacketLen = "));
                            rprintfu08(SMP.CWG->ReceivedPacketLen);
                            rprintf_rom(PSTR("\r\n"));
                            u08 i;
                            for(i=0;i<(SMP.CWG->ReceivedPacketLen+3);i++)
                              rprintfu08(SMP.CWG->bitbuf[i]);     
                            rprintf_rom(PSTR("\r\n"));                          
                            for(i=0;i<(SMP.CWG->ReceivedPacketLen+3);i++)
                              rprintfChar(SMP.CWG->bitbuf[i]);     
                            rprintf_rom(PSTR("\r\n"));                                                    
                          #endif
                          
                       
                        // Calculate the CRC
                        crc = Modem_CRC(&SMP.CWG->bitbuf[0],SMP.CWG->ReceivedPacketLen+1);   // include the Packet length in the calc
                        
                        // ************* CRC Result calculated over the packet ******
                        // **********************************************************
                        //crc = Modem_CRC_GetResult();
                        
                        crc_h = (u08)((crc >>8) & 0x00ff); 
                        crc_l = (u08)(crc & 0x00FF);  
                        reccrc_h = SMP.CWG->bitbuf[1+SMP.CWG->ReceivedPacketLen];
                        reccrc_l = SMP.CWG->bitbuf[2+SMP.CWG->ReceivedPacketLen];
                            
                        if(( reccrc_h == crc_h) && ( reccrc_l == crc_l))
                        {
                          //SMP.CWG->CRCPass = 1;                          
                          // **************************************************************
                          // *** Note this is bit of a hack because the Address filtering
                          // *** is actually a network layer function but this saves memory
                          // *** and MIP's ie battery
                          //***************************************************************
                          #define MODEM_ADDRESS_FILTERING
                          #ifdef MODEM_ADDRESS_FILTERING
                            // Check the destination address is for here or a broadcast    
                            if( ((SMP.CWG->bitbuf[4] == 0xFF)  && (SMP.CWG->bitbuf[5] == 0xFF)) 
                                || ( (SMP.CWG->bitbuf[4] == (u08)(Net.LocalAddr>> 8)) && (SMP.CWG->bitbuf[5] == (u08)Net.LocalAddr)) 
                                || (Modem.Promiscuous) )
                            {
                              Modem_AddRxMsg(&SMP.CWG->bitbuf[1],SMP.CWG->ReceivedPacketLen,SMP.CWG->StartRSSI);	        
                            }
                          #else    
                            Modem_AddRxMsg(&SMP.CWG->bitbuf[1],SMP.CWG->ReceivedPacketLen,SMP.CWG->StartRSSI);      
                          #endif                      
                        }
                        else{  
                          //rprintf("{CRC}\r\n");                        
                          rprintfChar('~');                                                  
                          //#define DEBUG_CRC
                          #ifdef DEBUG_CRC
                            rprintf_rom(PSTR("Calc Crc = "));rprintfu08(crc_h);rprintfu08(crc_l);  
                            rprintfChar(' ');
                            //rprintf_rom(PSTR("\r\nRecv Crc = "));
                            //rprintfu08(reccrc_h);
                            //rprintfu08(reccrc_l);                            
                            rprintfu08(crc_h);
                            rprintfu08(crc_l);                                                        
                            //rprintf_rom(PSTR("\r\n{")); 
                            
                            u08 jj;                                         
                            for(jj = 0;jj<SMP.CWG->ReceivedPacketLen;jj++)
                            {
                              rprintfu08(SMP.CWG->bitbuf[jj]);
                              rprintfChar(' ');
                            }
                            rprintf("}\r\n");          
                            
                          #endif    
                        }   
                        
                        // ********************************************
                        // A message was received, restart the RX timer
                        
                        if(Modem.NSamples >0)
                          Modem.NSamples = Modem.Start_NSample_Value;
                                                                                                           
                      // ****************************************                      
                      // **** Clear the variables 
                      // ***************************************
                      Modem_BlastStart();
                    }            
                  }
              }
              else // FINDING PREAMBLE 
              {
                // Clock what we consider the Preamble out on a debug pin
                #define DEBUG_PREAMBLE
                #ifdef DEBUG_PREAMBLE
                  if(SMP.bit)
                  {
                    DEBUG2_1;
                  }
                  else
                  {
                    DEBUG2_0;
                  }
                #endif
                
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
                if(++SMP.preamble_cnt > PREAMBLE_END_SYNC_MAXBITS)
                {      
                  SMP.Preamblefound = 0;                                                  
                  SMP.Sync = 0;
                  SMP.preamble_cnt = 0;
                  
                  #ifdef DEBUG_PREAMBLE
                    DEBUG2_0;
                  #endif
                  
                  // ***** DEbuggin
                  rprintfChar('*');
                }
              }               
            }   	
          }  
          // Keep track of where the greatest bit power is produced, 
          // If this matches for N bits, sync has been achieved.
          if(SMP.Ones_cnt > SMP.Ones_cmp){
            SMP.Ones_cmp = SMP.Ones_cnt;
            //SMP.Ones_cmp_sample = SMP.Sample;
            SMP.Ones_cmp_sample = Modem.sample_cnt;           
          }
      
          //if(++SMP.Sample >= MODEM_SAMPLE_CNT_TOP)        
          if(Modem.sample_cnt >= (MODEM_SAMPLE_CNT_TOP-1))
          {           
            // Check if it is the same sample number that gives the maximum output power   
            if(SMP.Sync == 0)
            {
              //DEBUG0_1;                
              if((SMP.Ones_cmp_sample == SMP.Ones_cmp_sample_previous))      
              {                                
                if(++SMP.Sync_cnt == MODEM_SYNC_THRESHOLD)
                {
                    SMP.preamble_cnt = 0;
                    SMP.Sync = 1;
                    Modem.WakeSignalfound_flg = 1;                    
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
             //SMP.Sample  = 0;  
          }          
        

              //if(SMP.Complete)
              //    Modem_BlastStart();    
              //else
              {
                  // ***** Switch back to the base modem mode *****
                  if((volatile u08)Modem.BaseMode == MODEM_IDLE)
                     Modem.NextState = MODEM_IDLE;          
              
                  if ((volatile u08)Modem.StartTx_flg == 1)
                  {  
                      Modem.NextState = MODEM_START_TX;
                  }          
                  else if((volatile u08)Modem.Switch_IDLE_flg)
                  {
                    Modem.Switch_IDLE_flg =0;
                    Modem.NextState = MODEM_IDLE;                          
                  }          
                  else if((volatile u08)Modem.CheckNSamples_flg)
                  {
                     Modem.NextState = MODEM_RX;          
                  
                    if(--Modem.NSamples == 0)
                    {
                      Modem.CheckNSamples_flg =0;
                      if(Modem.BaseMode == MODEM_IDLE)
                        Modem.NextState = MODEM_IDLE;            
                    }
                  }    
                  else if((volatile u08)Modem.Switch_TX_FF_flg)
                  {
                    Modem.Switch_TX_FF_flg = 0;
                    Modem.NextState = MODEM_TX_FF;                          
                  }
              }                          
    break;

		
		case MODEM_IDLE:	
      //#define MODEM_DEBUG_IDLE_TOGGLEPIN
      #ifdef MODEM_DEBUG_IDLE_TOGGLEPIN
        DEBUG2_1;
      #endif
      
      Modem.TXBusy = 0;
      Modem.RXBusy = 0;
      /*
      if((volatile u08)Mel.Mode != MEL_STANDBY)
        Mel_Mode(MEL_STANDBY);	      
      */
      LED_OFF_RED;

      //if((volatile u08)Mel.Mode != MEL_RX)
      //  Mel_Mode(MEL_RX);	      

        // ***** Switch back to the base modem mode *****
        if((volatile u08)Modem.BaseMode == MODEM_RX)
        {
           if((volatile u08)Mel.Mode != MEL_RX)
            Mel_Mode(MEL_RX);        
           Modem.NextState = MODEM_RX; 
        }
        if((volatile u08)Modem.BaseMode == MODEM_IDLE)
        {
           if((volatile u08)Mel.Mode != MEL_STANDBY) 
            Mel_Mode(MEL_STANDBY);        
           Modem.NextState = MODEM_IDLE; 
        }         
         
        
        if((volatile u08)Modem.Switch_RX_flg)
        {          
          Modem.Switch_RX_flg = 0;
          Mel_Mode(MEL_RX);         
          Modem_BlastStart();
          Modem.NextState = MODEM_RX;                
        }
        else if ((volatile u08)Modem.StartTx_flg == 1)
        {            
          Modem.NextState = MODEM_START_TX;
        }        
        else if((volatile u08)Modem.Switch_TX_FF_flg)
        {
          Modem.Switch_TX_FF_flg = 0;
          Modem.NextState = MODEM_TX_FF;                          
        }
		break;
     
	
    default:
    break;
  }

  if(++Modem.sample_cnt >= MODEM_SAMPLE_CNT_TOP)
  {
    Modem.sample_cnt = 0;   
    RxSerial.LastRxSerial_cnt++;
    Modem.AckTimer++;    
  }	
   Modem.State = Modem.NextState;		
   
   // ********************************
   // *** Modem tick counter *********
   // Interrupt is run ever 31 uS
   // ********************************
   
   if(++Modem.tick_low == 0)
   {
      if(++Modem.tick_high == BackGroundTimer.stoptick_high)
      {
        BackGroundTimer.timeout_flg =1;
      }
   }
   
    
  #ifdef MODEM_DEBUG_RX_TOGGLEPIN
    DEBUG2_0;
  #endif

}


//**************************************************
// Delay calculated off the Modem timer
// Resulution is in ms
// Returns control once time is up
//**************************************************
void Modem_Timer_Wait_ms(u16 delaytime)
{
u16 starttick;
u16 endtick;
u08 stoptick_high, stoptick_low;

  starttick = ((volatile u08)Modem.tick_high<<8) | (volatile u08)Modem.tick_low;

  //#define DEBUG_MODEMTIMER  
  #ifdef DEBUG_MODEMTIMER  
    rprintf_rom(PSTR("startmodemtick = "));
    //rprintfu08(starttick_wrap);
    rprintfu16(starttick);
  #endif
  
  // One tick is 31.875uS
  // therefore 1ms = 31.37 ticks
  // round to 32
  // calculate when the delay time is up
  // Wraps around
  
  endtick = (delaytime <<5) + starttick;
  
  stoptick_low = endtick &0x0FF;
  stoptick_high = endtick>>8;
  
  while(stoptick_high != (volatile u08)Modem.tick_high)
  {
    //rprintfChar('*');
  }

  #ifdef DEBUG_MODEMTIMER      
    rprintf_rom(PSTR("\r\nBackGroundTimer.stoptick_high = "));  
    rprintfu08(BackGroundTimer.stoptick_high);
    rprintf_rom(PSTR("\r\nBackGroundTimer.stoptick_low = "));  
    rprintfu08(BackGroundTimer.stoptick_low);
  #endif  
}



//**************************************************
// Delay calculated off the Modem timer
// Resulution is in ms
// Tasks sets the BackGroundTimer.timeout_flg when 
// it times out
//**************************************************
void Modem_StartBackground_Timer_ms(u16 delaytime)
{
u16 starttick;
u16 endtick;
//u08 starttick_wrap;

  BackGroundTimer.timeout_flg =0;


  starttick = ((volatile u08)Modem.tick_high<<8) | (volatile u08)Modem.tick_low;

  //#define DEBUG_BACKGROUNDTIMER  
  #ifdef DEBUG_BACKGROUNDTIMER
    rprintf_rom(PSTR("startmodemtick = "));
    //rprintfu08(starttick_wrap);
    rprintfu16(starttick);
  #endif
  
  // One tick is 31.875uS
  // therefore 1ms = 31.37 ticks
  // round to 32
  // calculate when the delay time is up
  // Wraps around
  
  endtick = (delaytime <<5) + starttick;
  
  BackGroundTimer.stoptick_low = endtick &0x0FF;
  BackGroundTimer.stoptick_high = endtick>>8;

  #ifdef DEBUG_BACKGROUNDTIMER
    //rprintf_rom(PSTR("\r\nBackGroundTimer.stoptick_wrap = "));
    //rprintfu08(BackGroundTimer.stoptick_wrap);
    
    rprintf_rom(PSTR("\r\nBackGroundTimer.stoptick_high = "));  
    rprintfu08(BackGroundTimer.stoptick_high);
    rprintf_rom(PSTR("\r\nBackGroundTimer.stoptick_low = "));  
    rprintfu08(BackGroundTimer.stoptick_low);
  #endif
  
}




//**************************************************
// Modem Transmitter
//**************************************************
void Modem_Init(void)
{

	// Clear the RSSI averager
//	Comms_ClearRSSIbuf();
	Modem.State = MODEM_IDLE;
	Modem.NextState = MODEM_IDLE;
  Modem_BlastStart(); 
  SMP.CWG = &CWG;
  
  // Initialise the Melexis RF TH7122
  Mel_Init();   
  Modem_Set_BaseMode(MODEM_RX);  
  //Modem.DestAddr[0] = Modem.DestAddr[0] = 0xFF;      // Start off in broadcast mode
  //Modem.LocalAddr[0] = Modem.LocalAddr[0] = 0x00;    // 0x0000 is reserved, hence no numbers here
  Modem_SetTxLevel(0);  
  
  Modem.Enable = 1;
  Modem.SamplerEnabled = 1;
	Init_timer0();	// Enable the timer that handles the Modem  
  MODEM_TIMER_ENABLE;
}




//**************************************************
// Modem_Switch 
// Used to switch between IDLE and RX modes
//**************************************************
void Modem_Receive_NSamples(u16 samples)
{  
  Modem.WakeSignalfound_flg = 0;
  Modem.CheckNSamples_flg = 1;
  Modem.NSamples = samples;
  Modem.Start_NSample_Value  = samples;
  Modem_Switch(MODEM_RX);
}


//**************************************************
// Modem_Switch 
// Used to switch between IDLE and RX modes
//**************************************************
void Modem_Switch(u08 mode)
{
  switch(mode)
  {
    case MODEM_RX:
      Modem.Switch_RX_flg = 1;
    break;
    
    case MODEM_IDLE:
      Modem.Switch_IDLE_flg = 1;
    break;
    
    case MODEM_TX_FF:
      Modem.Switch_TX_FF_flg = 1;    
    break;
/*    
    case MODEM_STANDBY:
      Modem.Switch_STANDBY = 1;        
    break;
*/       
  }
}




//******************************************
// Set the TX Frequency
//*****************************************
void Modem_SetTxFreq(u08 freq)
{  
  switch(freq)
  {
    case  FREQ_TX_403:
      Modem.TxFreq = freq;    
      Mel_SetTxFreq(freq);
    break;
    case  FREQ_TX_433:
      Modem.TxFreq = freq;        
      Mel_SetTxFreq(freq);      
    break;
  }
}


//****************************************
// Set the RX Frequency
//*****************************************
void Modem_SetRxFreq(u08 freq)
{  
  switch(freq)
  {
    case  FREQ_RX_403:
      Modem.RxFreq = freq;
      Mel_SetRxFreq(freq);      
    break;
    case  FREQ_RX_433:
      Modem.RxFreq = freq;    
      Mel_SetRxFreq(freq);            
    break;
  }
}






// **************************************************
// Sets the mode the modem enters after a valid 
// RX or TX
// **************************************************
void Modem_Set_BaseMode(u08 basemode)
{  
  Modem.BaseMode = basemode;
}


// **************************************************
// Sets the Transmit power level on the modem
// **************************************************
void Modem_Set_BaseTxLevel(u08 level)
{  
  Modem.BaseTxLevel = level;
  Modem.TxLevel = level;
	Mel.txLevel_u08 = Modem.TxLevel;  
}



// **************************************************
// Resets the transmit power back to the base setting
// **************************************************
void Modem_Reset_TxLevel(void)
{  
  Modem.TxLevel = Modem.BaseTxLevel;
	Mel.txLevel_u08 = Modem.TxLevel;  
}

// **************************************************
// Increments the transmit power level
// **************************************************
void Modem_TxLevel_inc(void)
{  
  #define MODEM_TX_LEVEL_MAX 3
  if(Modem.TxLevel < MODEM_TX_LEVEL_MAX)
    Modem.TxLevel++;
    
	Mel.txLevel_u08 = Modem.TxLevel;  
}



// **************************************************
// Increments the transmit power level
// **************************************************
void Modem_SetTxLevel(u08 level)
{  
  Modem.TxLevel = level;    
	Mel.txLevel_u08 = Modem.TxLevel;  
}



// **************************************************
// Sets the RX LNA sensitivity
// **************************************************
void Modem_Set_RxLNA(u08 lna)
{  
	Mel.lna_u08 = lna;  
}



// ************************************************************************************
// ************************************************************************************
// ************************************************************************************




//***************************************************************************
// Adds the message received over the air to the Q 
// 					                                                        
//***************************************************************************
inline void Modem_AddRxMsg(u08 *src_ptr,u08 len,u08 RSSI)
{	
u08 i;

	if(++ModemRxMsgQ.wr > (MAX_MSG_Q_SIZE-1)){
		ModemRxMsgQ.wr = 0;
	}
		
    for(i=0;i<len;i++){
      ModemRxMsgQ.Msg[ModemRxMsgQ.wr].buf[i] = *(src_ptr +i);
    }
	ModemRxMsgQ.Msg[ModemRxMsgQ.wr].buf[len] = 0x00; // Complete with NULL character
	ModemRxMsgQ.Msg[ModemRxMsgQ.wr].len = len;
	ModemRxMsgQ.Msg[ModemRxMsgQ.wr].rssi = RSSI;
	//ModemRxMsgQ.Msg[ModemRxMsgQ.wr].sourceaddr = sourceaddr;
  

    #ifdef DEBUG_RECEIVEDMSGBUF
	   debug_nn_ram(&ModemRxMsgQ.Msg[ModemRxMsgQ.wr].buf[0],1);
	   debug_buffer_hex(&ModemRxMsgQ.Msg[ModemRxMsgQ.wr].buf[0], len);
    #endif	
}


//***************************************************************************
// Checks if there is a new message waiting
// 					                                                        
//***************************************************************************
u08 Modem_CheckRxQ(void)
{
	if(ModemRxMsgQ.rd != ModemRxMsgQ.wr)
		return(1);
	else
		return(0);
}


//***************************************************************************
// Copies the next message out 
// 					                                                        
//***************************************************************************
sModemRxMsg *Modem_GetNextRxMsg(void)
{
  //debug_rom(PSTR("GetNextRxMsg"),1);

	if(ModemRxMsgQ.rd != ModemRxMsgQ.wr)
	{
		if(++ModemRxMsgQ.rd > (MAX_MSG_Q_SIZE-1))
		{
			ModemRxMsgQ.rd = 0;
		}		
		return(&ModemRxMsgQ.Msg[ModemRxMsgQ.rd]);
	}
	else
		return(0);
}



//***************************************************************************
// Modem_FastStart(void)
// Initialise the variables that will be used by the interrupt routine
// interrupt routine.                                                       
//*************************************************************************** 
inline void Modem_BlastStart(void)
{  
  // Initialise variables for Sample Decoder 
  SMP.ShiftReg=0;
  //SMP.Sample=0x00;
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
  //SMP.Complete = 0;  
  SMP.bitbuf_index = 0;
  SMP.bit_bitmask = 0x80; 
  SMP.len = 0;  
  
  //Modem_CRC_Reset();
  
  //SMP.CWG = &CWG;      
  //Comms.sample_cnt = 0;  
}


//***************************************************************************
// Modem_CheckReceivedMessage();
// Check the message size, and CRC, and if valid copy to the
// find the preable
// 


//*************************************************
// Create a Data Packet from the Message
// Includes the Preable, Payload Length, Payload, and CRC
// Returns : Packet length in bytes
//*************************************************
u08 Modem_CreatePkt(u08 *msg, u08 len,u08 *pkt)
{
u08 i;
u16 crc;
u08 *crcstart_ptr;
u08   crc_h,crc_l;  
u08 *tmp_ptr;

  tmp_ptr = pkt;
  // Add the preamble
  for(i=0;i<PREAMBLE_LEN;i++){
	*pkt = PREAMBLE_BYTE; pkt++;
  }
	
  *pkt = MAGIC_BYTE; pkt++; 

  // Payload Length
  *pkt = len;  
  crcstart_ptr = pkt;
  pkt++;

           
  // Copy the message into the packet payload
  for(i=0;i<len;i++)
  {
    *pkt = *msg;
    pkt++; msg++;
  }


  // Calculate the CRC
  crc = Modem_CRC(crcstart_ptr,len+1);   // include the message length in the calc
  crc_h = (u08)((crc >>8) & 0x00ff); 
  crc_l = (u08)(crc & 0x00FF); 

/*
  rprintf_rom(PSTR("Calc Crc = "));
  rprintfu16(crc);
  rprintf_rom(PSTR("\r\n"));
*/  
  
  
  *pkt = (u08)((crc >>8) & 0x00ff);  pkt++;
  *pkt = (u08)(crc & 0x00FF); pkt++;
  
  *pkt = 0x42;  // For glory at the end   
  
  return(len +CWG_PKT_HEADER_LEN); // return the constructed packet length  
}  
  




//*************************************************
// Create the Packet for transmission
// over the air
// 
//*************************************************
u08 Modem_Send(u08 *msg, u08 len)
{
  TX.pktbuf_len = Modem_CreatePkt(msg,len,&TX.pktbuf[0]);
  // The manchester encoding occurs during the interrupt 
  // that transmits the bits over the air
  
    //#define MODEM_DEBUG_SENDPKT
    #ifdef MODEM_DEBUG_SENDPKT
      rprintf_rom(PSTR("TX.pktbuf_len = "));
      rprintfu08(TX.pktbuf_len);
      rprintf_rom(PSTR("\r\nTX.pktbuf = "));
      u08 i;
      for(i= 0; i< TX.pktbuf_len; i++)
        rprintfu08(TX.pktbuf[i]);
      rprintf_rom(PSTR("\r\n"));
    #endif  
    
    // Check That the modem is not busy with a transmission
    if((volatile u08)Modem.TXBusy)
    {
      rprintf_rom(PSTR("!TX"));
      return(1);
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
 
    // ***********************************************************
    // NOTE INTRODUCE A COUNTER THAT FLAGS AN ERROR HERE PLEASE
    // REBOOT POSSIBLY TO AVOID LOCK UP
    // ***********************************************************
    while((volatile u08)Modem.StartTx_flg);
      //rprintf("*");
    while((volatile u08)Modem.TXBusy);
      //rprintf("&");

    return(0);
}


//*************************************************
// Calculate CRC 
// CRC is calculated over the packet payload
// including the length byte
//*************************************************

/*internal use only - puts a byte*/
void Modem_PutCRC(u08 b)
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


/*call this routine with your own data buffer*/
/* yes! it's really that simple!*/
u16 Modem_CRC (u08* Data,u08 Length)
{
   CRC_buffer.Part.Remainder = INITIAL_VALUE;
   while (Length--  >  0)
      Modem_PutCRC(*Data++);
   Modem_PutCRC(0);
   Modem_PutCRC(0);
   return CRC_buffer.Part.Remainder;
} 

/*
void Modem_CRC_Reset(void)
{
   CRC_buffer.Part.Remainder = INITIAL_VALUE;
} 

void Modem_CRC_Byte(u08 data)
{
      Modem_PutCRC(data);
} 

u16 Modem_CRC_GetResult(void)
{
   Modem_PutCRC(0);
   Modem_PutCRC(0);
   return CRC_buffer.Part.Remainder;
} 
*/


