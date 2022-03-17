#pragma once
#include "RHI.h"

class Render
{
public:
	Render();
	~Render();
	RHI* GetRHI();

	void run();
private:
	RHI* CreateRHI();

private:
	RHI* mRHI;
};

