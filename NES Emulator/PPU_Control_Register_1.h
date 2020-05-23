#ifndef PPU_CONTROL_REGISTER_1_H
#define PPU_CONTROL_REGISTER_1_H

#include "Bits.h"

//Data Types
typedef unsigned char byte;
typedef unsigned short ushort;

class PPU_CONTROL_REGISTER_1
{
	///////////////////////////////////////////////////////
	//PPU Control Register 1 ($2000) -> Write Only
	///////////////////////////////////////////////////////
	//7 6 5 4 3 2 10
	//| | | | | | ||
	//| | | | | | ++-Base nametable address
	//| | | | | |		(0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
	//| | | | | +----VRAM address increment per CPU read / write of PPUDATA
	//| | | | |			(0: add 1, going across; 1: add 32, going down)
	//| | | | +------Sprite pattern table address for 8x8 sprites
	//| | | |			(0: $0000; 1: $1000; ignored in 8x16 mode)
	//| | | +--------Background pattern table address
	//| | |				(0: $0000; 1: $1000)
	//| | +----------Sprite size
	//| |               (0: 8x8; 1: 8x16)
	//| +------------PPU master / slave select
	//|					(0: read backdrop from EXT pins; 1: output color on EXT pins)
	//+ -------------Generate an NMI at the start of the vertical blanking interval
	//					(0: off; 1: on)


	///////////////////////////////////////////////////////
	//Function Prototypes
	///////////////////////////////////////////////////////
private:
	ushort Get_Name_Table_Address(byte data);
	byte   Get_Amount_To_Increment(byte data);
	ushort Get_Sprite_Pattern_Table_Address(byte data);
	ushort Get_Background_Pattern_Table_Address(byte data);
	byte   Get_Sprite_Size(byte data);
	byte   Get_PPU_Master_Slave_Select(byte data);
	byte   Get_Execute_NMI_On_VBlank(byte data);

public:
	void Write(ushort &temp_vram_address_register, byte data);


	///////////////////////////////////////////////////////
	//Variables
	///////////////////////////////////////////////////////
public:
	ushort name_table_address;					//Bit 0 & Bit 1
	byte amount_to_increment;					//Bit 2
	ushort sprite_pattern_table_address;		//Bit 3
	ushort background_pattern_table_address;	//Bit 4
	byte sprite_size;							//Bit 5
	byte ppu_master_slave_select;				//Bit 6
	byte execute_nmi_on_vblank;					//Bit 7
};

#endif