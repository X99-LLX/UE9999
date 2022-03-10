#include "stdafx.h"
#include "GameInstance.h"

GameInstance::GameInstance()
{

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
	mLogic.Run();
	mEngine.Run();
}

void GameInstance::Destroy()
{
	mLogic.Destery();
	mEngine.Destery();
}
