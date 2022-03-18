#pragma once

#include "DXRenderItem.h"

#include "RenderItem.h"
#include "DXMesh.h"
#include "Camera.h"

class Scene
{
public:

	~Scene();
	bool Init();

	std::vector<std::shared_ptr<RenderItem>> Actors;

	//std::vector<std::shared_ptr<RenderItem>> Actors;

	Camera mCamera;

};

