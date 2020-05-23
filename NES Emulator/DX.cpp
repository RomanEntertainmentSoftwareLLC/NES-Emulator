#include "DX.h"
#include "ppu.h"
#include "Main.h"

LPDIRECT3D9 D3D = NULL;
LPDIRECT3DDEVICE9 Device = NULL;
D3DDISPLAYMODE Display_Mode;
D3DPRESENT_PARAMETERS Screen;
CUSTOM_VERTEX Vertex_List[4];
CUSTOM_VERTEX Title_Vertex_List[4];
LPDIRECT3DTEXTURE9 Title_Texture;
LPDIRECT3DTEXTURE9 Texture;
D3DLOCKED_RECT pData;
LPD3DXFONT pFont;

void DirectX_Initialize() {
	D3D = Direct3DCreate9(D3D_SDK_VERSION);
	memset(&Screen, 0, sizeof(D3DPRESENT_PARAMETERS));

	if (Fullscreen_Enabled == true) {
		Display_Mode.Width = 800;
		Display_Mode.Height = 600;
		Display_Mode.Format = D3DFMT_R5G6B5;
		Screen.Windowed = FALSE;
		Screen.BackBufferCount = 1;
		Screen.BackBufferWidth = Display_Mode.Width;
		Screen.BackBufferHeight = Display_Mode.Height;
		Screen.hDeviceWindow = hWnd;
	}
	else {
		D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display_Mode);
		Screen.Windowed = TRUE;
		Screen.BackBufferCount = 1;
		Screen.BackBufferWidth = window_width;
		Screen.BackBufferHeight = window_height;
	}
	Screen.SwapEffect = D3DSWAPEFFECT_COPY;
	Screen.BackBufferFormat = Display_Mode.Format;
	D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &Screen, &Device);

	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	//Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);
	//Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	//Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	//Device->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 4);
	//Device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	//Device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);

		// Create the D3DX Font
	D3DXCreateFont(Device, 12, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);


}

CUSTOM_VERTEX Create_Custom_Vertex(float X, float Y, DWORD Color, float TU, float TV)
{
	CUSTOM_VERTEX Vertex;

	Vertex.X = X;
	Vertex.Y = Y;
	Vertex.Z = 0;
	Vertex.RHW = 1;
	Vertex.Color = Color;
	Vertex.TU = TU;
	Vertex.TV = TV;

	return Vertex;
}

void Setup_Texture(LPDIRECT3DTEXTURE9 *T)
{
	D3DXCreateTexture(Device, 256, 256, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, T);
}

void Load_Texture_From_File(LPDIRECT3DTEXTURE9 *T, char *File_Path, int Transparency_Color)
{
	D3DXCreateTextureFromFileEx(Device, File_Path, 2048, 2048, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_FILTER_POINT, D3DX_FILTER_POINT, Transparency_Color, NULL, NULL, T);
}

void Load_Texture_From_Resource(LPDIRECT3DTEXTURE9 *T, int Resource, int Transparency_Color)
{
	D3DXCreateTextureFromResourceEx(Device, GetModuleHandle(NULL), MAKEINTRESOURCE(Resource), 2048, 2048, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, Transparency_Color, NULL, NULL, T);
}

void Create_Polygon()
{
	float x_offset = 16.0f;

	Vertex_List[0] = Create_Custom_Vertex(x_offset + 0.0f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 0, 0);
	Vertex_List[1] = Create_Custom_Vertex(x_offset + static_cast<float>(WINDOW_WIDTH), 0.0f, D3DCOLOR_XRGB(255, 255, 255), 1, 0);
	Vertex_List[2] = Create_Custom_Vertex(x_offset + 0.0f, static_cast<float>(WINDOW_WIDTH), D3DCOLOR_XRGB(255, 255, 255), 0, 1);
	Vertex_List[3] = Create_Custom_Vertex(x_offset + static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_WIDTH), D3DCOLOR_XRGB(255, 255, 255), 1, 1);
}

void Create_Title_Polygon()
{
	Title_Vertex_List[0] = Create_Custom_Vertex(16.0f + 0.0f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 0, 0);
	Title_Vertex_List[1] = Create_Custom_Vertex(16.0f + static_cast<float>(WINDOW_WIDTH), 0, D3DCOLOR_XRGB(255, 255, 255), 1, 0);
	Title_Vertex_List[2] = Create_Custom_Vertex(16.0f + 0.0f, static_cast<float>(WINDOW_HEIGHT), D3DCOLOR_XRGB(255, 255, 255), 0, 1);
	Title_Vertex_List[3] = Create_Custom_Vertex(16.0f + static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), D3DCOLOR_XRGB(255, 255, 255), 1, 1);
}

void Draw_Polygon()
{
	Device->SetFVF(CUSTOM_VERTEX_FORMAT);
	Device->SetTexture(0, Title_Texture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Title_Vertex_List, sizeof(CUSTOM_VERTEX));
}

void Draw_To_Texture()
{
	vector<int> newArr;

	//NES resulution - 256x240
	Texture->LockRect(0, &pData, 0, 0);

	newArr.resize(WINDOW_WIDTH * pData.Pitch);
	memcpy(newArr.data(), Video_Buffer32, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(int));

	//realloc(Video_Buffer32, 256 * pData.Pitch);
	//Video_Buffer32[10 + pData.Pitch * 10] = D3DCOLOR_XRGB(0, 0, 0);
	memcpy(pData.pBits, newArr.data(), WINDOW_WIDTH * pData.Pitch);
	Texture->UnlockRect(0);
}

void Draw_Frame()
{
    Draw_To_Texture();
    Device->SetFVF(CUSTOM_VERTEX_FORMAT);
    Device->SetTexture(0, Texture);
    Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertex_List, sizeof(CUSTOM_VERTEX));
}

void DrawTextString(const char *str, int x, int y, D3DCOLOR color)
{
	RECT TextRect = { x, y, 0, 0 };
	pFont->DrawText(NULL, str, -1, &TextRect, DT_LEFT | DT_NOCLIP, color);
}

void Draw_Pixel(float X1, float Y1, int Color)
{
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	Vertex_List[0] = Create_Custom_Vertex(X1, Y1, Color, 0, 0);
	Device->SetTexture(0, NULL);
	Device->DrawPrimitiveUP(D3DPT_POINTLIST, 1, Vertex_List, sizeof(CUSTOM_VERTEX));
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void Draw_Line(float X1, float Y1, float X2, float Y2, int Color)
{
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	Vertex_List[0] = Create_Custom_Vertex(X1, Y1, Color, 0, 0);
	Vertex_List[1] = Create_Custom_Vertex(X2, Y2, Color, 1, 0);
	Device->SetTexture(0, NULL);
	Device->DrawPrimitiveUP(D3DPT_LINELIST, 1, Vertex_List, sizeof(CUSTOM_VERTEX));
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void Draw_Rectangle(float X1, float Y1, float X2, float Y2, int Color)
{
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	Vertex_List[0] = Create_Custom_Vertex(X1, Y1, Color, 0, 0);
	Vertex_List[1] = Create_Custom_Vertex(X2, Y1, Color, 1, 0);
	Vertex_List[2] = Create_Custom_Vertex(X1, Y2, Color, 0, 1);
	Vertex_List[3] = Create_Custom_Vertex(X2, Y2, Color, 1, 1);
	Device->SetTexture(0, NULL);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertex_List, sizeof(CUSTOM_VERTEX));
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}