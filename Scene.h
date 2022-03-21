#pragma once


#include "Camera.h"

#include "Primitive.h"

class Scene
{
public:

	~Scene();
	bool Init();

	std::vector<std::shared_ptr<Primitive>> Actors1;
	Camera mCamera;

};

