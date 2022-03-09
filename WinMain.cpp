#include "stdafx.h"
#include <windows.h>
#include "Engine.h"
#include "GameInstance.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{

	/*Engine mEngine;
	mEngine.Init(hInstance);
	mEngine.Run();*/

	GameInstance Game(hInstance);
	Game.Init();
	Game.Run();
	return 0;
}