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
	~SceneRender() {}
	void AddTexture(std::string TexName, Texture* t);
	void AddMaterial(std::string MatName, Material* m);  
	void AddMesh(std::string MeshName, Mesh* m);
	void AddPipeline(std::string PipelineName, Pipeline* p);
	void AddShader(std::string ShaderName, Shader* s);
	void AddPrimitive(Primitive* p);

	std::vector<Primitive*> GetPrimitive() { return mPrimitivePool; }
	Material* GetMaterial(std::string MaterialName);
	Mesh* GetMesh(std::string MeshName);
	Shader* GetShader(std::string ShaderName);
	Pipeline* GetPipeline(std::string Pipelinename);
	Texture* GetTexture(std::string TextureName);
protected:
	std::unordered_map<std::string, Texture*> mTexturePool;
	std::unordered_map<std::string, Material*> mMaterialPool;
	std::unordered_map<std::string, Mesh*> mMeshPool;
	std::unordered_map<std::string, Pipeline*> mPipelinePool;
	std::unordered_map<std::string, Shader*> mShaderPool;
	std::vector<Primitive*> mPrimitivePool;
};

