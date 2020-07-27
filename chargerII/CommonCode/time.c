/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : time.c
   Ver nr.     : 1.0
   Description : C file for timekeeping                
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2004-06-06    JvA  
                 2004-11-29    Update for VtagII
 
*************************************************************************************/

#define TIME_C
#include "time.h"



void Time_settime(u08 hours,u08 minutes, u08 seconds)
{  
  u08 tens, units;
  
  i2c_buf[0]=0x00;
  
  // Convert decimal to BCD Hex
  tens = (seconds/10);
  units = seconds - (tens*10);
  i2c_buf[1]=(tens<<4) + units;    
  
  tens = (minutes/10);
  units = minutes - (tens*10);    
  i2c_buf[2]=(tens<<4) + units;
  
  tens = (hours/10);
  units = hours - (tens*10);  
  i2c_buf[3]=(tens<<4) + units;  

  i2cMasterSendNI(RTC_I2C_ADDR,4,(u08*)&i2c_buf[0]);
}

void Time_readtime(u08 *hours,u08 *minutes, u08 *seconds)
{
  i2c_buf[0]=0x00;
  i2cMasterSendNI(RTC_I2C_ADDR,1,(u08*)&i2c_buf[0]); 
  i2cMasterReceiveNI(RTC_I2C_ADDR, 3, (u08*)&i2c_buf[0]);

  *seconds = (i2c_buf[0] >>4)*10 + (i2c_buf[0] &0x0F);
  *minutes = (i2c_buf[1] >>4)*10 + (i2c_buf[1] &0x0F); 
  *hours = (i2c_buf[2] >>4)*10 + (i2c_buf[2] &0x0F);
}

/*
void Time_setdate(u08 year,u08 month, u08 date, u08 day)
{
  i2c_buf[0]=0x03;
  i2c_buf[1]=day;
  i2c_buf[2]=date;
  i2c_buf[3]=month | 0x80; // Mask the century bit
  i2c_buf[4]=year;
  
  i2cMasterSendNI(RTC_I2C_ADDR,5,(u08*)&i2c_buf[0]);
}


void Time_readdate(u08 *year,u08 *month, u08 *date, u08 *day)
{
  i2c_buf[0]=0x03;
  i2cMasterSendNI(RTC_I2C_ADDR,1,(u08*)&i2c_buf[0]); 
  i2cMasterReceiveNI(RTC_I2C_ADDR, 4, (u08*)&i2c_buf[0]);

  *day = i2c_buf[0];
  *date = i2c_buf[1];
  *month = i2c_buf[2] &0x7F;  // Mask the century bit out
  *year = i2c_buf[3];   
}

*/

//**************************************************
// Print the current time
//**************************************************
/*
void Time_printtime(void)
{
  Time_readtime((u08*)&Time.hours,(u08*)&Time.minutes,(u08*)&Time.seconds);                  
  rprintfu08(Time.hours);rprintf(":");rprintfu08(Time.minutes);rprintf(":");rprintfu08(Time.seconds);
  rprintf("\r\n");        
}
*/

//#define TIMER_SETALARM
#ifdef TIMER_SETALARM

  void Time_setalarm1_maskbits(void)
  {
    i2c_buf[0]=0x08;
    i2c_buf[1]= 0x80;
    i2c_buf[2]= 0x80;
    i2c_buf[3]= 0x80;
  
    i2cMasterSendNI(RTC_I2C_ADDR,4,(u08*)&i2c_buf[0]);  
  }

  void Time_setalarm1_seconds(u08 seconds)
  {
  u08 tmpmsb,tmplsb;
  
    // Read the seconds
    i2c_buf[0]=0x00;
    i2cMasterSendNI(RTC_I2C_ADDR,1,(u08*)&i2c_buf[0]); 
    i2cMasterReceiveNI(RTC_I2C_ADDR, 1, (u08*)&i2c_buf[0]);
  
    // Take the seconds and add
    tmplsb = (i2c_buf[0] &0x0F) +seconds;
    tmpmsb = (i2c_buf[0] &0xF0);
    if(tmplsb > 0x09)
    {
       tmplsb -= 10;
       tmpmsb += 0x10;
       if(tmpmsb >0x50)
        tmpmsb = 0x00;
    }
    
    tmpmsb += tmplsb;
    
    rprintfu08(i2c_buf[0]);
    rprintf(" ");
    rprintfu08(tmpmsb);
    rprintf("\r\n");
  
    Time_printtime();  
    
    
    
    i2c_buf[0]=0x07;
    i2c_buf[1]=tmpmsb;
    
    i2cMasterSendNI(RTC_I2C_ADDR,2,(u08*)&i2c_buf[0]);
  }
#endif




void Time_InitRTC(u08 type)
{
  i2c_buf[0]=0x0E;
//  i2c_buf[1]=((type&0x03) <<3)  | 0x01; // osc on INTCN ALARM1 on INTA# A1IE enable
  i2c_buf[1]=((type&0x03) <<3); // osc on INTCN ALARM1 on INTA# A1IE enable

  i2cMasterSendNI(0x68,2,(u08*)&i2c_buf[0]);
}


