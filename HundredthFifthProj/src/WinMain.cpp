#include "DirectX.h"
using namespace std;

string APPTITLE = "ħ��";
bool gameover = false;


LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand((unsigned)time(0));
	//��ʼ�������趨
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = APPTITLE.c_str();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	//�½����ھ�����̶����ڴ�С������궨λ
	HWND window = CreateWindow(APPTITLE.c_str(), APPTITLE.c_str(),
		WS_OVERLAPPED | WS_SYSMENU, 600, 200,
		SCREENW, SCREENH, nullptr, nullptr, hInstance, nullptr);
	if (window == nullptr) return 0;

	//��ʾ����
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	//Direct3D��ʼ��
	if (!Game_Init(window))
	{
		MessageBox(nullptr, "Initializing Direct3D Failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		gameover = true;
	}

	//��Ϣ����ѭ��
	MSG message;
	while (!gameover)
	{
		Game_Run(window);
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

	}

	//�ͷ�Direct3D���
	Game_End();

	return message.wParam;
}
