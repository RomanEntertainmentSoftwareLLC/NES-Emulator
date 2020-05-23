#pragma once

#include "6502 cpu.h"

//Data Types
typedef unsigned char byte;
typedef unsigned short ushort;

class PPU : public MOS6502 { // public PPUMemory, MOS6502 {
private:
	enum REGION {
		NTSC,
		PAL,
		DENDY
	};

	// $2000 (Write Only)
	class PPU_CONTROLLER {
		///////////////////////////////////////////////////////
		// PPU Controller ($2000) -> Write Only
		///////////////////////////////////////////////////////
		// 7654 3210
		// |||| ||||
		// |||| ||++-----Base nametable address
		// |||| ||			(0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
		// |||| |+-------VRAM address increment per CPU read / write of PPUDATA
		// |||| |			(0: add 1, going across; 1: add 32, going down)
		// |||| +--------Sprite pattern table address for 8x8 sprites
		// ||||				(0: $0000; 1: $1000; ignored in 8x16 mode)
		// |||+----------Background pattern table address
		// |||				(0: $0000; 1: $1000)
		// ||+-----------Sprite size(0: 8x8; 1: 8x16)
		// ||
		// |+------------PPU master / slave select
		// |					(0: read backdrop from EXT pins; 1: output color on EXT pins)
		// +-------------Generate an NMI at the start of the vertical blanking interval
		//					(0: off; 1: on)
	private:
	protected:
	public:
		int Get_Name_Table_Address();
		int Get_Amount_To_Increment();
		int Get_Sprite_Pattern_Table_Address();
		int Get_Background_Pattern_Table_Address();
		int Get_Sprite_Size();
		int Get_PPU_Selection();
		bool Get_Execute_NMI_On_VBlank();

		byte base_name_table_address;			// 2 bits
		byte vram_address_increment;			// 1 bit
		byte sprite_pattern_table_address;		// 1 bit
		byte background_pattern_table_address;	// 1 bit
		byte sprite_size;						// 1 bit
		byte ppu_master_slave_select;			// 1 bit
		byte generate_nmi;						// 1 bit
		byte reg;
		
		bool power_reset_30000_cycle_write;
	};

	// $2001 (Write Only)
	class PPU_MASK {
	private:
	protected:
	public:
		bool Get_Greyscale_Mode();
		bool Get_Background_Clipping();
		bool Get_Sprite_Clipping();
		bool Get_Background_Visibility();
		bool Get_Sprite_Visibility();
		bool Get_Intensify_Reds();
		bool Get_Intensify_Greens();
		bool Get_Intensify_Blues();

		byte greyscale_enabled;						// 1 bit
		byte show_background_at_leftmost_pixels;	// 1 bits
		byte show_sprites_at_leftmost_pixels;		// 1 bit
		byte background_enabled;					// 1 bit
		byte sprites_enabled;						// 1 bit
		byte emphasize_red;							// 1 bit
		byte emphasize_green;						// 1 bit
		byte emphasize_blue;						// 1 bit
		byte reg;
	};

	// $2002 (Read Only)
	class PPU_STATUS {
	private:
	protected:
	public:
		bool Get_Sprite_Overflow_Flag();
		bool Get_Sprite_0_Hit_Flag();
		bool Get_VBlank_Occuring_FLag();
		int Set_Sprite_Overflow_Flag();
		int Set_Sprite_0_Hit_Flag();
		int Set_VBlank_Occuring_Flag();

		byte least_significant_bits; // 5 bits
		byte sprite_overflow;		 // 1 bit
		byte sprite_0_hit;			 // 1 bit
		byte vblank_started;		 // 1 bit
		byte reg;
	};

	// $2003 (Write Only)
	struct OAM_ADDRESS {
		byte address; // 8 bits
	};

	// $2004 (Read / Write)
	struct OAM_DATA {
		byte data; // 8 bits
	};

	// $2005 (Write Only x2)
	struct PPU_SCROLL {
		byte x_scroll; // First write, 8 bits
		byte y_scroll; // Second write, 8 bits
		byte reg;
	};

	// $2006 (Write Only x2)
	struct PPU_ADDRESS {
		byte high_address; // First write, 8 bits
		byte low_address;  // Second write, 8 bits
		byte reg;
	};

	// $2007 (Read / Write)
	struct PPU_DATA {
		byte data; // 8 bits
	};

	struct PPU_SPRITE {
		byte x;
		byte y;
		byte oam_index;
		byte tile_index;
		byte attributes;
		byte tile_data_low;
		byte tile_data_high;
	};

	struct VRAM_ADDRESS {
		////---------------------------------------------------------------
		////			   VRAM address Register (15 bits)
		////---------------------------------------------------------------
		////	   14 13 12 11 10  9 8 7 6 5 4 3 2 1 0
		////	    y  y  y  N  N  Y Y Y Y Y X X X X X
		////		|  |  |  |  |  | | | | | +-+-+-+-+-------Coarse X Scroll
		////		|  |  |  |  |  +-+-+-+-+-----------------Coarse Y Scroll
		////		|  |  |  +--+----------------------------Nametable Select
		////		+--+--+----------------------------------Fine Y Scroll

		byte coarse_x_scroll;	// 5 bits
		byte coarse_y_scroll;	// 5 bits
		byte name_table_select; // 2 bits
		byte fine_y_scroll;		// 3 bits
		ushort address;			// 15 bits
	};
protected:
public:
	PPU();
	~PPU();
//////
	static void Memory_Mirror(unsigned short address, unsigned short start_address_chunk, unsigned short end_address_chunk, unsigned short start_address_mirror, unsigned short end_address_mirror);
//////	//static void Secondary_OAM_Clear(ushort ppu_cycle);
//////	//static void Evaluate_Status_Values();
//////	//static void Fire_Scanline();
//////
//////	//// NTSC
//////	//static void NTSC_Visible_Scanline();
//////	//static void NTSC_Post_Render_Scanline();
//////	//static void NTSC_VBlank_Scanline();
//////	//static void NTSC_Pre_Render_Scanline();
//////
//////	//// PAL
//////	//static void PAL_Visible_Scanline();
//////	//static void PAL_Post_Render_Scanline();
//////	//static void PAL_VBlank_Scanline();
//////	//static void PAL_Pre_Render_Scanline();
//////
//////	//// Needed methods for fetching ppu data
//////	//static ushort Get_Nametable_Address();
//////	//static ushort Get_Attribute_Table_Address();
//////	//static ushort Get_Background_Tile_Address();
//////
//////	//// Temporary methods for command prompt output
//////	//static void Visible_Scanline_Sprite();			// Outputs for the command Prompt
//////	//static void Visible_Scanline_Background();		// Outputs for the command Prompt
//////	//static void Post_Render_Scanline_Sprite();		// Outputs for the command Prompt
//////	//static void Post_Render_Scanline_Background();	// Outputs for the command Prompt
//////	//static void VBlank_Scanline_Sprite();			// Outputs for the command Prompt
//////	//static void VBlank_Scanline_Background();		// Outputs for the command Prompt
//////	//static void Pre_Render_Scanline_Sprite();		// Outputs for the command Prompt
//////	//static void Pre_Render_Scanline_Background();	// Outputs for the command Prompt
//////	//static void Stats(ushort temp_cycles);			// Outputs for the command Prompt
	//////////////////////
	//Constants:
	//////////////////////

	// See http://wiki.nesdev.com/w/index.php/Overscan for extra details
	// on top of http://wiki.nesdev.com/w/index.php/Cycle_reference_chart

	// PPU Name
	const char *NTSC_PPU_NAME = "Ricoh 2C02";

	// Resolution
	const ushort NTSC_PICTURE_WIDTH = 256;
	const ushort NTSC_PICTURE_HEIGHT = 240;
	const ushort NTSC_NOMINAL_VISIBLE_PICTURE_HEIGHT = 224; // Includes overscan (16 lines / 2) in half for top and bottom covered by CRTVs
	const ushort NTSC_TOTAL_PICTURE_WIDTH = 341;
	const ushort NTSC_TOTAL_PICTURE_HEIGHT = 262;
	const float NTSC_EXTRA_SCANLINE_WIDTH = 340.5f; // Note: Only for NTSC. Missing half a pixel
	const ushort NTSC_NUMBER_OF_VERTICAL_BLANK_SCANLINES = 20; // scanlines (≈ 2273 CPU cycles)
	const ushort NTSC_HORIZONTAL_BLANK_WIDTH = 85; // pixels wide
	const int NTSC_PIXELS_PER_FRAME = NTSC_TOTAL_PICTURE_WIDTH * NTSC_TOTAL_PICTURE_HEIGHT; // 341 × 262 = 89342 even frames, 341 × 261 + 340.5 = 89341.5 odd frames
	const int NTSC_PIXELS_PER_FRAME_AT_ZERO = NTSC_PIXELS_PER_FRAME - 1;

	// Clock Speeds
	const float NTSC_PPU_CLOCK_SPEED = 5.369318f; // MHz	21.477272 MHz machine clock speed ÷ 4 Machine cycles per ppu cycle
	const float NTSC_PPU_PIXELS_PER_CYCLE = 3.0f;
	const float NTSC_VERTICAL_SCAN_RATE = 60.0988f; // Hz
	const ushort NTSC_NUMBER_OF_PPU_CYCLES_PER_SCANLINE = 341; // ppu cycles per scanline
	const float NTSC_PPU_CYCLES_PER_CPU_CYCLE = 3.0f;
	const ushort NTSC_MACHINE_CYCLES_PER_PPU_CYCLE = 4;

	// Scanline Positions
	const ushort NTSC_VISIBLE_PICTURE_BEGIN = 0;
	const ushort NTSC_VISIBLE_PICTURE_END = 239;
	const ushort NTSC_POST_RENDER_SCANLINE = 240;
	const ushort NTSC_VERTICAL_BLANK_BEGIN = 241;
	const ushort NTSC_VERTICAL_BLANK_END = 260;
	const ushort NTSC_PRE_RENDER_SCANLINE = 261;

	// TODO: Pixel Positions


	// For NTSC, OAM can be writen when Vertical or forced blanking occurs
	// For NTSC, side and bottom bordes are Palette entry at $3F00
	// For NTSC, color emphasis is Blue (D7), green (D6), red (D5)

	// PPU Name
	const char *PAL_PPU_NAME = "Ricoh 2C07";

	// Resolution
	const ushort PAL_PICTURE_WIDTH = 256;
	const ushort PAL_PICTURE_HEIGHT = 239; // They say its 239 but I'm still missing a scanline to make it the 312 scanlines mentioned. 239 + 70 + 2 = 311
	const ushort PAL_NOMINAL_VISIBLE_PICTURE_HEIGHT = 268; // Includes overscan (29 lines / 2) in half for top and bottom covered by CRTVs
	const ushort PAL_TOTAL_PICTURE_WIDTH = 341;
	const ushort PAL_TOTAL_PICTURE_HEIGHT = 312;
	const ushort PAL_NUMBER_OF_VERTICAL_BLANK_SCANLINES = 70; // scanlines (≈ 7459 CPU cycles)
	const ushort PAL_HORIZONTAL_BLANK_WIDTH = 85; // pixels wide
	const int PAL_PIXELS_PER_FRAME = PAL_TOTAL_PICTURE_WIDTH * PAL_TOTAL_PICTURE_HEIGHT; // 341 × 312 = 106392
	const int PAL_PIXELS_PER_FRAME_AT_ZERO = PAL_PIXELS_PER_FRAME - 1;

	// Clock Speeds
	const float PAL_PPU_CLOCK_SPEED = 5.3203424f; // MHz		26.601712 MHz ÷ 5
	const float PAL_PPU_PIXELS_PER_CYCLE = 3.2f;
	const float PAL_VERTICAL_SCAN_RATE = 50.0070f; // Hz
	const ushort PAL_NUMBER_OF_PPU_CYCLES_PER_SCANLINE = 341; // ppu cycles per scanline
	const float PAL_PPU_CYCLES_PER_CPU_CYCLE = 3.2f;
	const ushort PAL_MACHINE_CYCLES_PER_PPU_CYCLE = 5;

	// Scanline Positions
	const ushort PAL_VISIBLE_PICTURE_BEGIN = 0;
	const ushort PAL_VISIBLE_PICTURE_END = 239;
	const ushort PAL_POST_RENDER_SCANLINE = 240;
	const ushort PAL_VERTICAL_BLANK_BEGIN = 241;
	const ushort PAL_VERTICAL_BLANK_END = 310;
	const ushort PAL_PRE_RENDER_SCANLINE = 311;

	// TODO: Pixel Positions


	// For PAL, OAM can be written only during first 20 scanlines after NMI (≈2131 CPU cycles)
	// For PAL, side and bottom bordes are always black ($0E), intruding on left and right 2 pixels and top 1 pixel of picture
	// For PAL, color emphasis is Blue (D7), red (D6), green (D5)

	static PPU_CONTROLLER ppu_controller;	
	static PPU_MASK ppu_mask;				// $2001 (Write Only)
	static PPU_STATUS ppu_status;			// $2002 (Read Only)
	static OAM_ADDRESS oam_address;			// $2003 (Write Only)
	static OAM_DATA oam_data;				// $2004 (Read / Write)
	static PPU_SCROLL ppu_scroll;			// $2005 (Write Only x2)
	static PPU_ADDRESS ppu_address;			// $2006 (Write Only x2)
	static PPU_DATA ppu_data;				// $2007 (Read / Write)


	static int cycles; // 4 machine cycles per ppu cycle (5 with PAL). Runs at 5.369318 MHz (5.3203424 MHz PAL)
	static ushort ppu_cycle_on_current_scanline; // ppu cycle count on the current scanline

//////	static REGION region;
	static int frame_count;
	static bool odd_frame; // Sets if an odd frame. (NTSC only)
//////	static float fps; // Number of frames per second. May not keep it in ppu and move it to where the main loop is.
//////					  // Only keep ppu chip related data here.
//////
//////	static ushort cpu_cycle_on_current_scanline; // cpu cycle count fired as it goes through the scanline
//////	static bool fracion_portion_of_cpu_cycle;
//////	static int sleep_time;
	static bool vram_address_high_low_toggle;
	static VRAM_ADDRESS vram_address_register;
	static VRAM_ADDRESS temp_vram_address_register;
//////	static byte fine_x_scroll;
	static ushort scanline;
//////	static ushort address;
//////	static byte nametable_byte;
//////	static byte attribute_table_byte;
//////	static byte background_low_tile_byte;
//////	static byte background_high_tile_byte;
//////
//////	static ushort unused_address;
//////	static byte unused_nametable_byte;
//////	static byte unused_attribute_table_byte;
//////	static byte unused_background_low_tile_byte;
//////	static byte unused_background_high_tile_byte;
//////
//////	static byte oam_memory[0xff];
//////	static SPRITE oam[8];
//////	static SPRITE secondary_oam[8];
//////
//////	static byte latch;
//////
//////	static STATUS status;
};

extern PPU ppu;

//Function Prototypes

//Sprites
int Sprite_Attributes_Get_Pallete(int Sprite_Attributes);
bool Sprite_Attributes_Get_Drawn_In_Background(int Sprite_Attributes);
bool Sprite_Attributes_Get_Horizontal_Flip(int Sprite_Attributes);
bool Sprite_Attributes_Get_Vertical_Flip(int Sprite_Attributes);

byte Get_Coarse_X(int address);
int Set_Coarse_X(int address, byte data);
byte Get_Coarse_Y(int address);
int Set_Coarse_Y(int address, byte data);
byte Get_Fine_Y(int address);
int Set_Fine_Y(int address, byte data);

void Do_Mirroring();
void Render_Scanline(int Scanline);
void Scanline_Start();
int Scanline_Start(int Address1, int Address2);
void Next_Frame();
void Next_Line();
int Next_Line(int address);
void Next_Tile(int &address);
void Fill_Color_LUT();
void Create_Real_NES_Pallete_NTSC();
void Blit_Screen();

//Global Variables
extern byte Sprite_Address_Register; //$2003

extern byte Mirror_Number[4];
extern int Mirror_Offset;
extern byte Name_Table_Array[4][0x400];
extern byte CHR_RAM[0x10000];
extern byte SPR_RAM[256];
extern byte Video_Buffer[256 * 240];
extern int Video_Buffer32[256 * 240];
extern bool Render_Frame;
extern byte Color_LUT[65536 * 8];
extern int Pallete[256];

//extern int vram_address_register;
//extern int temp_vram_address_register;
extern int loopy_x;

extern int Horizontal_Scroll_Offset_Register;
extern int Vertical_Scroll_Offset_Register;
extern ushort scanline;
extern int Amount_To_Increment;
extern int Name_Table_Address;


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


//--------------------------------------------------------------------------------

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