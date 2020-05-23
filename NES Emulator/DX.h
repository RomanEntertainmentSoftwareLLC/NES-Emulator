#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <stdlib.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

struct CUSTOM_VERTEX
{
	float X, Y, Z, RHW;
	DWORD Color;
	float TU, TV;
};

#define CUSTOM_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE)

void DirectX_Initialize();
CUSTOM_VERTEX Create_Custom_Vertex(float X, float Y, DWORD Color, float TU, float TV);
void Setup_Texture(LPDIRECT3DTEXTURE9 *T);
void Load_Texture_From_File(LPDIRECT3DTEXTURE9 *T, char *File_Path, int Transparency_Color);
void Load_Texture_From_Resource(LPDIRECT3DTEXTURE9 *T, int Resource, int Transparency_Color);
void Create_Polygon();
void Create_Title_Polygon();
void Draw_Polygon();
void Draw_To_Texture();
void Draw_Frame();
void DrawTextString(const char *str, int x, int y, D3DCOLOR color);
void Draw_Pixel(float X1, float Y1, int Color);
void Draw_Line(float X1, float Y1, float X2, float Y2, int Color);
void Draw_Rectangle(float X1, float Y1, float X2, float Y2, int Color);

extern LPDIRECT3DDEVICE9 Device;
extern LPDIRECT3D9 D3D;
extern CUSTOM_VERTEX Vertex_List[4];
extern CUSTOM_VERTEX Title_Vertex_List[4];
extern LPDIRECT3DTEXTURE9 Title_Texture;
extern LPDIRECT3DTEXTURE9 Texture;
extern D3DLOCKED_RECT pData;
extern LPD3DXFONT pFont;