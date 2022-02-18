#include "MyApp.h"
#include <DirectXColors.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		MyApp theApp(hInstance);

		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	

}