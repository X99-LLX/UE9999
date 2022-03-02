#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	
}

Camera::~Camera()
{

}

DirectX::XMVECTOR Camera::GetCameraPos() const
{
	return XMLoadFloat3(&mPosition);
}

DirectX::XMFLOAT3 Camera::GetCameraPos3f() const
{
	return mPosition;
}

void Camera::SetCameraPos(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
	mInitView = true;
}

void Camera::SetCameraPos(const XMFLOAT3& v)
{
	mPosition = v;
	mInitView = true;
}

DirectX::XMVECTOR Camera::GetRight() const
{
	return XMLoadFloat3(&mRight);
}

DirectX::XMFLOAT3 Camera::GetRight3f() const
{
	return mRight;
}

DirectX::XMVECTOR Camera::GetUp() const
{
	return XMLoadFloat3(&mUp);
}

DirectX::XMFLOAT3 Camera::GetUp3f() const
{
	return mUp;
}

DirectX::XMVECTOR Camera::GetTarget() const
{
	return XMLoadFloat3(&mTarget);
}

DirectX::XMFLOAT3 Camera::GetTarget3f() const
{
	return mTarget;
}

float Camera::GetNearZ() const
{
	return mNearZ;
}

float Camera::GetFarZ() const
{
	return mFarZ;
}

float Camera::GetAspect() const
{
	return mAspectRatio;
}

float Camera::GetFovY() const
{
	return mFovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth() const
{
	return mAspectRatio * mNearWindowsHeight;
}

float Camera::GetNearWindowHeight() const
{
	return mNearWindowsHeight;
}

float Camera::GetFarWindowWidth() const
{
	return mAspectRatio * mFarWindowsHeight;
}

float Camera::GetFarWindowHeight() const
{
	return mFarWindowsHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	mFovY = fovY;
	mAspectRatio = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowsHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowsHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	//构建左手透视投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspectRatio, mNearZ, mFarZ);
	
	XMStoreFloat4x4(&mProj, P);
}

void Camera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{

	//定义相机空间

	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mTarget, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);

	mInitView = true;
}

void Camera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	mInitView = true;
}

DirectX::XMMATRIX Camera::GetView() const
{
	assert(!mInitView);
	return XMLoadFloat4x4(&mView);
}

DirectX::XMMATRIX Camera::GetProj() const
{
	return XMLoadFloat4x4(&mProj);
}

DirectX::XMFLOAT4X4 Camera::GetView4x4f() const
{
	assert(!mInitView);
	return mView;
}

DirectX::XMFLOAT4X4 Camera::GetProj4x4f() const
{
	return mProj;
}

void Camera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(100 * d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));

	mInitView = true;
}

void Camera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(100 * d);
	XMVECTOR l = XMLoadFloat3(&mTarget);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	mInitView = true;
}

void Camera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mTarget, XMVector3TransformNormal(XMLoadFloat3(&mTarget), R));

	mInitView = true;
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	//XMMATRIX R = XMMatrixRotationY(angle);
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mUp), angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	//XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mTarget, XMVector3TransformNormal(XMLoadFloat3(&mTarget), R));

	mInitView = true;
}

void Camera::RotateZ(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	//XMMATRIX R = XMMatrixRotationY(angle);
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mTarget), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	/*XMStoreFloat3(&mTarget, XMVector3TransformNormal(XMLoadFloat3(&mTarget), R));*/

	mInitView = true;
}



void Camera::UpdateViewMatrix()
{
	if (mInitView)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mTarget);
		XMVECTOR P = XMLoadFloat3(&mPosition);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mTarget, L);

		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mTarget.x;
		mView(1, 2) = mTarget.y;
		mView(2, 2) = mTarget.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		mInitView = false;
	}
}

