#include "stdafx.h"
#include "Light.h"

//test must fix
#include "Engine.h"

Light::Light()
{
	UpdateView();
	UpdateProj();
	mLightData.LightDirection = mPosition;
}

void Light::Init()
{
	
}

void Light::UpdatePos()
{
	auto gt = Engine::GetEngine()->GetTimer();

	mPosition = glm::rotate(mPosition, gt->DeltaTime() * 0.2f, glm::vec3{ 0.0f,0.0f,1.0f });

	mLightData.LightDirection = mPosition;
	CB->CopyData(0, mLightData);
	UpdateView();
	UpdateProj();
}

void Light::UpdateView()
{
	mView = glm::lookAtLH(mPosition, mTarget, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Light::UpdateProj()
{
	glm::vec3 LS = d3dUtil::Vector3TransformCoord(mTarget, mView);

	float Radius = 2500;

	float l = LS.x - Radius;
	float b = LS.y - Radius;
	float n = LS.z - Radius;
	float r = LS.x + Radius;
	float t = LS.y + Radius;
	float f = LS.z + Radius;
	mProj = glm::orthoLH_ZO(l, r, b, t, n, f);
}
