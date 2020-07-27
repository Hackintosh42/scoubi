#ifndef TIMERS_H
#define TIMERS_H
#include "avrheaders.h"
#include "global.h"		
#include "uart.h"		



#define SYSTEMTIMER_US_TICK 32
#define WAIT_US(s)  SystemTimers.us_tick1 = 0; while(SystemTimers.us_tick1 <(u16)(s/SYSTEMTIMER_US_TICK) -1);
#define WAIT_MS(s)  u16 ii; for(ii=0;ii<s;ii++){SystemTimers.us_tick1 = 0;while(SystemTimers.us_tick1 <(u16)(1000/SYSTEMTIMER_US_TICK) -1);}


// structure that stores the current time of the reader
typedef struct
{
	u08 year;
	u08 month;
	u08 day;
	u08 hour;
	u08 min;
  u08 sec;
}sSystemTime;


// structure that stores the timer values or the system
typedef struct
{	
volatile  u16 us_tick;
}sSystemTimers;	

#ifdef TIMERS_C
volatile	sSystemTimers SystemTimers;
volatile sSystemTime 		SystemTime;  
#else
  extern volatile	sSystemTimers 		SystemTimers;
  extern volatile	sSystemTime 		SystemTime;

#endif  


// Timer/clock prescaler values and timer overflow rates
// tics = rate at which the timer counts up
// 8bitoverflow = rate at which the timer overflows 8bits (or reaches 256)
// 16bit [overflow] = rate at which the timer overflows 16bits (65536)
// 
// overflows can be used to generate periodic interrupts
//
// for 8MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 8MHz			8bitoverflow= 31250Hz		16bit= 122.070Hz
// 2 = CLOCK/8		tics= 1MHz			8bitoverflow= 3906.25Hz		16bit=  15.259Hz
// 3 = CLOCK/64		tics= 125kHz		8bitoverflow=  488.28Hz		16bit=   1.907Hz
// 4 = CLOCK/256	tics= 31250Hz		8bitoverflow=  122.07Hz		16bit=	0.477Hz
// 5 = CLOCK/1024	tics= 7812.5Hz		8bitoverflow=   30.52Hz		16bit=   0.119Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 4MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 4MHz			8bitoverflow= 15625Hz		16bit=  61.035Hz
// 2 = CLOCK/8		tics= 500kHz		8bitoverflow= 1953.125Hz	16bit=   7.629Hz
// 3 = CLOCK/64		tics= 62500Hz		8bitoverflow=  244.141Hz	16bit=   0.954Hz
// 4 = CLOCK/256	tics= 15625Hz		8bitoverflow=   61.035Hz	16bit=   0.238Hz
// 5 = CLOCK/1024	tics= 3906.25Hz		8bitoverflow=   15.259Hz	16bit=   0.060Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 3.69MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 3.69MHz		8bitoverflow= 14414Hz		16bit=  56.304Hz
// 2 = CLOCK/8		tics= 461250Hz		8bitoverflow= 1801.758Hz	16bit=   7.038Hz
// 3 = CLOCK/64		tics= 57625.25Hz	8bitoverflow=  225.220Hz	16bit=   0.880Hz
// 4 = CLOCK/256	tics= 14414.063Hz	8bitoverflow=   56.305Hz	16bit=   0.220Hz
// 5 = CLOCK/1024	tics=  3603.516Hz	8bitoverflow=   14.076Hz	16bit=   0.055Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 32.768KHz crystal on timer 2 (use for real-time clock)
// 0 = STOP
// 1 = CLOCK		tics= 32.768kHz		8bitoverflow= 128Hz
// 2 = CLOCK/8		tics= 4096kHz		8bitoverflow=  16Hz
// 3 = CLOCK/32		tics= 1024kHz		8bitoverflow=   4Hz
// 4 = CLOCK/64		tics= 512Hz			8bitoverflow=   2Hz
// 5 = CLOCK/128	tics= 256Hz			8bitoverflow=   1Hz
// 6 = CLOCK/256	tics= 128Hz			8bitoverflow=   0.5Hz
// 7 = CLOCK/1024	tics= 32Hz			8bitoverflow=   0.125Hz

#define TIMER_CLK_STOP			0x00	///< Timer Stopped
#define TIMER_CLK_DIV1			0x01	///< Timer clocked at F_CPU
#define TIMER_CLK_DIV8			0x02	///< Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64			0x03	///< Timer clocked at F_CPU/64
#define TIMER_CLK_DIV256		0x04	///< Timer clocked at F_CPU/256
#define TIMER_CLK_DIV1024		0x05	///< Timer clocked at F_CPU/1024
#define TIMER_CLK_T_FALL		0x06	///< Timer clocked at T falling edge
#define TIMER_CLK_T_RISE		0x07	///< Timer clocked at T rising edge
#define TIMER_PRESCALE_MASK		0x07	///< Timer Prescaler Bit-Mask

#define TIMERRTC_CLK_STOP		0x00	///< RTC Timer Stopped
#define TIMERRTC_CLK_DIV1		0x01	///< RTC Timer clocked at F_CPU
#define TIMERRTC_CLK_DIV8		0x02	///< RTC Timer clocked at F_CPU/8
#define TIMERRTC_CLK_DIV32		0x03	///< RTC Timer clocked at F_CPU/32
#define TIMERRTC_CLK_DIV64		0x04	///< RTC Timer clocked at F_CPU/64
#define TIMERRTC_CLK_DIV128		0x05	///< RTC Timer clocked at F_CPU/128
#define TIMERRTC_CLK_DIV256		0x06	///< RTC Timer clocked at F_CPU/256
#define TIMERRTC_CLK_DIV1024	0x07	///< RTC Timer clocked at F_CPU/1024
#define TIMERRTC_PRESCALE_MASK	0x07	///< RTC Timer Prescaler Bit-Mask

// default prescale settings for the timers
// these settings are applied when you call
// timerInit or any of the timer<x>Init
#define TIMER0PRESCALE		TIMER_CLK_DIV1		///< timer 0 prescaler default
#define TIMER1PRESCALE		TIMER_CLK_DIV64		///< timer 1 prescaler default
#define TIMER2PRESCALE		TIMERRTC_CLK_DIV64	///< timer 2 prescaler default

// interrupt macros for attaching user functions to timer interrupts
// use these with timerAttach( intNum, function )
#define TIMER0OVERFLOW_INT			0
#define TIMER1OVERFLOW_INT			1
#define TIMER1OUTCOMPAREA_INT		2
#define TIMER1OUTCOMPAREB_INT		3
#define TIMER1INPUTCAPTURE_INT		4
#define TIMER2OVERFLOW_INT			5
#define TIMER2OUTCOMPARE_INT		6
#ifdef OCR0	// for processors that support output compare on Timer0
#define TIMER0OUTCOMPARE_INT		7
#define TIMER_NUM_INTERRUPTS		8
#else
#define TIMER_NUM_INTERRUPTS		7
#endif

// default type of interrupt handler to use for timers
// *do not change unless you know what you're doing
// Value may be SIGNAL or INTERRUPT
#ifndef TIMER_INTERRUPT_HANDLER
#define TIMER_INTERRUPT_HANDLER		SIGNAL
#endif

// functions
void delay(unsigned short us);
void smalldelay(unsigned short us);


//! initializes timing system (all timers)
// runs all timer init functions
// sets all timers to default prescale values #defined in systimer.c
void timerInit(void);

// default initialization routines for each timer
void timer0Init(void);		///< initialize timer0
void timer1Init(void);		///< initialize timer1

void Init_timer0(void);
void Init_timer1(void);


// Clock prescaler set commands for each timer/counter
// you may use one of the #defines above like TIMER_CLK_DIVxxx
// to set the prescale value
void timer0SetPrescaler(u08 prescale);		///< set timer0 prescaler
void timer1SetPrescaler(u08 prescale);		///< set timer1 prescaler

// TimerAttach and Detach commands
//		These functions allow the attachment (or detachment) of any user function
//		to a timer interrupt.  "Attaching" one of your own functions to a timer
//		interrupt means that it will be called whenever that interrupt happens.
//		Using attach is better than rewriting the actual INTERRUPT() function
//		because your code will still work and be compatible if the timer library
//		is updated.  Also, using Attach allows your code and any predefined timer
//		code to work together and at the same time.  (ie. "attaching" your own
//		function to the timer0 overflow doesn't prevent timerPause from working,
//		but rather allows you to share the interrupt.)
//
//		timerAttach(TIMER1OVERFLOW_INT, myOverflowFunction);
//		timerDetach(TIMER1OVERFLOW_INT)
//
//		timerAttach causes the myOverflowFunction() to be attached, and therefore
//		execute, whenever an overflow on timer1 occurs.  timerDetach removes the
//		association and executes no user function when the interrupt occurs.
//		myOverflowFunction must be defined with no return value and no arguments:
//
//		void myOverflowFunction(void) { ... }

/*
//! Attach a user function to a timer interrupt
void timerAttach(u08 interruptNum, void (*userFunc)(void) );
//! Detach a user function from a timer interrupt
void timerDetach(u08 interruptNum);


// overflow counters
void timer0ClearOverflowCount(void);	///< clear timer0's overflow counter
long timer0GetOverflowCount(void);		///< read timer0's overflow counter
*/

void WaitRand_ms(u16 maxwaittime);
void Wait(u16 waittime);
void Wait_ms(u16 waittime);


/*
typedef struct{ 
	u08 tick;
}sTimer1;

#ifdef TIMERS_C
 sTimer1 Timer1;
#else
	extern sTimer1 Timer1;
#endif
*/



#endif // TIMERS_H

