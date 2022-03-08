#pragma once
#include "Win32Window.h"
#include "Scene.h"
#include "GameTimer.h"
#include "Camera.h"
#include "DX12Render.h"


class Engine
{
public:
	Engine();
	~Engine();

	bool Init(HINSTANCE ins);
	void Run();
	void Destery();

public:
	HINSTANCE mIns;

private:
	GameTimer mTimer;
	Scene mScene;
	DX12Render mRender;
	
	Win32Window mWindow;
};

