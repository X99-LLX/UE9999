#pragma once

#include "Actor.h"
#include "StaticMesh.h"
#include "Camera.h"

class Scene
{
public:

	bool Init();

	std::vector<std::shared_ptr<Actor>> Actors;
	Camera mCamera;

};

