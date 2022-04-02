#include "stdafx.h"
#include "SceneRender.h"

void SceneRender::AddTexture(std::string TexName, Texture* t)
{
	mTexturePool.insert({TexName,t});
}

void SceneRender::AddMaterial(std::string MatName, Material* m)
{
	mMaterialPool.insert({ MatName,m });
}

void SceneRender::AddMesh(std::string MeshName, Mesh* m)
{
	mMeshPool.insert({ MeshName,m });
}

void SceneRender::AddPipeline(std::string PipelineName, Pipeline* p)
{
	mPipelinePool.insert({ PipelineName,p });
}

void SceneRender::AddShader(std::string ShaderName, Shader* s)
{
	mShaderPool.insert({ ShaderName,s });
}

void SceneRender::AddPrimitive(std::string PrimitiveName, Primitive* p)
{
	mPrimitivePool.insert({ PrimitiveName,p });
}

Mesh* SceneRender::GetMesh(std::string MeshName)
{
	if (mMeshPool.find(MeshName) != mMeshPool.end())
	{
		return mMeshPool.at(MeshName);
	}
	return nullptr;
}
