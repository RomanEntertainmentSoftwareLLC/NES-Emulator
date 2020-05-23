#include "PPU_Control_Register_2.h"

///////////////////////////////////////////////////////
//PPU Control Register 2 ($2001)
///////////////////////////////////////////////////////

byte PPU_CONTROL_REGISTER_2::Get_Monochrome_Mode(byte data)
{
	//Monochrome Mode
	//             76543210
	//0x1 = Binary 00000001     0th Bit
	if (data & BIT_0)
		return 1;   //Monochrome
	else
		return 0;  //Color
	return 0;
}

byte PPU_CONTROL_REGISTER_2::Get_Background_Clipping(byte data)
{
	//Background Clipping
	//             76543210
	//0x2 = Binary 00000010     1st Bit
	if (data & BIT_1)
		return 1; //No Clipping
	else
		return 0; //Hide in left 8-pixel column
	return 0;
}

byte PPU_CONTROL_REGISTER_2::Get_Sprite_Clipping(byte data)
{
	//Sprite Clipping
	//             76543210
	//0x4 = Binary 00000100     2nd Bit
	if (data & BIT_2)
		return 1; //No Clipping
	else
		return 0; //Hide in left 8-pixel column
	return 0;
}

byte PPU_CONTROL_REGISTER_2::Get_Background_Visibility(byte data)
{
	//Background Visibility
	//             76543210
	//0x8 = Binary 00001000     3rd Bit
	if (data & BIT_3)
		return 1; //Displayed
	else
		return 0; //Not Displayed
	return 0;
}

byte PPU_CONTROL_REGISTER_2::Get_Sprite_Visibility(byte data)
{
	//Sprite Visibility
	//              76543210
	//0x10 = Binary 00010000     4th Bit
	if (data & BIT_4)
		return 1; //Displayed
	else
		return 0; //Not Displayed
	return 0;
}

byte PPU_CONTROL_REGISTER_2::Get_Intensify_Reds(byte data)
{
	//Intensify Reds
	//              76543210
	//0x20 = Binary 00100000     5th Bit
	if (data & BIT_5)
		return 1; //Intensify
	else
		return 0; //Don't intensify
	return 0;
}

byte PPU_CONTROL_REGISTER_2::Get_Intensify_Greens(byte data)
{
	//Intensify Greens
	//              76543210
	//0x40 = Binary 01000000     6th Bit
	if (data & BIT_6)
		return 1; //Intensify
	else
		return 0; //Don't intensify
	return 0;
}

byte PPU_CONTROL_REGISTER_2::Get_Intensify_Blues(byte data)
{
	//Intensify Blues
	//              76543210
	//0x80 = Binary 10000000     7th Bit
	if (data & BIT_7)
		return 1; //Intensify
	else
		return 0; //Don't intensify
	return 0;
}

void PPU_CONTROL_REGISTER_2::Write(byte data)
{
	this->monochrome_mode       = this->Get_Monochrome_Mode(data);
	this->background_clipping   = this->Get_Background_Clipping(data);
	this->sprite_clipping       = this->Get_Sprite_Clipping(data);
	this->background_visibility = this->Get_Background_Visibility(data);
	this->sprite_visibility     = this->Get_Sprite_Visibility(data);
	this->intensify_reds        = this->Get_Intensify_Reds(data);
	this->intensify_greens      = this->Get_Intensify_Greens(data);
	this->intensify_blues       = this->Get_Intensify_Blues(data);
}