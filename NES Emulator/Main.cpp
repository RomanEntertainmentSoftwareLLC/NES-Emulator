#include "Main.h"
#include "6502 cpu.h"
#include "6502 cpu memory.h"
#include "mmc.h"
#include "mappers.h"
#include "DX.h"
#include "resource.h"
#include "ppu.h"
#include "Math.h"
#include "Controls.h"

nes_rom_Type nes_rom;
byte Train[512]; //512 byte trainer found in some games for memory locations $7000-$71FF

bool Fullscreen_Enabled = false;
bool Running = false;
bool Cart_Loaded = false;

HWND hWnd;
MSG msg;
HMENU menu;
HACCEL hAccel;
MENUITEMINFO menuInfo;

OPENFILENAME Common_Dialog;
char NES_File_Path[1024];
string Recent_ROM[5];
int Recent_ROM_ID[5];
string Recent_ROM_Path[5];
bool Step = false;
int Old_Frame = 0;
bool Step_Flag = false;

int window_width;
int window_height;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW | CS_OWNDC, WindowProcedure, 0, 0, hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2)), LoadCursor(NULL, IDC_ARROW), NULL, MAKEINTRESOURCE(IDR_MENU1), "NES Emulator Class", NULL };
	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(0, "NES Emulator Class", "NES Emulator", WS_OVERLAPPEDWINDOW, (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH_WITH_BORDER) / 2, (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT_WITH_BORDER) / 2, WINDOW_WIDTH_WITH_BORDER, WINDOW_HEIGHT_WITH_BORDER, HWND_DESKTOP, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	menu = GetMenu(hWnd);
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	EnableMenuItem(menu, ID_FILE_FREENESROM, MF_GRAYED);
	EnableMenuItem(menu, ID_FILE_LOADSTATE, MF_GRAYED);
	EnableMenuItem(menu, ID_FILE_SAVESTATE, MF_GRAYED);
	CheckMenuItem(menu, ID_SLOT_0, MF_CHECKED);

	Recent_ROM_ID[0] = ID_RECENTROM_1;
	Recent_ROM_ID[1] = ID_RECENTROM_2;
	Recent_ROM_ID[2] = ID_RECENTROM_3;
	Recent_ROM_ID[3] = ID_RECENTROM_4;
	Recent_ROM_ID[4] = ID_RECENTROM_5;

	Create_Real_NES_Pallete_NTSC();
	Fill_Color_LUT();
	Default_Recent_ROMS();
	DirectX_Initialize();
	DirectInput_Initialize(hInstance);
	DirectInput_Initialize_Keyboard();
	Load_Texture_From_Resource(&Title_Texture, IDB_BITMAP1, D3DCOLOR_XRGB(0, 0, 0));
	Setup_Texture(&Texture);

	//FrameSkip = 1;
	Fullscreen_Enabled = false;
	Running = true;

	while (Running == true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			if (WM_QUIT == msg.message) break;
			TranslateAccelerator(hWnd, hAccel, &msg);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (Cart_Loaded == true)
			{
				cpu.Execute();
			}
			else
			{
				Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
				Device->BeginScene();
				Create_Title_Polygon();
				Draw_Polygon();
				Device->EndScene();
				Device->Present(NULL, NULL, NULL, NULL);
			}
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Shutdown();
		break;
	case WM_CLOSE:
		if (MessageBox(NULL, "Are you sure you want to exit?", "NES Emulator", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			DestroyWindow(hWnd);
			return(0);
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(NULL, "Are you sure you want to exit?", "NES Emulator", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hWnd);
				return(0);
			}
			break;
		}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_RECENTROM_1:
		{
			Cart_Loaded = Load_Cartridge(Recent_ROM_Path[0].c_str());

			if (Cart_Loaded == true)
			{
				menu = GetMenu(hWnd);

				EnableMenuItem(menu, ID_FILE_FREENESROM, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_LOADSTATE, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_SAVESTATE, MF_ENABLED);

				ppu.ppu_controller.power_reset_30000_cycle_write = false;
				ppu.vram_address_high_low_toggle = false;
				ppu.vram_address_register.address = 0;
				Sprite_Address_Register = 0;
				ppu.ppu_controller.reg = 0;
				ppu.ppu_mask.reg = 0;
				ppu.ppu_status.reg = 0;
			}
			break;
		}

		case ID_RECENTROM_2:
		{
			Cart_Loaded = Load_Cartridge(Recent_ROM_Path[1].c_str());

			if (Cart_Loaded == true)
			{
				menu = GetMenu(hWnd);

				EnableMenuItem(menu, ID_FILE_FREENESROM, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_LOADSTATE, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_SAVESTATE, MF_ENABLED);

				ppu.ppu_controller.power_reset_30000_cycle_write = false;
				ppu.vram_address_high_low_toggle = false;
				ppu.vram_address_register.address = 0;
				Sprite_Address_Register = 0;
				ppu.ppu_controller.reg = 0;
				ppu.ppu_mask.reg = 0;
				ppu.ppu_status.reg = 0;
			}
			break;
		}
		case ID_RECENTROM_3:
		{
			Cart_Loaded = Load_Cartridge(Recent_ROM_Path[2].c_str());

			if (Cart_Loaded == true)
			{
				menu = GetMenu(hWnd);

				EnableMenuItem(menu, ID_FILE_FREENESROM, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_LOADSTATE, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_SAVESTATE, MF_ENABLED);

				ppu.ppu_controller.power_reset_30000_cycle_write = false;
				ppu.vram_address_high_low_toggle = false;
				ppu.vram_address_register.address = 0;
				Sprite_Address_Register = 0;
				ppu.ppu_controller.reg = 0;
				ppu.ppu_mask.reg = 0;
				ppu.ppu_status.reg = 0;
			}
			break;
		}
		case ID_RECENTROM_4:
		{
			Cart_Loaded = Load_Cartridge(Recent_ROM_Path[3].c_str());

			if (Cart_Loaded == true)
			{
				menu = GetMenu(hWnd);

				EnableMenuItem(menu, ID_FILE_FREENESROM, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_LOADSTATE, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_SAVESTATE, MF_ENABLED);

				ppu.ppu_controller.power_reset_30000_cycle_write = false;
				ppu.vram_address_high_low_toggle = false;
				ppu.vram_address_register.address = 0;
				Sprite_Address_Register = 0;
				ppu.ppu_controller.reg = 0;
				ppu.ppu_mask.reg = 0;
				ppu.ppu_status.reg = 0;
			}
			break;
		}
		case ID_RECENTROM_5:
		{
			Cart_Loaded = Load_Cartridge(Recent_ROM_Path[4].c_str());

			if (Cart_Loaded == true)
			{
				menu = GetMenu(hWnd);

				EnableMenuItem(menu, ID_FILE_FREENESROM, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_LOADSTATE, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_SAVESTATE, MF_ENABLED);

				ppu.ppu_controller.power_reset_30000_cycle_write = false;
				ppu.vram_address_high_low_toggle = false;
				ppu.vram_address_register.address = 0;
				Sprite_Address_Register = 0;
				ppu.ppu_controller.reg = 0;
				ppu.ppu_mask.reg = 0;
				ppu.ppu_status.reg = 0;
			}
			break;
		}
		case ID_FILE_OPENNESROM:
		{
			ZeroMemory(&Common_Dialog, sizeof(Common_Dialog));
			Common_Dialog.lStructSize = sizeof(Common_Dialog);
			Common_Dialog.hwndOwner = NULL;
			Common_Dialog.lpstrFile = NES_File_Path;
			Common_Dialog.lpstrFile[0] = '\0';
			Common_Dialog.nMaxFile = sizeof(NES_File_Path);
			Common_Dialog.lpstrFilter = "NES\0*.nes\0All\0*.*\0";
			Common_Dialog.nFilterIndex = 1;
			Common_Dialog.lpstrFileTitle = NULL;
			Common_Dialog.nMaxFileTitle = 0;
			Common_Dialog.lpstrInitialDir = NULL;
			Common_Dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			GetOpenFileName(&Common_Dialog);

			if (*NES_File_Path != NULL) Cart_Loaded = Load_Cartridge(NES_File_Path);

			if (Cart_Loaded == true)
			{
				menu = GetMenu(hWnd);

				Recent_ROM_Path[4] = Recent_ROM_Path[3];
				Recent_ROM_Path[3] = Recent_ROM_Path[2];
				Recent_ROM_Path[2] = Recent_ROM_Path[1];
				Recent_ROM_Path[1] = Recent_ROM_Path[0];
				Recent_ROM_Path[0] = static_cast<string>(NES_File_Path);

				Recent_ROM[4] = Recent_ROM[3];
				Recent_ROM[3] = Recent_ROM[2];
				Recent_ROM[2] = Recent_ROM[1];
				Recent_ROM[1] = Recent_ROM[0];
				Recent_ROM[0] = nes_rom.File_Name;

				for (int i = 0; i <= 4; i++)
				{
					if (Recent_ROM[i].length() != 0)
						menuInfo.dwTypeData = const_cast<char *>(Recent_ROM[i].c_str());
					else
						menuInfo.dwTypeData = const_cast<char *>("Empty");

					menuInfo.cbSize = sizeof(menuInfo);
					menuInfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_DATA;
					menuInfo.fType = MFT_STRING;
					menuInfo.wID = Recent_ROM_ID[i];
					menuInfo.cch = _tcslen(Recent_ROM[i].c_str());
					SetMenuItemInfo(GetMenu(hWnd), Recent_ROM_ID[i], FALSE, &menuInfo);
				}

				EnableMenuItem(menu, ID_FILE_FREENESROM, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_LOADSTATE, MF_ENABLED);
				EnableMenuItem(menu, ID_FILE_SAVESTATE, MF_ENABLED);

				ppu.ppu_controller.power_reset_30000_cycle_write = false;
				ppu.vram_address_high_low_toggle = false;
				ppu.vram_address_register.address = 0;
				Sprite_Address_Register = 0;
				ppu.ppu_status.reg = 0;
				ppu.ppu_controller.reg = 0;
				ppu.ppu_mask.reg = 0;
			}

			break;
		}
		case ID_FILE_FREENESROM:
		{
			Free_ROM();
			break;
		}
		case ID_FILE_LOADSTATE:
		{
			MessageBox(NULL, "IM LOADED!", "NES Emulator", MB_ICONEXCLAMATION | MB_OK);
			break;
		}

		case ID_FILE_SAVESTATE:
		{
			MessageBox(NULL, "IM SAVED!", "NES Emulator", MB_ICONEXCLAMATION | MB_OK);
			break;
		}
		case ID_SLOT_0:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_1:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_2:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_3:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_4:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_5:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_6:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_7:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_8:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_CHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_UNCHECKED);
			break;
		}
		case ID_SLOT_9:
		{
			CheckMenuItem(menu, ID_SLOT_0, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_1, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_2, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_3, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_4, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_5, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_6, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_7, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_8, MF_UNCHECKED);
			CheckMenuItem(menu, ID_SLOT_9, MF_CHECKED);
			break;
		}
		case ID_FILE_EXIT:
		{
			if (MessageBox(NULL, "Are you sure you want to exit?", "NES Emulator", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hWnd);
				return(0);
			}
			break;
		}
		case ID_EMULATION_PAUSE:
		{
			if (cpu.paused == false)
			{
				cpu.paused = true;
				//Old_Frame = Frames_Rendered;
				EnableMenuItem(menu, ID_EMULATION_STEP, MF_ENABLED);
				CheckMenuItem(menu, ID_EMULATION_PAUSE, MF_CHECKED);
			}
			else
			{
				cpu.paused = false;
				EnableMenuItem(menu, ID_EMULATION_STEP, MF_DISABLED);
				CheckMenuItem(menu, ID_EMULATION_PAUSE, MF_UNCHECKED);
			}
			break;
		}
		case ID_EMULATION_STEP:
		{
			Step = true;
			break;
		}
		case ID_EMULATION_RESET:
		{
			cpu.interrupt.RESET();

			ppu.ppu_controller.power_reset_30000_cycle_write = false;
			ppu.vram_address_high_low_toggle = false;
			//Scrolling_Offset_Register = 0;
			//Sprite_Address_Register = 0;
			ppu.ppu_controller.reg = 0;
			ppu.ppu_mask.reg = 0;
			ppu.ppu_status.reg = 0;
			break;
		}
		case WM_SIZE:
		{
			window_width = LOWORD(lParam);
			window_height = HIWORD(lParam);
			break;
		}
		}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void Default_Recent_ROMS()
{
	for (int i = 0; i <= 4; i++)
	{
		menuInfo.dwTypeData = const_cast<char*>("Empty");

		menuInfo.cbSize = sizeof(menuInfo);
		menuInfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_DATA;
		menuInfo.fType = MFT_STRING;
		menuInfo.wID = Recent_ROM_ID[i];
		menuInfo.cch = _tcslen(Recent_ROM[i].c_str());
		SetMenuItemInfo(GetMenu(hWnd), Recent_ROM_ID[i], FALSE, &menuInfo);
	}
}

void Free_ROM()
{
	memset(cpu.memory_bank, 0, sizeof(cpu.memory_bank));

	memset(CHR_RAM, 0, sizeof(CHR_RAM));
	memset(SPR_RAM, 0, sizeof(SPR_RAM));
	SpecialWrite6000 = false;

	nes_rom.PRG_ROM.resize(0);
	nes_rom.CHR_ROM.resize(0);

	Cart_Loaded = false;

	EnableMenuItem(menu, ID_FILE_FREENESROM, MF_GRAYED);
	EnableMenuItem(menu, ID_FILE_LOADSTATE, MF_GRAYED);
	EnableMenuItem(menu, ID_FILE_SAVESTATE, MF_GRAYED);
}

void Render()
{
	Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0); // clear frame
	Device->BeginScene();
	//Rendering code goes here
	Device->EndScene();
	Device->Present(NULL, NULL, NULL, NULL);
}

void Shutdown()
{
	Running = false;
	Keyboard_Device->Unacquire();
	Keyboard_Device->Release();
	Device->Release();
	D3D->Release();
	CoUninitialize();
	PostQuitMessage(0);
	//HANDLE Process;
	//Process = OpenProcess(PROCESS_ALL_ACCESS, true, GetCurrentProcessId());
	//TerminateProcess(Process, 0);
}

bool Load_Cartridge(const char *File_Path)
{
	char Header[16] = { 0 };
	byte ROM_Control_Byte_1 = 0;
	byte ROM_Control_Byte_2 = 0;

	ifstream NES_File(File_Path, ios::in | ios::binary);

	if (NES_File)
	{
		NES_File.read(Header, 16);
		if (Header[0] == 'N' &&
			Header[1] == 'E' &&
			Header[2] == 'S' &&
			Header[3] == 0x1A)
		{
			memset(cpu.memory_bank, 0, sizeof(cpu.memory_bank));

			memset(CHR_RAM, 0, sizeof(CHR_RAM));
			memset(SPR_RAM, 0, sizeof(SPR_RAM));
			SpecialWrite6000 = false;

			nes_rom.PRG_ROM_Pages = Header[4];
			nes_rom.CHR_ROM_Pages = Header[5];

			nes_rom.PRG_ROM.resize(nes_rom.PRG_ROM_Pages * 0x4000);
			nes_rom.CHR_ROM.resize(nes_rom.CHR_ROM_Pages * 0x2000);

			fill(nes_rom.PRG_ROM.begin(), nes_rom.PRG_ROM.end(), 0);
			fill(nes_rom.CHR_ROM.begin(), nes_rom.CHR_ROM.end(), 0);

			ROM_Control_Byte_1 = static_cast<byte>(Header[6]);
			ROM_Control_Byte_2 = static_cast<byte>(Header[7]);

			nes_rom.Mapper = (ROM_Control_Byte_1 >> 4) + (ROM_Control_Byte_2 & 0xf0);

			if ((ROM_Control_Byte_1 & 0x1) != 0) nes_rom.Mirroring = VERTICAL; else nes_rom.Mirroring = nes_rom.Mirroring = HORIZONTAL;
			if ((ROM_Control_Byte_1 & 0x2) != 0) nes_rom.Save_RAM_Present = true;
			if ((ROM_Control_Byte_1 & 0x4) != 0) nes_rom.Trainer_Present = true;
			if ((ROM_Control_Byte_1 & 0x8) != 0) nes_rom.Mirroring = FOUR_SCREEN;

			nes_rom.File_Name = static_cast<string>(File_Path).substr(static_cast<string>(File_Path).find_last_of("\\") + 1);

			if (nes_rom.Trainer_Present)
			{
				NES_File.seekg(16);
				NES_File.read((char *)Train, 0x200);
				NES_File.seekg(528);
				NES_File.read(reinterpret_cast<char *>(nes_rom.PRG_ROM.data()), nes_rom.PRG_ROM.size());
				NES_File.seekg((nes_rom.PRG_ROM_Pages * 0x4000) + 528);
				NES_File.read(reinterpret_cast<char *>(nes_rom.CHR_ROM.data()), nes_rom.CHR_ROM.size());
			}
			else
			{
				NES_File.seekg(16);
				NES_File.read(reinterpret_cast<char *>(nes_rom.PRG_ROM.data()), nes_rom.PRG_ROM.size());
				NES_File.seekg((nes_rom.PRG_ROM_Pages * 0x4000) + 16);
				NES_File.read(reinterpret_cast<char *>(nes_rom.CHR_ROM.data()), nes_rom.CHR_ROM.size());
			}

			MMC_Reset();
			Setup_Mapper();

			cpu.interrupt.RESET();

			ppu.scanline = 0;

			ppu.ppu_controller.power_reset_30000_cycle_write = false;
			ppu.vram_address_high_low_toggle = false;
			Do_Mirroring();

			Controller_1 = 0x0;
			Controller_2 = 0x0;

			cpu.paused = false;

			char Text[255];
			sprintf_s(Text, sizeof(Text), "%s   Mapper: %i   PRG_ROM Pages: %i", nes_rom.File_Name.c_str(), nes_rom.Mapper, nes_rom.PRG_ROM_Pages);
			SetWindowText(hWnd, Text);

			//if (vram_address_high_low_toggle == true)
			//	MessageBox(NULL, "TRUE", "NES Emulator", MB_ICONERROR | MB_OK);
			//else
			//	MessageBox(NULL, "FALSE", "NES Emulator", MB_ICONERROR | MB_OK);


			return true;
		}
		else
			MessageBox(NULL, "Can't open the file. It is not a valid NES ROM.", "NES Emulator", MB_ICONERROR | MB_OK);
		return false;
	}
	else
	{
		MessageBox(NULL, "Invalid file path.", "NES Emulator", MB_ICONERROR | MB_OK);
		return false;
	}
	NES_File.close();
}