#ifndef PPU_CONTROL_REGISTER_2_H
#define PPU_CONTROL_REGISTER_2_H

#include "Bits.h"

//Data Types
typedef unsigned char byte;
typedef unsigned short ushort;

class PPU_CONTROL_REGISTER_2
{
	///////////////////////////////////////////////////////
	//PPU Control Register 2 ($2001) -> Write Only
	///////////////////////////////////////////////////////
	//--------------------------------------------------------------------------------
	//76543210
	//||||||||
	//|||||||+------Grayscale(0: normal color; 1: produce a monochrome display)
	//||||||+-------1: Show background in leftmost 8 pixels of screen; 0: Hide
	//|||||+--------1: Show sprites in leftmost 8 pixels of screen; 0: Hide
	//||||+---------1: Show background
	//|||+----------1: Show sprites
	//||+-----------Intensify reds(and darken other colors)
	//|+------------Intensify greens(and darken other colors)
	//+ ------------Intensify blues(and darken other colors)

	///////////////////////////////////////////////////////
	//Function Prototypes
	///////////////////////////////////////////////////////
private:
	byte Get_Monochrome_Mode(byte data);
	byte Get_Background_Clipping(byte data);
	byte Get_Sprite_Clipping(byte data);
	byte Get_Background_Visibility(byte data);
	byte Get_Sprite_Visibility(byte data);
	byte Get_Intensify_Reds(byte data);
	byte Get_Intensify_Greens(byte data);
	byte Get_Intensify_Blues(byte data);

public:
	void Write(byte data);

	///////////////////////////////////////////////////////
	//Variables
	///////////////////////////////////////////////////////
public:
	byte monochrome_mode;		//Bit 0
	byte background_clipping;	//Bit 1
	byte sprite_clipping;		//Bit 2
	byte background_visibility;	//Bit 3
	byte sprite_visibility;		//Bit 4
	byte intensify_reds;		//Bit 5
	byte intensify_greens;		//Bit 6
	byte intensify_blues;		//Bit 7
};

#endif