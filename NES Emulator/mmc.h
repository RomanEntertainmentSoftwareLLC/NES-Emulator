#pragma once

//Data Types
typedef unsigned char byte;
typedef unsigned short ushort;

//Function Prototypes
void Copy_Banks(int Destination, int Source, int Count);
void Bank_Switch(int Destination, int Source, int Count);
byte Mask_Bank_Address(byte bank);
void MMC_Reset();
void Setup_Banks();
void Select8KVROM(byte value);
void Select4KVROM(byte value, byte bank);
void Switch32kPrgRom(int start);
void Switch16kPrgRom(int start, int area);
void Switch8kChrRom(int start);

extern byte register_8000;
extern byte register_a000;
extern byte register_c000;
extern byte register_e000;

extern int PRG_ROM_BANK8;
extern int PRG_ROM_BANKA;
extern int PRG_ROM_BANKC;
extern int PRG_ROM_BANKE;

extern byte ppu_latch;

extern bool SpecialWrite6000;

extern int Current_MMC_Bank;

extern int Prevent_Bank_Switch_Source[8];
extern bool Allow_Xor;

extern int Current_PRG_ROM_Page[8];
extern int Current_CHR_ROM_Page[8];