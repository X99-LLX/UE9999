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
	auto offset = glm::sin(gt->TotalTime()) * 1000;
	mPosition.x += offset;
}

void Light::UpdateView()
{
	mView = glm::lookAtLH(mPosition, mPosition + mTarget, glm::vec3(0.0f, 0.0f, 1.0f));
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
