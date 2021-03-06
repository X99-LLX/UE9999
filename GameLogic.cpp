#include "stdafx.h"
#include "GameLogic.h"

void GameLogic::Init()
{
	mEngine = Engine::GetEngine();
	mAssetMgr = mEngine->GetAssetMgr();
	LoadMap("mapactor");
}

void GameLogic::LoadMap(std::string MapName)
{
	mAssetMgr->LoadMap(MapName, &mEngine->GetScene()->mPrimitives);
}

bool GameLogic::Run()
{
	Win32Window::GetInput()->Update();
	return true;
}

void GameLogic::Destery()
{

}
