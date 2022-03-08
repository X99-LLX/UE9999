#pragma once
#include "Engine.h"
#include "ResourceManage.h"

class GameLogic
{
public:
	void Init();
	void LoadMap(std::string MapName);


private:
	Engine* mEngine;
	ResourceManage* mAssetMgr;
};

