#include "ppu.h"
#include "bits.h"

///////////////////////////////////////////////////////
// PPU Status ($2002) -> Read Only
///////////////////////////////////////////////////////
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

PPU::PPU_STATUS PPU::ppu_status;			// $2002 (Read Only)

bool PPU::PPU_STATUS::Get_Sprite_Overflow_Flag()
{
	//Sprite Overflow Flag
	//              76543210
	//0x40 = Binary 00100000     5th Bit
	if (ppu.ppu_status.reg & BIT_5)
		return true;
	else
		return false;
	return false;
}

bool PPU::PPU_STATUS::Get_Sprite_0_Hit_Flag()
{
	//Sprite 0 Hit Flag
	//              76543210
	//0x40 = Binary 01000000     6th Bit
	if (ppu.ppu_status.reg & BIT_6)
		return true;
	else
		return false;
	return false;
}

bool PPU::PPU_STATUS::Get_VBlank_Occuring_FLag()
{
	//VBlank Occuring_Flag
	//              76543210
	//0x80 = Binary 10000000     7th Bit
	if (ppu.ppu_status.reg & BIT_7)
		return true;
	else
		return false;
	return false;
}

int PPU::PPU_STATUS::Set_Sprite_Overflow_Flag()
{
	//Sprite Overflow Flag
	//              76543210
	//0x40 = Binary 00100000     5th Bit
	ppu.ppu_status.sprite_overflow = 1;
	return ppu.ppu_status.reg | BIT_5;
}

int PPU::PPU_STATUS::Set_Sprite_0_Hit_Flag()
{
	//Sprite 0 Hit Flag
	//              76543210
	//0x40 = Binary 01000000     6th Bit
	ppu.ppu_status.sprite_0_hit = 1;
	return ppu.ppu_status.reg | BIT_6;
}

int PPU::PPU_STATUS::Set_VBlank_Occuring_Flag()
{
	//VBlank Occuring_Flag
	//              76543210
	//0x80 = Binary 10000000     7th Bit
	ppu.ppu_status.vblank_started = 1;
	return ppu.ppu_status.reg | BIT_7;
}