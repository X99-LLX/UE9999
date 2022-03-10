#include "stdafx.h"
#include <windows.h>
#include "GameInstance.h"
#define PlatFormWin32



#ifdef PlatFormUndef



#elif defined(PlatFormWin32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	GameInstance Game;
	Game.Init();
	Game.Run();
	return 0;
}

#elif defined(PlatformIOS)


#elif defined(PlatformAndroid)

#endif