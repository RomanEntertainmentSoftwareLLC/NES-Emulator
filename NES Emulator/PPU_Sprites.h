#pragma once

#include "Math.h"

//Function Prototypes
void Render_Sprites(int Scanline, bool Drawn_In_Foreground);

//Data Types
typedef unsigned char byte;
//typedef unsigned short ushort;

struct Sprite_Type
{
	Vector2D Position;					//The X and Y coordinates to the sprites
	byte Attributes;
	int Tile_Height;
	byte Tile_Index;
	int Pallete;
	bool Vertical_Flip;
	bool Horizontal_Flip;
	bool Drawn_In_Background;
	int Pattern_Table_Address;
};

//extern int Sprites_Crossed;
extern Sprite_Type Sprite[64];