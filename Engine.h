#pragma once
#include "Win32Window.h"
#include "Scence.h"
#include "GameTimer.h"
#include "Camera.h"
#include "DX12Render.h"


class Engine
{
public:
	Engine(HINSTANCE ins);
	~Engine();

	bool Init();
	void Run();
	void Destery();

public:
	HINSTANCE mIns;

private:
	GameTimer mTimer;
	Scence mScence;
	DX12Render mRender;
	Camera mCamera;
	Win32Window mWindow;
};

