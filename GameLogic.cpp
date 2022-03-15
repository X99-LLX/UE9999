#include "stdafx.h"
#include "GameLogic.h"

void GameLogic::Init()
{
	mEngine = Engine::GetEngine();
	mAssetMgr = mEngine->GetAssetMgr();
	LoadMap("mapactor");
	mEngine->GetRender()->BuildGeometry();
}

void GameLogic::LoadMap(std::string MapName)
{
	mAssetMgr->LoadMap(MapName,mEngine->GetScene()->Actors);
}

bool GameLogic::Run()
{
	Win32Window::GetInput()->Update();
	return true;
}

void GameLogic::Destery()
{

}
