#include "stdafx.h"
#include "ResourceManage.h"
#include "Engine.h"
#include "StoWS.h"


ResourceManage::~ResourceManage()
{
	
}

bool ResourceManage::Init()
{
	ClearAsset();
	return true;
}


void ResourceManage::LoadMap(std::string MapName, std::vector<std::shared_ptr<Primitive>>* Actors)
{
	LoadTexture("tile");
	LoadTexture("tile_nmap");
	std::string FilePath = "Data/" + MapName + ".dat";
	std::ifstream fin(FilePath, std::ios::binary);

	if (!fin.is_open())
	{
		return;
	}

	INT32 Num;
	fin.read((char*)&Num, sizeof(int));

	for (int i = 0; i < Num; i++)
	{

		auto actorinfo = std::make_shared<Primitive>();
		INT32 TempNum;
		fin.read((char*)&actorinfo->GetTransform(), sizeof(int) * 10);
		fin.read((char*)&TempNum, sizeof(int));
		std::string name;
		name.resize(TempNum);
		fin.read((char*)name.data(), TempNum);
		actorinfo->SetMeshName(name);
		Actors->push_back(actorinfo);
		LoadMeshAsset(actorinfo->GetMeshName());

	}
	fin.close();

	

	CreateShader("BaseShader", L"Shaders\\color.hlsl");
	CreateShader("ShadowShader", L"Shaders\\Shadows.hlsl");
	CreateShader("BloomShader", L"Shaders\\color.hlsl");

	CreateShader("BloomSetUpShader", L"Shaders\\BloomSetUp.hlsl");
	CreateShader("BloomDown", L"Shaders\\BloomDown.hlsl");
	CreateShader("BloomUp", L"Shaders\\BloomUp.hlsl");
	CreateShader("BloomMergeps", L"Shaders\\BloomMergeps.hlsl");
	CreateShader("BloomToneMap", L"Shaders\\BloomToneMap.hlsl");
	CreateShader("BloomMix", L"Shaders\\BloomMix.hlsl");

	//cyberpunk
	CreateShader("CyberpunkShader", L"Shaders\\Cyberpunk.hlsl");
	CreatePipeline("CyberpunkShader", "CyberpunkPSO", PsoType::HDRPso);




	CreatePipeline("BaseShader", "BasePSO", PsoType::BasePSO);

	CreatePipeline("ShadowShader", "ShadowPSO", PsoType::ShadowPSO);
	CreatePipeline("BloomShader", "BloomPSO", PsoType::HDRPso);
	CreatePipeline("BloomSetUpShader", "SetUpPSO", PsoType::HDRPso);
	CreatePipeline("BloomDown", "DownPSO", PsoType::HDRPso);
	CreatePipeline("BloomUp", "UpPSO", PsoType::HDRPso);
	CreatePipeline("BloomMergeps", "MergepsPSO", PsoType::HDRPso);
	CreatePipeline("BloomMix", "MixPSO", PsoType::HDRPso);
	CreatePipeline("BloomToneMap", "ToneMapPSO", PsoType::BasePSO);

	//CreateMaterial("BaseMaterial", "BasePSO");
	CreateMaterial("BaseMaterial", "BloomPSO");
}

Texture* ResourceManage::GetTexture(std::string name)
{
	return TextureAsset.find(name)->second.get();
}

void ResourceManage::LoadMeshAsset(std::string filename)
{
	filename = filename.erase(filename.length() - 1, 1);
	std::string FilePath = "Data/" + filename + ".dat";
	std::ifstream fin(FilePath, std::ios::binary);

	if (MeshAsset.find(filename) == MeshAsset.end())
	{
		int test;
		auto meshinfo = std::make_shared<Mesh>();

		INT32 Num;
		fin.read((char*)&Num, sizeof(int));
		std::string name;
		name.resize(Num);
		fin.read((char*)name.data(), Num);
		meshinfo->SetName(name);
		fin.read((char*)&test, sizeof(int));
		fin.read((char*)&test, sizeof(int));
		fin.read((char*)&test, sizeof(int));
		fin.read((char*)&test, sizeof(int));
		fin.read((char*)&Num, sizeof(int));
		MeshVertexInfo temp = meshinfo->GetVertexinfo();
		temp.mIndex.resize(Num);
		fin.read((char*)temp.mIndex.data(), sizeof(int) * Num);
		fin.read((char*)&Num, sizeof(int));
		temp.mVertex.resize(Num);
		fin.read((char*)temp.mVertex.data(), sizeof(glm::vec3) * Num);
		fin.read((char*)&Num, sizeof(int));
		temp.mNormal.resize(Num);
		fin.read((char*)temp.mNormal.data(), sizeof(glm::vec4) * Num);
		fin.read((char*)&Num, sizeof(int));
		temp.mTangent.resize(Num);
		fin.read((char*)temp.mTangent.data(), sizeof(glm::vec4) * Num);
		fin.read((char*)&Num, sizeof(int));
		temp.mTexCoord.resize(Num);
		fin.read((char*)temp.mTexCoord.data(), sizeof(glm::vec2) * Num);
		fin.close();
		meshinfo->SetMeshInfo(temp);
		meshinfo->SetMaterialName("BaseMaterial");
		MeshAsset.insert({ filename ,meshinfo });
	}

}

void ResourceManage::ClearAsset()
{
	MeshAsset.clear();
	TextureAsset.clear();
	MaterialAsset.clear();
}

void ResourceManage::LoadTexture(std::string Name)
{
	auto Tex = std::make_shared<Texture>();
	Tex->SetName(Name);
	Tex->SetPath(L"Textures/" + SToWS(Name) + L".dds");
	TextureAsset.insert({ Name, Tex });
}

void ResourceManage::CreateMaterial(std::string materialname, std::string pipelinename)
{
	auto material = std::make_shared<Material>();
	material->SetName(materialname);
	material->SetPipeline(pipelinename);
	material->AddTexture("tile");
	material->AddTexture("tile_nmap");
	MaterialAsset.insert({ materialname,material });
}

void ResourceManage::CreatePipeline(std::string shadername, std::string PsoName, PsoType pt)
{
	auto pipeline = std::make_shared<Pipeline>();
	pipeline->SetShaderName(shadername);
	pipeline->SetPsoName(PsoName);
	pipeline->mType = pt;
	if (pt == PsoType::HDRPso) 
	{
		pipeline->mFormat = ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	PipelineAsset.insert({ PsoName,pipeline });
}

void ResourceManage::CreateShader(std::string shadername, std::wstring path)
{
	auto shader = std::make_shared<Shader>();
	shader->SetName(shadername);
	shader->SetPath(path);
	ShaderAsset.insert({ shadername,shader });
}
