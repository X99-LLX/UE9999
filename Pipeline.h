#pragma once
#include "Shader.h"

enum class PsoType
{
	BasePSO,
	ShadowPSO,
};

class Pipeline
{
public:
	Pipeline() {}
	Pipeline(Pipeline* p)
	{
		ShaderName = p->GetShaderName();
		PsoName = p->GetPsoName();
	}
	virtual ~Pipeline();
	void SetPsoName(std::string n) { PsoName = n; }
	void SetShaderName(std::string n) { ShaderName = n; }

	std::string GetPsoName() { return PsoName; }
	std::string GetShaderName() { return ShaderName; }

	PsoType mType;

protected:
	std::string ShaderName;
	std::string PsoName;
};

