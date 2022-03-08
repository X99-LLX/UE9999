#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	UpdateViewMatrix();
	UpdateProjMatrix(0.25f * glm::pi<float>(), 1.0f, 1.0f, 100000.0f);
}

Camera::~Camera()
{

}

glm::vec3 Camera::GetCameraPos() const
{
	return mPosition;
}

void Camera::SetCameraPos(glm::vec3 Pos)
{
	mPosition = Pos;
	mInitView = true;
}

glm::vec3 Camera::GetRight() const
{
	return mRight;
}

glm::vec3 Camera::GetUp() const
{
	return mUp;
}

glm::vec3 Camera::GetTarget() const
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

void Camera::UpdateProjMatrix(float fovY, float aspect, float zn, float zf)
{
	mFovY = fovY;
	mAspectRatio = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowsHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowsHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	//mProj = glm::perspective(glm::radians(fovY), mAspectRatio, mNearZ, mFarZ);
	mProj = glm::perspectiveLH_ZO(fovY, mAspectRatio, mNearZ, mFarZ);
}

void Camera::LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 worldUp)
{
	glm::vec3 T = glm::normalize(target - pos);
	glm::vec3 R = glm::normalize(glm::cross(worldUp, T));
	glm::vec3 U = glm::cross(T, R);

	mPosition = pos;
	mTarget = T;
	mRight = R;
	mUp = U;

	mInitView = true;
}

glm::mat4x4 Camera::GetView() const
{
	return mView;
}

glm::mat4x4 Camera::GetProj() const
{
	return mProj;
}


void Camera::Walk(glm::vec3 Trans)
{
	glm::mat4 Translate = glm::mat4(1.0f);
	Translate = glm::translate(Translate, Trans);
	mPosition = Translate * glm::vec4(mPosition, 1.0f);
	mInitView = true;
}

void Camera::Pitch(float angle)
{
	glm::mat4 Rotate = glm::mat4(1.0f);
	Rotate = glm::rotate(Rotate, glm::radians(angle), mRight);
	mUp = Rotate * glm::vec4(mUp, 1.0f);
	mTarget = glm::cross(mRight, mUp);
	mInitView = true;
}

void Camera::Yaw(float angle)
{
	glm::mat4 Rotate = glm::mat4(1.0f);
	Rotate = glm::rotate(Rotate, glm::radians(angle), mUp);
	mRight = Rotate * glm::vec4(mRight, 1.0f);
	mTarget = glm::cross(mRight, mUp);
	mInitView = true;
}

void Camera::Roll(float angle)
{
	glm::mat4 Rotate = glm::mat4(1.0f);
	Rotate = glm::rotate(Rotate, glm::radians(angle), mTarget);
	mUp = Rotate * glm::vec4(mUp, 1.0f);
	mTarget = glm::cross(mUp, mTarget);
	mInitView = true;
}

void Camera::UpdateViewMatrix()
{
	if (mInitView)
	{
		glm::vec3 R = mRight;
		glm::vec3 U = mUp;
		glm::vec3 L = mTarget;
		glm::vec3 P = mPosition;

		L = glm::normalize(L);
		U = glm::normalize(glm::cross(L, R));
		R = glm::cross(U, L);

		mRight = R;
		mUp = U;
		mTarget = L;

		mView = glm::lookAtLH(mPosition, mTarget, mUp);

		mInitView = false;
	}
}


