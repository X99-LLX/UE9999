#pragma once
#include "MathHelper.h"

using namespace DirectX;


 
class Camera
{
public:
	Camera();
	~Camera();
	XMVECTOR GetCameraPos()const;
	XMFLOAT3 GetCameraPos3f()const;
	void SetCameraPos(float x, float y, float z);
	void SetCameraPos(const XMFLOAT3& v);

	XMVECTOR GetRight()const;
	XMFLOAT3 GetRight3f()const;
	XMVECTOR GetUp()const;
	XMFLOAT3 GetUp3f()const;
	XMVECTOR GetTarget()const;
	XMFLOAT3 GetTarget3f()const;

	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	
	void SetLens(float fovY, float aspect, float zn, float zf);

	
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const; 
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	
	void Strafe(float d);
	void Walk(float d);

	
	void Pitch(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);
	
	void UpdateViewMatrix();

public:
	XMFLOAT3 mPosition = { 0.0f,0.0f,0.0f };
	XMFLOAT3 mUp = { 0.0f,1.0f,0.0f };
	XMFLOAT3 mTarget = { 0.0f,0.0f,1.0f };
	XMFLOAT3 mRight = { 1.0f,0.0f,0.0f };

	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspectRatio = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowsHeight = 0.0f;
	float mFarWindowsHeight = 0.0f;
	
	bool mInitView = true;

	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
};

