#include "ppu.h"
#include "Main.h"
#include "PPU_Sprites.h"
#include "PPU_Background.h"
#include "DX.h"
#include "bits.h"

PPU::OAM_ADDRESS PPU::oam_address;			// $2003 (Write Only)
PPU::OAM_DATA PPU::oam_data;				// $2004 (Read / Write)
PPU::PPU_SCROLL PPU::ppu_scroll;			// $2005 (Write Only x2)
PPU::PPU_ADDRESS PPU::ppu_address;			// $2006 (Write Only x2)
PPU::PPU_DATA PPU::ppu_data;				// $2007 (Read / Write)

PPU::VRAM_ADDRESS PPU::vram_address_register;
PPU::VRAM_ADDRESS PPU::temp_vram_address_register;
int PPU::cycles;
ushort PPU::ppu_cycle_on_current_scanline;
bool PPU::vram_address_high_low_toggle;
ushort PPU::scanline;
int PPU::frame_count;
bool PPU::odd_frame;

PPU ppu;

byte Sprite_Address_Register = 0; //$2003

byte Mirror_Number[4] = { 0 };
int Mirror_Offset = 0;

byte Name_Table_Array[4][0x400] = { 0 };
byte CHR_RAM[0x10000] = { 0 }; // actual ppu memory is from 0x0000 - 0x3fff. 0x4000-0xffff are mirrors
byte SPR_RAM[256] = { 0 };

byte Video_Buffer[256 * 240] = { 0 };
int Video_Buffer32[256 * 240] = { 0 };

bool Render_Frame = false;

byte Color_LUT[65536 * 8] = { 0 };
int Pallete[256] = { 0 };

int Horizontal_Scroll = 0;
int Vertical_Scroll = 0;

int Amount_To_Increment = 0;
int Name_Table_Address = 0;

//int vram_address_register = 0; //15 Bit VRAM address Register
//int temp_vram_address_register = 0; //15 Bit Temporary VRAM address Register

///////////////////////VRAM address Register/////////////////////
//		These are what the 15 bits represent:
//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
//	    y  y  y  N  N  Y Y Y Y Y X X X X X
//		|  |  |  |  |  | | | | | +-+-+-+-+-------coarse X scroll
//		|  |  |  |  |  +-+-+-+-+-----------------coarse Y scroll
//		|  |  |  +--+----------------------------nametable select
//		+--+--+----------------------------------fine Y scroll

int loopy_x = 0;


///////////////////////////////////////////////////////
//PPU Status Register ($2002) -> Read Only
///////////////////////////////////////////////////////
//--------------------------------------------------------------------------------
//76543210
//||||||||
//|||+++++------Least significant bits 
//|||					Least significant bits previously written into a PPU
//|||				register due to register not being updated for this address.
//||+-----------Sprite overflow. 
//||					The intent was for this flag to be set whenever more than
//||				eight sprites appear on a scanline, but a hardware bug causes
//||				the actual behavior to be more complicated and generate false
//||				positives as well as false negatives; see PPU sprite evaluation.
//||				This flag is set during sprite evaluation and cleared at dot 1
//||				(the second dot) of the pre-render line.
//|+------------Sprite 0 Hit.
//|						Set when a nonzero pixel of sprite 0 overlaps a nonzero 
//|					background pixel; cleared at dot 1 of the pre-render line.
//|					Used for raster timing.
//+-------------Vertical blank has started
//					(0: not in VBLANK; 1: in VBLANK).
//					Set at dot 1 of line 241 (the line *after* the post-render line.)
//				Cleared after reading $2002 and at dot 1 of the pre-render line.
//--------------------------------------------------------------------------------


PPU::PPU() {

}

PPU::~PPU() {

}

void PPU::Memory_Mirror(unsigned short address, unsigned short start_address_chunk, unsigned short end_address_chunk, unsigned short start_address_mirror, unsigned short end_address_mirror) {
	// Note: This is not a physical write to the address per say. It mearly mirrors the "already written" address to
	//       the missing locations in memory where its suppose to be mirrored!
	//
	// address: The address being written that will be mirrored within a mirror range
	//			Ex. If the range of the chunk is between 0x0000 and 0x07ff,
	//              the mirror is between 0x0800 and 0x1fff,
	//				and the address that was written to is at 0x0000, then
	//				the data at 0x0000 will be mirrored at 0x0800, 0x1000, and 0x1800.
	//			Note: the address must be in between the start address chunk and end address chunk or it won't make sense.
	// start_address_chunk: The beginning range of where the address was written to enlies.
	// end_address_chunk: The end range of where the address was written to enlies.
	// start_address_mirror: The beginning section of where to mirror the written address multiple times in chunks.
	// end_address_mirror: The end section of where to mirror the written address multiple times in chunks.

	if (address >= start_address_chunk && address <= end_address_chunk) {
		unsigned short mirrorChunkSize = (end_address_chunk - start_address_chunk) + 1;
		int offset = address % mirrorChunkSize;
		int numberOfMirrors = ((end_address_mirror - start_address_mirror) / mirrorChunkSize) + 1;
		int numberOfTimesMirrored = 0;

		if (numberOfMirrors > 0) {
			for (int i = 0; i < numberOfMirrors; i++) {
				CHR_RAM[start_address_mirror + offset + (mirrorChunkSize * i)] = CHR_RAM[start_address_chunk + offset + (mirrorChunkSize * i)];
				numberOfTimesMirrored++;
			}
		}
	}
}

///////////////////////////////////////////////////////
//Sprites
///////////////////////////////////////////////////////
//OAM (Object Attribute Memory)
//Up to 64 sprite positions and attributes are stored in OAM. Each sprite's information occupies four bytes.

//Byte 0
//Y position of top of sprite

//Sprite data is delayed by one scanline; you must subtract 1 from the sprite's Y coordinate before writing it here.
//Hide a sprite by writing any values in $EF-$FF here.

//-----------------------------------------------------------------------
//Byte 1
//Tile index number

//For 8x8 sprites, this is the tile number of this sprite within the pattern table selected in bit 3 of PPUCTRL ($2000).
//For 8x16 sprites, the PPU ignores the pattern table selection and selects a pattern table from bit 0 of this number.

//76543210
//||||||||
//|||||||+- Bank ($0000 or $1000) of tiles
//+++++++-- Tile number of top of sprite (0 to 254; bottom half gets the next tile)

//Thus, the pattern table memory map for 8x16 sprites looks like this:
//
//    $00: $0000-$001F
//    $01: $1000-$101F
//    $02: $0020-$003F
//    $03: $1020-$103F
//    $04: $0040-$005F
//    [...]
//    $FE: $0FE0-$0FFF
//    $FF: $1FE0-$1FFF
//
//-----------------------------------------------------------------------

//Byte 3
//X position of left side of sprite
//
//X-scroll values of F9-FF do NOT result in the sprite wrapping around to the left side of the screen.
//
//'-----------------------------------------------------------------------

//Sprite Attributes Binary Checks (Byte 2)
//------------------------------------------------------
//Based on everything2.com/title/NES+PPU
//http://wiki.nesdev.com/w/index.php/PPU_OAM

//x+$02: Sprite Attributes
//76543210
//||||||||
//||||||++- Palette (4 to 7) of sprite
//|||+++--- Unused
//||+------ Priority (0: in front of background; 1: behind background)
//|+------- Flip hoizontally?
//+-------- Flip vertically?

//Flipping does not change the position of the sprite's bounding box, just the position of pixels within the sprite.
//If, for example, a sprite covers (120, 130) through (127, 137), it'll still cover the same area when flipped.In
//8x16 mode, vertical flip affects the whole sprite as a unit, not the individual tiles; the odd-numbered tile of a
//vertically flipped sprite is drawn on top. This behavior differs from the behavior of the unofficial 16x32 and 32x64
//pixel sprite sizes on the Super NES, which vertically flip each square sub-region.

int Sprite_Attributes_Get_Pallete(int Sprite_Attributes)
{
	return 16 + (Sprite_Attributes & (BIT_0 + BIT_1)) * 4;
}

bool Sprite_Attributes_Get_Drawn_In_Background(int Sprite_Attributes)
{
	if (Sprite_Attributes & BIT_5)
		return true;
	else
		return false;
	return false;
}

bool Sprite_Attributes_Get_Horizontal_Flip(int Sprite_Attributes)
{
//Horizontal Flip
//              76543210
//0x40 = Binary 01000000     6th Bit
if (Sprite_Attributes & BIT_6)
		return true;
	else
		return false;
	return false;
}

bool Sprite_Attributes_Get_Vertical_Flip(int Sprite_Attributes)
{
//Vertical Flip
//              76543210
//0x80 = Binary 10000000     7th Bit
	if (Sprite_Attributes & BIT_7)
		return true;
	else
		return false;
	return false;
}

byte Get_Fine_Y(int address)
{
	//Gets the Fine Y values from the 15 Bit VRAM address Register

	//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	//	   (y  y  y) N  N  Y Y Y Y Y X X X X X
	//		|  |  |  |  |  | | | | | +-+-+-+-+-------coarse X scroll
	//		|  |  |  |  |  +-+-+-+-+-----------------coarse Y scroll
	//		|  |  |  +--+----------------------------nametable select
	//		+--+--+----------------------------------fine Y scroll

	//	   $7000 in binary is 0111000000000000
	//	   Where 111 is where the Fine Y values are found as shown above
	//     in Bits 12, 13, and 14. Fine Y's values range from 0 to 7, and
	//     are obtained by shifting those 3 bits to the right by 12, which
	//     results in 0000000000000111.
	return (address & (BIT_12 + BIT_13 + BIT_14)) >> 12;
}


int Set_Fine_Y(int address, byte data)
{
	//Sets the Fine Y values from the 15 Bit VRAM address Register

	//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	//	   (y  y  y) N  N  Y Y Y Y Y X X X X X
	//		|  |  |  |  |  | | | | | +-+-+-+-+-------coarse X scroll
	//		|  |  |  |  |  +-+-+-+-+-----------------coarse Y scroll
	//		|  |  |  +--+----------------------------nametable select
	//		+--+--+----------------------------------fine Y scroll

	return (address & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4 + BIT_5 + BIT_6 + BIT_7 + BIT_8 + BIT_9 + BIT_10 + BIT_11)) | ((data & (BIT_0 + BIT_1 + BIT_2)) << 12);
}

byte Get_Coarse_X(int address)
{
	//Gets the Coarse X values from the 15 Bit VRAM address Register

	//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	//	    y  y  y  N  N  Y Y Y Y Y(X X X X X)
	//		|  |  |  |  |  | | | | | +-+-+-+-+-------coarse X scroll
	//		|  |  |  |  |  +-+-+-+-+-----------------coarse Y scroll
	//		|  |  |  +--+----------------------------nametable select
	//		+--+--+----------------------------------fine Y scroll

	//	   $1F in binary is 0000000000011111
	//	   Where 11111 is where the Coarse X values are found as shown above
	//     in Bits 0, 1, 2, 3, 4. Coarse X values can range from 0 to 31. No
	//     shift is needed to obtain the value.
	return (address & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4));
}

int Set_Coarse_X(int address, byte data)
{
	//Sets the Coarse X values from the 15 Bit VRAM address Register

	//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	//	    y  y  y  N  N  Y Y Y Y Y(X X X X X)
	//		|  |  |  |  |  | | | | | +-+-+-+-+-------coarse X scroll
	//		|  |  |  |  |  +-+-+-+-+-----------------coarse Y scroll
	//		|  |  |  +--+----------------------------nametable select
	//		+--+--+----------------------------------fine Y scroll

	return (address & (BIT_5 + BIT_6 + BIT_7 + BIT_8 + BIT_9 + BIT_10 + BIT_11 + BIT_12 + BIT_13 + BIT_14)) | (data & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4));
}

byte Get_Coarse_Y(int address)
{
	//Gets the Coarse Y values from the 15 Bit VRAM address Register

	//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	//	    y  y  y  N  N (Y Y Y Y Y)X X X X X
	//		|  |  |  |  |  | | | | | +-+-+-+-+-------coarse X scroll
	//		|  |  |  |  |  +-+-+-+-+-----------------coarse Y scroll
	//		|  |  |  +--+----------------------------nametable select
	//		+--+--+----------------------------------fine Y scroll

	//	   0x3E0 in binary is 0000001111100000
	//	   Where 11111 is where the Coarse Y values are found as shown above
	//     in Bits 5, 6, 7, 8, 9. Coarse Y values can range from 0 to 31. A right
	//     shift of 5 bits is needed to obtain the value, which results in
	//     0000000000011111
	return (address & (BIT_5 + BIT_6 + BIT_7 + BIT_8 + BIT_9)) >> 5;
}

int Set_Coarse_Y(int address, byte data)
{
	//Sets the Coarse Y values from the 15 Bit VRAM address Register

	//	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	//	    y  y  y  N  N (Y Y Y Y Y)X X X X X
	//		|  |  |  |  |  | | | | | +-+-+-+-+-------coarse X scroll
	//		|  |  |  |  |  +-+-+-+-+-----------------coarse Y scroll
	//		|  |  |  +--+----------------------------nametable select
	//		+--+--+----------------------------------fine Y scroll

	return (address & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4 + BIT_10 + BIT_11 + BIT_12 + BIT_13 + BIT_14)) | ((data & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4)) << 5);
}

void Do_Mirroring()
{
	Mirror_Offset = (((nes_rom.Mirroring + 1) % 3) * 0x400);

	if (nes_rom.Mirroring == HORIZONTAL)
	{
		Mirror_Number[0] = 0; Mirror_Number[1] = 0;
		Mirror_Number[2] = 1; Mirror_Number[3] = 1; //<---- Horizontal Mirroring
	}
	else if (nes_rom.Mirroring == VERTICAL)
	{
		Mirror_Number[0] = 0; Mirror_Number[1] = 1; //^
		Mirror_Number[2] = 0; Mirror_Number[3] = 1; //^ Vertical Mirroring
	}
	else if (nes_rom.Mirroring == SINGLE_SCREEN)
	{
		Mirror_Number[0] = 0; Mirror_Number[1] = 0;
		Mirror_Number[2] = 0; Mirror_Number[3] = 0;
	}
	else if (nes_rom.Mirroring == FOUR_SCREEN)
	{
		Mirror_Number[0] = 0; Mirror_Number[1] = 1;
		Mirror_Number[2] = 2; Mirror_Number[3] = 3;
	}
}

void Render_Scanline(int Scanline)
{
	//Do_Mirroring();

	//if (Scanline >= 240) return;

	//if (Scanline == 0)
	//	memset(Video_Buffer, 16, NES_SCREEN_RESOLUTION);

	//if (Get_Background_Visibility() == false)
	//{
	//	if (Scanline > (SPR_RAM[0] + 8)) 
	//		PPU_Status = Set_Sprite_0_Hit_Flag();
	//	return;
	//}

	//if (Scanline == 0)
	//	Scrolling_Offset_Register = vram_address_register;
	//else
	//	Scrolling_Offset_Register = Scanline_Start(Scrolling_Offset_Register, vram_address_register);

	//Render_Sprites(Scanline, false);
	//Render_Background(Scanline);
	//Render_Sprites(Scanline, true);

	Do_Mirroring();
	
	if (Scanline >= 240) return;

	if (Scanline == 0)
		memset(Video_Buffer, 16, NES_SCREEN_RESOLUTION);
	
	if ((ppu.ppu_mask.reg & BIT_3) == 0)
	{
		if (Scanline > (SPR_RAM[0] + 8)) {
			ppu.ppu_status.reg = ppu.ppu_status.Set_Sprite_0_Hit_Flag();
		}
		return;
	}

	//SetWindowText(hWnd, "MADE IT!!!");

	if (Scanline == 0)
		ppu.vram_address_register.address = ppu.temp_vram_address_register.address;
	else
		ppu.vram_address_register.address = Scanline_Start(ppu.vram_address_register.address, ppu.temp_vram_address_register.address);

	//if (Scanline == 0)
	//	Next_Frame();
	//else
	//	Scanline_Start();

	Render_Sprites(Scanline, false);
	Render_Background(Scanline);
	Render_Sprites(Scanline, true);

	Next_Line();
}

void Scanline_Start()
{
	ppu.vram_address_register.address = (ppu.vram_address_register.address & 0x7BE0) | (ppu.temp_vram_address_register.address & 0x41F);
}

int Scanline_Start(int Address1, int Address2) 
{
	////'#define LOOPY_SCANLINE_START(v,t) \
	////'  { \
	////'    v = (v & 0xFBE0) | (t & 0x041F); \
	////'  }

	////---------------------------------------------------------------
	////			   VRAM address Register
	////---------------------------------------------------------------
	////	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	////	    y  y  y  N  N  Y Y Y Y Y X X X X X
	////		|  |  |  |  |  | | | | | +-+-+-+-+-------Coarse X Scroll
	////		|  |  |  |  |  +-+-+-+-+-----------------Coarse Y Scroll
	////		|  |  |  +--+----------------------------Nametable Select
	////		+--+--+----------------------------------Fine Y Scroll

	//0111 1011 1110 0000 or 0x7BE0 in VRAM address register has:
	//		- Coarse Y Scroll at bits 9, 8, 7, 6, and 5
	//		- Nametable select at bits 11 but not 10 o.O
	//		- Fine Y scroll at bits 14, 13, and 12
	//		Notice there is no Coarse X Scroll and bit 10 is zero.
	//		The missing bits are ORed using the Temp VRAM address register
	//‭0000 0100 0001 1111‬ or 0x41F in Temp VRAM address register has:
	//		Coarse X Scroll at bits 4, 3, 2, 1, and 0
	//		Nametable select at bits 10

	// The Temp VRAM address register is compared to the missing 
	// binary bits which are 0 in 0x7BE0 in the VRAM address register.
	// No shifting is needed

	// Also note that 0x7BE0 is used and not 0xFBE0. That is because the VRAM is 15 bits, and not 16.
	// Although it'll still work, the 16th bit is never used.

	return (Address1 & 0x7BE0) | (Address2 & 0x41F);
}


void Next_Frame()
{
	ppu.vram_address_register.address = ppu.temp_vram_address_register.address;
}

int Next_Line(int address)
{
	int Temp_Scrolling_Offset_Register;

	Temp_Scrolling_Offset_Register = address;

	if ((Temp_Scrolling_Offset_Register & 0x7000) == 0x7000) //is subtile y offset == 7?
	{
		Temp_Scrolling_Offset_Register = Temp_Scrolling_Offset_Register & 0x8FFF; // subtile y offset = 0 
		if ((Temp_Scrolling_Offset_Register & 0x3E0) == 0x3A0) //Name Table line == 29?
		{
			Temp_Scrolling_Offset_Register = Temp_Scrolling_Offset_Register ^ 0x800;   //Switch Name Tables (bit 11)
			Temp_Scrolling_Offset_Register = Temp_Scrolling_Offset_Register & 0xFC1F;  //Name Table line = 0
		}
		else
		{
			if ((Temp_Scrolling_Offset_Register & 0x3E0) == 0x3E0)  //line == 31?
			{
				Temp_Scrolling_Offset_Register = Temp_Scrolling_Offset_Register & 0xFC1F; //Name Table line = 0
			}
			else
			{
				Temp_Scrolling_Offset_Register = Temp_Scrolling_Offset_Register + 0x20;
			}
		}
	}
	else
	{
		Temp_Scrolling_Offset_Register = Temp_Scrolling_Offset_Register + 0x1000;
	}

	return Temp_Scrolling_Offset_Register & 0xFFFF;
}

void Next_Line()
{
	byte fine_y = Get_Fine_Y(ppu.vram_address_register.address);

	if (fine_y < 7)																//If fine Y is less than 7
	{
		fine_y++;																//Increment fine Y
		ppu.vram_address_register.address = Set_Fine_Y(ppu.vram_address_register.address, fine_y);		//Put the result of Fine Y in the address register
	}
	else
	{
		fine_y = 0;																//Fine Y = 0
		ppu.vram_address_register.address = Set_Fine_Y(ppu.vram_address_register.address, fine_y);		//Put the result of Fine Y in the address register
		
		byte coarse_y = Get_Coarse_Y(ppu.vram_address_register.address);					//Give coarse Y a readable and useable value
		
		if (coarse_y == 29)														//If Name Table line == 29
		{
			coarse_y = 0;														//Coarse Y = 0
			ppu.vram_address_register.address ^= BIT_11;									//Switch vertical nametable
		}
		else if (coarse_y == 31)
			coarse_y = 0;														//Coarse Y = 0, nametable not switched
		else
			coarse_y++;															//Increment coarse Y	
		
		ppu.vram_address_register.address = Set_Coarse_Y(ppu.vram_address_register.address, coarse_y);	//Put the result of Coarse Y in the address register
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//if ((vram_address_register & 0x7000) == 0x7000)			/* is subtile y offset == 7? */
	//{
	//	vram_address_register = vram_address_register & 0x8FFF;				/* subtile y offset = 0 */
	//	if ((vram_address_register & 0x3E0) == 0x3A0)			//Name Table line == 29?
	//	{
	//		vram_address_register = vram_address_register ^ 0x800;			//Switch Name Tables (bit 11)
	//		vram_address_register = vram_address_register & 0xFC1F;			//Name Table line = 0
	//	}
	//	else
	//	{
	//		if ((vram_address_register & 0x3E0) == 0x3E0)		//line == 31?
	//			vram_address_register = vram_address_register & 0xFC1F;		//Name Table line = 0
	//		else
	//			vram_address_register = vram_address_register + 0x20;
	//	}
	//}
	//else
	//	vram_address_register = vram_address_register + 0x1000;

}

void Fill_Color_LUT()
{
	int C;

	for (int B1 = 0; B1 <= 255; B1++)
	{
		for (int B2 = 0; B2 <= 255; B2++)
		{
			for (int X = 0; X <= 7; X++)
			{
				if (B1 & static_cast<int>(pow(2.0,X)))
					C = 1;
				else
					C = 0;
				if (B2 & static_cast<int>(pow(2.0, X)))
					C += 2;
				Color_LUT[(B1 * 0x800) + (B2 * 0x8) + X] = C;
			}
		}
	}
}

void Create_Real_NES_Pallete_NTSC()
{
	//This is based on the pallete from http://forums.nesdev.com/viewtopic.php?f=21&t=8121

	//In my opinion this is closer to the NES than the other NTSC pallete

	int Current_Color;

	//Row 1
	Pallete[0] = D3DCOLOR_XRGB(104, 104, 104);
	Pallete[1] = D3DCOLOR_XRGB(0, 42, 136);
	Pallete[2] = D3DCOLOR_XRGB(20, 18, 167);
	Pallete[3] = D3DCOLOR_XRGB(59, 0, 164);
	Pallete[4] = D3DCOLOR_XRGB(92, 0, 126);
	Pallete[5] = D3DCOLOR_XRGB(110, 0, 64);
	Pallete[6] = D3DCOLOR_XRGB(108, 6, 0);
	Pallete[7] = D3DCOLOR_XRGB(86, 29, 0); //One of the coin block animations in Super Mario Bros
	Pallete[8] = D3DCOLOR_XRGB(51, 53, 0);
	Pallete[9] = D3DCOLOR_XRGB(11, 72, 0);
	Pallete[10] = D3DCOLOR_XRGB(0, 82, 0);
	Pallete[11] = D3DCOLOR_XRGB(0, 79, 8);
	Pallete[12] = D3DCOLOR_XRGB(0, 64, 77);
	Pallete[13] = D3DCOLOR_XRGB(0, 0, 0);
	Pallete[14] = D3DCOLOR_XRGB(0, 0, 0);
	Pallete[15] = D3DCOLOR_XRGB(0, 0, 0);

	//Row 2
	Pallete[16] = D3DCOLOR_XRGB(173, 173, 173);
	Pallete[17] = D3DCOLOR_XRGB(4, 62, 210);	// CORRECT 'Blue: Punch Out Ring Floor
	Pallete[18] = D3DCOLOR_XRGB(66, 64, 255);
	Pallete[19] = D3DCOLOR_XRGB(117, 39, 254);
	Pallete[20] = D3DCOLOR_XRGB(160, 26, 204);
	Pallete[21] = D3DCOLOR_XRGB(183, 30, 123);
	Pallete[22] = D3DCOLOR_XRGB(145, 39, 1);	// CORRECT 'Dark Red: Marios suspenders in Super Mario Bros, Marios hair, mustache, and boots as fire mario.
	Pallete[23] = D3DCOLOR_XRGB(113, 61, 0);	// CORRECT 'Brown: Ground Bricks and regular bricks of outdoor worlds in Super Mario Bros. Background of Super Mario Bros Title.
	Pallete[24] = D3DCOLOR_XRGB(87, 89, 1);		// CORRECT 'Dark Greenish Yellow 'Marios Shirt
	Pallete[25] = D3DCOLOR_XRGB(56, 135, 0);
	Pallete[26] = D3DCOLOR_XRGB(0, 121, 0);		// CORRECT 'Dark Green: Hills, turtle shells, pipe, top of flag pole in Super Mario Bros. Green spots on 1-Up.
	Pallete[27] = D3DCOLOR_XRGB(0, 143, 50);
	Pallete[28] = D3DCOLOR_XRGB(0, 99, 133);	// CORRECT 'Dark Turquoise: Ground and ceiling bricks in dark worlds such as 1-2 of Super Mario Bros 'D3DCOLOR_XRGB(0, 104, 101) '
	Pallete[29] = D3DCOLOR_XRGB(0, 0, 0);
	Pallete[30] = D3DCOLOR_XRGB(0, 0, 0);
	Pallete[31] = D3DCOLOR_XRGB(0, 0, 0);

	//Row 3
	Pallete[32] = D3DCOLOR_XRGB(255, 254, 255);
	Pallete[33] = D3DCOLOR_XRGB(100, 176, 255);
	Pallete[34] = D3DCOLOR_XRGB(134, 129, 255); // CORRECT 'Sky Blue: Sky of outdoor worlds in Super Mario Bros
	Pallete[35] = D3DCOLOR_XRGB(198, 118, 255);
	Pallete[36] = D3DCOLOR_XRGB(243, 106, 255);
	Pallete[37] = D3DCOLOR_XRGB(254, 110, 204);
	Pallete[38] = D3DCOLOR_XRGB(254, 129, 112);
	Pallete[39] = D3DCOLOR_XRGB(221, 141, 23);	// CORRECT 'Peach/Light Yello: Marios skin, Bright color coins, bright color ? blocks in Super Mario Bros. Mushroom main color.
	Pallete[40] = D3DCOLOR_XRGB(188, 190, 0);
	Pallete[41] = D3DCOLOR_XRGB(127, 200, 0);	// CORRECT 'Green: Pipes in Super Mario Bros. Bright Green Bushes
	Pallete[42] = D3DCOLOR_XRGB(92, 228, 48);
	Pallete[43] = D3DCOLOR_XRGB(69, 224, 130);
	Pallete[44] = D3DCOLOR_XRGB(72, 205, 222);
	Pallete[45] = D3DCOLOR_XRGB(79, 79, 79);
	Pallete[46] = D3DCOLOR_XRGB(0, 0, 0);
	Pallete[47] = D3DCOLOR_XRGB(0, 0, 0);

	//Row 4
	Pallete[48] = D3DCOLOR_XRGB(255, 255, 255); // CORRECT 'White: Super Mario Bros Text. Clouds
	Pallete[49] = D3DCOLOR_XRGB(192, 223, 255);
	Pallete[50] = D3DCOLOR_XRGB(211, 210, 255);
	Pallete[51] = D3DCOLOR_XRGB(232, 200, 255);
	Pallete[52] = D3DCOLOR_XRGB(251, 194, 255);
	Pallete[53] = D3DCOLOR_XRGB(254, 196, 234);
	Pallete[54] = D3DCOLOR_XRGB(252, 204, 197); // CORRECT 'Pinkish White: Super Mario Bros Title Logo, White in bricks
	Pallete[55] = D3DCOLOR_XRGB(253, 219, 175); // CORRECT 'Pinkish White 2: Marios Shirt with Fireflower in Super Mario Bros
	Pallete[56] = D3DCOLOR_XRGB(228, 229, 148);
	Pallete[57] = D3DCOLOR_XRGB(207, 239, 150);
	Pallete[58] = D3DCOLOR_XRGB(189, 244, 171);
	Pallete[59] = D3DCOLOR_XRGB(179, 243, 204);
	Pallete[60] = D3DCOLOR_XRGB(181, 235, 242);
	Pallete[61] = D3DCOLOR_XRGB(184, 184, 184);
	Pallete[62] = D3DCOLOR_XRGB(0, 0, 0);
	Pallete[63] = D3DCOLOR_XRGB(0, 0, 0);

	for (Current_Color = 0; Current_Color <= 63; Current_Color++)
	{
		Pallete[Current_Color + 64] = Pallete[Current_Color];
		Pallete[Current_Color + 128] = Pallete[Current_Color];
		Pallete[Current_Color + 192] = Pallete[Current_Color];
	}
}

void Blit_Screen()
{
	int current_pixel = 0;
	for (int y = 0; y < 240; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			// The REAL border that you are trying to emulate is not the NES itself.
			// It is the fact that most TV's back in the 80s were wider than what was drawn on the
			// screen. So it still draws 256x240, but the excess in the beginning and the end is still
			// the background color.

			// To pull this off in the emulator, we make the window 32 pixels wider. 16 in the beginning,
			// and 16 in the end. We then clear the screen not with black, but with the NES background color.
			// And finally, the rendered polygon we are rendering to is offseted 16 pixels to the right.

			//if (x >= 8 && x <= 248)
			//Video_Buffer32[current_pixel] = Pallete[26];
				Video_Buffer32[current_pixel] = Pallete[CHR_RAM[Video_Buffer[current_pixel] + 0x3f00]];
			//else
			//	Video_Buffer32[current_pixel] = Pallete[CHR_RAM[Video_Buffer[30464] + 0x3f00]];
			current_pixel++;
		}
	}

	// Clear the background color not with 0 for black, but with the NES specified background color
	Device->Clear(0, NULL, D3DCLEAR_TARGET, Pallete[CHR_RAM[0x3f00]], 1.0f, 0);
	Device->BeginScene();
	Create_Polygon();
	Draw_Frame();

	//for (int i = 0; i < 64; i++) {
		//char Text[255];
		//sprintf_s(Text, sizeof(Text), "%i", i);
		//DrawTextString(Text, Sprite[i].Position.X + 16, Sprite[i].Position.Y, D3DCOLOR_XRGB(255, 255, 255));

		//Draw_Pixel(Sprite[i].Position.X + 16, Sprite[i].Position.Y, D3DCOLOR_XRGB(255, 255, 255));
	//}

						//char Text[255];
						//sprintf_s(Text, sizeof(Text), "Name_Table_Address: %x", Name_Table_Address);
						//DrawTextString(Text, 10, 10, D3DXCOLOR(255, 255, 255, 255));
	//int v = 18;
	//Draw_Rectangle((float)Sprite[v].Position.X, (float)Sprite[v].Position.Y, (float)Sprite[v].Position.X + 8.0f, (float)Sprite[v].Position.Y + 8.0f, D3DXCOLOR(255.0f, 255.0f, 255.0f, 255.0f));
	

	//char Text[255];
	//sprintf_s(Text, sizeof(Text), "PC: %x", cpu.reg.pc);
	//DrawTextString(Text, 10, 30, D3DCOLOR_XRGB(0, 255, 0));
	//sprintf_s(Text, sizeof(Text), "Sprite[0].Position.Y: %i", Sprite[v].Position.Y);
	//DrawTextString(Text, 10, 45, D3DCOLOR_XRGB(0, 255, 0));
	//sprintf_s(Text, sizeof(Text), "Attribute_Address: %x", Attribute_Address);
	//DrawTextString(Text, 10, 60, D3DCOLOR_XRGB(0, 255, 0));
	//sprintf_s(Text, sizeof(Text), "LookUp: %i" , LookUp);
	//DrawTextString(Text, 10, 75, D3DCOLOR_XRGB(0, 255, 0));
	//sprintf_s(Text, sizeof(Text), "Fine_Y_Scroll: %i", Fine_Y_Scroll);
	//DrawTextString(Text, 10, 90, D3DCOLOR_XRGB(0, 255, 0));
	//sprintf_s(Text, sizeof(Text), "PC: %x", PC);
	//DrawTextString(Text, 10, 105, D3DCOLOR_XRGB(0, 255, 0));
	Device->EndScene();
	Device->Present(NULL, NULL, NULL, NULL);
}

