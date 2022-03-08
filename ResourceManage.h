#pragma once
#include "Actor.h"
#include "StaticMesh.h"

class ResourceManage
{
public:

	bool Init();
	void LoadMap(std::string MapName,std::vector<std::shared_ptr<Actor>>& Actors);

private:
	std::shared_ptr<StaticMesh> LoadMeshAsset(std::string filename);
	void ClearAsset();

private:
	std::unordered_map<std::string, std::shared_ptr<StaticMesh>> AssetIndex;
};

