#include "stdafx.h"
#include <windows.h>
#include "GameInstance.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	GameInstance Game;
	Game.Init();
	Game.Run();
	return 0;
}