#pragma once
#include "Actor.h"
#include "StaticMesh.h"

class ResourceManage
{
public:

	bool Init();
	void LoadMap(std::string MapName,std::vector<std::shared_ptr<Actor>>& Actors);
	Texture* GetTexture(std::string name);

private:
	std::shared_ptr<StaticMesh> LoadMeshAsset(std::string filename);
	void ClearAsset();
	void LoadTexture(std::string Name);

private:
	std::unordered_map<std::string, std::shared_ptr<StaticMesh>> MeshAsset;
	std::unordered_map<std::string, std::shared_ptr<Texture>> TextureAsset;
};

