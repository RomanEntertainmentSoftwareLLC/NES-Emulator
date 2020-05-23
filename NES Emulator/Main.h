#pragma once

//Preprocessor Directives
#include <Windows.h>
#include <Commdlg.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <tchar.h>

using namespace std;

//Data Types
typedef unsigned char byte;
//typedef unsigned short ushort;

//Function Prototypes
bool Load_Cartridge(const char *File_Path);
void Free_ROM();
void Default_Recent_ROMS();
void Render();
void Shutdown();
//int main();

//Mirroring Enumeration
enum MIRRORING_ENUM{HORIZONTAL, VERTICAL, SINGLE_SCREEN, FOUR_SCREEN}; 

struct nes_rom_Type
{
	byte PRG_ROM_Pages;
	byte CHR_ROM_Pages;
	vector<byte> PRG_ROM;
	vector<byte> CHR_ROM;
	byte Mapper;
	MIRRORING_ENUM Mirroring;
	bool Save_RAM_Present;
	bool Trainer_Present;
	string File_Name;
};

const int WINDOW_WIDTH = 256;
const int WINDOW_HEIGHT = 240;
const int WINDOW_WIDTH_WITH_BORDER = (WINDOW_WIDTH + 32) + 16;
const int WINDOW_HEIGHT_WITH_BORDER = WINDOW_HEIGHT + 21 + (30 + 8);
const int NES_SCREEN_RESOLUTION = WINDOW_WIDTH * WINDOW_HEIGHT;

extern HWND hWnd;
extern bool Fullscreen_Enabled;
extern bool Running;
extern nes_rom_Type nes_rom;
extern byte Train[512];
extern bool Cart_Loaded;
extern string Recent_ROM[5];
extern int Recent_ROM_ID[5];
extern string Recent_ROM_Path[5];
extern bool Step;
extern int Old_Frame;
extern bool Step_Flag;
extern int window_width;
extern int window_height;