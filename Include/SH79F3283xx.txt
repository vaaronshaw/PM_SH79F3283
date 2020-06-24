/*--------------------------------------------------------------------------
REG79F3283.H

Header file for generic SH79xx series microcontroller.
Copyright (c) 1996-2009 Sionwealth Electronic and Sinowealth Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/
#ifndef REG79F3283_H
#define REG79F3283_H
/* ------------------- BYTE Register-------------------- */
/* CPU */
sfr PSW = 0xD0;            
sfr ACC = 0xE0;
sfr B 	= 0xF0;
sfr AUXC = 0xF1;
sfr SP = 0x81;
sfr DPL = 0x82;
sfr DPH = 0x83;
sfr DPL1 = 0x84;
sfr DPH1 = 0x85;
sfr INSCON   = 0x86;
sfr XPAGE = 0xF7;

/* POWER  */
sfr PCON  = 0x87;
sfr SUSLO = 0x8E;

/* CLOCK */
sfr CLKCON = 0xB2;

/* SCM */
sfr SCMCON = 0xA1; 


/* FLASH */
sfr IB_OFFSET = 0xFB;
sfr IB_DATA = 0xFC;
sfr IB_CON1 = 0xF2;
sfr IB_CON2 = 0xF3;
sfr IB_CON3 = 0xF4;
sfr IB_CON4 = 0xF5;
sfr IB_CON5 = 0xF6;
sfr FLASHCON = 0xA7;
/* WDT */
sfr RSTSTAT = 0xB1;

/* INTERRUPT */
sfr IEN0= 0xA8;
sfr IEN1 = 0xA9;
sfr IENC = 0xBA;
sfr IENC1 = 0xBB;
sfr IPL0 = 0xB8;
sfr IPL1 = 0xB9;
sfr IPH0 = 0xB4;
sfr IPH1 = 0xB5;
sfr EXF0 = 0xE8;
sfr EXF1 = 0xD8;
sfr TCON = 0x88;
sfr EXCON = 0x8B;

/* TIMER 2/3/4/5*/
sfr T2CON = 0xC8;
sfr T2MOD = 0xC9;
sfr RCAP2L = 0xCA;
sfr RCAP2H = 0xCB;
sfr TL2 = 0xCC;
sfr TH2 = 0xCD;

sfr T3CON = 0x88;  /*bank1*/
sfr TL3 = 0x8C;  /*bank1*/
sfr TH3 = 0x8D;  /*bank1*/
sfr T4CON = 0xC8;  /*bank1*/
sfr TL4 = 0xCC;  /*bank1*/
sfr TH4 = 0xCD;  /*bank1*/
sfr T5CON = 0xC0; /*bank1*/
sfr TL5 = 0xCE; /*bank1*/
sfr TH5 = 0xCF; /*bank1*/
sfr SWTHL = 0x89;

/* buzzer */
sfr BUZCON = 0xBD;

/* PORT */
sfr P0 = 0x80;
sfr P1 = 0x90;
sfr P2 = 0xA0;
sfr P3 = 0xB0;
sfr P4 = 0xC0;
sfr P5 = 0x80;  /*bank1*/
sfr P0CR = 0xE1;
sfr P1CR = 0xE2;
sfr P2CR = 0xE3;
sfr P3CR = 0xE4;
sfr P4CR = 0xE5;
sfr P5CR = 0xE1;  /*bank1*/
sfr P0PCR = 0xE9;
sfr P1PCR = 0xEA;
sfr P2PCR = 0xEB;
sfr P3PCR = 0xEC;
sfr P4PCR = 0xED;
sfr P5PCR = 0xE9;  /*bank1*/

sfr P0OS = 0xEF;

/* EUART 0 */
sfr SCON = 0x98;
sfr SBUF = 0x99;
sfr SADDR = 0x9A;
sfr SADEN = 0x9B;	
sfr RXCON = 0x9F;
/* EUART 1 */		  /*BANK1 */
sfr SCON1= 0x98;
sfr SBUF1 = 0x99;
sfr SADDR1 = 0x9A;
sfr SADEN1 = 0x9B;	
sfr	SBRTH = 0x9D;
sfr	SBRTL = 0x9C;
sfr	BFINE = 0x9E;

/* ADC */
sfr ADCON = 0x93;
sfr ADT = 0x94;
sfr ADCH = 0x95;
sfr ADDL = 0x96;
sfr ADDH = 0x97;
sfr ADCON1 = 0x92;

/* PWM */
sfr PWMEN = 0xCF;
sfr PWMLO = 0xE7;
sfr PWMEN1 = 0xB7;
sfr PWM0C = 0xD2;
sfr PWM0PL = 0xD3;
sfr PWM0PH = 0xD4;
sfr PWM0DL = 0xD5;
sfr PWM0DH = 0xD6;
sfr PWM0DT = 0xD1;
sfr PWM1C = 0xD9;
sfr PWM1P = 0xDA;
sfr PWM1D = 0xDB;

/* LCD/LED */
sfr DISPCON = 0xAB;
sfr DISPCLK0 = 0xAC;
sfr DISPCLK1 = 0xAA;
sfr P0SS = 0xB6;
sfr P1SS = 0X9C;
sfr P2SS = 0x9D;
sfr P3SS = 0x9E;
sfr MODCON = 0xAE;

/* LPD */
sfr LPDCON = 0XB3;

/* SPI */
sfr SPCON = 0XA2;
sfr SPSTA = 0XF8;
sfr SPDAT = 0XA3;

/* Frequency Detect */
sfr INVCON = 0XC5;
sfr INVCTL = 0XC6;
sfr INVCTH = 0XC7;

/* CMP */
sfr CMP1CON = 0XC1;
sfr CMP2CON = 0XC2;
sfr CMPCON0 = 0XC3;
sfr CMPCON1 = 0XC4;

/*CRC */
sfr CRCCON = 0XFD;
sfr CRCDL = 0XF9;
sfr CRCDH = 0XFA;

/*--------------------------  BIT Register -------------------- */
/*  PSW   */
sbit CY = 0xD7;
sbit AC = 0xD6;
sbit F0 = 0xD5;
sbit RS1 = 0xD4;
sbit RS0 = 0xD3;
sbit OV = 0xD2;
sbit F1 = 0xD1;
sbit P = 0xD0;

/*  T2CON  */
sbit TF2 = 0xCF;
sbit EXF2 = 0xCE;
sbit RCLK = 0xCD;
sbit TCLK = 0xCC;
sbit EXEN2 = 0xCB;
sbit TR2 = 0xCA;
sbit C_T2 = 0xC9;
sbit CP_RL2 = 0xC8;

/*  TCON  */
sbit IE1 = 0x8B;
sbit IT1 = 0x8A;
sbit IE0 = 0x89;
sbit IT0 = 0x88;

/*  IPL0   */ 
sbit PADCL = 0xBE;
sbit PT2L = 0xBD;
sbit PS0L = 0xBC;
sbit PCMPL = 0xBB;
sbit PX1L = 0xBA;
sbit PT5L = 0xB9;
sbit PX0L = 0xB8;

/*  IEN0   */
sbit EA = 0xAF;
sbit EADC = 0xAE;
sbit ET2 = 0xAD;
sbit ES0 = 0xAC;
sbit ECMP = 0xAB;
sbit EX1 = 0xAA;
sbit ET5 = 0xA9;
sbit EX0 = 0xA8;

/*  EXF0  */
sbit IE2 = 0xE8;
sbit IE3 = 0xE9;
sbit IT20 = 0xEA;
sbit IT21 = 0xEB;
sbit IT30 = 0xEC;
sbit IT31 = 0xED;
sbit IT40 = 0xEE;
sbit IT41 = 0xEF;

/*  EXF1  */
sbit IF40 = 0xD8;
sbit IF41 = 0xD9;
sbit IF42 = 0xDA;
sbit IF43 = 0xDB;
sbit IF44 = 0xDC;
sbit IF45 = 0xDD;
sbit IF46 = 0xDE;
sbit IF47 = 0xDF;

/*  SCON  */
sbit SM0_FE = 0x9F;
sbit SM1_RXOV = 0x9E;
sbit SM2_TXCOL = 0x9D;
sbit REN = 0x9C;
sbit TB8 = 0x9B;
sbit RB8 = 0x9A;
sbit TI = 0x99;
sbit RI = 0x98;

/*  SCON  */		   /* bank1 */
sbit SM10_FE = 0x9F;
sbit SM11_RXOV1 = 0x9E;
sbit SM12_TXCOL1 = 0x9D;
sbit REN1 = 0x9C;
sbit TB18 = 0x9B;
sbit RB18 = 0x9A;
sbit TI1 = 0x99;
sbit RI1 = 0x98;

/* T3CON */
sbit T3CLKS0 = 0x88;  /* bank1 */
sbit TR3 = 0x8A;  /* bank1 */
sbit T3PS0 = 0x8C;  /* bank1 */
sbit T3PS1 = 0x8D;  /* bank1 */
sbit TF3 = 0x8F;  /* bank1 */

/* T4CON */
sbit T4CLKS = 0xC8;  /* bank1 */
sbit TR4 = 0xC9;  /* bank1 */
sbit T4M0 = 0xCA;  /* bank1 */
sbit T4M1 = 0xCB;  /* bank1 */
sbit T4PS0 = 0xCC;  /* bank1 */
sbit T4PS1 = 0xCD;  /* bank1 */
sbit TC4 = 0xCE;  /* bank1 */
sbit TF4 = 0xCF;  /* bank1 */

/* P0 */
sbit P0_0 = P0^0;
sbit P0_1 = P0^1;
sbit P0_2 = P0^2;
sbit P0_3 = P0^3;
sbit P0_4 = P0^4;
sbit P0_5 = P0^5;
sbit P0_6 = P0^6;
sbit P0_7 = P0^7;

/* P1 */
sbit P1_0 = P1^0;
sbit P1_1 = P1^1;
sbit P1_2 = P1^2;
sbit P1_3 = P1^3;
sbit P1_4 = P1^4;
sbit P1_5 = P1^5;
sbit P1_6 = P1^6;
sbit P1_7 = P1^7;

/* P2 */
sbit P2_0 = P2^0;
sbit P2_1 = P2^1;
sbit P2_2 = P2^2;
sbit P2_3 = P2^3;
sbit P2_4 = P2^4;
sbit P2_5 = P2^5;
sbit P2_6 = P2^6;
sbit P2_7 = P2^7;

/* P3 */
sbit P3_0 = P3^0;
sbit P3_1 = P3^1;
sbit P3_2 = P3^2;
sbit P3_3 = P3^3;
sbit P3_4 = P3^4;
sbit P3_5 = P3^5;
sbit P3_6 = P3^6;
sbit P3_7 = P3^7;

/* P4 */
sbit P4_0 = P4^0;
sbit P4_1 = P4^1;
sbit P4_2 = P4^2;
sbit P4_3 = P4^3;
sbit P4_4 = P4^4;
sbit P4_5 = P4^5;
sbit P4_6 = P4^6;
sbit P4_7 = P4^7;

/* P5 */
sbit P5_0 = P5^0;  /* bank1 */
sbit P5_1 = P5^1;  /* bank1 */
sbit P5_2 = P5^2;  /* bank1 */
sbit P5_3 = P5^3;  /* bank1 */
sbit P5_4 = P5^4;  /* bank1 */
sbit P5_5 = P5^5;  /* bank1 */

#endif


//sbit LED_DATA	= P3.0;OUT
//sbit LED_CLOCK	= P3.1;OUT
//sbit OUT1 		= P3.2;OUT
////sbit KEY1 		= P3.7;IN
//
//sfr TMP	= 0x29;
//sfr LED0 = 0x30;
//sfr LED1 = LED0+1;
//sfr LED2 = LED0+2;
//sfr LED3 = LED0+3;
//sfr LED4 = LED0+4;
//sfr LED5 = LED0+5;
//sfr LED6 = LED0+6;
//sfr LED7 = LED0+7;
//sfr LED8 = LED0+8;
//sfr LED9 = LED0+9;
//sfr LED10 = LED0+10;
//sfr LED11 = LED0+11;
//sfr LED12 = LED0+12;
//sfr LED13 = LED0+13;
//sfr LED14 = LED0+14;
//sfr LED15 = LED0+15;
//sfr BLACK = LED0+16;" "
//sfr DIS_P = LED0+17;"P"
