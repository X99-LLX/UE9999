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
	void BloomPass();
	void CyberpunkPass();

	void BasePass();
	void SetUpPass();
	void BloomDown(std::string rtname, std::string srname);
	void BloomUp(std::string minRt, std::string OtherRt ,std::string UpRT);
	void BloomMergeps(std::string OtherRt, std::string UpRT);
	


	void ShowColorBufferPass();
	void TestTriangle();

private:
	RHI* mRHI;
	SceneRender mSceneRender;

	glm::mat4 lightT = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	std::shared_ptr<Mesh> mTriangle;
	std::string mCurrentColorBuffer = "Color";
};

