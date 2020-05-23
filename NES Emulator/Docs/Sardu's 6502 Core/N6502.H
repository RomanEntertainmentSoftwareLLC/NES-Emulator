#ifndef _N6502_
#define _N6502_

#define NMI_VECTOR   0xFFFA
#define RESET_VECTOR 0xFFFC
#define BRK_VECTOR   0xFFFE
#define IRQ_VECTOR   0xFFFE

#define F_CARRY  0x01
#define F_ZERO   0x02
#define F_INT    0x04
#define F_DEC    0x08
#define F_BREAK  0x10
#define F_RSRVD  0x20
#define F_OVER   0x40
#define F_SIGN   0x80


//asm funcs
extern "C" {
 int  __cdecl n6502_execute();
 void __cdecl n6502_nmi();
 void __cdecl n6502_int();
};


//nes 6502 cpu
class n6502cpu
{
 public:
 byte a,s,x,y,f;
 word pc;
 byte trapbadops;

 byte reserved[8];

 int cycles;
 int totalcycles;
 byte *PCBASE;

 byte *rom; //>=0x8000
 byte *ram; //<0x2000
 dword readtrap;  //reads from 0x2000-0x7FFF
 dword writetrap; //writes from 0x2000-0x7FFF

 dword breakpoint; //current breakpoint set


 void setram(byte *t) {ram=t;}
 void setrom(byte *t) {rom=t-0x8000;}


 //cycle stuff
 void resetcycles() {cycles=totalcycles=0;}
 int getcycleselapsed() {return totalcycles-cycles;}
 void abort() {totalcycles-=cycles; cycles=0;}

 //returns 1 on breakpoint, -1 on badopcode, 0 on success
 int execute(int c)
 {
  cycles=c; totalcycles+=c;

  #ifdef CONSOLE
  if (!breakpoint) return n6502_execute();
  while (c>0)
    {
     if (pc==breakpoint) return 1;
     cycles=1;
     if (n6502_execute()) return -1;
     c+=cycles;
    }
  return 0;
  #else
  return n6502_execute();
  #endif
 }

 int trace()
 {
  cycles=1; totalcycles++;
  return n6502_execute();
 }


 //int/reset stuff
 void donmi() {n6502_nmi();}
 void doint() {n6502_int();}

 //hard reset
 void hardreset()
 {
  breakpoint=0;
  a=x=y=0; f=F_RSRVD|F_ZERO; s=0xFF;
  cycles=totalcycles=0;
  pc=*((word *)&rom[RESET_VECTOR]);
 };

 //soft reset
 void softreset()
 {
  breakpoint=0;
  cycles=totalcycles=0;
  pc=*((word *)&rom[RESET_VECTOR]);
 };


 //debug
 void dumpreg();
 void dumpdisasm();
 void dumpinvalid();
 void debugkey(char k);
};

extern "C" n6502cpu ncpu; //main cpu

#endif


