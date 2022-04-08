#pragma once
#include "Camera.h"
#include "Light.h"
#include "Primitive.h"

class Scene
{
public:

	~Scene();
	bool Init();

	std::vector<std::shared_ptr<Primitive>> mPrimitives;
	Camera mCamera;

	std::shared_ptr<Light> mLight = nullptr;
};

