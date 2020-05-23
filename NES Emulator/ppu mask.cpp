#include "ppu.h"
#include "bits.h"

///////////////////////////////////////////////////////
// PPU Mask ($2001) -> Write Only
///////////////////////////////////////////////////////
// 76543210
// ||||||||
// |||||||+------Grayscale(0: normal color; 1: produce a monochrome display)
// ||||||+-------1: Show background in leftmost 8 pixels of screen; 0: Hide
// |||||+--------1: Show sprites in leftmost 8 pixels of screen; 0: Hide
// ||||+---------1: Show background
// |||+----------1: Show sprites
// ||+-----------Intensify reds(and darken other colors)
// |+------------Intensify greens(and darken other colors)
// + ------------Intensify blues(and darken other colors)

PPU::PPU_MASK PPU::ppu_mask;				// $2001 (Write Only)

bool PPU::PPU_MASK::Get_Greyscale_Mode()
{
	// Greyscale Mode
	//              76543210
	// 0x1 = Binary 00000001     0th Bit
	if (ppu.ppu_mask.reg & BIT_0)
		return true;   // Greyscale
	else
		return false;  // Color
	return false;
}

bool PPU::PPU_MASK::Get_Background_Clipping()
{
	// Background Clipping
	//              76543210
	// 0x2 = Binary 00000010     1st Bit
	if (ppu.ppu_mask.reg & BIT_1)
		return true; // No Clipping
	else
		return false; // Hide in left 8-pixel column
	return false;
}

bool PPU::PPU_MASK::Get_Sprite_Clipping()
{
	// Sprite Clipping
	//              76543210
	// 0x4 = Binary 00000100     2nd Bit
	if (ppu.ppu_mask.reg & BIT_2)
		return true; // No Clipping
	else
		return false; // Hide in left 8-pixel column
	return false;
}

bool PPU::PPU_MASK::Get_Background_Visibility()
{
	// Background Visibility
	//              76543210
	// 0x8 = Binary 00001000     3rd Bit
	if (ppu.ppu_mask.reg & BIT_3)
		return true; // Displayed
	else
		return false; // Not Displayed
	return false;
}

bool PPU::PPU_MASK::Get_Sprite_Visibility()
{
	// Sprite Visibility
	//               76543210
	// 0x10 = Binary 00010000     4th Bit
	if (ppu.ppu_mask.reg & BIT_4)
		return true; // Displayed
	else
		return false; // Not Displayed
	return false;
}

bool PPU::PPU_MASK::Get_Intensify_Reds()
{
	// Intensify Reds
	//               76543210
	// 0x20 = Binary 00100000     5th Bit
	if (ppu.ppu_mask.reg & BIT_5)
		return true; // Intensify
	else
		return false; // Don't intensify
	return false;
}

bool PPU::PPU_MASK::Get_Intensify_Greens()
{
	// Intensify Greens
	//               76543210
	// 0x40 = Binary 01000000     6th Bit
	if (ppu.ppu_mask.reg & BIT_6)
		return true; // Intensify
	else
		return false; // Don't intensify
	return false;
}

bool PPU::PPU_MASK::Get_Intensify_Blues()
{
	// Intensify Blues
	//               76543210
	// 0x80 = Binary 10000000     7th Bit
	if (ppu.ppu_mask.reg & BIT_7)
		return true; // Intensify
	else
		return false; // Don't intensify
	return false;
}