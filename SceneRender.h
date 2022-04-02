#pragma once
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "Pipeline.h"
#include "Shader.h"
#include "Primitive.h"

class SceneRender
{
	SceneRender() {}
	~SceneRender() {}
	void AddTexture(std::string TexName, Texture* t);
	void AddMaterial(std::string MatName, Material* m);  
	void AddMesh(std::string MeshName, Mesh* m);
	void AddPipeline(std::string PipelineName, Pipeline* p);
	void AddShader(std::string ShaderName, Shader* s);
	void AddPrimitive(std::string PrimitiveName, Primitive* p);

	std::unordered_map<std::string, Primitive*> GetPrimitive() { return mPrimitivePool; }
	Mesh* GetMesh(std::string MeshName);
protected:
	std::unordered_map<std::string, Texture*> mTexturePool;
	std::unordered_map<std::string, Material*> mMaterialPool;
	std::unordered_map<std::string, Mesh*> mMeshPool;
	std::unordered_map<std::string, Pipeline*> mPipelinePool;
	std::unordered_map<std::string, Shader*> mShaderPool;
	std::unordered_map<std::string, Primitive*> mPrimitivePool;
};

