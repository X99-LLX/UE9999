#pragma once
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "Pipeline.h"
#include "Shader.h"
#include "Primitive.h"
#include "Singleton.h"

class SceneRender : public Singleton<SceneRender>
{
public:
	SceneRender() {}
	~SceneRender();
	void AddTexture(std::string TexName, std::shared_ptr<Texture> t);
	void AddMaterial(std::string MatName, std::shared_ptr<Material> m);
	void AddMesh(std::string MeshName, std::shared_ptr<Mesh> m);
	void AddPipeline(std::string PipelineName, std::shared_ptr<Pipeline> p);
	void AddShader(std::string ShaderName, std::shared_ptr<Shader> s);
	void AddPrimitive(std::shared_ptr<Primitive> p);

	std::vector<std::shared_ptr<Primitive>> GetPrimitive() { return mPrimitivePool; }
	std::shared_ptr<Material> GetMaterial(std::string MaterialName);
	std::shared_ptr<Mesh> GetMesh(std::string MeshName);
	std::shared_ptr<Shader> GetShader(std::string ShaderName);
	std::shared_ptr<Pipeline> GetPipeline(std::string Pipelinename);
	std::shared_ptr<Texture> GetTexture(std::string TextureName);
protected:
	std::unordered_map<std::string, std::shared_ptr<Texture>> mTexturePool;
	std::unordered_map<std::string, std::shared_ptr<Material>> mMaterialPool;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> mMeshPool;
	std::unordered_map<std::string, std::shared_ptr<Pipeline>> mPipelinePool;
	std::unordered_map<std::string, std::shared_ptr<Shader>> mShaderPool;
	std::vector<std::shared_ptr<Primitive>> mPrimitivePool;
};

