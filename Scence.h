#pragma once

#include "Actor.h"
#include "StaticMesh.h"

class Scence
{
public:
	std::vector<Actor> Actors;
	std::unordered_map<std::string, StaticMesh> AssetIndex;

	void ReadScenceDat(const std::string filename);
	bool ReadStaticMeshDat(std::string filename, StaticMesh& mesh);
};

