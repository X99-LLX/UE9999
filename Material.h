#pragma once
#include "Texture.h"
#include "Pipeline.h"

class Material
{
public:
	Material() {}
	Material(Material* m) 
	{
		MaterialName = m->GetName();
		PipelineName = m->GetPipelineName();
		Textures = m->GetTextures();
		mMatData = m->GetMatData();
	}
	virtual ~Material() {}
	void SetMatData(MatData m) { mMatData = m; }
	void SetName(std::string n) { MaterialName = n; }
	void SetPipeline(std::string p) { PipelineName = p; }
	void AddTexture(std::string t) { Textures.push_back(t); }

	MatData& GetMatData() { return mMatData; }
	std::string GetName() { return MaterialName; }
	std::string GetPipelineName() { return PipelineName; }
	std::vector<std::string> GetTextures() { return Textures; }

protected:
	std::string MaterialName;
	
	MatData mMatData;

	std::string PipelineName;
	std::vector<std::string> Textures;
};

