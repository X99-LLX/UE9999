#include "stdafx.h"
#include "Engine.h"

Engine::Engine(HINSTANCE ins)
{
	mIns = ins;
}

bool Engine::Init()
{
	if (!mWindow.InitWnd(mRender.GetDevice(), mIns))
	{
		return false;
	}
	if (!mRender.InitRender(&mWindow))
	{
		return false;
	}
	
	mCamera.SetCameraPos(glm::vec3(0.0f, 0.0f, 1000.0f));

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
