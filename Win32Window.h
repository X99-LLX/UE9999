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
	bool InitWnd(ID3D12Device* D);
    virtual void SetWidgthAndHeigh(int w, int h) override;
	virtual bool CreateMainWindow();

	float GetAspectRatio();
	HWND mMainWnd = nullptr;

	static InputSystem* GetInput();


protected:

	ID3D12Device* Device = nullptr;
	LPCWSTR Caption = L"DefaultCaption";
	bool mAppPaused = false;       
	bool mMinimized = false;       
	bool mMaximized = false;       
	bool mResizing = false;        
	bool mFullscreenState = false; 

	static InputSystem mInput;
};

