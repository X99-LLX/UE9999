#include "stdafx.h"
#include "InputSystem.h"
#include "Engine.h"

void InputSystem::KeyDown(UINT8 KeyCode)
{
	Keys[KeyCode] = true;
}

void InputSystem::KeyUp(UINT8 KeyCode)
{
	Keys[KeyCode] = false;
}

bool InputSystem::IsKeyDown(UINT8 KeyCode)
{
	return Keys[KeyCode];
}

void InputSystem::MouseDown(WPARAM btnState, int x, int y, HWND hwnd)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(hwnd);
}

void InputSystem::MouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void InputSystem::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{

		float dx = glm::radians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = glm::radians(0.25f * static_cast<float>(y - mLastMousePos.y));

		Engine::GetEngine()->GetScene()->mCamera.Pitch(dy);
		Engine::GetEngine()->GetScene()->mCamera.Yaw(dx);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{

	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
