#include "stdafx.h"
#include "Scene.h"

bool Scene::Init()
{
	mCamera.SetCameraPos(glm::vec3(0.0f, 0.0f, 5000.0f));
	return true;
}


