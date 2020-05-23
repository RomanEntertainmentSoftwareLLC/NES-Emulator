#include "PPU_Control_Register_1.h"

///////////////////////////////////////////////////////
//PPU Control Register 1 ($2000)
///////////////////////////////////////////////////////

ushort PPU_CONTROL_REGISTER_1::Get_Name_Table_Address(byte data)
{
	//Name Table Address
	//             76543210
	//0x1 = Binary 00000001		0th Bit
	//0x2 = Binary 00000010     1st Bit
	switch (data & (BIT_0 + BIT_1))
	{
	case 0x0: return 0x2000; break;
	case 0x1: return 0x2400; break;
	case 0x2: return 0x2800; break;
	case 0x3: return 0x2C00; break;
	}
	return 0;
}

byte PPU_CONTROL_REGISTER_1::Get_Amount_To_Increment(byte data)
{
	//Amount To Increment
	//             76543210
	//0x4 = Binary 00000100     2nd Bit
	if (data & BIT_2)
		return 32;
	else
		return 1;
	return 0;
}

ushort PPU_CONTROL_REGISTER_1::Get_Sprite_Pattern_Table_Address(byte data)
{
	//Sprite Pattern Table Address
	//             76543210
	//0x8 = Binary 00001000      3rd Bit
	if (data & BIT_3)
		return 0x1000;
	else
		return 0x0;
	return 0;
}

ushort PPU_CONTROL_REGISTER_1::Get_Background_Pattern_Table_Address(byte data)
{
	//Background Pattern Table Address
	//              76543210
	//0x10 = Binary 00010000      4th Bit
	if (data & BIT_4)
		return 0x1000;
	else
		return 0x0;
	return 0;
}

byte PPU_CONTROL_REGISTER_1::Get_Sprite_Size(byte data)
{
	//Sprite Size
	//              76543210
	//0x20 = Binary 00100000     5th Bit
	if (data & BIT_5)
		return 16; //Sprite Tile Size (In Pixels) = 8x16
	else
		return 8;  //Sprite Tile Size (In Pixels) = 8x8
	return 0;
}

byte PPU_CONTROL_REGISTER_1::Get_PPU_Master_Slave_Select(byte data)
{
	//PPU Selection: Note - This is unused by the NES.
	//              76543210
	//0x40 = Binary 01000000     6th Bit
	if (data & BIT_6)
		return 1; //Slave
	else
		return 0; //Master
	return 0;
}

byte PPU_CONTROL_REGISTER_1::Get_Execute_NMI_On_VBlank(byte data)
{
	//Execute NMI On VBlank
	//              76543210
	//0x80 = Binary 10000000     7th Bit
	if (data & BIT_7)
		return 1;
	else
		return 0;
	return 0;
}

void PPU_CONTROL_REGISTER_1::Write(ushort &temp_vram_address_register, byte data)
{
	this->name_table_address               = this->Get_Name_Table_Address(data);
	this->amount_to_increment              = this->Get_Amount_To_Increment(data);
	this->sprite_pattern_table_address     = this->Get_Sprite_Pattern_Table_Address(data);
	this->background_pattern_table_address = this->Get_Background_Pattern_Table_Address(data);
	this->sprite_size                      = this->Get_Sprite_Size(data);
	this->ppu_master_slave_select          = this->Get_PPU_Master_Slave_Select(data);
	this->execute_nmi_on_vblank            = this->Get_Execute_NMI_On_VBlank(data);
	//t: ...BA.. ........ = d : ......BA

	//Where (BIT_0 + BIT_1 + BIT_2  + BIT_3  +
	//       BIT_4 + BIT_5 + BIT_6  + BIT_7  +
	//	     BIT_8 + BIT_9 + BIT_12 + BIT_13 +
	//	     BIT_14) = 0x73FF = 111 0011 1111 1111

	//Where (BIT_0 + BIT_1) = 0x3 = 0000 0011

	//	BA in the data is shifted 10 bits to the left and compared to the missing 
	//	binary space 00 in 0x73FF in the temp_vram_address_register.
	temp_vram_address_register = (temp_vram_address_register & (BIT_0 + BIT_1 + BIT_2  + BIT_3  +
										                        BIT_4 + BIT_5 + BIT_6  + BIT_7  +
										                        BIT_8 + BIT_9 + BIT_12 + BIT_13 +
										                        BIT_14)) | (data & (BIT_0 + BIT_1)) << 10;
}