#pragma once
#include "Texture.h"
#include "Pipeline.h"

class Material
{
public:
	Material();
	~Material() { delete mPipeline; }
	std::string GetName() { return MaterialName; }


protected:
	std::string MaterialName;
	

	glm::vec4 BaseColor;
	glm::vec3 FresnelR0;
	float Metallic;
	float Specular;
	float Roughness;

	Pipeline* mPipeline;
	std::unordered_map<std::string, Texture*> MatTextures;
};

