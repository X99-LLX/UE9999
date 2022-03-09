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

bool Engine::Init()
{
	mIns = GetModuleHandle(0);
	if (!mWindow.InitWnd(mRender.GetDevice()))
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
	

	return true;
}

bool Engine::Run()
{
	mRender.Update(mTimer);
	mRender.Draw(mTimer);
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
