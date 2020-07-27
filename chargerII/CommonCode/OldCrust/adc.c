
#define ADC_C
#include "adc.h"




/******************************************/
/* Measure the RSSI on ADC6               */
/* Returns Voltage in mV                  */
/******************************************/
u16 Adc_ReadRSSI_u16(void)
{
u16 av;
//u08 i;

     ADMUX = 0x40 | ADC_RSSI;
     ADCSRA |= 0x40;                      // start new A/D conversion
     while (!(ADCSRA & (1<<ADIF)));        // wait until ADC is ready
	 av = ADC;
	 //CLRBIT(ADCSRA,ADIF);                     // clear ADC interrupt flag  
   ADCSRA = ADCSRA & ~(1<<ADIF);
    return(av);
}



/******************************************/
/* Measure the RSSI on ADC6               */
/* Returns Voltage in mV                  */
/******************************************/
u08 Adc_ReadRSSI_u08(void)
{
u08 av;
//u08 i;

     ADMUX = (1<<ADLAR)| 0x40 | ADC_RSSI;
     ADCSRA |= 0x40;                      // start new A/D conversion
     while (!(ADCSRA & (1<<ADIF)));        // wait until ADC is ready
	 //av = ADC;
	 av = ADCH;
//	 CLRBIT(ADCSRA,ADIF);                     // clear ADC interrupt flag  
    ADCSRA = ADCSRA & ~(1<<ADIF);
    return(av);
}





/*
//==========================================
// Analog to Digital  Reading from channel
//------------------------------------------
u16 Adc_Read(u08 chan)
{
u08 i;
u16 av;

     ADMUX = 0x40 | chan;
    //Calculate a average out of the next 8 A/D conversions
    for(av=0,i=8;i;--i)
    {
        ADCSRA |= 0x40;                      // start new A/D conversion
        while (!(ADCSRA & (1<<ADIF)))        // wait until ADC is ready
            ;      
        av = av+ADC;
    }
    av = av/8;
    CLRBIT(ADCSRA,ADIF);                     // clear ADC interrupt flag  
    
    return(av);
}

*/


