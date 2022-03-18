#pragma once
#include "DXRenderItem.h"
#include "DXMesh.h"

class ResourceManage
{
public:
	~ResourceManage();

	bool Init();
	void LoadMap(std::string MapName,std::vector<std::shared_ptr<RenderItem>>& Actors);
	GPUTexture* GetTexture(std::string name);

private:
	std::shared_ptr<GPUMesh> LoadMeshAsset(std::string filename);
	void ClearAsset();
	void LoadTexture(std::string Name);

public:

	std::unordered_map<std::string, std::shared_ptr<GPUMesh>> MeshAsset;
	std::unordered_map<std::string, std::shared_ptr<GPUTexture>> TextureAsset;
};

