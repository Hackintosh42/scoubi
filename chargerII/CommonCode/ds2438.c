/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : ds620.c
   Ver nr.     : 1.0
   Description : Dallas DS620 temperature sensor                
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-05-12    JvA  
               
 
*************************************************************************************/

#define DS2438_C
#include "ds2438.h"



//**************************************************
// void DS2438_init(void)
//**************************************************
void DS2438_init(u08* OW_IDENTITY)
{
u08 status;
 	owTouchReset(OW.Port);
    
  status = DS2438_Read_Reg(OW_IDENTITY, 0x00,0x00);  
  //rprintf_rom(PSTR("\r\n\r\nStatus = "));
  //rprintfu08(status); rprintf("\r\n");
  // set the status bits
  status = 0; //1<<3;

  //rprintf_rom(PSTR("\r\n\r\n writing reg Status\r\n"));  
  DS2438_Write_Reg(OW_IDENTITY, 0x00,0x00,status);

  status = DS2438_Read_Reg(OW_IDENTITY, 0x00,0x00);
  //rprintf_rom(PSTR("\r\n\r\nStatus = "));
  //rprintfu08(status); rprintf("\r\n");      
}

//**************************************************
// Convert A/D command
//**************************************************
void DS2438_Convert_V(u08* OW_IDENTITY)
{
u08 buff[1];
    
  buff[0] = DS2438_CONVERT_V;  
  owSelectDevice(OW_IDENTITY);
	owBlock(OW.Port, FALSE, buff, 1);    
}


//**************************************************
// Reads all 9 bytes in, 
//**************************************************
void DS2438_Read_Scratchpad(u08* OW_IDENTITY, u08 pagenumber, u08* destptr)
{
u08 buff[11];
u08 i;

    for(i=0;i<10;i++)
      buff[i] = 0xFF;
	
	owSelectDevice(OW_IDENTITY);
  
	buff[0] = DS2438_READ_SCRATCHPAD;
  buff[1] = pagenumber;
  
  // The remaining 9 bytes will be the data
	owBlock(OW.Port, FALSE, buff, 11);
  
  // Copy the data to the destination
  for(i=0;i<9;i++)
    *(destptr+i) = buff[i+2];
  
#ifdef DS2438_DEBUG
  rprintf_rom(PSTR("Read_Scratchpad = \r\n"));
  for(i=0;i<9;i++)
  {
    rprintfu08(*(destptr+i));rprintf(" ");
  }
  rprintf_rom(PSTR("\r\n"));    
#endif  

}

//**************************************************
// Write all 9 bytes in, 
//**************************************************
void DS2438_Write_Scratchpad(u08* OW_IDENTITY, u08 pagenumber, u08* srcptr)
{
u08 buff[11];
u08 i;

	owSelectDevice(OW_IDENTITY);
  
	buff[0] = DS2438_WRITE_SCRATCHPAD;
  buff[1] = pagenumber;
  // Copy the data to be sent
  for(i=0;i<8;i++)
    buff[i+2] = *(srcptr+i);
      
  // The remaining 9 bytes will be the data
	owBlock(OW.Port, FALSE, buff, 10); 
}


//**************************************************
// u08 DS2438_Recall_Page(u08)
//**************************************************
void DS2438_Copy_Scratchpad(u08* OW_IDENTITY,u08 pagenumber)
{
	u08 buff[2];
		buff[0] = buff[1] = 0xFF;
	
	owSelectDevice(OW_IDENTITY);
  
	buff[0] = DS2438_COPY_SCRATCHPAD;
  buff[1] = pagenumber;

	owBlock(OW.Port, FALSE, buff, 2);
}


//**************************************************
// u08 DS2438_Recall_Page(u08)
//**************************************************
void DS2438_Recall_Page(u08* OW_IDENTITY,u08 pagenumber)
{

	u08 buff[2];
		buff[0] = buff[1] = 0xFF;
	
	owSelectDevice(OW_IDENTITY);
  
	buff[0] = DS2438_RECALL_MEMORY;
  buff[1] = pagenumber;

	owBlock(OW.Port, FALSE, buff, 2);
}

//**************************************************
// 
//**************************************************

u08 DS2438_Read_Reg(u08* OW_IDENTITY,u08 pagenumber, u08 reg)
{
u08 data[11];


  DS2438_Recall_Page(OW_IDENTITY, pagenumber);
  Wait(100);
  DS2438_Read_Scratchpad(OW_IDENTITY, pagenumber, data);
  
#ifdef DS2438_DEBUG  
  u08 i;
  rprintf_rom(PSTR("ReadregPage data = \r\n"));
  for(i=0;i<9;i++)
  {
    rprintfu08(data[i]);
    rprintf(" ");
  }
  rprintf_rom(PSTR("\r\n"));
#endif  

  return(data[reg]);
}


//**************************************************
// 
//**************************************************
void DS2438_Write_Reg(u08* OW_IDENTITY,u08 pagenumber, u08 reg,u08 databyte)
{
u08 databuf[11];

  DS2438_Recall_Page(OW_IDENTITY, pagenumber);
  Wait(100);  
  DS2438_Read_Scratchpad(OW_IDENTITY, pagenumber, databuf);
  
#ifdef DS2438_DEBUG  
  u08 i;
  rprintf_rom(PSTR("Writereg Page data = \r\n"));
  for(i=0;i<9;i++)
  {
    rprintfu08(databuf[i]);
    rprintf(" ");
  }
  rprintf_rom(PSTR("\r\n"));
#endif  
  
  databuf[reg] = databyte;
  DS2438_Write_Scratchpad(OW_IDENTITY, pagenumber, databuf);
  Wait(100);  
  DS2438_Copy_Scratchpad(OW_IDENTITY, pagenumber);
  
}



//**************************************************
// u16 DS2438_read_Vad(void)
// Returns the volatage in mV
//**************************************************
u16 DS2438_read_Vad(u08* OW_IDENTITY)
{  
  u08 data[11]; 
  u16 returnval;
  
  DS2438_Convert_V(OW_IDENTITY);
  // Wait for it to complete
  Wait(2000);
  
  DS2438_Recall_Page(OW_IDENTITY, 0x00);
  DS2438_Read_Scratchpad(OW_IDENTITY, 0x00, data);

#ifdef DS2438_DEBUG
  u08 i;
  rprintf_rom(PSTR("read_Vad Page data = \r\n"));
  for(i=0;i<11;i++){
    rprintfu08(data[i]);rprintf(" ");
  }
  rprintf_rom(PSTR("\r\n"));  
#endif  
  
  returnval = ((data[4] <<8) + data[3]);  
  returnval = returnval*10; // 10mV per bit
 
  return(returnval);   
}









