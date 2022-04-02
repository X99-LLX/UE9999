#pragma once
#include "RHI.h"

class Render
{
public:
	Render();
	~Render();
	RHI* GetRHI();
	void BuildResource(std::vector<std::shared_ptr<Primitive>> RI);
	
	void DrawScene(std::vector<std::shared_ptr<Primitive>> primitives);

	
private:
	void BasePass(std::vector<std::shared_ptr<Primitive>> primitives);
	void ShadowPass(std::vector<std::shared_ptr<Primitive>> primitives);

	void UpdateRenderData(std::vector<std::shared_ptr<Primitive>> primitives);
	RHI* CreateRHI();

private:
	RHI* mRHI;
};

