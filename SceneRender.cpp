#include "stdafx.h"
#include "SceneRender.h"

SceneRender::~SceneRender()
{

}

void SceneRender::AddTexture(std::string TexName, std::shared_ptr<Texture> t)
{
	mTexturePool.insert({ TexName,t });
}

void SceneRender::AddMaterial(std::string MatName, std::shared_ptr<Material> m)
{
	mMaterialPool.insert({ MatName,m });
}

void SceneRender::AddMesh(std::string MeshName, std::shared_ptr<Mesh> m)
{
	mMeshPool.insert({ MeshName,m });
	/*OutputDebugStringA("----------------------------");
	OutputDebugStringA(MeshName.c_str());
	OutputDebugStringA("\n");*/
}

void SceneRender::AddPipeline(std::string PipelineName, std::shared_ptr<Pipeline> p)
{
	mPipelinePool.insert({ PipelineName,p });
}

void SceneRender::AddShader(std::string ShaderName, std::shared_ptr<Shader> s)
{
	mShaderPool.insert({ ShaderName,s });
}

void SceneRender::AddPrimitive(std::shared_ptr<Primitive> p)
{
	mPrimitivePool.push_back(p);
}

std::shared_ptr<Material> SceneRender::GetMaterial(std::string MaterialName)
{
	if (mMaterialPool.find(MaterialName) != mMaterialPool.end())
	{
		return mMaterialPool.at(MaterialName);
	}
	return nullptr;
}

std::shared_ptr<Mesh> SceneRender::GetMesh(std::string MeshName)
{
	if (mMeshPool.find(MeshName) != mMeshPool.end())
	{
		return mMeshPool.at(MeshName);
	}
	return nullptr;
}

std::shared_ptr<Shader> SceneRender::GetShader(std::string ShaderName)
{
	if (mShaderPool.find(ShaderName) != mShaderPool.end())
	{
		return mShaderPool.at(ShaderName);
	}
	return nullptr;
}

std::shared_ptr<Pipeline> SceneRender::GetPipeline(std::string Pipelinename)
{
	if (mPipelinePool.find(Pipelinename) != mPipelinePool.end())
	{
		return mPipelinePool.at(Pipelinename);
	}
	return nullptr;
}

std::shared_ptr<Texture> SceneRender::GetTexture(std::string TextureName)
{
	if (mTexturePool.find(TextureName) != mTexturePool.end())
	{
		return mTexturePool.at(TextureName);
	}
	return nullptr;
}
