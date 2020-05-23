#include "ppu.h"
#include "bits.h"

///////////////////////////////////////////////////////
// PPU Controller ($2000) -> Write Only
///////////////////////////////////////////////////////
// 76543210
// ||||||||
// ||||||++------Base nametable address
// ||||||			(0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
// |||||+--------VRAM address increment per CPU read / write of PPUDATA
// |||||				(0: add 1, going across; 1: add 32, going down)
// |||| +--------Sprite pattern table address for 8x8 sprites
// ||||				(0: $0000; 1: $1000; ignored in 8x16 mode)
// |||+----------Background pattern table address
// |||				(0: $0000; 1: $1000)
// ||+-----------Sprite size(0: 8x8; 1: 8x16)
// ||
// |+------------PPU master / slave select
// |				(0: read backdrop from EXT pins; 1: output color on EXT pins)
// + ------------Generate an NMI at the start of the vertical blanking interval
// 					(0: off; 1: on)

PPU::PPU_CONTROLLER PPU::ppu_controller;	// $2000 (Write Only)

int PPU::PPU_CONTROLLER::Get_Name_Table_Address() {
	// Name Table address
	//              76543210
	// 0x1 = Binary 00000001	 0th Bit
	// 0x2 = Binary 00000010     1st Bit
	switch (ppu.ppu_controller.reg & (BIT_0 + BIT_1)) {
		case 0x0: return 0x2000; break;
		case 0x1: return 0x2400; break;
		case 0x2: return 0x2800; break;
		case 0x3: return 0x2C00; break;
	}
	return 0;
}

int PPU::PPU_CONTROLLER::Get_Amount_To_Increment() {
	// Amount To Increment
	//              76543210
	// 0x4 = Binary 00000100     2nd Bit
	if (ppu.ppu_controller.reg & BIT_2)
		return 32;
	else
		return 1;
	return 0;
}

int PPU::PPU_CONTROLLER::Get_Sprite_Pattern_Table_Address() {
	// Sprite Pattern Table Address
	//              76543210
	// 0x8 = Binary 00001000      3rd Bit
	if (ppu.ppu_controller.reg & BIT_3)
		return 0x1000;
	else
		return 0x0;
	return 0;
}

int PPU::PPU_CONTROLLER::Get_Background_Pattern_Table_Address() {
	// Background Pattern Table Address
	//               76543210
	// 0x10 = Binary 00010000      4th Bit
	if (ppu.ppu_controller.reg & BIT_4)
		return 0x1000;
	else
		return 0x0;
	return 0;
}

int PPU::PPU_CONTROLLER::Get_Sprite_Size() {
	// Sprite Size
	//               76543210
	// 0x20 = Binary 00100000     5th Bit
	if (ppu.ppu_controller.reg & BIT_5)
		return 16; // Sprite Tile Size (In Pixels) = 8x16
	else
		return 8;  // Sprite Tile Size (In Pixels) = 8x8
	return 0;
}

int PPU::PPU_CONTROLLER::Get_PPU_Selection() {
	// PPU Selection: Note - This is unused by the NES.
	//               76543210
	// 0x40 = Binary 01000000     6th Bit
	if (ppu.ppu_controller.reg & BIT_6)
		return 1; // Slave
	else
		return 0; // Master
	return 0;
}

bool PPU::PPU_CONTROLLER::Get_Execute_NMI_On_VBlank() {
	// Execute NMI On VBlank
	//               76543210
	// 0x80 = Binary 10000000     7th Bit
	if (ppu.ppu_controller.reg & BIT_7)
		return true;
	else
		return false;
	return false;
}