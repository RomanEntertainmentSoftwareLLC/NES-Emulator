//n6502 test program
//Copyright 1997 Bloodlust Software

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

//set this if compiling as a win32 console app
#define WIN95

//set this if a disassembler is linked in
#define DISASM

//typedefs
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned dword;

#include "n6502\n6502.h"


#ifdef WIN95
#include <windows.h>
dword gettime() {return timeGetTime();}
#endif

#ifdef DISASM
int disasm(char *s,byte *base,unsigned short pc);
#endif



byte *RAM;
byte *ROM;


int vintenable=0;
int invblank=0;

//------------------------------------
//trap handlers

byte __fastcall NESread(dword a)
{
// printf("read[%04X]\n",a);
 switch(a)
 {
  case 0x2002: return invblank ? 0x80 : 0;
  default: return 0;
 }
}

int __fastcall NESwrite(byte d,dword a)
{
// printf("write[%04X]=%02X\n",a,d);
 switch (a)
 {
  case 0x2000: vintenable=d&0x80; break;
 }
 return 0;
}


//------------------------------------

//emulates one frame
int emulateframe()
{
 //during vblank
 invblank=1;
 if (vintenable) ncpu.donmi();
 if (ncpu.execute(30*115)) {ncpu.dumpinvalid(); return -1;}

 //during frame
 invblank=0;
 if (ncpu.execute(232*115)) {ncpu.dumpinvalid(); return -1;}

 return 0;
}

//------------------------------------


void main(int argc,char *arg[])
{
 #ifdef WIN95
 HANDLE hProc=GetCurrentProcess();
 SetPriorityClass(hProc,HIGH_PRIORITY_CLASS);
 #endif

 printf("n6502 test program\n");
 printf("Copyright 1997 Bloodlust Software\n");

 char romfile[32]="mario.rom";
 if (argc>1) strcpy(romfile,arg[1]);

 //read rom
 FILE *f=fopen(romfile,"rb");
 if (!f) return;
 ROM=(byte *)malloc(0x8000);
 switch (fread(ROM,0x4000,2,f))
 {
  case 0: printf("unable to read 16k from file\n"); return;
  case 1: memcpy(ROM+0x4000,ROM,0x4000);
 }
 fclose(f);
 printf("ROM allocated\n");

 //allocate RAM
 RAM=(byte *)malloc(0x8000);
 memset(RAM,0,0x8000);
 printf("RAM allocated\n");

 //setup cpu
 ncpu.setram(RAM);
 ncpu.setrom(ROM);
 ncpu.readtrap=(dword)NESread;
 ncpu.writetrap=(dword)NESwrite;
 ncpu.trapbadops=1;

 //reset cpu
 ncpu.hardreset();
 printf("CPU reset\n");

 ncpu.dumpdisasm();


 int done=0;
 do
 {
  char key=getch();

  switch (key)
  {
   case 'q':
   case 27: done=1;  break;

   case 'f': //frame
     emulateframe();
     printf("frame emulated\n");
    break;

   case 'h':
   {
    int frames=0;
    #ifdef WIN95
    unsigned start=gettime();
    #endif
    while (!kbhit()){if (emulateframe()) break; frames++;}
    while(kbhit()) getch();
    printf("frames: %d\n",frames);
    #ifdef WIN95
    unsigned length=gettime()-start;
    printf("%d frames/sec\n",frames*1000/length);
    #endif
   }
   break;

   default:  ncpu.debugkey(key);
  };
 } while (!done);
}





void n6502cpu::debugkey(char k)
{
 switch(k)
 {
  case 't': //trace
    if (trace()) dumpinvalid();
    dumpdisasm();
   break;
  case 'r': dumpreg(); break; //regs
  case 'i': dumpdisasm(); break; //disasm

  case 'n': //NMI
     donmi();
     printf("NMI triggered\n");
    break;

  case 'm': //INT
     doint();
     printf("INT triggered\n");
    break;

  case 'b': //set break point
    {
     printf("break: ");
     scanf("%X",&breakpoint);
    }
    break;

  #ifdef DISASM
  case 'y': //run to after current inst
  {
   char s[128];
   int len;
   if (pc<0x8000) len=disasm(s,ram,pc);
         else     len=disasm(s,rom,pc);
   breakpoint=pc+len;
  }
  #endif

  case 'g': //go
   do
   {
    unsigned cycles=5000000;
    #ifdef WIN95
    unsigned start=gettime();
    #endif
    if (execute(cycles)) {dumpdisasm(); break;}
    #ifdef WIN95
    unsigned time=gettime()-start;
    printf("%d cycles/sec\n",cycles/time*1000);
    #endif
   } while (!kbhit());
  break;

 }
}

void n6502cpu::dumpreg()
{
 printf("A:%02X X:%02X Y:%02X S:%02X FLAG: %c%c%c%c%c%c%c%c\n",
   a,x,y,s,
   (f&F_SIGN) ? 'S' : 's',
   (f&F_OVER) ? 'O' : 'o',
   (f&F_RSRVD) ? 'R' : 'r',
   (f&F_BREAK) ? 'B' : 'b',
   (f&F_DEC) ? 'D' : 'd',
   (f&F_INT) ? 'I' : 'i',
   (f&F_ZERO) ? 'Z' : 'z',
   (f&F_CARRY) ? 'C' : 'c'

   );
}

void n6502cpu::dumpdisasm()
{
 #ifdef DISASM
 char s[128]; s[0]=0;
 if (pc<0x8000) disasm(s,ram,pc);
       else     disasm(s,rom,pc);
 printf("%s\n",s);
 #else
 printf("[%04X] ???\n",pc);
 #endif
}


void n6502cpu::dumpinvalid()
{
 printf("invalid opcode: %02X\n",pc<0x8000 ? ram[pc] : rom[pc]);
}

























