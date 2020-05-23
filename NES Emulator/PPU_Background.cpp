#include "PPU_Background.h"
#include "ppu.h"
#include "Math.h"
#include "bits.h"

void Render_Background(int Scanline) {
	//if (Get_Sprite_Visibility() == 0) return; //FUCKS UP DOUBLE DRAGON
	if (ppu.ppu_mask.Get_Background_Visibility() == false) return;

	int Virtual_Scanline = 0;
	int Background_Pattern_Table_Address = 0;
	//int Name_Table_Address = 0;
	int Name_Table_Number = 0;
	byte Tile_Row = 0;
	int Tile_Y_Offset = 0;
	int Tile_Counter = 0;
	byte Pixel_Color = 0;
	int Background_Tile_Offset = 0;
	byte Tile_Index = 0;
	byte Low_Byte = 0;
	byte High_Byte = 0;
	int LookUp = 0;
	int TileX = 0;
	int TileY = 0;
	int Aa = 0;
	int Offset = 0;
	int Attribute_Table = 0;
	int Background_Pallete = 0;
	int scanline;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	////---------------------------------------------------------------
	////			   VRAM address Register
	////---------------------------------------------------------------
	////	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	////	    y  y  y  N  N  Y Y Y Y Y X X X X X
	////		|  |  |  |  |  | | | | | +-+-+-+-+-------Coarse X Scroll
	////		|  |  |  |  |  +-+-+-+-+-----------------Coarse Y Scroll
	////		|  |  |  +--+----------------------------Nametable Select
	////		+--+--+----------------------------------Fine Y Scroll

	////---------------------------------------------------------------
	////              Attribute address Register
	////---------------------------------------------------------------
	////11 10 9 8 7 6 5 4 3 2 1 0
	//// N  N 1 1 1 1 Y Y Y X X X
	//// |  | | | | | | | | + + +----------------------High 3 bits of Coarse X
	//// |  | | | | | + + +----------------------------High 3 bits of Coarse Y
	//// |  | + + + +----------------------------------Attribute Offset(960 bytes)
	////++---------------------------------------------Nametable Select

	byte Coarse_X_Scroll;
	byte High_3_Bits_Of_Coarse_X;
	byte Horizontal_Scroll_Offset = 0;

	byte Coarse_Y_Scroll;
	byte Fine_Y_Scroll;
	byte High_3_Bits_Of_Coarse_Y;
	byte Vertical_Scroll_Offset = 0;

	scanline = Scanline;

	Coarse_X_Scroll = ppu.vram_address_register.address & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4);
	High_3_Bits_Of_Coarse_X = Coarse_X_Scroll << 3;
	Horizontal_Scroll_Offset = High_3_Bits_Of_Coarse_X | loopy_x;

	Coarse_Y_Scroll = (ppu.vram_address_register.address >> 5) & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4);
	High_3_Bits_Of_Coarse_Y = Coarse_Y_Scroll << 3;
	Fine_Y_Scroll = (ppu.vram_address_register.address >> 12) & (BIT_0 + BIT_1 + BIT_2);
	Vertical_Scroll_Offset = ((High_3_Bits_Of_Coarse_Y | Fine_Y_Scroll)) - scanline;

	///////////////////////////////////////////////////////////////////////////////////////////////////

	//KEEP THESE IN THE RENDER SCANLINE METHOD!!!
	//---------------------------------------------------------
	//Next_Line(); //FUCKS UP START MENU IN ZELDA!!!
	//ppu.vram_address_register.address = Next_Line(ppu.vram_address_register.address); //ALSO FUCKS UP START MENU IN ZELDA!!!


	Virtual_Scanline = scanline + Vertical_Scroll_Offset;

	if (Virtual_Scanline > 480)
		Virtual_Scanline = 0;

	if (scanline >= ppu.NTSC_VISIBLE_PICTURE_BEGIN && scanline <= ppu.NTSC_VISIBLE_PICTURE_END) {
		Tile_Row = (Virtual_Scanline / 8) % 32;
		Tile_Y_Offset = Virtual_Scanline & 7;

		Name_Table_Address = (ppu.vram_address_register.address & 0x2C00);
		Name_Table_Number =  (Name_Table_Address & 0xC00) >> 10; //(ppu.temp_vram_address_register.address >> 10) && (BIT_0 + BIT_1);

		Attribute_Table = 0x3C0;
		Background_Pattern_Table_Address = ppu.ppu_controller.Get_Background_Pattern_Table_Address();

		//NOTE: 1/2/2020
		//REPLACED for (Tile_Counter = (Horizontal_Scroll_Offset >> 3); Tile_Counter <= 31; Tile_Counter++) with
		//         for (Tile_Counter = 0; Tile_Counter <= 31; Tile_Counter++)

		for (Tile_Counter = 0; Tile_Counter <= 31; Tile_Counter++)
		{
			Tile_Index = Name_Table_Array[Mirror_Number[Name_Table_Number]][Tile_Counter + Tile_Row * 32];
			TileX = Tile_Counter * 8 - Horizontal_Scroll_Offset + 7;

			if (TileX < 7)
				Offset = TileX;
			else
				Offset = 7;

			TileX = TileX + scanline * 256;
			LookUp = Name_Table_Array[Mirror_Number[Name_Table_Number]][(Attribute_Table + (Tile_Counter / 4) + (Tile_Row / 4) * 8)];

			switch ((Tile_Counter & 2) | (Tile_Row & 2) * 2)
			{
			case 0: Background_Pallete = (LookUp << 2) & 12; break;
			case 2: Background_Pallete = LookUp & 12; break;
			case 4: Background_Pallete = (LookUp >> 2) & 12; break;
			case 6: Background_Pallete = (LookUp >> 4) & 12; break;
			}

			Background_Tile_Offset = Background_Pattern_Table_Address + (Tile_Index * 16);

			//NOTE 1/2/2020
			//THIS CHUNK OF CODE I REMOVED IS PROBABLY NOT NEEDED AT ALL
			
			//if (Tile_Y_Offset == 0)
			//{
			//	for (TileY = 0; TileY <= 7; TileY++)
			//	{
			//		Low_Byte = CHR_RAM[Background_Tile_Offset + TileY];
			//		High_Byte = CHR_RAM[Background_Tile_Offset + TileY + 8];
			//		Aa = (Low_Byte << 11) + (High_Byte << 3);
			//		for (int Current_Pixel = 0; Current_Pixel <= Offset; Current_Pixel++)
			//		{
			//			Pixel_Color = Color_LUT[Aa + Current_Pixel];
			//			if (Pixel_Color % 4 != 0) Video_Buffer[TileX - Current_Pixel] = static_cast<byte>(Pixel_Color | Background_Pallete);
			//		}
			//		if (TileX <= (256 * 240) - 256)
			//			TileX = TileX + 256;
			//	}
			//}
			//else
			//{
				Low_Byte = CHR_RAM[Background_Tile_Offset + Tile_Y_Offset];
				High_Byte = CHR_RAM[Background_Tile_Offset + Tile_Y_Offset + 8];
				Aa = (Low_Byte << 11) + (High_Byte << 3);
				for (int Current_Pixel = 0; Current_Pixel <= Offset; Current_Pixel++)
				{
					Pixel_Color = Color_LUT[Aa + Current_Pixel];
					if (Pixel_Color % 4 != 0) Video_Buffer[TileX - Current_Pixel] = static_cast<byte>(Pixel_Color | Background_Pallete);
				}
			//}
		}

		Name_Table_Address ^= 0x400;
		Name_Table_Number = (Name_Table_Address & 0xC00) >> 10;

		for (Tile_Counter = 0; Tile_Counter <= (Horizontal_Scroll_Offset >> 3); Tile_Counter++)
		{
			Tile_Index = Name_Table_Array[Mirror_Number[Name_Table_Number]][Tile_Counter + Tile_Row * 32];
			TileX = Tile_Counter * 8 + 256 - Horizontal_Scroll_Offset + 7;

			if (TileX > 255)
				Offset = TileX - 255;
			else
				Offset = 0;

			TileX = TileX + scanline * 256;
			LookUp = Name_Table_Array[Mirror_Number[Name_Table_Number]][(Attribute_Table + (Tile_Counter / 4) + (Tile_Row / 4) * 0x8)];

			switch ((Tile_Counter & 2) | (Tile_Row & 2) * 2)
			{
			case 0: Background_Pallete = (LookUp << 2) & 12; break;
			case 2: Background_Pallete = LookUp & 12; break;
			case 4: Background_Pallete = (LookUp >> 2) & 12; break;
			case 6: Background_Pallete = (LookUp >> 4) & 12; break;
			}

			Background_Tile_Offset = Background_Pattern_Table_Address + (Tile_Index * 16);

			//NOTE 1/2/2020
			//THIS CHUNK OF CODE I REMOVED IS PROBABLY NOT NEEDED AT ALL

			//if (Tile_Y_Offset == 0)
			//{
			//	for (TileY = 0; TileY <= 7; TileY++)
			//	{
			//		Low_Byte = CHR_RAM[Background_Tile_Offset + TileY];
			//		High_Byte = CHR_RAM[Background_Tile_Offset + TileY + 8];
			//		Aa = (Low_Byte << 11) + (High_Byte << 3);
			//		for (int Current_Pixel = Offset; Current_Pixel <= 7; Current_Pixel++)
			//		{
			//			Pixel_Color = Color_LUT[Aa + Current_Pixel];
			//			if (Pixel_Color % 4 != 0) Video_Buffer[TileX - Current_Pixel] = static_cast<byte>(Pixel_Color | Background_Pallete);
			//		}
			//		if (TileX <= (256 * 240) - 256)
			//			TileX = TileX + 256;
			//	}
			//}
			//else
			//{
				Low_Byte = CHR_RAM[Background_Tile_Offset + Tile_Y_Offset];
				High_Byte = CHR_RAM[Background_Tile_Offset + Tile_Y_Offset + 8];
				Aa = (Low_Byte << 11) + (High_Byte << 3);
				for (int Current_Pixel = Offset; Current_Pixel <= 7; Current_Pixel++)
				{
					Pixel_Color = Color_LUT[Aa + Current_Pixel];
					if (Pixel_Color % 4 != 0) Video_Buffer[TileX - Current_Pixel] = static_cast<byte>(Pixel_Color | Background_Pallete);
				}
			//}
		}
	}

	////---------------------------------------------------------------
	////			   VRAM address Register
	////---------------------------------------------------------------
	////	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
	////	    y  y  y  N  N  Y Y Y Y Y X X X X X
	////		|  |  |  |  |  | | | | | +-+-+-+-+-------Coarse X Scroll
	////		|  |  |  |  |  +-+-+-+-+-----------------Coarse Y Scroll
	////		|  |  |  +--+----------------------------Nametable Select
	////		+--+--+----------------------------------Fine Y Scroll

	////---------------------------------------------------------------
	////              Attribute address Register
	////---------------------------------------------------------------
	////11 10 9 8 7 6 5 4 3 2 1 0
	//// N  N 1 1 1 1 Y Y Y X X X
	//// |  | | | | | | | | + + +----------------------High 3 bits of Coarse X
	//// |  | | | | | + + +----------------------------High 3 bits of Coarse Y
	//// |  | + + + +----------------------------------Attribute Offset(960 bytes)
	////++---------------------------------------------Nametable Select

	////Coarse_X_Scroll = vram_address_register & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4);
	////High_Bits_Of_Coarse_X = Coarse_X_Scroll << 3;
	////Horizontal_Scroll_Offset = High_Bits_Of_Coarse_X | Horizontal_Scroll;
	////
	////Coarse_Y_Scroll = static_cast<byte>(vram_address_register >> 5) & (BIT_0 + BIT_1 + BIT_2 + BIT_3 + BIT_4);
	////High_Bits_Of_Coarse_Y = Coarse_Y_Scroll << 3;
	////Fine_Y_Scroll = static_cast<byte>(vram_address_register >> 12) & (BIT_0 + BIT_1 + BIT_2);
	////Vertical_Scroll_Offset = Scanline - ((High_Bits_Of_Coarse_Y | Fine_Y_Scroll));
	//
	////Old way
	////Horizontal_Scroll_Offset_Register = (Scrolling_Offset_Register & 31) * 8 + Horizontal_Scroll;
	////Vertical_Scroll_Offset_Register = ((Scrolling_Offset_Register / 32 & 31) * 8 | ((Scrolling_Offset_Register / 0x1000) & 7)) - Scanline;

	//Scrolling_Offset_Register = Next_Line(Scrolling_Offset_Register);
	////Commented out 2/4/15
	////Scrolling_Offset_Register = Scrolling_Offset_Register & 0xFFFF;


	////vram_address_register = vram_address_register & 0xFFFF;

	////if (Scanline == 240) Set_VBlank_Occuring_Flag();
	//if (Scanline == 239) Set_VBlank_Occuring_Flag();
	////////////////////////////////////////////////////////////////////////////////////////////

	//Virtual_Scanline = Scanline + Vertical_Scroll_Offset_Register;
	//if (Virtual_Scanline > 480) Virtual_Scanline = Virtual_Scanline - 480;

	//Tile_Row = (Virtual_Scanline / 8) % 30;
	//Tile_Y_Offset = Virtual_Scanline & 7;
	//Name_Table_Address = 0x2000 + (Scrolling_Offset_Register & 0xC00);
	////Name_Table_Address = 0x2000 + (Scrolling_Offset_Register & 0x0FFF);
	//Name_Table_Number = (Name_Table_Address & 0xC00) / 0x400;
	////Attribute_Address = 0x23C0 | (vram_address_register & 0x0C00) | ((vram_address_register >> 4) & 0x38) | ((vram_address_register >> 2) & 0x07); //0x23C0 + (vram_address_register & 0x0C00) + ((vram_address_register & 0x0380) >> 4);
	//Attribute_Table = 0x3C0;
	//PPU_Controller_Background_Pattern_Table_Address = (PPU_Control_Register_1 & BIT_4) * 256;

	//for (Tile_Counter = (Horizontal_Scroll_Offset_Register / 8); Tile_Counter <= 31; Tile_Counter++)
	//{
	//	Tile_Index = Name_Table_Array[Mirror_Number[Name_Table_Number]][Tile_Counter + Tile_Row * 32];
	//	Current_Scanline = ((Tile_Counter) * 8 - Horizontal_Scroll_Offset_Register + 7);

	//	if (Current_Scanline < 7) Offset = Current_Scanline; else Offset = 7;

	//	Current_Scanline = Current_Scanline + (256 * Scanline);

	//	LookUp = Name_Table_Array[Mirror_Number[Name_Table_Number]][0x3C0 + ((Tile_Counter / 4) + ((Tile_Row / 4) * 8))];

	//	switch ((Tile_Counter & 2) | (Tile_Row & 2) * 2)
	//	{
	//		case 0: Background_Pallete = (LookUp * 4) & 12; break;
	//		case 2: Background_Pallete = LookUp & 12; break;
	//		case 4: Background_Pallete = (LookUp / 4) & 12; break;
	//		case 6: Background_Pallete = (LookUp / 16) & 12; break;
	//	}

	//	
	//	Background_Tile_Offset = PPU_Controller_Background_Pattern_Table_Address + (Tile_Index * 16);

	//	if (Tile_Y_Offset == 0)
	//	{
	//		for (Current_Tile.Y = 0; Current_Tile.Y <= 7; Current_Tile.Y++)
	//		{
	//			Low_Byte = CHR_RAM[Background_Tile_Offset + Current_Tile.Y];
	//			High_Byte = CHR_RAM[Background_Tile_Offset + Current_Tile.Y + 8];
	//			Aa = static_cast<int>(Low_Byte * 0x800 + High_Byte * 0x8);

	//			for (Current_Pixel = 0; Current_Pixel <= Offset; Current_Pixel++)
	//			{
	//				Color = Color_LUT[Aa + Current_Pixel];
	//				if (Color % 4 != 0) Video_Buffer[Current_Scanline - Current_Pixel] = static_cast<byte>(Color | Background_Pallete);
	//			}

	//			//Our array can only hold 256 * 240 - 1 = 61439
	//			//This will prevent overflow
	//			if (Current_Scanline < (256 * 240)) Current_Scanline += 256;
	//		}
	//	}
	//	else
	//	{
	//		Low_Byte = CHR_RAM[Background_Tile_Offset + Tile_Y_Offset];
	//		High_Byte = CHR_RAM[Background_Tile_Offset + Tile_Y_Offset + 8];
	//		Aa = static_cast<int>(Low_Byte * 0x800 + High_Byte * 0x8);

	//		for (Current_Pixel = 0; Current_Pixel <= Offset; Current_Pixel++)
	//		{
	//			Color = Color_LUT[Aa + Current_Pixel];
	//			if (Color % 4 != 0) Video_Buffer[Current_Scanline - Current_Pixel] = static_cast<byte>(Color | Background_Pallete);
	//		}
	//	}
	//}

	////Name Table 2  | Name Table 3
	////	($2800)     |   ($2C00)
	////-----------------------------
	//// Name Table 0 | Name Table 1
	////	($2000)     |   ($2400)

	//
	//Name_Table_Address = Name_Table_Address ^ 0x400;
	//Name_Table_Number = (Name_Table_Address & 0xC00) / 0x400;
	////Name_Table_Number = ((vram_address_register & 0xC00) ^ 0x400) >> 10;
	//Attribute_Address = 0x3C0;//0x23C0 + (vram_address_register & 0x0C00) + ((vram_address_register & 0x0380) >> 4);

	//for (Tile_Counter = 0; Tile_Counter <= (Horizontal_Scroll_Offset_Register / 8); Tile_Counter++)
	//{
	//	Current_Scanline = Tile_Counter * 8 + 256 - Horizontal_Scroll_Offset_Register + 7;
	//	
	//	if (Current_Scanline >= 256) Offset = Current_Scanline - 255; else Offset = 0;

	//	Current_Scanline = Current_Scanline + Scanline * 256;

	//	LookUp = Name_Table_Array[Mirror_Number[Name_Table_Number]][0x3C0 + ((Tile_Counter >> 2) + ((Tile_Row >> 2) << 3))];

	//	switch ((Tile_Counter & 2) | (Tile_Row & 2) * 2)
	//	{
	//		case 0: Background_Pallete = (LookUp * 4) & 12; break;
	//		case 2: Background_Pallete = LookUp & 12; break;
	//		case 4: Background_Pallete = (LookUp / 4) & 12; break;
	//		case 6: Background_Pallete = (LookUp / 16) & 12; break;
	//	}

	//	Tile_Index = Name_Table_Array[Mirror_Number[Name_Table_Number]][Tile_Counter + Tile_Row * 32];
	//	Background_Tile_Offset = PPU_Controller_Background_Pattern_Table_Address + (Tile_Index * 16);

	//	if (Tile_Y_Offset == 0)
	//	{
	//		for (Current_Tile.Y = 0; Current_Tile.Y <= 7; Current_Tile.Y++)
	//		{
	//			Low_Byte = CHR_RAM[Background_Tile_Offset + Current_Tile.Y];
	//			High_Byte = CHR_RAM[Background_Tile_Offset + Current_Tile.Y + 8];
	//			Aa = static_cast<int>(Low_Byte * 0x800 + High_Byte * 8);

	//			for (Current_Pixel = Offset; Current_Pixel <= 7; Current_Pixel++)
	//			{
	//				Color = Color_LUT[Aa + Current_Pixel];
	//				if (Color % 4 != 0) Video_Buffer[Current_Scanline - Current_Pixel] = static_cast<byte>(Color | Background_Pallete);
	//			}

	//			//Our array can only hold 256 * 240 - 1 = 61439
	//			//This will prevent overflow
	//			if (Current_Scanline < (256 * 240)) Current_Scanline += 256;
	//		}
	//	}
	//	else
	//	{
	//		Low_Byte = CHR_RAM[Background_Tile_Offset + Tile_Y_Offset];
	//		High_Byte = CHR_RAM[Background_Tile_Offset + Tile_Y_Offset + 8];
	//		Aa = static_cast<int>(Low_Byte * 0x800 + High_Byte * 8);
	//		for (Current_Pixel = Offset; Current_Pixel <= 7; Current_Pixel++)
	//		{
	//			Color = Color_LUT[Aa + Current_Pixel];
	//			//char Text[255];
	//			//sprintf_s(Text, sizeof(Text), "Tile X: %i", Current_Tile.X - Current_Pixel);
	//			//MessageBox(NULL, Text, "NES Emulator", MB_ICONEXCLAMATION | MB_OK);
	//			if (Color % 4 != 0) Video_Buffer[Current_Scanline - Current_Pixel] = static_cast<byte>(Color | Background_Pallete);
	//		}
	//	}
	//}
}