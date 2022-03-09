#include "stdafx.h"
#include "Scene.h"

bool Scene::Init()
{
	//mCamera.SetCameraPos(glm::vec3(5000.0f, 0.0f, 5000.0f));
	mCamera.Init();
	return true;
}


