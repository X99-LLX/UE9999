#pragma once
#include "Shader.h"

enum class PsoType
{
	BasePSO,
	ShadowPSO,
	HDRPso,
};

class Pipeline
{
public:
	Pipeline() {}
	Pipeline(Pipeline* p)
	{
		ShaderName = p->GetShaderName();
		PsoName = p->GetPsoName();
		mType = p->mType;
		mFormat = p->mFormat;
	}
	virtual ~Pipeline();
	void SetPsoName(std::string n) { PsoName = n; }
	void SetShaderName(std::string n) { ShaderName = n; }

	std::string GetPsoName() { return PsoName; }
	std::string GetShaderName() { return ShaderName; }

	PsoType mType;
	ColorFormat mFormat = ColorFormat::DXGI_FORMAT_R8G8B8A8_UNORM;
protected:
	std::string ShaderName;
	std::string PsoName;
};

