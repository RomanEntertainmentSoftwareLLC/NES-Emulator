#include "Controls.h"
#include "Main.h"

int Controller_1 = 0x0;
int Controller_2 = 0x0;

byte Get_Key[256] = { 0 };

LPDIRECTINPUT8 DI;
LPDIRECTINPUTDEVICE8 Keyboard_Device;
byte Keyboard_State[256];

void DirectInput_Initialize(HINSTANCE hInstance)
{
	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DI, NULL);
}

void DirectInput_Initialize_Keyboard()
{
	DI->CreateDevice(GUID_SysKeyboard, &Keyboard_Device, NULL);
	Keyboard_Device->SetDataFormat(&c_dfDIKeyboard);
	Keyboard_Device->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	Keyboard_Device->Acquire();
}

bool DirectInput_Key_State(int Key_Code)
{
	Keyboard_Device->GetDeviceState(256, (LPVOID)Keyboard_State);
	return Keyboard_State[Key_Code] && 0x80;
}

int Get_Controller1_Data()
{
	int num = 0;

	if (DirectInput_Key_State(DIK_X))
		num |= 0x1;

	if (DirectInput_Key_State(DIK_Z))
		num |= 0x2;

	if (DirectInput_Key_State(DIK_RSHIFT))
		num |= 0x4;

	if (DirectInput_Key_State(DIK_RETURN))
		num |= 0x8;

	if (DirectInput_Key_State(DIK_UP))
		num |= 0x10;

	if (DirectInput_Key_State(DIK_DOWN))
		num |= 0x20;

	if (DirectInput_Key_State(DIK_LEFT))
		num |= 0x40;

	if (DirectInput_Key_State(DIK_RIGHT))
		num |= 0x80;

	if (DirectInput_Key_State(DIK_SPACE))
		Step = true;

	//if (DirectInput_Key_State(DIK_NUMPAD1))
	//	num |= 0x200;

	//if (DirectInput_Key_State(DIK_NUMPAD0))
	//	num |= 0x400;

	//if (DirectInput_Key_State(DIK_NUMPADENTER))
	//	num |= 0x800;

	//if (DirectInput_Key_State(DIK_NUMPAD8))
	//	num |= 0x1000;

	//if (DirectInput_Key_State(DIK_NUMPAD5))
	//	num |= 0x2000;

	//if (DirectInput_Key_State(DIK_NUMPAD4))
	//	num |= 0x4000;

	//if (DirectInput_Key_State(DIK_NUMPAD6))
	//	num |= 0x8000;

	return num;
}

int Get_Controller2_Data()
{
	int num = 0x0;

	if (DirectInput_Key_State(DIK_NUMPAD2))
		num |= 0x1;

	if (DirectInput_Key_State(DIK_NUMPAD1))
		num |= 0x2;

	if (DirectInput_Key_State(DIK_NUMPAD0))
		num |= 0x4;

	if (DirectInput_Key_State(DIK_NUMPADENTER))
		num |= 0x8;

	if (DirectInput_Key_State(DIK_NUMPAD8))
		num |= 0x10;

	if (DirectInput_Key_State(DIK_NUMPAD5))
		num |= 0x20;

	if (DirectInput_Key_State(DIK_NUMPAD4))
		num |= 0x40;

	if (DirectInput_Key_State(DIK_NUMPAD6))
		num |= 0x80;

	return num;
}
