#include "stdafx.h"
#include "GameInstance.h"

GameInstance::GameInstance(HINSTANCE ins)
{
	mIns = ins;
}

GameInstance::~GameInstance()
{

}

void GameInstance::Init()
{
	mEngine.Init(mIns);
	mLogic.Init();
}

void GameInstance::Run()
{
	mLogic.Run();
	mEngine.Run();
}

void GameInstance::Destroy()
{
	mLogic.Destery();
	mEngine.Destery();
}
