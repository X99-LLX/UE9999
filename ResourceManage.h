#pragma once
#include "DXRenderItem.h"
#include "DXMesh.h"

#include "Primitive.h"
#include "Texture.h"

class ResourceManage
{
public:
	~ResourceManage();
	bool Init();
	void LoadMap(std::string MapName, std::vector<std::shared_ptr<Primitive>>& Actors);
	Texture* GetTexture(std::string name);

private:
	Mesh* LoadMeshAsset(std::string filename);
	void ClearAsset();
	void LoadTexture(std::string Name);

public:
	std::unordered_map<std::string, std::shared_ptr<Mesh>> MeshAsset1;
	std::unordered_map<std::string, std::shared_ptr<Texture>> TextureAsset1;
};

