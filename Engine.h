#pragma once
#include "Win32Window.h"
#include "Scene.h"
#include "GameTimer.h"
#include "Camera.h"
#include "DX12RHI.h"
#include "ResourceManage.h"
#include "Window.h"


class Engine 
{
public:
	Engine();
	~Engine();

	bool Init();
	bool Run();
	void Destery();

	static Engine* GetEngine();
	ResourceManage* GetAssetMgr();
	Scene* GetScene();

	DX12RHI* GetRender();

	GameTimer* GetTimer();
	Window* GetWindow();

protected:
	Window* CreateMainWnd();

public:
	static Engine* mEngine;
private:
	GameTimer mTimer;
	Scene mScene;
	ResourceManage mAssetMgr;
	DX12RHI mRender;
	Window* testwin;
};

