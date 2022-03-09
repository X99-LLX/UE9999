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
	mAssetMgr->LoadMap(MapName,mEngine->GetScene()->Actors);
}

void GameLogic::Run()
{
	mEngine->GetRender()->BuildGeometry(mEngine->GetScene());
}

void GameLogic::Destery()
{

}
