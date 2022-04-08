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
	}
	virtual ~Material() {}
	void SetName(std::string n) { MaterialName = n; }
	void SetPipeline(std::string p) { PipelineName = p; }
	void AddTexture(std::string t) { Textures.push_back(t); }

	std::string GetName() { return MaterialName; }
	std::string GetPipelineName() { return PipelineName; }
	std::vector<std::string> GetTextures() { return Textures; }

protected:
	std::string MaterialName;
	
	glm::vec4 BaseColor = glm::vec4(1.0f);
	glm::vec3 FresnelR0 = glm::vec3(1.0f);
	float Metallic = 0;
	float Specular = 0;
	float Roughness = 0;

	std::string PipelineName;
	std::vector<std::string> Textures;
};

