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

void SceneRender::AddPrimitive(Primitive* p)
{
	mPrimitivePool.push_back(p);
}

Material* SceneRender::GetMaterial(std::string MaterialName)
{
	if (mMaterialPool.find(MaterialName) != mMaterialPool.end())
	{
		return mMaterialPool.at(MaterialName);
	}
	return nullptr;
}

Mesh* SceneRender::GetMesh(std::string MeshName)
{
	if (mMeshPool.find(MeshName) != mMeshPool.end())
	{
		return mMeshPool.at(MeshName);
	}
	return nullptr;
}

Shader* SceneRender::GetShader(std::string ShaderName)
{
	if (mShaderPool.find(ShaderName) != mShaderPool.end())
	{
		return mShaderPool.at(ShaderName);
	}
	return nullptr;
}

Pipeline* SceneRender::GetPipeline(std::string Pipelinename)
{
	if (mPipelinePool.find(Pipelinename) != mPipelinePool.end())
	{
		return mPipelinePool.at(Pipelinename);
	}
	return nullptr;
}

Texture* SceneRender::GetTexture(std::string TextureName)
{
	if (mTexturePool.find(TextureName) != mTexturePool.end())
	{
		return mTexturePool.at(TextureName);
	}
	return nullptr;
}
