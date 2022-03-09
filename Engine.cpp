#include "stdafx.h"
#include "Engine.h"

Engine* Engine::mEngine = nullptr;

Engine::Engine()
{
	mEngine = this;
}

Engine::~Engine()
{

}

bool Engine::Init(HINSTANCE ins)
{
	mIns = ins;
	if (!mWindow.InitWnd(mRender.GetDevice(), mIns))
	{
		return false;
	}
	if (!mRender.InitRender(&mWindow,&mScene))
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
	
	mScene.mCamera.SetCameraPos(glm::vec3(0.0f, 0.0f, 1000.0f));

	return true;
}

void Engine::Run()
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
			mRender.Update(mTimer);
			mRender.Draw(mTimer);
		}
	}
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
