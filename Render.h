#pragma once
#include "RHI.h"

class Render
{
public:
	Render();
	~Render();
	RHI* GetRHI();
	void BuildResource();
	//test
	void TestDraw();

private:

	RHI* CreateRHI();
	//test
	void UpdateRenderData();
	void UpdatePrimitiveMVP(Primitive* p);
	void ShadowPass();
	void BasePass();
private:
	RHI* mRHI;
	SceneRender mSceneRender;

	glm::mat4 lightT = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
};

