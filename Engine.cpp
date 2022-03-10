#include "stdafx.h"
#include "Engine.h"

Engine* Engine::mEngine = nullptr;

Engine::Engine()
{
	mEngine = this;
}

Engine::~Engine()
{
	testwin = nullptr;
}

bool Engine::Init()
{
	testwin = CreateMainWnd();
	if (!testwin->InitWnd())
	{
		return false;
	}
	if (!mRender.InitRender())
	{
		return false;
	}
	if (!mScene.Init()) 
	{
		return false;
	}
	if (!mAssetMgr.Init()) 
	{
		return false;
	}
	return true;
}

bool Engine::Run()
{
	MSG msg = { 0 };
	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();
			testwin->GetInput()->Update();
			mRender.Update();
			mRender.Draw();
		}
	}
	
	return true;
}

void Engine::Destery()
{
	
}

Engine* Engine::GetEngine()
{
	return mEngine;
}

ResourceManage* Engine::GetAssetMgr()
{
	return &mAssetMgr;
}

Scene* Engine::GetScene()
{
	return &mScene;
}

DX12Render* Engine::GetRender()
{
	return &mRender;
}

GameTimer* Engine::GetTimer()
{
	return &mTimer;
}

Window* Engine::GetWindow()
{
	return testwin;
}

Window* Engine::CreateMainWnd()
{
#ifdef _PLATFORM_WIN32
	return new Win32Window();
#endif

#ifdef _PLATFORM_IOS
	return new IOSWindow();
#endif
}
