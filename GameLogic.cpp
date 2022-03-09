#include "stdafx.h"
#include "GameLogic.h"

void GameLogic::Init()
{
	mEngine = Engine::GetEngine();
	mAssetMgr = mEngine->GetAssetMgr();
	LoadMap("mapactor");
	mEngine->GetRender()->BuildGeometry(mEngine->GetScene());
}

void GameLogic::LoadMap(std::string MapName)
{
	mAssetMgr->LoadMap(MapName,mEngine->GetScene()->Actors);
}

bool GameLogic::Run()
{
	InputSystem* TempInput = Win32Window::GetInput();

	if (TempInput->IsKeyDown(87) || TempInput->IsKeyDown(65)
		|| TempInput->IsKeyDown(83) || TempInput->IsKeyDown(68)
		|| TempInput->IsKeyDown(81) || TempInput->IsKeyDown(69))
	{
		glm::vec3 Target = glm::vec3(0.0f);
		glm::vec3 Up = glm::vec3(0.0f);
		glm::vec3 Right = glm::vec3(0.0f);

		if (!TempInput->IsKeyDown(87) == TempInput->IsKeyDown(83))
		{
			Target = glm::cross(mEngine->GetScene()->mCamera.GetRight(), mEngine->GetScene()->mCamera.GetUp());
			Target = 100.0f * (float)(TempInput->IsKeyDown(87) - (float)TempInput->IsKeyDown(83)) * Target;
		}
		if (!TempInput->IsKeyDown(65) == TempInput->IsKeyDown(68))
		{
			Right = mEngine->GetScene()->mCamera.GetRight();
			Right = 100.0f * (float)(TempInput->IsKeyDown(68) - (float)TempInput->IsKeyDown(65)) * Right;
		}
		if (!TempInput->IsKeyDown(81) == TempInput->IsKeyDown(69))
		{
			Up = mEngine->GetScene()->mCamera.GetUp();
			Up = 100.0f * (float)(TempInput->IsKeyDown(81) - (float)TempInput->IsKeyDown(69)) * Up;
		}

		glm::vec3 Trans = Right + Up + Target;
		mEngine->GetScene()->mCamera.Walk(Trans);
	}
	return true;
}

void GameLogic::Destery()
{

}
