#pragma once
#include "Win32Window.h"
#include "Scene.h"
#include "GameTimer.h"
#include "Camera.h"
#include "DX12Render.h"
#include "ResourceManage.h"

class Engine
{
public:
	Engine();
	~Engine();

	bool Init(HINSTANCE ins);
	void Run();
	void Destery();

	static Engine* GetEngine();
	ResourceManage* GetAssetMgr();
	Scene* GetScene();
public:
	HINSTANCE mIns;
	static Engine* mEngine;
private:
	GameTimer mTimer;
	Scene mScene;
	ResourceManage mAssetMgr;
	DX12Render mRender;
	Win32Window mWindow;
};

