#include "PPU_Sprites.h"
#include "Math.h"
#include "ppu.h"

Sprite_Type Sprite[64] = { 0 };

void Render_Sprites(int Scanline, bool Drawn_In_Foreground) {
	if (ppu.ppu_mask.Get_Sprite_Visibility() == false) return;

	//int Tile_Counter;
	byte Pixel_Color;
	int Sprite_Offset;
	int Index;

	byte Low_Byte;
	byte High_Byte;
	int Current_X_Pixel;
	int Sprite_Scanline_To_Draw;
	int scanline = Scanline;

	for (int Current_Sprite_Address = 252; Current_Sprite_Address >= 0; Current_Sprite_Address -= 4) {
		if (scanline >= ppu.NTSC_VISIBLE_PICTURE_BEGIN && scanline <= ppu.NTSC_VISIBLE_PICTURE_END) {
			Index = Current_Sprite_Address / 4;

			Sprite[Index].Position.Y = SPR_RAM[Current_Sprite_Address] + 1;
			Sprite[Index].Tile_Index = SPR_RAM[Current_Sprite_Address + 1];
			Sprite[Index].Attributes = SPR_RAM[Current_Sprite_Address + 2];
			Sprite[Index].Position.X = SPR_RAM[Current_Sprite_Address + 3];

			Sprite[Index].Tile_Height = ppu.ppu_controller.Get_Sprite_Size();
			Sprite[Index].Pattern_Table_Address = ppu.ppu_controller.Get_Sprite_Pattern_Table_Address();
			Sprite[Index].Drawn_In_Background = Sprite_Attributes_Get_Drawn_In_Background(Sprite[Index].Attributes);
			Sprite[Index].Pallete = Sprite_Attributes_Get_Pallete(Sprite[Index].Attributes);
			Sprite[Index].Vertical_Flip = Sprite_Attributes_Get_Vertical_Flip(Sprite[Index].Attributes);
			Sprite[Index].Horizontal_Flip = Sprite_Attributes_Get_Horizontal_Flip(Sprite[Index].Attributes);
           
			if ((Sprite[Index].Drawn_In_Background != Drawn_In_Foreground) && (Sprite[Index].Position.Y <= scanline) && ((Sprite[Index].Position.Y + Sprite[Index].Tile_Height) > scanline))
			{
				if (Sprite[Index].Tile_Height == 8)
				{
					if (Sprite[Index].Vertical_Flip == false)
						Sprite_Scanline_To_Draw = scanline - Sprite[Index].Position.Y;
					else
						Sprite_Scanline_To_Draw = Sprite[Index].Position.Y + 7 - scanline;

					Sprite_Offset = Sprite[Index].Pattern_Table_Address + (Sprite[Index].Tile_Index * 16);
					Low_Byte = CHR_RAM[Sprite_Offset + Sprite_Scanline_To_Draw];
					High_Byte = CHR_RAM[Sprite_Offset + Sprite_Scanline_To_Draw + 8];

					for (Current_X_Pixel = 0; Current_X_Pixel <= 7; Current_X_Pixel++)
					{
						if (Sprite[Index].Horizontal_Flip == true)
						{
							Pixel_Color = Sprite[Index].Pallete + (((High_Byte & (1 << Current_X_Pixel)) >> Current_X_Pixel) << 1) + ((Low_Byte & (1 << Current_X_Pixel)) >> Current_X_Pixel);
						}
						else
						{
							Pixel_Color = Sprite[Index].Pallete + (((High_Byte & (1 << (7 - Current_X_Pixel))) >> (7 - Current_X_Pixel)) << 1) + ((Low_Byte & (1 << (7 - Current_X_Pixel))) >> (7 - Current_X_Pixel));
						}

						if (Pixel_Color % 4 != 0)
						{
							if ((Sprite[Index].Position.X + Current_X_Pixel) <= 256)
							{
								Video_Buffer[scanline * 256 + Sprite[Index].Position.X + Current_X_Pixel] = Sprite[Index].Pallete | Pixel_Color;
								if (Current_Sprite_Address == 0)
									ppu.ppu_status.reg = ppu.ppu_status.Set_Sprite_0_Hit_Flag();
							}
						}
					}
				}

				else if (Sprite[Index].Tile_Height == 16)
				{
					if (Sprite[Index].Vertical_Flip == false)
						Sprite_Scanline_To_Draw = scanline - Sprite[Index].Position.Y;
					else
						Sprite_Scanline_To_Draw = Sprite[Index].Position.Y + 15 - scanline;

					if (Sprite_Scanline_To_Draw < 8)
					{
						if (Sprite[Index].Tile_Index % 2 == 0)
							Sprite_Offset = 0x0 + Sprite[Index].Tile_Index * 16;
						else
							Sprite_Offset = 0x1000 + (Sprite[Index].Tile_Index - 1) * 16;
					}
					else
					{
						Sprite_Scanline_To_Draw = Sprite_Scanline_To_Draw - 8;
						if (Sprite[Index].Tile_Index % 2 == 0)
							Sprite_Offset = 0x0 + (Sprite[Index].Tile_Index + 1) * 16;
						else
							Sprite_Offset = 0x1000 + Sprite[Index].Tile_Index * 16;
					}

					Low_Byte = CHR_RAM[Sprite_Offset + Sprite_Scanline_To_Draw];
					High_Byte = CHR_RAM[Sprite_Offset + Sprite_Scanline_To_Draw + 8];

					for (Current_X_Pixel = 0; Current_X_Pixel <= 7; Current_X_Pixel++)
					{
						if (Sprite[Index].Horizontal_Flip == true)
							Pixel_Color = Sprite[Index].Pallete + (((High_Byte & (1 << (Current_X_Pixel))) >> (Current_X_Pixel)) << 1) + ((Low_Byte & (1 << (Current_X_Pixel))) >> (Current_X_Pixel));
						else
							Pixel_Color = Sprite[Index].Pallete + (((High_Byte & (1 << (7 - Current_X_Pixel))) >> (7 - Current_X_Pixel)) << 1) + ((Low_Byte & (1 << (7 - Current_X_Pixel))) >> (7 - Current_X_Pixel));

						if (Pixel_Color % 4 != 0)
						{
							if ((Sprite[Index].Position.X + Current_X_Pixel) <= 256)
							{
								Video_Buffer[scanline * 256 + Sprite[Index].Position.X + Current_X_Pixel] = Sprite[Index].Pallete | Pixel_Color;
								if (Current_Sprite_Address == 0)
									ppu.ppu_status.reg = ppu.ppu_status.Set_Sprite_0_Hit_Flag();
							}
						}
					}
				}
			}
        }
    }
}
