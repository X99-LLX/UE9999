#include "stdafx.h"
#include <windows.h>
#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{

	Engine mEngine;
	mEngine.Init(hInstance);
	mEngine.Run();
	return 0;
}