#include "stdafx.h"
#include "Scene.h"

Scene::~Scene()
{

}

bool Scene::Init()
{
	
	mCamera.Init();
	return true;
}


