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

void InputSystem::MouseDown()
{
	GetCursorPos(&mLastMousePos);
	SetCapture(GetActiveWindow());
}

void InputSystem::MouseUp()
{
	ReleaseCapture();
}

void InputSystem::OnMouseMove()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		POINT TempPoint = { 0 };
		GetCursorPos(&TempPoint);

		float dx = 0.25f * static_cast<float>(TempPoint.x - mLastMousePos.x);
		float dy = 0.25f * static_cast<float>(TempPoint.y - mLastMousePos.y);

		Engine::GetEngine()->GetScene()->mCamera.Pitch(glm::radians(dy));
		Engine::GetEngine()->GetScene()->mCamera.Yaw(glm::radians(dx));
	}
	GetCursorPos(&mLastMousePos);
}

void InputSystem::Update()
{
	if (GetAsyncKeyState('W') & 0x8000)
		Engine::GetEngine()->GetScene()->mCamera.Walk(glm::vec3(30.0f, 0.0f, 0.0f));

	if (GetAsyncKeyState('S') & 0x8000)
		Engine::GetEngine()->GetScene()->mCamera.Walk(glm::vec3(-30.0f, 0.0f, 0.0f));

	if (GetAsyncKeyState('A') & 0x8000)
		Engine::GetEngine()->GetScene()->mCamera.Walk(glm::vec3(0.0f, -30.0f, 0.0f));

	if (GetAsyncKeyState('D') & 0x8000)
		Engine::GetEngine()->GetScene()->mCamera.Walk(glm::vec3(0.0f, 30.0f, 0.0f));

	if (GetAsyncKeyState('Q') & 0x8000)
		Engine::GetEngine()->GetScene()->mCamera.Walk(glm::vec3(0.0f, 0.0f, 30.0f));

	if (GetAsyncKeyState('E') & 0x8000)
		Engine::GetEngine()->GetScene()->mCamera.Walk(glm::vec3(0.0f, 0.0f, -30.0f));

	
}
