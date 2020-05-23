#pragma once

#include "DX.h"

typedef unsigned char byte;
//typedef unsigned short ushort;

void DirectInput_Initialize(HINSTANCE hInstance);
void DirectInput_Initialize_Keyboard();
bool DirectInput_Key_State(int Key_Code);
int Get_Controller1_Data();
int Get_Controller2_Data();

extern LPDIRECTINPUT8 DI;
extern LPDIRECTINPUTDEVICE8 Keyboard_Device;
extern byte Keyboard_State[256];

extern int Controller_1;
extern int Controller_2;

extern byte Get_Key[256];