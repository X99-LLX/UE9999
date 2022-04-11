#pragma once
#include "Primitive.h"
#include "Texture.h"
#include "Singleton.h"
#include "Material.h"

class ResourceManage : public Singleton<ResourceManage>
{
public:
	~ResourceManage();
	bool Init();
	void LoadMap(std::string MapName, std::vector<std::shared_ptr<Primitive>>* Actors);
	Texture* GetTexture(std::string name);

private:
	void LoadMeshAsset(std::string filename);
	void ClearAsset();
	void LoadTexture(std::string Name);
	void CreateMaterial(std::string materialname, std::string pipelinename);
	void CreatePipeline(std::string shadername, std::string PsoName, PsoType pt);
	void CreateShader(std::string shadername, std::wstring path);
public:
	std::unordered_map<std::string, std::shared_ptr<Mesh>> MeshAsset;
	std::unordered_map<std::string, std::shared_ptr<Texture>> TextureAsset;
	std::unordered_map<std::string, std::shared_ptr<Material>> MaterialAsset;
	std::unordered_map<std::string, std::shared_ptr<Pipeline>> PipelineAsset;
	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderAsset;
};

