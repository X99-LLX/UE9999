#pragma once

#include "GameTimer.h"
#include "InputSystem.h"

class Window
{
public:
	virtual ~Window();
	virtual void SetWidgthAndHeigh(int w, int h);
	virtual bool CreateMainWindow();
	virtual bool InitWnd();
	virtual float GetAspectRatio();
	int Width = 960;
	int Height = 540;

	HWND mMainWnd = nullptr;


	static InputSystem* GetInput();
	static InputSystem mInput;
};

