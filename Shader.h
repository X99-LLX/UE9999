#pragma once

class Shader
{
public:
	Shader() {}
	Shader(Shader* s)
	{
		ShaderName = s->GetShaderName();
		FilePath = s->GetShaderFilePath();
	}
	virtual ~Shader();

	void SetName(std::string n) { ShaderName = n; }
	void SetPath(std::wstring n) { FilePath = n; }

	virtual std::string GetShaderName() { return ShaderName; }
	virtual std::wstring GetShaderFilePath() { return FilePath; }
protected:
	std::string ShaderName;
	std::wstring FilePath;
};

