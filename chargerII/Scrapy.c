/*   
  //**************************************************
  // void Comms_Slave_WaitClockEdge()
  // Waits for a rising edge on the clock line
  //**************************************************  
   void Comms_Slave_WaitRisingClockEdge(void)
   {
    u08 sck,prevsck;
      prevsck = 1;
      while(1)      
      {
        sck = Comms_SPI_SCK_Read();      
        if((sck==1) &&(prevsck == 0))
          break;
        prevsck = sck;  
      }      
   }

  //**************************************************
  // void Comms_Slave_WaitRisingClockEdge()
  // Waits for a rising edge on the clock line
  //**************************************************  
   void Comms_Slave_WaitFallingClockEdge(void)
   {
    u08 sck,prevsck;
      prevsck = 0;
      while(1)      
      {
        sck = Comms_SPI_SCK_Read();      
        if((sck==0) &&(prevsck == 1))
          break;
        prevsck = sck;  
      }      
   }


  
  //**************************************************
  // void Comms_Slave_WriteByte(u08 data)
  // writes a Byte from the Battery Pack to Charger
  //**************************************************
  void Comms_Slave_WriteByte(u08 data)
  {
  u08 i;

    Comms_Slave_WaitRisingClockEdge();
  
    // Clock line is high, and a falling edge 
    for(i=0;i<8;i++)
    {
      u08 bit = data & 0x80;
      if(bit)
        Comms_SPI_MISO(1);
      else
        Comms_SPI_MISO(0);
      
      Comms_Slave_WaitFallingClockEdge();
      
      data = data<<1;
    }  
  }
  
  
  //**************************************************
  // u08 Comms_Slave_ReadByte(void)
  // Send a Byte to the Battery Pack
  //**************************************************
  u08 Comms_Slave_ReadByte(void)
  {
  u08 i,data,bit;
  
    // Clock line is low, and a rising edge signals time for new data
    // data bit is sampled just before the SCK line is changed to low
    data = 0;
    for(i=0;i<8;i++)
    {
      Comms_Slave_WaitRisingClockEdge();          
      bit = Comms_SPI_MOSI_Read();
      data = data <<1;    
      if(bit)    
        data |= 0x01;                     
    }
    return(data);
  }
      
  
  //**************************************************
  // void Comms_SendPkt(u08 *pkt, u08 len)
  // Sends a packet of information 
  //**************************************************
  void Comms_Slave_GetPkt(u08 *pkt, u08 len)
  {
  u08 i;
  
    
    for(i=0;i<len;i++)
    {
      *pkt = Comms_Slave_ReadByte();
      pkt++;
    }
  }  
  
*/



//**************************************************
// void Comms_SPI_SCK(u08 level)
// 
//**************************************************
/*
void Comms_SPI_SCK(u08 level)
{
  if(level)
    SPI_PORT = SPI_PORT | (1<< SPI_SCK);
  else
    SPI_PORT = SPI_PORT & ~(1<< SPI_SCK);  
}
*/




//**************************************************
// void Comms_SPI_MISO(u08 level)
// 
//**************************************************
/*
u08 Comms_SPI_SCK_Read()
{
    if(SPI_PORT_PIN & (1<< SPI_SCK))
      return(1);
    else
      return(0);
}
*/



//**************************************************
// void Comms_SPI_MOSI(u08 level)
// 
//**************************************************
/*
void Comms_SPI_MOSI(u08 level)
{
  if(level)
    SPI_PORT = SPI_PORT | (1<< SPI_MOSI);
  else
    SPI_PORT = SPI_PORT & ~(1<< SPI_MOSI);  
}
*/


//**************************************************
// void Comms_SPI_MISO(u08 level)
// 
//**************************************************
/*
void Comms_SPI_MISO(u08 level)
{
  if(level)
    SPI_PORT = SPI_PORT | (1<< SPI_MISO);
  else
    SPI_PORT = SPI_PORT & ~(1<< SPI_MISO);  
}
*/



//**************************************************
// void Comms_SPI_MISO(u08 level)
// 
//**************************************************
/*
u08 Comms_SPI_MISO_Read()
{
    if(SPI_PORT_PIN & (1<< SPI_MISO))
      return(1);
    else
      return(0);
}
*/



//**************************************************
// void Comms_SPI_MISO(u08 level)
// 
//**************************************************
/*
u08 Comms_SPI_MOSI_Read()
{
    if(SPI_PORT_PIN & (1<< SPI_MOSI))
      return(1);
    else
      return(0);
}
*/


