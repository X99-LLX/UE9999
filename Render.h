#pragma once
#include "RHI.h"

class Render
{
public:
	Render();
	~Render();
	RHI* GetRHI();
	void BuildResource(std::vector<std::shared_ptr<Primitive>> RI);
	void Init();
	void Draw(std::vector<std::shared_ptr<Primitive>> primitives);
	void DrawShadow(std::vector<std::shared_ptr<Primitive>> primitives);
private:
	RHI* CreateRHI();

private:
	RHI* mRHI;
};

