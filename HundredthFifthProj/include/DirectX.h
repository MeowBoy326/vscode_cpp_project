#pragma once
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800
#include <d3dx9.h>
#include <d3d9.h>
#include <ctime>
#include <Windows.h>
#include <string>
#include <dinput.h>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <stack>
#include <functional>
#include <yvals.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"dxguid.lib")

//������
#define D3DFVF_MYVETREX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define SCREENW 600		//��Ļ���
#define SCREENH 600		//��Ļ�߶�

using std::stack;
using std::function;

struct Vetrex
{
	D3DXVECTOR3 pos;		//λ������
	D3DXVECTOR3 normal;		//����
	float u, v;				//��������
	Vetrex()
		: pos{}, normal{}, u{}, v{}
	{
	}

	Vetrex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: pos(x,y,z), normal(nx, ny, nz), u(u), v(v)
	{
	}
};

extern LPDIRECT3D9 d3d;							//D3D9�ӿ�
extern LPDIRECT3DDEVICE9 d3ddev;				//D3D9�豸
extern D3DPRESENT_PARAMETERS d3dpp;				//D3D����


extern LPDIRECTINPUT8 dinput;					//����ӿ�
extern LPDIRECTINPUTDEVICE8 dikeyboard;			//���̽ӿ�
extern char keys[256];							//��¼����״̬
extern LPDIRECTINPUTDEVICE8 dimouse;			//���ӿ�
extern DIMOUSESTATE mouse_state;				//���״̬
extern bool slide;								//��껬����


extern LPDIRECT3DTEXTURE9 red, yellow, blue,
green, white, orange, black;		//ħ������ɫ

extern LPDIRECT3DVERTEXBUFFER9 vb;				//D3D���㻺��
extern LPDIRECT3DINDEXBUFFER9 ib;				//D3D���㻺��

extern LPD3DXFONT font;							//����
extern RECT font_rect;							//�������
extern RECT window_rect;						//�������
extern POINT mouse_pos;							//���λ��
extern POINT click_pos;							//���λ��

extern DWORD start_time;						//��Ϸ��ʼ��ʱ��
extern DWORD finish_time;						//���ħ����ʱ��
extern DWORD current_time;						//��ʱ��
extern DWORD frameCnt;							//֡������
extern DWORD fps;								//֡ÿ��				
extern DWORD delay;								//һ֡���
extern DWORD frame_timer;						//֡��ʱ��
extern DWORD key_down_counter;					//�������������18֡һ��
extern DWORD slide_delay;						//�����ӳ�

extern bool use_pick;							//Ϊtrueʱʹ�ö�̬ʰȡ��falseʱʹ�þ�̬���

//�������ʼλ�ã�����Ŀ�꣬��Ӱ�����ӽ�
extern D3DXVECTOR3 pos;
extern D3DXVECTOR3 target;
extern D3DXVECTOR3 up;

extern D3DMATERIAL9 material;					//�������

extern D3DLIGHT9 light;							//�ƹ�

extern stack<function<void()>> func_stk;		//����ջ��������ħ�������

//������
// DirectX����
LPDIRECT3DTEXTURE9 LoadTexture(const char * filename);


void WorldTransform();
void ViewTransform(const D3DXVECTOR3& eye, const D3DXVECTOR3& lookAt, const D3DXVECTOR3& up);
void PerspectiveTransform(float fieldOfView, float aspectRatio, float nearRange, float farRange);


bool Direct3D_Init(HWND window);
void Direct3D_Reset(HWND window);
void Direct3D_Shutdown();

//DirectInput����
bool DirectInput_Init(HWND window);
void DirectInput_Update();
void DirectInput_Shutdown();
bool Key_Down(int key);
bool Mouse_Click(int button);

//��ѧ����
double MySin(double _X);
double MyCos(double _X);
//��Ϸ����
bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();
void PreView();


void Key_Update(HWND window);
void Mouse_Update(HWND window);

bool Click_Rect(const POINT& LeftBottom, const POINT& LeftTop, const POINT& RightTop, const POINT& RightBottom);
bool IsPick(HWND window, const Vetrex& LeftBottom, const Vetrex& LeftTop, const Vetrex& RightTop, const Vetrex& RightBottom);
//�Զ�������ȸ�����ת����ֻ����VS2017��ʹ�ã�
_STD_BEGIN
std::string to_string(float _Val, int precision);
_STD_END