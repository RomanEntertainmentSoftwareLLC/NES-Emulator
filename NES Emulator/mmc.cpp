#include "mmc.h"
#include "ppu.h"
#include "Main.h"
#include "bits.h"
#include "6502 cpu memory.h"


byte register_8000 = 0;
byte register_a000 = 0;
byte register_c000 = 0;
byte register_e000 = 0;

int PRG_ROM_BANK8 = 0;
int PRG_ROM_BANKA = 0;
int PRG_ROM_BANKC = 0;
int PRG_ROM_BANKE = 0;

byte ppu_latch = 0;

bool SpecialWrite6000 = false;

int Current_MMC_Bank = 0;

int Prevent_Bank_Switch_Source[8] = { 0 };
bool Allow_Xor = false;

int Current_PRG_ROM_Page[8] = { 0 };
int Current_CHR_ROM_Page[8] = { 0 };


void MMC_Reset()
{
	PRG_ROM_BANK8 = -1;
	PRG_ROM_BANKA = -1;
	PRG_ROM_BANKC = -1;
	PRG_ROM_BANKE = -1;

	for (int I = 0; I <= 7; I++)
	{
		Prevent_Bank_Switch_Source[I] = -1;
	}
}

void Copy_Banks(int Destination, int Source, int Count)
{
	if (nes_rom.Mapper == 4 && Allow_Xor == true)
	{
		int I = 0;
		for (I = 0; I < Count; I++)
		{
			memcpy(&CHR_RAM[(Destination + I) * 0x400], &nes_rom.CHR_ROM[(Source + I) * 0x400], 0x400);

		}
	}
	else
	{
		if (nes_rom.CHR_ROM.size() > 1)
			memcpy(&CHR_RAM[Destination * 0x400], &nes_rom.CHR_ROM[Source * 0x400], (Count * 0x400));
	}
}

void Bank_Switch(int Destination, int Source, int Count)
{
	int A = 0;
	int B = 0;
	int C = 0;

	Allow_Xor = (Count <= 2); //only xor with MMC3_ChrAddr with banks of 1 or 2k
	for (B = 0; B < Count; B++)
	{
		if (Prevent_Bank_Switch_Source[Destination + B] != Source + B)
		{
			C++; //we copy banks in groups, not 1 at a time. a little faster.
			Prevent_Bank_Switch_Source[Destination + B] = Source + B;
		}
		else
		{
			if (C > 0)
				Copy_Banks(Destination + A, Source + A, C);
			A = B + 1;
			C = 0;
		}
	}
	if (C > 0)
		Copy_Banks(Destination + A, Source + A, C);
}

byte Mask_Bank_Address(byte bank)
{

	if (bank >= nes_rom.PRG_ROM_Pages * 2)
	{
		byte i = 0xff;
		do
		{
			i /= 2;
		} while ((bank & i) >= nes_rom.PRG_ROM_Pages * 2);
		return bank & i;
	}
	else
		return bank;
}

byte Mask_CHR_ROM(byte Page, int Mask)
{
	int i;
	if (Mask == 0)
		Mask = 256;
	if (Mask & Mask - 1) //if mask is not a power of 2
	{
		i = 1;
		while (i < Mask); //find smallest power of 2 >= mask
		{
			i += i;
		} 
	}
	else
		i = Mask;

	i = (Page & (i - 1));
	if (i >= Mask)
		i = Mask - 1;
	return static_cast<byte>(i);
}

void Setup_Banks()
{
	register_8000 = Mask_Bank_Address(register_8000);
	register_a000 = Mask_Bank_Address(register_a000);
	register_c000 = Mask_Bank_Address(register_c000);
	register_e000 = Mask_Bank_Address(register_e000);

	//char Text[255];
	//sprintf_s(Text, sizeof(Text), "%i %i %i %i", register_8000, register_a000, register_c000, register_e000);
	//MessageBox(hWnd, Text, "NES Emulator", MB_ICONERROR | MB_OK);

	if (PRG_ROM_BANK8 != register_8000) memcpy(&cpu.memory_bank[0x8000], &nes_rom.PRG_ROM[register_8000 * 0x2000], 0x2000);
	if (PRG_ROM_BANKA != register_a000) memcpy(&cpu.memory_bank[0xA000], &nes_rom.PRG_ROM[register_a000 * 0x2000], 0x2000);
	if (PRG_ROM_BANKC != register_c000) memcpy(&cpu.memory_bank[0xC000], &nes_rom.PRG_ROM[register_c000 * 0x2000], 0x2000);
	if (PRG_ROM_BANKE != register_e000) memcpy(&cpu.memory_bank[0xE000], &nes_rom.PRG_ROM[register_e000 * 0x2000], 0x2000);

	PRG_ROM_BANK8 = static_cast<int>(register_8000);
	PRG_ROM_BANKA = static_cast<int>(register_a000);
	PRG_ROM_BANKC = static_cast<int>(register_c000);
	PRG_ROM_BANKE = static_cast<int>(register_e000);

	//register_8000 = Current_PRG_ROM_Page[0];
	//register_a000 = Current_PRG_ROM_Page[1];
	//register_c000 = Current_PRG_ROM_Page[2];
	//register_e000 = Current_PRG_ROM_Page[3];

	//memcpy(&memory_bank[0x8000], &nes_rom.PRG_ROM[register_8000 * 0x2000], 0x2000);
	//memcpy(&memory_bank[0xA000], &nes_rom.PRG_ROM[register_a000 * 0x2000], 0x2000);
	//memcpy(&memory_bank[0xC000], &nes_rom.PRG_ROM[register_c000 * 0x2000], 0x2000);
	//memcpy(&memory_bank[0xE000], &nes_rom.PRG_ROM[register_e000 * 0x2000], 0x2000);
}

void Select8KVROM(byte value)
{
	value = Mask_CHR_ROM(value, nes_rom.CHR_ROM_Pages);
	Bank_Switch(0, value * 8, 8);
	//MessageBox(hWnd, "BAM", "FUCK", MB_OK);
}

void Select4KVROM(byte value, byte bank)
{
	Current_MMC_Bank = bank;
	value = Mask_CHR_ROM(value, nes_rom.CHR_ROM_Pages * 2);
	Bank_Switch(bank * 4, value * 4, 4);
}

void Switch32kPrgRom(int start)
{
	int i;
	switch (nes_rom.PRG_ROM_Pages)
	{
		case BIT_1: start = (start & 0x7); break;
		case BIT_2: start = (start & 0xf); break;
		case BIT_3: start = (start & 0x1f); break;
		case BIT_4: start = (start & 0x3f); break;
		case BIT_5: start = (start & 0x7f); break;
		case BIT_6: start = (start & 0xff); break;
		case BIT_7: start = (start & 0x1ff); break;
	}
	for (i = 0; i < 8; i++)
	{
		Current_PRG_ROM_Page[i] = start + i;
		//char Text[255];
		//sprintf_s(Text, sizeof(Text), "%i", Current_PRG_ROM_Page[i]);
		//MessageBox(hWnd, Text, "NES Emulator", MB_ICONERROR | MB_OK);

	}
}

void Switch16kPrgRom(int start, int area)
{
	int i;
	switch (nes_rom.PRG_ROM_Pages)
	{
		case BIT_1: start = (start & 0x7); break;
		case BIT_2: start = (start & 0xf); break;
		case BIT_3: start = (start & 0x1f); break;
		case BIT_4: start = (start & 0x3f); break;
		case BIT_5: start = (start & 0x7f); break;
		case BIT_6: start = (start & 0xff); break;
		case BIT_7: start = (start & 0x1ff); break;
	}
	for (i = 0; i < 4; i++)
	{
		Current_PRG_ROM_Page[4 * area + i] = start + i;
	}
}

void Switch8kChrRom(int start)
{
	int i;
	switch (nes_rom.CHR_ROM_Pages)
	{
		case (BIT_1): start = (start & 0xf); break;
		case (BIT_2): start = (start & 0x1f); break;
		case (BIT_3): start = (start & 0x3f); break;
		case (BIT_4): start = (start & 0x7f); break;
		case (BIT_5): start = (start & 0xff); break;
		case (BIT_6): start = (start & 0x1ff); break;
	}
	for (i = 0; i < 8; i++)
	{
		Current_CHR_ROM_Page[i] = (start + i);
	}
}