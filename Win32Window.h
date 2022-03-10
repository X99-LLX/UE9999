#pragma once
#include <windows.h>
#include <string>
#include "Window.h"
#include "InputSystem.h"

class Win32Window :
    public Window
{
public:
    Win32Window();
    ~Win32Window();
	bool InitWnd();
	virtual bool CreateMainWindow();
protected:
	LPCWSTR Caption = L"DefaultCaption";
};

