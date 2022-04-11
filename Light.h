#pragma once
#ifdef _RHI_DX12
#include "UpLoadBuffer.h"
#endif

class Light
{

public:
	Light();
	~Light() {}
	void Init();
	void UpdatePos();
	void UpdateView();
	void UpdateProj();
	glm::mat4x4 GetView() { return mView; }
	glm::mat4x4 GetProj() { return mProj; }

	LightData mLightData;

	//test must fix
#ifdef _RHI_DX12
	std::shared_ptr<UploadBuffer<LightData>> CB = nullptr;
#endif

private:
	glm::vec3 mPosition = { 1000.0f,0.0f,1000.0f };
	glm::vec3 mTarget = { 1.0f,0.0f,-0.7f };
	glm::vec3 mUp = glm::vec3(0.0f, 0.0f, 1.0f);
	float Radius = 2500;

	glm::mat4x4 mView = glm::mat4(1.0f);
	glm::mat4x4 mProj = glm::mat4(1.0f);

};

