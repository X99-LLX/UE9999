#pragma once

#include "Actor.h"
#include "StaticMesh.h"
#include "Camera.h"

class Scene
{
public:
	std::vector<std::shared_ptr<Actor>> Actors;
	std::unordered_map<std::string, std::shared_ptr<StaticMesh>> AssetIndex;

	Camera mCamera;

	void ReadScenceDat(const std::string filename);
	bool ReadStaticMeshDat(std::string filename, StaticMesh& mesh);
};

