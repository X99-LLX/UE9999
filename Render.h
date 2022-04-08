#pragma once
#include "RHI.h"

class Render
{
public:
	Render();
	~Render();
	RHI* GetRHI();
	void BuildResource();
	
	void DrawScene(std::vector<std::shared_ptr<Primitive>> primitives);

	
	//test
	void TestDraw();

private:
	void BasePass(std::vector<std::shared_ptr<Primitive>> primitives);
	void ShadowPass(std::vector<std::shared_ptr<Primitive>> primitives);

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

