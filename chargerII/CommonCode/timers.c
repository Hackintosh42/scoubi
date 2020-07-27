
#define TIMERS_C
#include "timers.h"

// Global variables
// time registers
volatile unsigned long TimerPauseReg;
volatile unsigned long Timer0Reg0;
volatile unsigned long Timer2Reg0;

// Program ROM constants
// the prescale division values stored in 2^n format
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
unsigned char __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,0,3,6,8,10};
// the prescale division values stored in 2^n format
// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024
unsigned char __attribute__ ((progmem)) TimerRTCPrescaleFactor[] = {0,0,3,5,6,7,8,10};


typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc[TIMER_NUM_INTERRUPTS];

// delay for a minimum of <us> microseconds 
// the time resolution is dependent on the time the loop takes 
// e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us 

/*
void delay(unsigned short us) 
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty) 

	// one loop takes 5 cpu cycles 
	for (i=0; i < delay_loops; i++) {};
} 
*/
void smalldelay(unsigned short us) 
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (us)/5*CYCLES_PER_US; // +3 for rounding up (dirty) 

	// one loop takes 5 cpu cycles 
	for (i=0; i < delay_loops; i++) {};
} 



void Init_timer0(void)
{  
  timer0SetPrescaler(TIMER_CLK_DIV1);
  //outp(0, TCNT0);							// reset TCNT0	

#ifdef ATMEGA8  
//	sbi(TIMSK, TOIE0);						// enable TCNT0 overflow interrupt
#endif
#ifdef ATMEGA88  
	TIMSK0 |= (1<<TOIE0);						// enable TCNT0 overflow interrupt
//	sbi(TIMSK0, OCIE0A);					// enable OCIE0A compare interrupt	
	
#endif
}


void Init_timer0_cmp(void)
{
    timer0SetPrescaler(TIMER_CLK_DIV1);
//	outp(0, TCNT0);							// reset TCNT0	

	OCR0A = 200;
//	cbi(TCCR0B,WGM02);
//	sbi(TCCR0A,WGM01);
//	cbi(TCCR0A,WGM00);
//	cbi(TCCR0A,COM0A0);
//	cbi(TCCR0A,COM0A1);
	
	TIMSK0 |= (1<< OCIE0A);						// enable OCIE0A compare interrupt	
}


void Init_timer1(void)
{

  TCCR1A  = 0x00;
  TCCR1A &= ~(1<<COM1A1);
  TCCR1A &= ~(1<<COM1B1);
  TCCR1B = 0x00;

  //sbi(TCCR1B,WGM12);
  //timer1SetPrescaler(TIMER_CLK_DIV64);  
  timer1SetPrescaler(TIMER_CLK_DIV1);    
  //ICR1 = 0x1FFF; // top of the timer
  //OCR1A = 0x7FFF; // Top of the timer
  OCR1A = 0xFFFF; // Top of the timer
  ICR1 = 0xFFFF;
  
  //sbi(TIMSK1,OCIE1A);
  TIMSK1 |=(1<<TOIE1);
  
  TIFR1 |= (1<<TOV1);
     
}



/*
void timerInit(void)
{
	u08 intNum;

// detach all user functions from interrupts
	for(intNum=0; intNum<TIMER_NUM_INTERRUPTS; intNum++)
		timerDetach(intNum);
    

	// initialize all timers
	timer0Init();
	//timer1Init();

	// enable interrupts
	sei();
}

void timer0Init()
{
	// initialize timer 0
	timer0SetPrescaler( TIMER0PRESCALE );	// set prescaler
	outp(0, TCNT0);							// reset TCNT0
#ifdef ATMEGA8    
	sbi(TIMSK, TOIE0);						// enable TCNT0 overflow interrupt
#endif
#ifdef ATMEGA88
	sbi(TIMSK0, TOIE0);						// enable TCNT0 overflow interrupt
#endif  

	timer0ClearOverflowCount();				// initialize time registers
}

void timer1Init(void)
{
	// initialize timer 1
	timer1SetPrescaler( TIMER1PRESCALE );	// set prescaler
	outp(0, TCNT1H);						// reset TCNT1
	outp(0, TCNT1L);
#ifdef ATMEGA8  
	sbi(TIMSK, TOIE1);						// enable TCNT1 overflow
#endif
#ifdef ATMEGA88
	sbi(TIMSK0, TOIE1);						// enable TCNT1 overflow
#endif  
}
*/

void timer0SetPrescaler(u08 prescale)
{
	// set prescaler on timer 0
#ifdef ATMEGA8
	outp( (inp(TCCR0) & ~TIMER_PRESCALE_MASK) | prescale, TCCR0);
#endif

#ifdef ATMEGA88
	//outp( (inp(TCCR0B) & ~TIMER_PRESCALE_MASK) | prescale, TCCR0B);
	TCCR0B = ((TCCR0B) & ~TIMER_PRESCALE_MASK) | prescale;
  
#endif

  
}

void timer1SetPrescaler(u08 prescale)
{
	// set prescaler on timer 1
//	outp( (inp(TCCR1B) & ~TIMER_PRESCALE_MASK) | prescale, TCCR1B);
	TCCR1B = (TCCR1B & ~TIMER_PRESCALE_MASK) | prescale;

}

/*
void timerAttach(u08 interruptNum, void (*userFunc)(void) )
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntFunc[interruptNum] = userFunc;
	}
}

void timerDetach(u08 interruptNum)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run nothing
		TimerIntFunc[interruptNum] = 0;
	}
}



void timer0ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer0Reg0 = 0;	// initialize time registers
}

long timer0GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer0ClearOverflowCount() command was called)
	return Timer0Reg0;
}



//! Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW0)
{
	Timer0Reg0++;			// increment low-order counter

	// increment pause counter
	TimerPauseReg++;

	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}

//! Interrupt handler for tcnt1 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW1)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OVERFLOW_INT])
		TimerIntFunc[TIMER1OVERFLOW_INT]();
}

*/



//**************************************************
// Wait routine. Wait Time in Milliseconds.
//**************************************************
void Wait(u16 waittime)
{
u16 i;

  for(i=0;i< waittime;i++)
   delay(100);
}


void WaitRand_ms(u16 maxwaittime)
{
u16 r = rand();
u16 i;
  
  // Leftshift the random number until it is smaller than 
  u32 t =  (u32)r * (u32)maxwaittime;
  t = t >>16;
  u16 waittime  = (u16)t;
/*
  rprintf_rom(PSTR("Rand = "));
  rprintfu16(r);
  rprintf_rom(PSTR("\r\nwaittime = "));
  rprintfu16(waittime);
  rprintf_rom(PSTR("\r\n"));
*/  
  

  for(i=0;i<waittime;i++)
  {
   asm volatile(	\
      "ldi r21, 240\n\t"	\
      "L_%=: nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "dec r21\n\t"		\
      "brne L_%=\n\t"		\
      ::: "r21"
      );
  }
}


//**************************************************
// Wait_ms routine. Wait Time in Milliseconds.
// Only accurate if no interrupts are running
//**************************************************
void Wait_ms(u16 waittime)
{
u16 i;

waittime = waittime <<1; // multiply by 2
  for(i=0;i<waittime;i++)
  {
    asm volatile(	\
      "ldi r21, 240\n\t"	\
      "L_%=: nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "nop\n\t"		\
      "dec r21\n\t"		\
      "brne L_%=\n\t"		\
      ::: "r21"
      );
    }
}



