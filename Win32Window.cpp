#include "stdafx.h"
#include "Win32Window.h"

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:                        //窗口将被销毁时
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		Win32Window::GetInput()->MouseDown(wParam, lParam, lParam, hwnd);
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		Win32Window::GetInput()->MouseUp(wParam, lParam, lParam);
		return 0;

	case WM_MOUSEMOVE:
		Win32Window::GetInput()->OnMouseMove(wParam, lParam, lParam);
		return 0;

	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		else
		{
			Win32Window::GetInput()->KeyUp(static_cast<UINT8>(wParam));
		}
		return 0;
	case WM_KEYDOWN:
		Win32Window::GetInput()->KeyDown(static_cast<UINT8>(wParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

InputSystem Win32Window::mInput;


Win32Window::Win32Window()
{
	
}

Win32Window::~Win32Window()
{

}

bool Win32Window::InitWnd(ID3D12Device* D)
{
	Device = D;
	return CreateMainWindow();
}

void Win32Window::SetWidgthAndHeigh(int w, int h)
{
	Width = w;
	Height = h;
}

bool Win32Window::CreateMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"DefaultWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return false;
	}

	RECT R = { 0, 0, Width, Height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mMainWnd = CreateWindow(wc.lpszClassName, Caption,WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
		CW_USEDEFAULT, width, height, 0, 0, GetModuleHandle(0), 0);
	
	ShowWindow(mMainWnd, SW_SHOW);

	UpdateWindow(mMainWnd);
	return true;
}

float Win32Window::GetAspectRatio()
{
	return (float)Width / Height;
}

InputSystem* Win32Window::GetInput()
{
	return &mInput;
}

