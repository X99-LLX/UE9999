#include "stdafx.h"
#include "GameInstance.h"

GameInstance::GameInstance()
{
	mIns = GetModuleHandle(0);
}

GameInstance::~GameInstance()
{

}

void GameInstance::Init()
{
	mEngine.Init();
	mLogic.Init();
}

void GameInstance::Run()
{
	MSG msg = { 0 };

	mEngine.GetTimer()->Reset();

	while (msg.message != WM_QUIT)
	{
		
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mEngine.GetTimer()->Tick();

			mLogic.Run();
			mEngine.Run();
		}
	}
}

void GameInstance::Destroy()
{
	mLogic.Destery();
	mEngine.Destery();
}
