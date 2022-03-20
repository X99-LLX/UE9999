#include "stdafx.h"
#include "Engine.h"

Engine* Engine::mEngine = nullptr;

Engine::Engine()
{
	mEngine = this;
}

Engine::~Engine()
{
	delete mWindow;
}

bool Engine::Init()
{
	mWindow = CreateMainWnd();
	if (!mWindow->InitWnd())
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
	TestRender.GetRHI()->Init();
	return true;
}

bool Engine::Run()
{
	MSG msg = { 0 };
	mTimer.Reset();

	TestRender.BuildResource(mScene.Actors1);

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
			mWindow->GetInput()->Update();

			TestRender.Draw(mScene.Actors1);
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

Render* Engine::GetRender()
{
	return &TestRender;
}


GameTimer* Engine::GetTimer()
{
	return &mTimer;
}

Window* Engine::GetWindow()
{
	return mWindow;
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
