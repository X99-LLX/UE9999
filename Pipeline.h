#pragma once
#include "Shader.h"

class Pipeline
{
public:
	Pipeline() {}
	Pipeline(Shader* Shader);
	Pipeline(Pipeline* p)
	{
		PipelineShader = p->GetShader();
	}
	virtual ~Pipeline();

	virtual Shader* GetShader() { return PipelineShader; }
	virtual std::string GetShaderName() { return PipelineShader->GetShaderName(); }
protected:
	Shader* PipelineShader;
};

