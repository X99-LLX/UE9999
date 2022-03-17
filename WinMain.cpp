#include "stdafx.h"
#include <windows.h>
#include "GameInstance.h"

#ifdef PlatFormUndef
int main()
{
	return 0;
}


#elif _PLATFORM_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	GameInstance Game;
	Game.Init();
	Game.Run();
	return 0;
}


#endif