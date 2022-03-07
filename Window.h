#pragma once

#include "GameTimer.h"

class Window
{
public:
	virtual ~Window();
	virtual void SetWidgthAndHeigh(int w, int h) = 0;
	virtual bool CreateMainWindow();
	int Width = 800;
	int Height = 600;
};

