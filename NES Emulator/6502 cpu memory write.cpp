#include "6502 cpu memory.h"
#include "MMC.h"
#include "Controls.h"
#include "ppu.h"
#include "bits.h"
#include "mappers.h"
#include "Main.h"

void CPU_Memory::Write_PRGROM(ushort address, byte data) {
	memory_bank[address] = data;
	Mapper_Write(address, data);
}

void CPU_Memory::Write_SRAM(ushort address, byte data) {
	if (SpecialWrite6000 == true)
		Mapper_Write(address, data);
	else
		memory_bank[address] = data;
	//Bank6000[address & 0x1FFF] = data;
}

void CPU_Memory::Write_EXP_Rom(ushort address, byte data) {
	memory_bank[address] = data;
	if (nes_rom.Mapper == 79)
		Mapper_Write(address, data);
}

void CPU_Memory::Write_CPU_Test_Mode(ushort address, byte data) {

}

void CPU_Memory::Write_IO_Registers(ushort address, byte data) {
	memory_bank[address] = data;

	switch (address) {
	case 0x4000: //pAPU Pulse 1 Control Register
				 //$4000 / $4004   DDLC VVVV   Duty (D), envelope loop / length counter disable (L), constant volume (C), volume/envelope (V)
		break;
	case 0x4001: //pAPU Pulse 1 Ramp Control Register
				 //$4001 / $4005   EPPP NSSS   Sweep unit: enabled (E), period (P), negate (N), shift (S)
		break;
	case 0x4002: //pAPU Pulse 1 Fine Tune (FT) Register
				 //$4002 / $4006   TTTT TTTT   Timer low (T)
		break;
	case 0x4003: //pAPU Pulse 1 Coarse Tune (CT) Register
				 //$4003 / $4007   LLLL LTTT   Length counter load (L), timer high (T)
		break;
	case 0x4004:
		break;
	case 0x4005:
		break;
	case 0x4006:
		break;
	case 0x4007:
		break;
	case 0x4008:
		break;
	case 0x4009:
		break;
	case 0x400A:
		break;
	case 0x400B:
		break;
	case 0x400C:
		break;
	case 0x400D:
		//Not used at all. Noise channel only uses $400C, $400E, and $400F according to all the documentation I've read.
		break;
	case 0x400E:
		break;
	case 0x400F:
		break;
	case 0x4010:
		break;
	case 0x4011:
		break;
	case 0x4012:
		break;
	case 0x4013:
		break;
	case 0x4014: { //SPR-RAM DMA ($4014)
		for (int I = 0; I <= 255; I++)
			SPR_RAM[I] = Read_Memory(data * 256 + I);
		break;
	}
	case 0x4015:
		break;
	case 0x4016: {
		if (controller_1_strobe == 1) {
			Controller_1 = Get_Controller1_Data();
		}

		if (controller_2_strobe == 1) {
			Controller_2 = Get_Controller2_Data();
		}

		if (controller_1_strobe == 0)
			controller_1_strobe = data & BIT_0;
		if (controller_2_strobe == 0)
			controller_2_strobe = data & BIT_0;
		break;
	}
	case 0x4017:
		break;
	}
}

void CPU_Memory::Write_PPU_Registers(ushort address, byte data)
{
	memory_bank[address] = data;
	ppu_latch = data;

	address = 0x2000 + (address & 0x7);

	Memory_Mirror(address, 0x2000, 0x2007, 0x2008, 0x3fff);

	switch (address) {
		case 0x2000: { // PPU Control Register 1 ($2000) -> Write Only

			// NOTE: After power/reset, writes to this register are ignored for about 30,000 cycles
			if (cpu.cycles >= 30000)
				ppu.ppu_controller.power_reset_30000_cycle_write = true;

			///////////////////////////////////////////////////////////////////////////////////////
			// VRAM address Register
			///////////////////////////////////////////////////////////////////////////////////////
			//		These are what the 15 bits represent:
			//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
			//	    y  y  y  N  N  Y Y Y Y Y X X X X X
			//		|  |  |  |  |  | | | | | +-+-+-+-+-------coarse X scroll
			//		|  |  |  |  |  +-+-+-+-+-----------------coarse Y scroll
			//		|  |  |  +--+----------------------------nametable select
			//		+--+--+----------------------------------fine Y scroll

			///////////////////////////////////////////////////////////////////////////////////////
			// Data
			///////////////////////////////////////////////////////////////////////////////////////
			// 7  bit  0
			// ---- ----
			// VPHB SINN
			// |||| ||||
			// |||| ||++---	Base nametable address
			// |||| ||		(0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
			// |||| |+-----	VRAM address increment per CPU read / write of PPUDATA
			// |||| |		(0: add 1, going across; 1: add 32, going down)
			// |||| +------	Sprite pattern table address for 8x8 sprites
			// ||||			(0: $0000; 1: $1000; ignored in 8x16 mode)
			// |||+--------	Background pattern table address(0: $0000; 1: $1000)
			// || +-------- Sprite size(0: 8x8 pixels; 1: 8x16 pixels)
			// | +---------	PPU master / slave select
			// |			(0: read backdrop from EXT pins; 1: output color on EXT pins)
			// + ---------- Generate an NMI at the start of the
			//				vertical blanking interval(0: off; 1: on)

			// temp:  1  1  1  0  0  1  1   1  1  1  1  1  1  1  1 = data : . . . . . . 1 1
			//	     14 13 12 11 10  9  8   7  6  5  4  3  2  1  0          7 6 5 4 3 2 1 0

			// Where (BIT_0 + BIT_1 + BIT_2  + BIT_3  +
			//        BIT_4 + BIT_5 + BIT_6  + BIT_7  +
			//	      BIT_8 + BIT_9 + BIT_12 + BIT_13 +
			//	      BIT_14) = 0x73FF = 111 0011 1111 1111

			// Where (BIT_0 + BIT_1) = 0x3 = 0000 0011

			// 11 (binary bits) in the data is shifted 10 bits to the left and compared to the missing 
			// binary space 00 in 0x73FF in the temp_vram_address_register.

			ppu.ppu_controller.reg = data;
			ppu.ppu_controller.base_name_table_address = data & (BIT_0 + BIT_1);
			ppu.ppu_controller.vram_address_increment = (data & BIT_2) >> 2;
			ppu.ppu_controller.sprite_pattern_table_address = (data & BIT_3) >> 3;
			ppu.ppu_controller.background_pattern_table_address = (data & BIT_4) >> 4;
			ppu.ppu_controller.sprite_size = (data & BIT_5) >> 5;
			ppu.ppu_controller.ppu_master_slave_select = (data & BIT_6) >> 6;
			ppu.ppu_controller.generate_nmi = (data & BIT_7) >> 7;

			//if (ppu.ppu_controller.power_reset_30000_cycle_write == true) {
				ppu.temp_vram_address_register.address = (ppu.temp_vram_address_register.address & (BIT_0 + BIT_1 + BIT_2 + BIT_3 +
					BIT_4 + BIT_5 + BIT_6 + BIT_7 +
					BIT_8 + BIT_9 + BIT_12 + BIT_13 +
					BIT_14)) | (ppu.ppu_controller.base_name_table_address << 10);
			//}

			break;
		}
		case 0x2001: { // PPU Control Register 2 ($2001) -> Write Only
			// 7  bit  0
			// ---- ----
			// BGRs bMmG
			// |||| ||||
			// |||| |||+-	Greyscale(0: normal color, 1 : produce a greyscale display)
			// |||| ||+--	1: Show background in leftmost 8 pixels of screen, 0 : Hide
			// |||| |+---	1 : Show sprites in leftmost 8 pixels of screen, 0 : Hide
			// |||| +----	1 : Show background
			// |||+------	1 : Show sprites
			// ||+-------	Emphasize red
			// |+--------	Emphasize green
			// +---------	Emphasize blue

			ppu.ppu_mask.reg =									data;
			ppu.ppu_mask.greyscale_enabled =					(data & BIT_0);
			ppu.ppu_mask.show_background_at_leftmost_pixels =	(data & BIT_1) >> 1;
			ppu.ppu_mask.show_sprites_at_leftmost_pixels =		(data & BIT_2) >> 2;
			ppu.ppu_mask.background_enabled =					(data & BIT_3) >> 3;
			ppu.ppu_mask.sprites_enabled =						(data & BIT_4) >> 4;
			ppu.ppu_mask.emphasize_red =						(data & BIT_5) >> 5;
			ppu.ppu_mask.emphasize_green =						(data & BIT_6) >> 6;
			ppu.ppu_mask.emphasize_blue =						(data & BIT_7) >> 7;

			break;
		}
		case 0x2002: { // PPU Status Register ($2002) -> Read only
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
			ppu.ppu_status.reg =				data;
			ppu.ppu_status.sprite_overflow =	(data & BIT_5) >> 5;
			ppu.ppu_status.sprite_0_hit =		(data & BIT_6) >> 6;
			ppu.ppu_status.vblank_started =		(data & BIT_7) >> 7;

			ppu_latch = data;
			break;
		}
		case 0x2003: { // SPR-RAM address Register ($2003) -> Write Only
			Sprite_Address_Register = data;
			break;
		}
		case 0x2004: { // SPR-RAM I/O Register
			SPR_RAM[Sprite_Address_Register] = data;
			Sprite_Address_Register = (Sprite_Address_Register + 1) & 0xff;

			//////////////////Sprite_Address_Register = (Sprite_Address_Register + 1) & 0xff;
			//////////////////SPR_RAM[Sprite_Address_Register] = data;
			break;
		}
		case 0x2005: { // VRAM address Register 1 (aka PPU Background Scrolling Offset, PPUSCROLL) -> Write Only x2
			// This register is used to change the scroll position, that is, to tell the PPU which pixel of the nametable
			// selected through PPUCTRL should be at the top left corner of the rendered screen. Typically, this register
			// is written to during vertical blanking, so that the next frame starts rendering from the desired location,
			// but it can also be modified during rendering in order to split the screen. Changes made to the vertical
			// scroll during rendering will only take effect on the next frame.

			// After reading PPUSTATUS to reset the address latch, write the horizontal and vertical scroll offsets here
			// just before turning on the screen:

			// BIT PPUSTATUS
			// ; possibly other code goes here
			// LDA cam_position_x
			// STA PPUSCROLL
			// LDA cam_position_y
			// STA PPUSCROLL

			// Horizontal offsets range from 0 to 255. "Normal" vertical offsets range from 0 to 239, while values of 240 to 255
			// are treated as -16 through -1 in a way, but tile data is incorrectly fetched from the attribute table.

			// By changing the values here across several frames and writing tiles to newly revealed areas of the nametables,
			// one can achieve the effect of a camera panning over a large background.

			if (ppu.vram_address_high_low_toggle == false) {
				///////////////////////////////////////////////////////////////////////////////
				// First Write (Tile X):
				///////////////////////////////////////////////////////////////////////////////
				// temp:  1  1  1  1  1  1  1    1  1  1  0  0  0  0  0 = data : 1 1 1 1 1 . . .
				//	     14 13 12 11 10  9  8    7  6  5  4  3  2  1  0          7 6 5 4 3 2 1 0

				// Where (BIT_5  + BIT_6  + BIT_7  + BIT_8 + 
				//        BIT_9  + BIT_10 + BIT_11 + BIT_12 +
				//	      BIT_13 + BIT_14) = 0x7FE0 = 111 1111 1110 0000

				// 11111 in the data is shifted 3 bits to the right and compared to the missing 
				// binary space 00000 in 0x7FE0 in the temp_vram_address_register.

				ppu.temp_vram_address_register.address = (ppu.temp_vram_address_register.address & (BIT_5 + BIT_6 + BIT_7 + BIT_8 +
					BIT_9 + BIT_10 + BIT_11 + BIT_12 +
					BIT_13 + BIT_14)) | (data >> 3);

				// fine x scroll:  0  0  0  0  0  0  0    0  0  0  0  0  1  1  1 = data : . . . . . 1 1 1
				//	    (3 bits)  14 13 12 11 10  9  8    7  6  5  4  3  2  1  0          7 6 5 4 3 2 1 0
				// No shifting is needed
				loopy_x = data & (BIT_0 + BIT_1 + BIT_2);

				// Toggle the high low toggle to true
				ppu.vram_address_high_low_toggle = true;
			}
			else {
				///////////////////////////////////////////////////////////////////////////////
				// Second Write (Tile Y):
				///////////////////////////////////////////////////////////////////////////////
				// There are two sections of this second write we need to tackle on this temp vram address register:
				// temp:  0  0  0  1  1  0  0   0  0  0  1  1  1  1  1 = data : 1 1 1 1 1 1 1 1
				//	     14 13 12 11 10  9  8   7  6  5  4  3  2  1  0          7 6 5 4 3 2 1 0

				// The first thing is the Coarse Y Scroll section (5 bits) which are bits 5, 6, 7, 8, and 9
				// The data's bits 3, 4, 5, 6, and 7 will be shifted 2 bits to the right to compare the missing 
				// binary space 00000 in 0x7C1F or 111 1100 0001 1111

				// The second thing is the Fine Y Scroll section (3 bits) which are bits 12, 13, and 14.
				// The data's bits 0, 1, and 2 will be shifted 12 bits to the left to compare the missing
				// binary space 000 in 0x0FFF or 000 1111 1111 1111

				///////////////////////////////////////////////////////////////////////////////
				// Section 1 of Second Write: Coarse Y Scroll
				///////////////////////////////////////////////////////////////////////////////
				// temp:  1  1  1  1  1  X  X   X  X  X  1  1  1  1  1 = data : 1 1 1 1 1 . . .
				//	     14 13 12 11 10  9  8   7  6  5  4  3  2  1  0          7 6 5 4 3 2 1 0

				// Where (BIT_0  + BIT_1  + BIT_2  + BIT_3  + 
				//        BIT_4  + BIT_10 + BIT_11 + BIT_12 +
				//	      BIT_13 + BIT_14) = 0x7C1F = 111 1100 0001 1111

				// Where (BIT_3 + BIT_4 + BIT_5 + BIT_6 +
				//        BIT_7) = 0xF8 = 1111 1000

				//	11111 in the data is shifted 2 bits to the left and compared to the missing 
				//	binary space 00000 in 0x7C1F in the temp_vram_address_register.
				ppu.temp_vram_address_register.address = (ppu.temp_vram_address_register.address & (BIT_0 + BIT_1 + BIT_2 + BIT_3 +
					BIT_4 + BIT_10 + BIT_11 + BIT_12 +
					BIT_13 + BIT_14)) | ((data & (BIT_3 + BIT_4 + BIT_5 + BIT_6 +
						BIT_7)) << 2);

				///////////////////////////////////////////////////////////////////////////////
				// Section 2 of Second Write: Fine Y Scroll
				///////////////////////////////////////////////////////////////////////////////

				// temp:  0  0  0  1  1  1  1   1  1  1  1  1  1  1  1 = data : . . . . . 1 1 1
				//	     14 13 12 11 10  9  8   7  6  5  4  3  2  1  0          7 6 5 4 3 2 1 0

				// Where (BIT_0 + BIT_1 + BIT_2  + BIT_3  +
				//        BIT_4 + BIT_5 + BIT_6  + BIT_7  +
				//	      BIT_8 + BIT_9 + BIT_10 + BIT_11 + = 0x0FFF = 000 1111 1111 1111

				// Where (BIT_0 + BIT_1 + BIT_2) = 0x7 = 0000 0111

				// 111 in the data is shifted 12 bits to the left and compared to the missing 
				// binary space 000 in 0x0FFF in the temp_vram_address_register.
				ppu.temp_vram_address_register.address = (ppu.temp_vram_address_register.address & (BIT_0 + BIT_1 + BIT_2 + BIT_3 +
					BIT_4 + BIT_5 + BIT_6 + BIT_7 +
					BIT_8 + BIT_9 + BIT_10 + BIT_11)) | ((data & (BIT_0 + BIT_1 + BIT_2)) << 12);
				// Toggle the high low toggle to false
				ppu.vram_address_high_low_toggle = false;
			}
			break;
		}
		case 0x2006: { // VRAM address Register 2 (aka PPUADDR) -> Write Only x2
			// Because the CPU and the PPU are on separate buses, neither has direct access to the other's memory.The CPU writes
			// to VRAM through a pair of registers on the PPU. First it loads an address into PPUADDR, and then it writes repeatedly
			// to PPUDATA to fill VRAM.

			// After reading PPUSTATUS to reset the address latch, write the 16-bit address of VRAM you want to access here, upper byte
			// first. For example, to set the VRAM address to $2108:

			// lda #$21
			// STA PPUADDR
			// lda #$08
			// STA PPUADDR

			// Valid addresses are $0000-$3FFF; higher addresses will be mirrored down.

			// Note:
			// Access to PPUSCROLL and PPUADDR during screen refresh produces interesting raster effects; the starting position
			// of each scanline can be set to any pixel position in nametable memory.

			if (ppu.vram_address_high_low_toggle == false) {
				///////////////////////////////////////////////////////////////////////////////
				// First Write:
				///////////////////////////////////////////////////////////////////////////////
				// temp: 1  0  0  0  0  0  0   1  1  1  1  1  1  1  1 = data : . . 1 1 1 1 1 1
				//	    14 13 12 11 10  9  8   7  6  5  4  3  2  1  0          7 6 5 4 3 2 1 0

				// Where (BIT_0 + BIT_1 + BIT_2 + BIT_3 +
				//        BIT_4 + BIT_5 + BIT_6 + BIT_7 +
				//        BIT_14) = 0x40FF = 100 0000 1111 1111

				// Where (BIT_0 + BIT_1 + BIT_2 + BIT_3 +
				//        BIT_4 + BIT_5) = 0x003F = 000 0000 0011 1111

				// 111111 in the data is shifted 8 bits to the left and compared to the missing 
				// binary space 000000 in 0x40FF in the temp_vram_address_register.
				ppu.temp_vram_address_register.address = (ppu.temp_vram_address_register.address & (BIT_0 + BIT_1 + BIT_2 + BIT_3 +
																			BIT_4 + BIT_5 + BIT_6 + BIT_7 +
																			BIT_14)) | ((data & (BIT_0 + BIT_1 + BIT_2 + BIT_3 +
																				BIT_4 + BIT_5)) << 8);
				ppu.temp_vram_address_register.address &= ~(1 << 14); // Clear the 14th bit. NOT HAVING THIS FUCKS UP TMNT!!!

				// Toggle the high low toggle to true
				ppu.vram_address_high_low_toggle = true;
			}
			else {
				///////////////////////////////////////////////////////////////////////////////
				// Second Write:
				///////////////////////////////////////////////////////////////////////////////
				// temp:  1  1  1  1  1  1  1   0  0  0  0  0  0  0  0 = data : 1 1 1 1 1 1 1 1
				//	     14 13 12 11 10  9  8   7  6  5  4  3  2  1  0          7 6 5 4 3 2 1 0

				//Where (BIT_8  + BIT_9  + BIT_10 + BIT_11 +
				//       BIT_12 + BIT_13 + BIT_14) = 0x7F00 = 111 1111 0000 0000

				// 11111111 in the data is compared to the missing 
				// binary space 00000000 in 0x7F00 in the temp_vram_address_register.

				ppu.temp_vram_address_register.address = (ppu.temp_vram_address_register.address & (BIT_8 + BIT_9 + BIT_10 + BIT_11 +
					BIT_12 + BIT_13 + BIT_14)) | data;
				// VRAM now equals Temp VRAM
				ppu.vram_address_register.address = ppu.temp_vram_address_register.address;

				// Toggle the high low toggle to false
				ppu.vram_address_high_low_toggle = false;
			}
			break;
		}
		case 0x2007: { //VRAM I/O Register

			//vram_address_register = vram_address_register & 0x3FFF;

			//if ((vram_address_register >= 0x2000) && (vram_address_register <= 0x2FFF)) { //3000 - 3EFF Mirroring of 2000 - 2FFF
			//	Name_Table_Array[Mirror_Number[(vram_address_register & 0xC00) / 0x400]][vram_address_register & 0x3FF] = data;
			//}
			//else {
			//	if (vram_address_register <= 0x3FFF)
			//		CHR_RAM[vram_address_register] = data;
			//	if ((vram_address_register & 0xFFEF) == 0x3F00)
			//		CHR_RAM[vram_address_register ^ 16] = data;
			//}

			//vram_address_register = vram_address_register + Get_Amount_To_Increment();
			//vram_address_high_low_toggle = false;

			//break;
		
			if ((ppu.vram_address_register.address >= 0x0000) && (ppu.vram_address_register.address <= 0x1FFF)) {													// Pattern Table 0 and 1
				CHR_RAM[ppu.vram_address_register.address] = data;
			}
			else if ((ppu.vram_address_register.address >= 0x2000) && (ppu.vram_address_register.address <= 0x2FFF)) {	
				// Name Table 0-3 and Attribute Table 0-3
				int name_table_number = ((ppu.vram_address_register.address & 0x2C00) & 0xC00) >> 10;
				Name_Table_Array[Mirror_Number[name_table_number]][ppu.vram_address_register.address & 0x3FF] = data;
				CHR_RAM[ppu.vram_address_register.address] = data;

				// Mirror it
				ppu.Memory_Mirror(ppu.vram_address_register.address, 0x2000, 0x2EFF, 0x3000, 0x3EFF);
				// ppu.Memory_Mirror(ppu.vram_address_register.address, 0x2000, 0x2FFF, 0x3000, 0x3FFF); // Crashes NESSTRESS AT PPU

				// Note: $3F00 - $3FFF will override the mirrors with Image and Sprite Palette data
				//       once this data is used
			}
			else if ((ppu.vram_address_register.address >= 0x3000) && (ppu.vram_address_register.address <= 0x3EFF)) {												// Mirrors 0x2000-0x2EFF
				// Formula is vram_address mod number_range + origin				
				ushort temp_vram_address_register = ppu.vram_address_register.address % 0x1000 + 0x2000;
				int name_table_number = ((temp_vram_address_register & 0x2C00) & 0xC00) >> 10;
				// Fixed NESStress "PPU $3000 Mirroring" at this line
				Name_Table_Array[Mirror_Number[name_table_number]][temp_vram_address_register & 0x3FF] = data;
			
				CHR_RAM[ppu.vram_address_register.address] = data;

				// Mirror it
				ppu.Memory_Mirror(temp_vram_address_register, 0x2000, 0x2EFF, 0x3000, 0x3EFF);
			}
			else if ((ppu.vram_address_register.address >= 0x3F00) && (ppu.vram_address_register.address <= 0x3F1F)) {												// Image Palette and Sprite Palette
				CHR_RAM[ppu.vram_address_register.address] = data;

				// Mirror it
				ppu.Memory_Mirror(ppu.vram_address_register.address, 0x3F00, 0x3F1F, 0x3F20, 0x3FFF);
			}
			else if ((ppu.vram_address_register.address >= 0x3F20) && (ppu.vram_address_register.address <= 0x3FFF)) {												// Mirrors 0x3F00-0x3F1F
				// Formula is vram_address mod mirror_chunk_size + origin	
				ushort temp_vram_address_register = ppu.vram_address_register.address % 20 + 0x3F00;

				CHR_RAM[ppu.vram_address_register.address] = data;

				// Mirror it
				ppu.Memory_Mirror(temp_vram_address_register, 0x3F00, 0x3F1F, 0x3F20, 0x3FFF);
			}

			// Palette related? Look into it
			if ((ppu.vram_address_register.address & 0x7FEF) == 0x3F00) {
				CHR_RAM[ppu.vram_address_register.address ^ 16] = data;
			}

			ppu.vram_address_register.address = ppu.vram_address_register.address + ppu.ppu_controller.Get_Amount_To_Increment();
			ppu.vram_address_register.address = ppu.vram_address_register.address & 0x3FFF;

			ppu.vram_address_high_low_toggle = false;

			break;
		}
	}
}

void CPU_Memory::Write_RAM(ushort address, byte data) {
	memory_bank[address & 0x07ff] = data;
	Memory_Mirror(address, 0x0000, 0x07ff, 0x0800, 0x1fff);
}