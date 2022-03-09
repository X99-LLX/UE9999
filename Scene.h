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

	/*std::unordered_map<std::string, std::shared_ptr<StaticMesh>> AssetIndex;
	void ReadScenceDat(const std::string filename);
	bool ReadStaticMeshDat(std::string filename, StaticMesh& mesh);*/
};

