#include "mappers.h"
#include "mmc.h"
#include "Main.h"
#include "ppu.h"

byte Register[4] = { 0 };
int Sequence = 0;
int Accumulator_Data = 0;
bool High_PRGROM_Switching;
bool Low_PRGROM_Switching;
int PRGROM_Switching_Mode = 0;
int VROM_Switching_Size = 0;
int Mapper1_Address;

bool Setup_Mapper()
{
	switch (nes_rom.Mapper)
	{
		case 0:
			if (nes_rom.CHR_ROM_Pages) 
				Select8KVROM(0);
			register_8000 = 0;    // 0000 0000
			register_a000 = 1;	  // 0000 0001
			register_c000 = 0xfe; // 1111 1110
			register_e000 = 0xff; // 1111 1111
			//Switch32kPrgRom(0);
			//Switch8kChrRom(0);
			Setup_Banks();
			break;
		case 1:
			Select8KVROM(0);
			register_8000 = 0;
			register_a000 = 1;
			register_c000 = 0xfe;
			register_e000 = 0xff;
			//Switch32kPrgRom(0);
			//Switch8kChrRom(0);
			Setup_Banks();
			Sequence = 0;
			Accumulator_Data = 0;
			memset(Register, 0, sizeof(Register));
			Register[0] = 0x1F;
			Register[3] = 0;
			break;
		case 2:
			if (nes_rom.CHR_ROM_Pages) Select8KVROM(0);
			register_8000 = 0;
			register_a000 = 1;
			register_c000 = 0xfe;
			register_e000 = 0xff;
			Setup_Banks();
			break;
		default:
			char Text[255];
			sprintf_s(Text, sizeof(Text), "Mapper %i is not supported. Please choose another NES ROM.", nes_rom.Mapper);
			MessageBox(NULL, Text, "NES Emulator", MB_ICONERROR | MB_OK);
			Free_ROM();
			return false;
			break;
	}

	return true;

}

void Mapper_Write(ushort address, byte data)
{
	switch (nes_rom.Mapper)
	{
		case 0: break;
		case 1: Mapper_1_Write(address, data); break;
		case 2: Mapper_2_Write(address, data); break;
	}
}

void Mapper_1_Write(ushort address, byte data) {
	int bank_select = 0;
 
    if (data & 0x80)
	{
		Register[0] = Register[0] | 0xC;
		Accumulator_Data = Register[(address / 0x2000) & 3];
		Sequence = 5;
	}
	else
	{
		if (data & 1) 
			Accumulator_Data = Accumulator_Data | static_cast<int>((pow(2.0, Sequence)));
		Sequence = Sequence + 1;
    }
    
	if (Sequence == 5)
	{
		Register[address / 0x2000 & 3] = Accumulator_Data;
		Sequence = 0;
		Accumulator_Data = 0;
        
		if (nes_rom.PRG_ROM_Pages == 0x20) //512k cart 
			bank_select = (Register[1] & 0x10) * 2;
		else //other carts
			bank_select = 0;

        if (Register[0] & 2) //enable panning
            nes_rom.Mirroring = static_cast<MIRRORING_ENUM>((Register[0] & 1) ^ 1);
        else //disable panning
			nes_rom.Mirroring = static_cast<MIRRORING_ENUM>(2);     
		
		Do_Mirroring();
     
		switch (nes_rom.Mirroring)
		{
			case HORIZONTAL:
			{
				Mirror_Offset = 0x400;
				break;
			}
			case VERTICAL:
			{
				Mirror_Offset = 0x800;
				break;
			}
			case SINGLE_SCREEN:
			{
				Mirror_Offset = 0;
				break;
			}
		}
   
		if ((Register[0] & 8) == 0) //base boot select $8000?
		{
			register_8000 = 4 * (Register[3] & 15) + bank_select;
			register_a000 = 4 * (Register[3] & 15) + bank_select + 1;
			register_c000 = 4 * (Register[3] & 15) + bank_select + 2;
			register_e000 = 4 * (Register[3] & 15) + bank_select + 3;
			Setup_Banks();
		}
		else if (Register[0] & 4) //16k banks
		{
			register_8000 = ((Register[3] & 15) * 2) + bank_select;
			register_a000 = ((Register[3] & 15) * 2) + bank_select + 1;
			register_c000 = 0xFE;
			register_e000 = 0xFF;
			Setup_Banks();
		}
        else //32k banks
		{
			register_8000 = 0;
			register_a000 = 1;
			register_c000 = ((Register[3] & 15) * 2) + bank_select;
            register_e000 = ((Register[3] & 15) * 2) + bank_select + 1;
			Setup_Banks();
        }
      
        if (Register[0] & 0x10) //4k
        {		  
            Select4KVROM(Register[1], 0);
            Select4KVROM(Register[2], 1);
  	    }
        else //8k
            Select8KVROM(Register[1] / 2);

	}

	/////////////////////

	////char Text[255];
	////sprintf_s(Text, sizeof(Text), "address: %x", address);
	////MessageBox(NULL, Text, "NES Emulator", MB_ICONEXCLAMATION | MB_OK);

	//if ((address >= 0x8000) && (address <= 0xFFFF))
	//{
	//	//  8000h-FFFFh
	//	//Bit 0  Serial data loaded to 5bit shift register (LSB = 1st write)
	//	//Bit 7  Clear 5bit shift register (1 = Reset, next write will be "1st write")
	//	//On fifth write, data in shift register is copied to Register 0..3 (depending on
	//	//upper address bits), and the shift register is automatically cleared.
	//
	//	//Load register ($8000-$FFFF) 
	//	//7  bit  0
	//	//---- ----
	//	//Rxxx xxxD
	//	//|		  |
	//	//|		  +-Data bit to be shifted into shift register, LSB first
	//	//+---------1: Reset shift register and write Control with(Control OR $0C),
	//	//locking PRG ROM at $C000 - $FFFF to the last bank.
	//}
	//if ((address >= 0x8000) && (address <= 0x9FFF))
	//{
	//	//8000h - 9FFFh  Register 0 - Configuration Register
	//	//	Bit0 - 1 Name Table Mirroring
	//	//		0, 1  Single - Screen(BLK0 only)
	//	//		2    Two - Screen Vertical Mirroring
	//	//		3    Two - Screen Horizontal Mirroring
	//	//	Bit2 - 3 PRG - Switching Mode(usually 3)
	//	//		0, 1	Switchable 32K Area at 8000h - FFFFh(via Register 3)
	//	//		2		Switchable 16K Area at C000h - FFFFh(via Register 3)
	//	//				And Fixed  16K Area at 8000h - BFFFh(always 1st 16K)
	//	//		3		Switchable 16K Area at 8000h - BFFFh(via Register 3)
	//	//				And Fixed  16K Area at C000h - FFFFh(always last 16K)
	//	//	Bit4   VROM Switching Size(for carts with VROM)
	//	//		0    Swap 8K of VROM at PPU 0000h
	//	//		1    Swap 4K of VROM at PPU 0000h and 1000h

	//	//Control (internal, $8000-$9FFF) 
	//	//4bit0
	//	//-----
	//	//CPPMM
	//	//|||||
	//	//|||++- Mirroring (0: one-screen, lower bank; 1: one-screen, upper bank;
	//	//|||               2: vertical; 3: horizontal)
	//	//|++--- PRG ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
	//	//|                         2: fix first bank at $8000 and switch 16 KB bank at $C000;
	//	//|                         3: fix last bank at $C000 and switch 16 KB bank at $8000)
	//	//+----- CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
	//	
	//	Mapper1_Address = 0x8000;

	//	if ((data & 0x80) == 0x80)
	//	{
	//		Accumulator_Data = Register[(address & 0x7FFF) >> 13];
	//		Sequence = 0;
	//	}
	//	else
	//	{
	//		if (data & 1) Accumulator_Data |= (1 << Sequence); //static_cast<int>(pow(2.0, Sequence));
	//		Sequence++;

	//		if (Sequence == 5)
	//		{
	//			Register[(address & 0x7FFF) >> 13] = Accumulator_Data;
	//			Sequence = 0;
	//			Accumulator_Data = 0;

	//			//Register 3 is restricted to sixteen 16K banks, cartridges with more than 256K
	//			//PRG ROM use Bit4 of Register 0 - 2 to expand the available memory area:
	//			//	Register 0, Bit 4
	//			//		<1024K carts>
	//			//			0 = Ignore 256K selection register 1
	//			//			1 = Acknowledge 256K selection register 1
	//			//	Register 1, Bit4 - 256K ROM Selection Register 0
	//			//		<512K carts>
	//			//			0 = Swap banks from first 256K of PRG
	//			//			1 = Swap banks from second 256K of PRG
	//			//		<1024K carts with bit 4 of register 0 off>
	//			//			0 = Swap banks from first 256K of PRG
	//			//			1 = Swap banks from third 256K of PRG
	//			//		<1024K carts with bit 4 of register 0 on>
	//			//			Low bit of 256K PRG bank selection
	//			//	Register 2, Bit4 - 256K ROM Selection Register 1
	//			//		<1024K carts with bit 4 of register 0 off>
	//			//			Store but ignore this bit(base 256K selection on 256K selection Reg 0)
	//			//		<1024K carts with bit 4 of register 0 on>
	//			//			High bit of 256K PRG bank selection

	//			if (nes_rom.PRG_ROM_Pages == 32) //512k cart
	//				bank_select = (Register[1] & BIT_4) * 2;
	//			else //other carts
	//				bank_select = 0;

	//			if ((Register[0] & (BIT_0 + BIT_1)) == 0)
	//				nes_rom.Mirroring = SINGLE_SCREEN;
	//			else if ((Register[0] & (BIT_0 + BIT_1)) == 1)
	//				nes_rom.Mirroring = SINGLE_SCREEN;
	//			else if ((Register[0] & (BIT_0 + BIT_1)) == 2)
	//				nes_rom.Mirroring = VERTICAL;
	//			else if ((Register[0] & (BIT_0 + BIT_1)) == 3)
	//				nes_rom.Mirroring = HORIZONTAL;

	//			if ((Register[0] & ((BIT_2 + BIT_3))) == 0)
	//				PRGROM_Switching_Mode = 0;
	//			else if ((Register[0] & ((BIT_2 + BIT_3))) == 4)
	//				PRGROM_Switching_Mode = 1;
	//			else if ((Register[0] & ((BIT_2 + BIT_3))) == 8)
	//				PRGROM_Switching_Mode = 2;
	//			else if ((Register[0] & ((BIT_2 + BIT_3))) == 12)
	//				PRGROM_Switching_Mode = 3;

	//			if (Register[0] & BIT_4) //4k
	//				VROM_Switching_Size = 4096;
	//			else //8k
	//				VROM_Switching_Size = 8192;

	//			Do_Mirroring();

	//			switch (nes_rom.Mirroring)
	//			{
	//				case HORIZONTAL: Mirror_Offset = 0x400; break;
	//				case VERTICAL: Mirror_Offset = 0x800; break;
	//				case SINGLE_SCREEN: Mirror_Offset = 0; break;
	//			}
	//		}
	//	}
	//}
	//else if ((address >= 0xA000) && (address <= 0xBFFF))
	//{
	//	//A000h-BFFFh  Register 1
	//	//	Bit4-0 Select 4K or 8K VROM bank at 0000h (4K and 8K Mode, see Reg0/Bit4)
	//	
	//	//CHR bank 0 (internal, $A000-$BFFF)
	//	//4bit0
	//	//-----
	//	//CCCCC
	//	//|||||
	//	//+++++- Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
	//	
	//	//MMC1 can do CHR banking in 4KB chunks.Known carts with CHR RAM have 8 KiB, so 
	//	//that makes 2 banks.RAM vs ROM doesn't make any difference for address lines. 
	//	//For carts with 8 KiB of CHR (be it ROM or RAM), MMC1 follows the common behavior 
	//	//of using only the low-order bits: the bank number is in effect ANDed with 1. 
	//	
	//	Mapper1_Address = 0xA000;

	//	if ((data & 0x80) == 0x80)
	//	{
	//		Accumulator_Data = Register[(address & 0x7FFF) >> 13];
	//		Sequence = 0;
	//	}
	//	else
	//	{
	//		if (data & 1) Accumulator_Data |= (1 << Sequence); //static_cast<int>(pow(2.0, Sequence));
	//		Sequence++;

	//		if (Sequence == 5)
	//		{
	//			Register[(address & 0x7FFF) >> 13] = Accumulator_Data;
	//			Sequence = 0;
	//			Accumulator_Data = 0;

	//			if (nes_rom.CHR_ROM_Pages > 0)
	//			{
	//				if (VROM_Switching_Size == 4096) //4k
	//				{
	//					Select4KVROM(Register[1], 0);
	//					Select4KVROM(Register[2], 1);
	//				}
	//				else if (VROM_Switching_Size == 8192) //8k
	//				{
	//					Select8KVROM(Register[1] >> 1);
	//				}
	//			}
	//		}
	//	}
	//}
	//else if ((address >= 0xC000) && (address <= 0xDFFF))
	//{
	//	//C000h-DFFFh  Register 2
	//	//	Bit4 - 0 Select 4K VROM bank at 1000h (used in 4K Mode only, see Reg0 / Bit4)
	//	
	//	//CHR bank 1 (internal, $C000-$DFFF)
	//	//
	//	//4bit0
	//	//-----
	//	//CCCCC
	//	//|||||
	//	//+++++- Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)

	//	Mapper1_Address = 0xC000;

	//	if ((data & 0x80) == 0x80)
	//	{
	//		Accumulator_Data = Register[(address & 0x7FFF) >> 13];
	//		Sequence = 0;
	//	}
	//	else
	//	{
	//		if (data & 1) Accumulator_Data |= (1 << Sequence); //static_cast<int>(pow(2.0, Sequence));
	//		Sequence++;

	//		if (Sequence == 5)
	//		{
	//			Register[(address & 0x7FFF) >> 13] = Accumulator_Data;
	//			Sequence = 0;
	//			Accumulator_Data = 0;

	//			if (nes_rom.CHR_ROM_Pages > 0)
	//			{
	//				if (VROM_Switching_Size == 4096)
	//				{
	//					Select4KVROM(Register[1], 1);
	//					Select4KVROM(Register[2], 1);
	//				}
	//			}
	//		}
	//	}
	//}
	//else if ((address >= 0xE000) && (address <= 0xFFFF))
	//{
	//	//E000h-FFFFh  Register 3
	//	//	Bit3 - 0 Select 16K or 2x16K ROM bank(see Reg0 / Bit3 - 2)
	//	//	Bit4   Unused ?

	//	//PRG bank (internal, $E000-$FFFF)
	//	//
	//	//4bit0
	//	//-----
	//	//RPPPP
	//	//|||||
	//	//|++++- Select 16 KB PRG ROM bank (low bit ignored in 32 KB mode)
	//	//+----- PRG RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A)

	//	Mapper1_Address = 0xE000;

	//	if ((data & 0x80) == 0x80)
	//	{
	//		//Register[0] = Register[0] | 0xC;
	//		Accumulator_Data = Register[(address & 0x7FFF) >> 13];
	//		Sequence = 0;
	//	}
	//	else
	//	{
	//		if (data & 1) Accumulator_Data |= (1 << Sequence); //static_cast<int>(pow(2.0, Sequence));
	//		Sequence++;

	//		if (Sequence == 5)
	//		{
	//			Register[(address & 0x7FFF) >> 13] = Accumulator_Data;
	//			Sequence = 0;
	//			Accumulator_Data = 0;

	//			if (PRGROM_Switching_Mode == 2 || PRGROM_Switching_Mode == 3) //16k banks
	//			{
	//				if (PRGROM_Switching_Mode == 3)
	//				{
	//					//Switch bank at 0x8000 and reset 0xC000
	//					//		3		Switchable 16K Area at 8000h - BFFFh(via Register 3)
	//					//				And Fixed  16K Area at C000h - FFFFh(always last 16K)
	//					register_8000 = ((Register[3] & 15) * 2) + bank_select;
	//					register_a000 = ((Register[3] & 15) * 2) + bank_select + 1;
	//					register_c000 = 0xFE;
	//					register_e000 = 0xFF;
	//					Setup_Banks();
	//				}
	//				else if (PRGROM_Switching_Mode == 2)
	//				{
	//					//Switch bank at 0xC000 and reset 0x8000
	//					//		2		Switchable 16K Area at C000h - FFFFh(via Register 3)
	//					//				And Fixed  16K Area at 8000h - BFFFh(always 1st 16K)
	//					register_8000 = 0;
	//					register_a000 = 1;
	//					register_c000 = ((Register[3] & 15) * 2) + bank_select + 2;
	//					register_e000 = ((Register[3] & 15) * 2) + bank_select + 3;
	//					Setup_Banks();
	//				}
	//			}
	//			else if (PRGROM_Switching_Mode == 0 || PRGROM_Switching_Mode == 1)//32k banks
	//			{
	//				//0, 1	Switchable 32K Area at 8000h - FFFFh(via Register 3)
	//				register_8000 = 4 * (Register[3] & 15) + bank_select;
	//				register_a000 = 4 * (Register[3] & 15) + bank_select + 1;
	//				register_c000 = 4 * (Register[3] & 15) + bank_select + 2;
	//				register_e000 = 4 * (Register[3] & 15) + bank_select + 3;
	//				Setup_Banks();
	//			}
	//		}
	//	}
	//}
}

void Mapper_2_Write(ushort address, byte data)
{
	register_8000 = (data * 2);
	register_a000 = register_8000 + 1;
	Setup_Banks();
}