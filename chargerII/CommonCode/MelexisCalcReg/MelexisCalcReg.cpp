// MelexisCalcReg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Values for *mhz from Nedialkov
//PR=80, RR=80, NR=4232, NT=4339. 

#define RF_FREQ        8000000
//#define RF_FREQ         7150500	

//#define MHZ900
#define MHZ433


#ifdef MHZ433
#define RXTX_OFFSET 107
#endif

FILE *stream;

// datatype definitions macros
typedef unsigned char  u08;
typedef   signed char  s08;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;
typedef unsigned long long u64;
typedef   signed long long s64;


//*******************************************/
// Data Structure for the Melexis Registers */
//*******************************************/

// RegA
#define RR_BP         0
#define OPMODE_BP     10
#define LNAGAIN_BP    12
#define SETTOONE_A_BP 13
#define TXPOWER_BP    14
#define PACTRL_BP     16
#define LOCKMODE_BP   17
#define CPCUR_BP      18
#define MODSEL_BP     19
#define DATAPOL_BP    20
#define IDLE_BP       21

#define RR_SZ         10
#define OPMODE_SZ     2
#define LNAGAIN_SZ    1
#define SETTOONE_A_SZ 1
#define TXPOWER_SZ    2
#define PACTRL_SZ     1
#define LOCKMODE_SZ   1
#define CPCUR_SZ      1
#define MODSEL_SZ     1
#define DATAPOL_SZ    1
#define IDLE_SZ       1


// RegB
#define RT_BP        0
#define ROMIN_BP     10
#define ROMAX_BP     13
#define OA2_BP       16
#define AFC_BP       17
#define LNAHYST_BP   18
#define DELPLL_BP    19
#define SETTOONE_B_BP  20
#define PKDET_BP     21


#define RT_SZ        10
#define ROMIN_SZ     3 
#define ROMAX_SZ     3
#define OA2_SZ       1 
#define AFC_SZ       1 
#define LNAHYST_SZ   1 
#define DELPLL_SZ    1 
#define SETTOONE_B_SZ  1 
#define PKDET_SZ     1


// RegC
#define NR_BP        0
#define BAND_BP      17
#define VCOCUR_BP    18
#define PFDPOL_BP    20
#define LNACTRL_BP   21

#define NR_SZ        17
#define BAND_SZ      1
#define VCOCUR_SZ    2 
#define PFDPOL_SZ    1 
#define LNACTRL_SZ   1


// RegD
#define NT_BP        0
#define ERTM_BP      17
#define LDTM_BP      19
#define MODCTRL_BP   21

#define NT_SZ        17
#define ERTM_SZ      2 
#define LDTM_SZ      2 
#define MODCTRL_SZ   1 


enum{
FREQ_411_MHZ,
FREQ_433_MHZ,
FREQ_456_MHZ
};

enum{ MEL_STANDBY,
       MEL_TX,
       MEL_RX,
       MEL_IDLE
};   

enum {
MEL_REG_A = 0,
MEL_REG_B,
MEL_REG_C_RX_433,
MEL_REG_C_RX_403,
MEL_REG_C_TX_433,
MEL_REG_C_TX_403,
MEL_REG_D_403,
MEL_REG_D_433,
};




typedef struct{ 
  u32  RR;
  u08  OPMODE;
  u08  LNAGAIN;
  u08  SETTOONE_A;
  u08  TXPOWER;
  u08  PACTRL;
  u08  LOCKMODE;
  u08  CPCUR;
  u08  MODSEL;
  u08  DATAPOL;
  u08  IDLE;
}sMel_RegA;

typedef struct{ 
  u16 RT;
  u08  ROMIN;
  u08  ROMAX;
  u08  OA2;
  u08  AFC;
  u08  LNAHYST;
  u08  DELPLL;
  u08  SETTOONE_B;
  u08  PKDET;
}sMel_RegB;

typedef struct{ 
  u32 NR_433;
  u32 NR_403;
  u08  BAND;
  u08  VCOCUR_TX;
  u08  VCOCUR_RX;
  u08  PFDPOL;
  u08  LNACTRL;
}sMel_RegC;

typedef struct{ 
  u32 NT_433;
  u32 NT_403;
  u08  ERTM;
  u08  LDTM;
  u08  MODCTRL;
}sMel_RegD;





typedef struct{ 
u32 A;
u32 B;
u32 C;
u32 D;

sMel_RegA RegA;
sMel_RegB RegB;
sMel_RegC RegC;
sMel_RegD RegD;
}sMel_Reg;


sMel_Reg Mel_Reg;



void rprintf(char *stringg)
{
	printf(stringg);
	fprintf(stream,stringg);
}

void rprintfu08(u08 tmp)
{
	printf("%0.2x",tmp);
	fprintf(stream,"%0.2x",tmp);

}


void Mel_Wrreg(u32 data)
{
}


/************************************/
/* Construct the Melexis registers  */
/************************************/
void Mel_LoadReg(u08 reg)
{
u32 one;
one = 0x01;
switch(reg)
{
  case MEL_REG_A:

    Mel_Reg.A  = 0x00 ; 
    Mel_Reg.A <<= IDLE_SZ;        Mel_Reg.A |= (Mel_Reg.RegA.IDLE & ((1<<IDLE_SZ) -1));
    Mel_Reg.A <<= DATAPOL_SZ;     Mel_Reg.A |= (Mel_Reg.RegA.DATAPOL & ((1<<DATAPOL_SZ) -1));
    Mel_Reg.A <<= MODSEL_SZ;      Mel_Reg.A |= (Mel_Reg.RegA.MODSEL & ((1<<MODSEL_SZ) -1));
    Mel_Reg.A <<= CPCUR_SZ;       Mel_Reg.A |= (Mel_Reg.RegA.CPCUR & ((1<<CPCUR_SZ) -1));
    Mel_Reg.A <<= LOCKMODE_SZ;    Mel_Reg.A |= (Mel_Reg.RegA.LOCKMODE & ((1<<LOCKMODE_SZ) -1));
    Mel_Reg.A <<= PACTRL_SZ;      Mel_Reg.A |= (Mel_Reg.RegA.PACTRL & ((1<<PACTRL_SZ) -1));
    Mel_Reg.A <<= TXPOWER_SZ;     Mel_Reg.A |= (Mel_Reg.RegA.TXPOWER & ((1<<TXPOWER_SZ) -1));
    Mel_Reg.A <<= SETTOONE_A_SZ;  Mel_Reg.A |= (Mel_Reg.RegA.SETTOONE_A & ((1<<SETTOONE_A_SZ) -1));
    Mel_Reg.A <<= LNAGAIN_SZ;     Mel_Reg.A |= (Mel_Reg.RegA.LNAGAIN & ((1<<LNAGAIN_SZ) -1));
    Mel_Reg.A <<= OPMODE_SZ;      Mel_Reg.A |= (Mel_Reg.RegA.OPMODE & ((1<<OPMODE_SZ) -1));
    Mel_Reg.A <<= RR_SZ;          Mel_Reg.A |= (Mel_Reg.RegA.RR & ((1<<RR_SZ) -1));
/*
    Mel_Wrreg(Mel_Reg.A);
    rprintf("\r\n AAAA = ");
    rprintfu08((Mel_Reg.A>>24) & 0xFF);rprintfu08((Mel_Reg.A>>16) & 0xFF);rprintfu08((Mel_Reg.A>>8) & 0xFF);rprintfu08((Mel_Reg.A) & 0xFF);  
    rprintf(" ");
*/
  

  break;
  
  case MEL_REG_B:
    Mel_Reg.B = 0x01;
    Mel_Reg.B <<= PKDET_SZ;       Mel_Reg.B |= (Mel_Reg.RegB.PKDET & ((1<<PKDET_SZ) -1));
    Mel_Reg.B <<= SETTOONE_B_SZ;  Mel_Reg.B |= (Mel_Reg.RegB.SETTOONE_B & ((1<<SETTOONE_B_SZ) -1));
    Mel_Reg.B <<= DELPLL_SZ;      Mel_Reg.B |= (Mel_Reg.RegB.DELPLL & ((1<<DELPLL_SZ) -1));
    Mel_Reg.B <<= LNAHYST_SZ;     Mel_Reg.B |= (Mel_Reg.RegB.LNAHYST & ((1<<LNAHYST_SZ) -1));
    Mel_Reg.B <<= AFC_SZ;         Mel_Reg.B |= (Mel_Reg.RegB.AFC & ((1<<AFC_SZ) -1));
    Mel_Reg.B <<= OA2_SZ;         Mel_Reg.B |= (Mel_Reg.RegB.OA2 & ((1<<OA2_SZ) -1));
    Mel_Reg.B <<= ROMAX_SZ;       Mel_Reg.B |= (Mel_Reg.RegB.ROMAX & ((1<<ROMAX_SZ) -1));
    Mel_Reg.B <<= ROMIN_SZ;       Mel_Reg.B |= (Mel_Reg.RegB.ROMIN & ((1<<ROMIN_SZ) -1));
    Mel_Reg.B <<= RT_SZ;          Mel_Reg.B |= (Mel_Reg.RegB.RT & ((1<<RT_SZ) -1));

/*
    Mel_Wrreg(Mel_Reg.B);
    rprintf("\r\n BBBB = ");
    rprintfu08((Mel_Reg.B>>24) & 0xFF);rprintfu08((Mel_Reg.B>>16) & 0xFF);rprintfu08((Mel_Reg.B>>8) & 0xFF);rprintfu08((Mel_Reg.B) & 0xFF);  
    rprintf(" ");
*/		
	
  break;

  case MEL_REG_C_RX_433:
    Mel_Reg.C = 0x02;
    Mel_Reg.C <<= LNACTRL_SZ;     Mel_Reg.C |= (Mel_Reg.RegC.LNACTRL & ((1<<LNACTRL_SZ) -1));
    Mel_Reg.C <<= PFDPOL_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.PFDPOL & ((1<<PFDPOL_SZ) -1));
    Mel_Reg.C <<= VCOCUR_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.VCOCUR_RX & ((1<<VCOCUR_SZ) -1));
    Mel_Reg.C <<= BAND_SZ;        Mel_Reg.C |= (Mel_Reg.RegC.BAND & ((1<<BAND_SZ) -1));
    Mel_Reg.C <<= NR_SZ;          Mel_Reg.C |= (Mel_Reg.RegC.NR_433 & ((one<<NR_SZ) -1));	
  break;

  case MEL_REG_C_RX_403:
    Mel_Reg.C = 0x02;
    Mel_Reg.C <<= LNACTRL_SZ;     Mel_Reg.C |= (Mel_Reg.RegC.LNACTRL & ((1<<LNACTRL_SZ) -1));
    Mel_Reg.C <<= PFDPOL_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.PFDPOL & ((1<<PFDPOL_SZ) -1));
    Mel_Reg.C <<= VCOCUR_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.VCOCUR_RX & ((1<<VCOCUR_SZ) -1));
    Mel_Reg.C <<= BAND_SZ;        Mel_Reg.C |= (Mel_Reg.RegC.BAND & ((1<<BAND_SZ) -1));
    Mel_Reg.C <<= NR_SZ;          Mel_Reg.C |= (Mel_Reg.RegC.NR_403 & ((one<<NR_SZ) -1));	
  break;
/*
  case MEL_REG_C_TAG_RX:
    Mel_Reg.C = 0x02;
    Mel_Reg.C <<= LNACTRL_SZ;     Mel_Reg.C |= (Mel_Reg.RegC.LNACTRL & ((1<<LNACTRL_SZ) -1));
    Mel_Reg.C <<= PFDPOL_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.PFDPOL & ((1<<PFDPOL_SZ) -1));
    Mel_Reg.C <<= VCOCUR_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.VCOCUR_RX & ((1<<VCOCUR_SZ) -1));
    Mel_Reg.C <<= BAND_SZ;        Mel_Reg.C |= (Mel_Reg.RegC.BAND & ((1<<BAND_SZ) -1));
    Mel_Reg.C <<= NR_SZ;          Mel_Reg.C |= (Mel_Reg.RegC.NR_tag & ((one<<NR_SZ) -1));	
  break;
*/


  case MEL_REG_C_TX_433:
    Mel_Reg.C = 0x02;
    Mel_Reg.C <<= LNACTRL_SZ;     Mel_Reg.C |= (Mel_Reg.RegC.LNACTRL & ((1<<LNACTRL_SZ) -1));
    Mel_Reg.C <<= PFDPOL_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.PFDPOL & ((1<<PFDPOL_SZ) -1));
    Mel_Reg.C <<= VCOCUR_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.VCOCUR_TX & ((1<<VCOCUR_SZ) -1));
    Mel_Reg.C <<= BAND_SZ;        Mel_Reg.C |= (Mel_Reg.RegC.BAND & ((1<<BAND_SZ) -1));
    Mel_Reg.C <<= NR_SZ;          Mel_Reg.C |= (Mel_Reg.RegC.NR_433 & ((one<<NR_SZ) -1));	
  break;

  case MEL_REG_C_TX_403:
    Mel_Reg.C = 0x02;
    Mel_Reg.C <<= LNACTRL_SZ;     Mel_Reg.C |= (Mel_Reg.RegC.LNACTRL & ((1<<LNACTRL_SZ) -1));
    Mel_Reg.C <<= PFDPOL_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.PFDPOL & ((1<<PFDPOL_SZ) -1));
    Mel_Reg.C <<= VCOCUR_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.VCOCUR_TX & ((1<<VCOCUR_SZ) -1));
    Mel_Reg.C <<= BAND_SZ;        Mel_Reg.C |= (Mel_Reg.RegC.BAND & ((1<<BAND_SZ) -1));
    Mel_Reg.C <<= NR_SZ;          Mel_Reg.C |= (Mel_Reg.RegC.NR_403 & ((one<<NR_SZ) -1));	
  break;

  /*
  case MEL_REG_C_TAG_TX:
    Mel_Reg.C = 0x02;
    Mel_Reg.C <<= LNACTRL_SZ;     Mel_Reg.C |= (Mel_Reg.RegC.LNACTRL & ((1<<LNACTRL_SZ) -1));
    Mel_Reg.C <<= PFDPOL_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.PFDPOL & ((1<<PFDPOL_SZ) -1));
    Mel_Reg.C <<= VCOCUR_SZ;      Mel_Reg.C |= (Mel_Reg.RegC.VCOCUR_TX & ((1<<VCOCUR_SZ) -1));
    Mel_Reg.C <<= BAND_SZ;        Mel_Reg.C |= (Mel_Reg.RegC.BAND & ((1<<BAND_SZ) -1));
    Mel_Reg.C <<= NR_SZ;          Mel_Reg.C |= (Mel_Reg.RegC.NR_tag & ((one<<NR_SZ) -1));	
  break;
*/



  case MEL_REG_D_403:
    Mel_Reg.D = 0x03; 
    Mel_Reg.D <<= MODCTRL_SZ;     Mel_Reg.D |= (Mel_Reg.RegD.MODCTRL & ((1<<MODCTRL_SZ) -1));
    Mel_Reg.D <<= LDTM_SZ;        Mel_Reg.D |= (Mel_Reg.RegD.LDTM & ((1<<LDTM_SZ) -1));
    Mel_Reg.D <<= ERTM_SZ;        Mel_Reg.D |= (Mel_Reg.RegD.ERTM & ((1<<ERTM_SZ) -1));
    Mel_Reg.D <<= NT_SZ;          Mel_Reg.D |= (Mel_Reg.RegD.NT_403 & ((one<<NT_SZ) -1));    
  break;



  case MEL_REG_D_433:
    Mel_Reg.D = 0x03; 
    Mel_Reg.D <<= MODCTRL_SZ;     Mel_Reg.D |= (Mel_Reg.RegD.MODCTRL & ((1<<MODCTRL_SZ) -1));
    Mel_Reg.D <<= LDTM_SZ;        Mel_Reg.D |= (Mel_Reg.RegD.LDTM & ((1<<LDTM_SZ) -1));
    Mel_Reg.D <<= ERTM_SZ;        Mel_Reg.D |= (Mel_Reg.RegD.ERTM & ((1<<ERTM_SZ) -1));
    Mel_Reg.D <<= NT_SZ;          Mel_Reg.D |= (Mel_Reg.RegD.NT_433 & ((one<<NT_SZ) -1));    
  break;
/*
  case MEL_REG_D_TAG:
    Mel_Reg.D = 0x03; 
    Mel_Reg.D <<= MODCTRL_SZ;     Mel_Reg.D |= (Mel_Reg.RegD.MODCTRL & ((1<<MODCTRL_SZ) -1));
    Mel_Reg.D <<= LDTM_SZ;        Mel_Reg.D |= (Mel_Reg.RegD.LDTM & ((1<<LDTM_SZ) -1));
    Mel_Reg.D <<= ERTM_SZ;        Mel_Reg.D |= (Mel_Reg.RegD.ERTM & ((1<<ERTM_SZ) -1));
    Mel_Reg.D <<= NT_SZ;          Mel_Reg.D |= (Mel_Reg.RegD.NT_tag & ((one<<NT_SZ) -1));    
  break;
*/

}
}


/******************************************/
/* Switch between RX/TX IDLE and Standby  */
/*****************************************/
void Mel_Mode(u08 data)
{
  switch(data)
  {
    case MEL_STANDBY:
      //printf("\r\nMEL_STANDBY ");    
      Mel_Reg.RegA.OPMODE = 0x00;  
      Mel_LoadReg(MEL_REG_A);      
      break;
  
    case MEL_TX :
      //rprintf("\r\nMEL_TX ");          
      Mel_Reg.RegA.OPMODE = 0x02;
      Mel_LoadReg(MEL_REG_A);      
      break;
    
    case MEL_RX :
      //rprintf("\r\nMEL_RX ");                
      Mel_Reg.RegA.OPMODE = 0x01;    
      Mel_LoadReg(MEL_REG_A);      
      break;
    
    case MEL_IDLE:
      //rprintf("\r\nMEL_IDLE ");                
      Mel_Reg.RegA.OPMODE = 0x03;    
      Mel_LoadReg(MEL_REG_A);      
      break;    
   } 
}


void Mel_Init(void)
{

// Fro=7.1505
// Reg A
#ifdef MHZ433
	#if RF_FREQ == 7150500 
	Mel_Reg.RegA.RR         = 32;
	#endif
	#if RF_FREQ == 8000000 
	//PR=80, RR=80, NR=4232, NT=4339. 
	//  Mel_Reg.RegA.RR         = 34;
	Mel_Reg.RegA.RR         = 80;
#endif

#ifdef MHZ900
	Mel_Reg.RegA.RR         = 32;
#endif



#endif

  Mel_Reg.RegA.OPMODE     = 0;   // Standby Mode
  Mel_Reg.RegA.LNAGAIN    = 1;
  Mel_Reg.RegA.SETTOONE_A = 1;
  Mel_Reg.RegA.TXPOWER    = 3;
  Mel_Reg.RegA.PACTRL     = 1;
  Mel_Reg.RegA.LOCKMODE   = 0;
  Mel_Reg.RegA.CPCUR      = 0;
  Mel_Reg.RegA.MODSEL     = 1;
  Mel_Reg.RegA.DATAPOL    = 0;
  Mel_Reg.RegA.IDLE       = 1;

#ifdef MHZ433
	// Reg B
	#if RF_FREQ == 7150500 
	Mel_Reg.RegB.RT         = 32;
	#endif
	#if RF_FREQ == 8000000 
	//  Mel_Reg.RegB.RT         = 32;
	Mel_Reg.RegB.RT         = 80;
	#endif
#endif
#ifdef MHZ900
	Mel_Reg.RegB.RT         = 32;
#endif

#if RF_FREQ == 7150500 
  Mel_Reg.RegB.ROMIN      = 2;
  Mel_Reg.RegB.ROMAX      = 7;
#else
  Mel_Reg.RegB.ROMIN      = 0;
  Mel_Reg.RegB.ROMAX      = 0;
#endif

  Mel_Reg.RegB.OA2        = 0;
  Mel_Reg.RegB.AFC        = 0;
  Mel_Reg.RegB.LNAHYST    = 0;
  Mel_Reg.RegB.DELPLL     = 1;
  Mel_Reg.RegB.SETTOONE_B = 1;
  Mel_Reg.RegB.PKDET      = 0;

// Reg C
  /*
#ifdef MHZ433
	#if RF_FREQ == 7150500 
	//  Mel_Reg.RegC.NR         = 1894;
		Mel_Reg.RegC.NR_tag = 1894;
		Mel_Reg.RegC.NR_433 = 1894;  //433.92Mhz
		Mel_Reg.RegC.NR_403 = 1894; //4032;   //403.92Mhz
	#endif
	#if RF_FREQ == 8000000 
	//Mel_Reg.RegC.NR         = 1799;
		//Mel_Reg.RegC.NR_tag = 4232;
		//Mel_Reg.RegC.NR_tag = 4232;
		//Mel_Reg.RegC.NR_433 = 4232;  //433.92Mhz
		//Mel_Reg.RegC.NR_403 = 3932; //4032;   //403.92Mhz
	#endif
	Mel_Reg.RegC.BAND       = 0;

#endif
#ifdef MHZ900
		Mel_Reg.RegC.BAND       = 1;
		//Mel_Reg.RegC.NR_tag = 3660;
		Mel_Reg.RegC.NR_433 = 3660;
		Mel_Reg.RegC.NR_403 = 3660;
#endif

//  Mel_Reg.RegC.VCOCUR     = 0x03;
  Mel_Reg.RegC.VCOCUR_RX     = 0x00;
  Mel_Reg.RegC.VCOCUR_TX     = 0x03;
  Mel_Reg.RegC.PFDPOL     = 0;
  Mel_Reg.RegC.LNACTRL    = 1;
*/

// Reg D
#ifdef MHZ433
	#if RF_FREQ == 7150500 
//	Mel_Reg.RegD.NT         = 1942;
	Mel_Reg.RegD.NT_433	= 1942;
	Mel_Reg.RegD.NT_tag   	= 1942;
	Mel_Reg.RegD.NT_403 = 1942;
	#endif
	#if RF_FREQ == 8000000 
	//  Mel_Reg.RegD.NT         = 1736;
	//Mel_Reg.RegD.NT_433	= 4339;
	// Correct value for 433Mhz = 4339
	Mel_Reg.RegD.NT_433	=  4339; //Mel_Reg.RegC.NR_433 + RXTX_OFFSET;
	//Mel_Reg.RegD.NT_433	=  4100; //Mel_Reg.RegC.NR_433 + RXTX_OFFSET;

	// Correct value for 403Mhz = 4039
	Mel_Reg.RegD.NT_403 =  4300; //Mel_Reg.RegC.NR_403 + RXTX_OFFSET;
	#endif
#endif

  Mel_Reg.RegD.ERTM       = 0;
  Mel_Reg.RegD.LDTM       = 1;
  Mel_Reg.RegD.MODCTRL    = 1;



#ifdef MHZ900
	Mel_Reg.RegD.NT_433	= 3660;
	Mel_Reg.RegD.NT_tag   	= 3660;
	Mel_Reg.RegD.NT_403 = 3660;
#endif

// Reg C
#ifdef MHZ433
	#if RF_FREQ == 7150500 
	//  Mel_Reg.RegC.NR         = 1894;
		Mel_Reg.RegC.NR_tag = 1894;
		Mel_Reg.RegC.NR_433 = 1894;  //433.92Mhz
		Mel_Reg.RegC.NR_403 = 1894; //4032;   //403.92Mhz
	#endif
	#if RF_FREQ == 8000000 
	//Mel_Reg.RegC.NR         = 1799;
		//Mel_Reg.RegC.NR_tag = 4232;
		//Mel_Reg.RegC.NR_tag = 4232;
		//Mel_Reg.RegC.NR_433 = 4232;  //433.92Mhz
		//Mel_Reg.RegC.NR_403 = 3932; //4032;   //403.92Mhz
		
#define HIGHSIDE_INJECTION
#ifdef LOWSIDE_INJECTION
		Mel_Reg.RegC.NR_433 = Mel_Reg.RegD.NT_433 - RXTX_OFFSET;  //433.92Mhz
		Mel_Reg.RegC.NR_403 = Mel_Reg.RegD.NT_403 - RXTX_OFFSET;  //433.92Mhz
#endif
#ifdef HIGHSIDE_INJECTION
		Mel_Reg.RegC.NR_433 = Mel_Reg.RegD.NT_433 + RXTX_OFFSET;  //433.92Mhz
		Mel_Reg.RegC.NR_403 = Mel_Reg.RegD.NT_403 + RXTX_OFFSET;  //433.92Mhz
#endif


	#endif
	Mel_Reg.RegC.BAND       = 0;
#endif
#ifdef MHZ900
		Mel_Reg.RegC.BAND       = 1;
		//Mel_Reg.RegC.NR_tag = 3660;
		Mel_Reg.RegC.NR_433 = 3660;
		Mel_Reg.RegC.NR_403 = 3660;
#endif

//  Mel_Reg.RegC.VCOCUR     = 0x03;
  Mel_Reg.RegC.VCOCUR_RX     = 0x00;
  Mel_Reg.RegC.VCOCUR_TX     = 0x03;
  Mel_Reg.RegC.PFDPOL     = 0;
  Mel_Reg.RegC.LNACTRL    = 1;



// Load the Regs
  Mel_LoadReg(MEL_REG_A);
  Mel_LoadReg(MEL_REG_B);

}


int _tmain(int argc, _TCHAR* argv[])
{


int numclosed;

   /* Open for write */
if( (stream = fopen( "C:\\work\\VXTCommonCode\\melexisregval.h", "w+" )) == NULL )
      printf( "The file was not opened\n" );
  // else
      //printf( "The file  was opened\n" );


	rprintf("//Auto generated by Melexis TH71221 Register Calculator\r\n//Fairbridge Technologies 2004\r\n//J van Aardt\r\n\r\n");
	rprintf("\r\n // 19 July 2005");
	Mel_Init();

	rprintf("\r\n");
	Mel_Mode(MEL_STANDBY);
	rprintf("\r\n #define MEL_REGA_LOADVAL_STANDBY  0x");
    rprintfu08((Mel_Reg.A>>24) & 0xFF);rprintfu08((Mel_Reg.A>>16) & 0xFF);rprintfu08((Mel_Reg.A>>8) & 0xFF);rprintfu08((Mel_Reg.A) & 0xFF);  

	Mel_Reg.RegA.TXPOWER    = 0;
	Mel_Mode(MEL_TX);
    rprintf("\r\n #define MEL_REGA_LOADVAL_TX_LVL0       0x");
    rprintfu08((Mel_Reg.A>>24) & 0xFF);rprintfu08((Mel_Reg.A>>16) & 0xFF);rprintfu08((Mel_Reg.A>>8) & 0xFF);rprintfu08((Mel_Reg.A) & 0xFF);  

	Mel_Reg.RegA.TXPOWER    = 1;
	Mel_Mode(MEL_TX);
    rprintf("\r\n #define MEL_REGA_LOADVAL_TX_LVL1       0x");
    rprintfu08((Mel_Reg.A>>24) & 0xFF);rprintfu08((Mel_Reg.A>>16) & 0xFF);rprintfu08((Mel_Reg.A>>8) & 0xFF);rprintfu08((Mel_Reg.A) & 0xFF);  

	Mel_Reg.RegA.TXPOWER    = 2;
	Mel_Mode(MEL_TX);
    rprintf("\r\n #define MEL_REGA_LOADVAL_TX_LVL2       0x");
    rprintfu08((Mel_Reg.A>>24) & 0xFF);rprintfu08((Mel_Reg.A>>16) & 0xFF);rprintfu08((Mel_Reg.A>>8) & 0xFF);rprintfu08((Mel_Reg.A) & 0xFF);  

	Mel_Reg.RegA.TXPOWER    = 3;
	Mel_Mode(MEL_TX);
    rprintf("\r\n #define MEL_REGA_LOADVAL_TX_LVL3       0x");
    rprintfu08((Mel_Reg.A>>24) & 0xFF);rprintfu08((Mel_Reg.A>>16) & 0xFF);rprintfu08((Mel_Reg.A>>8) & 0xFF);rprintfu08((Mel_Reg.A) & 0xFF);  



	Mel_Mode(MEL_RX);
    rprintf("\r\n #define MEL_REGA_LOADVAL_RX       0x");
    rprintfu08((Mel_Reg.A>>24) & 0xFF);rprintfu08((Mel_Reg.A>>16) & 0xFF);rprintfu08((Mel_Reg.A>>8) & 0xFF);rprintfu08((Mel_Reg.A) & 0xFF);  
	

  rprintf("\r\n #define MEL_REGB_LOADVAL  0x");
  rprintfu08((Mel_Reg.B>>24) & 0xFF);rprintfu08((Mel_Reg.B>>16) & 0xFF);rprintfu08((Mel_Reg.B>>8) & 0xFF);rprintfu08((Mel_Reg.B) & 0xFF);  
  rprintf(" ");

  Mel_LoadReg(MEL_REG_C_TX_433);
  rprintf("\r\n #define MEL_REGC_LOADVAL_TX_433  0x");
  rprintfu08((Mel_Reg.C>>24) & 0xFF);rprintfu08((Mel_Reg.C>>16) & 0xFF);rprintfu08((Mel_Reg.C>>8) & 0xFF);rprintfu08((Mel_Reg.C) & 0xFF);  
  rprintf(" ");

  Mel_LoadReg(MEL_REG_C_TX_403);
  rprintf("\r\n #define MEL_REGC_LOADVAL_TX_403  0x");
  rprintfu08((Mel_Reg.C>>24) & 0xFF);rprintfu08((Mel_Reg.C>>16) & 0xFF);rprintfu08((Mel_Reg.C>>8) & 0xFF);rprintfu08((Mel_Reg.C) & 0xFF);  
  rprintf(" ");
/*
  Mel_LoadReg(MEL_REG_C_TAG_TX);
  rprintf("\r\n #define MEL_REGC_LOADVAL_TAG_TX  0x");
  rprintfu08((Mel_Reg.C>>24) & 0xFF);rprintfu08((Mel_Reg.C>>16) & 0xFF);rprintfu08((Mel_Reg.C>>8) & 0xFF);rprintfu08((Mel_Reg.C) & 0xFF);  
  rprintf(" ");
*/

  Mel_LoadReg(MEL_REG_C_RX_433);
  rprintf("\r\n #define MEL_REGC_LOADVAL_RX_433  0x");
  rprintfu08((Mel_Reg.C>>24) & 0xFF);rprintfu08((Mel_Reg.C>>16) & 0xFF);rprintfu08((Mel_Reg.C>>8) & 0xFF);rprintfu08((Mel_Reg.C) & 0xFF);  
  rprintf(" ");

  Mel_LoadReg(MEL_REG_C_RX_403);
  rprintf("\r\n #define MEL_REGC_LOADVAL_RX_403  0x");
  rprintfu08((Mel_Reg.C>>24) & 0xFF);rprintfu08((Mel_Reg.C>>16) & 0xFF);rprintfu08((Mel_Reg.C>>8) & 0xFF);rprintfu08((Mel_Reg.C) & 0xFF);  
  rprintf(" ");
/*
  Mel_LoadReg(MEL_REG_C_TAG_RX);
  rprintf("\r\n #define MEL_REGC_LOADVAL_TAG_RX  0x");
  rprintfu08((Mel_Reg.C>>24) & 0xFF);rprintfu08((Mel_Reg.C>>16) & 0xFF);rprintfu08((Mel_Reg.C>>8) & 0xFF);rprintfu08((Mel_Reg.C) & 0xFF);  
  rprintf(" ");
*/



  //-----------------------------------------------------

  Mel_LoadReg(MEL_REG_D_403);
  rprintf("\r\n #define MEL_REGD_LOADVAL_403  0x");
  rprintfu08((Mel_Reg.D>>24) & 0xFF);rprintfu08((Mel_Reg.D>>16) & 0xFF);rprintfu08((Mel_Reg.D>>8) & 0xFF);rprintfu08((Mel_Reg.D) & 0xFF);  
  rprintf(" ");


  Mel_LoadReg(MEL_REG_D_433);
  rprintf("\r\n #define MEL_REGD_LOADVAL_433  0x");
  rprintfu08((Mel_Reg.D>>24) & 0xFF);rprintfu08((Mel_Reg.D>>16) & 0xFF);rprintfu08((Mel_Reg.D>>8) & 0xFF);rprintfu08((Mel_Reg.D) & 0xFF);  
  rprintf(" ");

  /*
  Mel_LoadReg(MEL_REG_D_TAG);
  rprintf("\r\n #define MEL_REGD_LOADVAL_TAG  0x");
  rprintfu08((Mel_Reg.D>>24) & 0xFF);rprintfu08((Mel_Reg.D>>16) & 0xFF);rprintfu08((Mel_Reg.D>>8) & 0xFF);rprintfu08((Mel_Reg.D) & 0xFF);  
  rprintf(" ");
*/

	 fprintf(stream,"\r\n\r\n");
     if(fclose( stream ) )
      printf( "The file  was not closed\n" );

		return 0;
}

