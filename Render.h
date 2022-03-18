#pragma once
#include "RHI.h"

class Render
{
public:
	Render();
	~Render();
	RHI* GetRHI();
	void BuildRes(std::vector<std::shared_ptr<RenderItem>> RI);
	void run();
private:
	RHI* CreateRHI();

private:
	RHI* mRHI;
};

