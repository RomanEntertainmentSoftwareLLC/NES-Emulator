#include "6502 cpu memory.h"
#include "MMC.h"
#include "Controls.h"
#include "ppu.h"
#include "bits.h"
#include "mappers.h"
#include "Main.h"

byte Current_1 = 0;
byte Current_2 = 0;

byte CPU_Memory::Read_PRGROM(ushort address) {
	return memory_bank[address];
}

byte CPU_Memory::Read_SRAM(ushort address) {
	return memory_bank[address];
}

byte CPU_Memory::Read_EXP_ROM(ushort address) {
	return 0;
}

byte CPU_Memory::Read_CPU_Test_Mode(ushort address) {
	return 0;
}

byte CPU_Memory::Read_IO_Registers(ushort address) {
	byte outByte = 0;
	int data = 0;

	switch (address) {
	case 0x4016: { //Controller 1
		if (controller_1_strobe == 1) {
			//ReadMemory8_Inline = Controller_1(Controller_1_Count)
			//Controller_1_Count = (Controller_1_Count + 1) And 7

			outByte = Controller_1 & BIT_0;
			Controller_1 = (Controller_1 >> 1);
			Current_1++;
			data = outByte + 0x40;

			if (Current_1 == 24) {
				Current_1 = 0;
				controller_1_strobe = 0;
			}
		}
		return data;
		break;
	}
	case 0x4017: { //Controller 2
		if (controller_2_strobe == 1) {
			outByte = Controller_2 & BIT_0;
			Controller_2 = (Controller_2 >> 1);
			Current_2++;
			data = outByte + 0x40;

			if (Current_2 == 24) {
				Current_2 = 0;
				controller_2_strobe = 0;
			}

		}
		return data;
		break;
	}
	}
	return ppu_latch;
}

byte CPU_Memory::Read_PPU_Registers(ushort address) {
	address = 0x2000 + (address & 0x7);

	byte value = 0;

	switch (address) {
		case 0x2000: //PPU Control Register 1 ($2000) -> Write Only
			return ppu_latch;
		case 0x2001: //PPU Control Register 2 ($2001) -> Write Only
			return ppu_latch;
		case 0x2002: { //PPU Status Register ($2002) -> Read Only
			//	7  bit  0
			//	---- ----
			//	VSO. ....
			//	|||| ||||
			//	|||+-++++---- Least significant bits previously written into a PPU register
			//	|||				(due to register not being updated for this address)
			//	||+---------- Sprite overflow.The intent was for this flag to be set
			//	||				whenever more than eight sprites appear on a scanline, but a
			//	||				hardware bug causes the actual behavior to be more complicated
			//	||				and generate false positives as well as false negatives; see
			//	||				PPU sprite evaluation.This flag is set during sprite
			//	||				evaluation and cleared at dot 1 (the second dot) of the
			//	||				pre - render line.
			//	|+----------- Sprite 0 Hit.Set when a nonzero pixel of sprite 0 overlaps
			//	|				a nonzero background pixel; cleared at dot 1 of the pre - render
			//	|				line.Used for raster timing.
			//	+------------ Vertical blank has started(0: not in vblank; 1: in vblank).
			//					Set at dot 1 of line 241 (the line *after* the post - render
			//					line); cleared after reading $2002 and at dot 1 of the
			//					pre - render line.

			// Reading the status register will clear the high low toggle				(CHECK)
			// Reading the status register will also clear bit 7				        (CHECK)
			// It also clears the address latch used by PPUSCROLL and PPUADDR			(CHECK)
			// It does NOT clear the sprite 0 hit or overflow bit
			//			               
			///////////////////////////FALSE////////////////////
			//                         
			// So we will need 0x9f or 1001 1111‬
			// And as you can see, S and O are completely ignored.
			///////////////////////////FALSE/////////////////////
			// (Reason that above statement is false, or even 0x7f 01111111 and 0x1f 00011111
			// is because 0xff && 0x9f clears S and O
			//            0xff && 0x7f clears V
			//            0xff && 0x1f clears V S and O
			// So in conclusion, don't AND it. Just make the value have the ppu latch)  (CHECK)
			// V gets cleared only if it is set.										(CHECK)

			// Clear the high low toggle
			ppu.vram_address_high_low_toggle = false;

			// Changed the above algorithm to this one
			value = ppu_latch;
			value |= ppu.ppu_status.reg; // Set the value of the ppu status register to the overall value.

			// Reading the ppu status register will clear bit 7
			if (value & BIT_7) // If bit 7 (Vertical Blank) is set
				ppu.ppu_status.reg &= ~(1 << 7); // Clear bit 7. Leave the remaining 7 bits alone

			// Latch used by the PPUSCROLL($2005) and PPUADDR($2006) is also cleared
			ppu_latch = 0;

			return value;
		}
		case 0x2003:
			return ppu_latch;
		case 0x2004:
			//value = ppu_latch;
			//ppu_latch = SPR_RAM[Sprite_Address_Register];
			//Sprite_Address_Register = (Sprite_Address_Register + 1) & 0xFF;
			//return value;
			return SPR_RAM[(Sprite_Address_Register + 1) & 0xff];
		case 0x2005: { // VRAM address Register 1 (aka PPU Background Scrolling Offset, PPUSCROLL) -> Write Only x2
			return ppu_latch;
		}
		case 0x2006: { // VRAM address Register 2 (aka PPUADDR) -> Write Only x2
			return ppu_latch;
		}
		case 0x2007: {
			//---------------------------------------------------------------
			//			   VRAM address Register (15 Bit)
			//---------------------------------------------------------------
			//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
			//	    y  y  y  N  N  Y Y Y Y Y X X X X X
			//		|  |  |  |  |  | | | | | +-+-+-+-+-------Coarse X Scroll
			//		|  |  |  |  |  +-+-+-+-+-----------------Coarse Y Scroll
			//		|  |  |  +--+----------------------------Nametable Select
			//		+--+--+----------------------------------Fine Y Scroll

			value = ppu_latch;

			byte nametable_select = (ppu.vram_address_register.address >> 10) & (BIT_0 + BIT_1);

			if ((ppu.vram_address_register.address >= 0x0000) && (ppu.vram_address_register.address <= 0x1FFF)) { // Pattern Table 0 and 1
				ppu_latch = CHR_RAM[ppu.vram_address_register.address & 0x3FFF];
			}
			else if ((ppu.vram_address_register.address >= 0x2000) && (ppu.vram_address_register.address <= 0x2FFF)) { // Name Table 0-3 and Attribute Table 0-3
				ppu_latch = Name_Table_Array[Mirror_Number[nametable_select]][ppu.vram_address_register.address & 0x3FF];
			}
			else if ((ppu.vram_address_register.address >= 0x3000) && (ppu.vram_address_register.address <= 0x3EFF)) { // Mirrors of 0x2000-0x2EFF
				ppu_latch = CHR_RAM[ppu.vram_address_register.address & 0x3FFF];
			}
			else if ((ppu.vram_address_register.address >= 0x3F00) && (ppu.vram_address_register.address <= 0x3F1F)) { // Image Data and Sprite Data
				ppu_latch = CHR_RAM[ppu.vram_address_register.address & 0x3FFF];
			}
			else if ((ppu.vram_address_register.address >= 0x3F20) && (ppu.vram_address_register.address <= 0x3FFF)) { // Mirrors of 0x3F00-0x3F1F
				ppu_latch = CHR_RAM[ppu.vram_address_register.address & 0x3FFF];
			}
			else if ((ppu.vram_address_register.address >= 0x4000) && (ppu.vram_address_register.address <= 0xFFFF)) { // Mirrors of 0x0000-0x3FFF
				ppu_latch = CHR_RAM[ppu.vram_address_register.address & 0x3FFF];
			}

			ppu.vram_address_register.address = ppu.vram_address_register.address + ppu.ppu_controller.Get_Amount_To_Increment();
			ppu.vram_address_register.address = ppu.vram_address_register.address & 0x3FFF;

			return value;
		}
	}

	return 0;
}

byte CPU_Memory::Read_RAM(ushort address)
{
	return memory_bank[address & 0x7FF];
}
