#ifndef __SFR_DEF_H_
#define __SFR_DEF_H_
#define _nop_() __asm__("nop").

__sfr __at(0xC0) SCON1;
__sfr __at(0xC1) SBUF1;


////////////////////////////////////////////////////////////////////////////////
// SFR
/*
__sfr __at(0x80) P0;
__sfr __at(0x81) SP;
__sfr __at(0x82) DPL0;
__sfr __at(0x83) DPH0;
__sfr __at(0x84) DPL1;
__sfr __at(0x85) DPH1;
__sfr __at(0x86) DPS;
__sfr __at(0x87) PCON;


__sfr __at(0x88) TCON;
__sfr __at(0x89) TMOD;
__sfr __at(0x8A) TL0;
__sfr __at(0x8B) TL1;
__sfr __at(0x8C) TH0;
__sfr __at(0x8D) TH1;
__sfr __at(0x8E) CKCON;

__sfr __at(0x90) P1;
__sfr __at(0x91) EIF;
__sfr __at(0x92) WTST;
__sfr __at(0x93) DPX0;
__sfr __at(0x95) DPX1;

__sfr __at(0x98) SCON0;
__sfr __at(0x99) SBUF0;

__sfr __at(0xA0) P2;

__sfr __at(0xA8) IE;

__sfr __at(0xB0) P3;

__sfr __at(0xB8) IP;

__sfr __at(0xC0) SCON1;
__sfr __at(0xC1) SBUF1;

__sfr __at(0xC8) T2CON;
__sfr __at(0xC9) T2IF;
__sfr __at(0xCA) RLDL;
__sfr __at(0xCB) RLDH;
__sfr __at(0xCC) TL2;
__sfr __at(0xCD) TH2;
__sfr __at(0xCE) CCEN;

__sfr __at(0xD0) PSW;

__sfr __at(0xD8) WDCON;

__sfr __at(0xE0) ACC;

__sfr __at(0xE8) EIE;
__sfr __at(0xE9) STATUS;
__sfr __at(0xEA) MXAX;
__sfr __at(0xEB) TA;

__sfr __at(0xF0) B;

__sfr __at(0xF8) EIP;
*/
////////////////////////////////////////////////////////////////////////////////
// Bit define


/*  EIF  */
//sbit INT6F    = EIF^4;
//sbit INT5F    = EIF^3;
//sbit INT4F    = EIF^2;
//sbit INT3F    = EIF^1;
//sbit INT2F    = EIF^0;

#endif
