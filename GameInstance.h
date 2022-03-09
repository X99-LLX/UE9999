#pragma once
#include "Engine.h"
#include "GameLogic.h"

class GameInstance
{
public:
	GameInstance();
	~GameInstance();

	void Init();
	void Run();
	void Destroy();

public:
	HINSTANCE mIns;
	Engine mEngine;
	GameLogic mLogic;
};

