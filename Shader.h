#pragma once

class Shader
{
public:
	Shader() {}
	Shader(const std::string ShaderName, const std::wstring FilePath, std::string VS = "VS", std::string PS = "PS");
	virtual ~Shader();

	virtual std::string GetShaderName() { return ShaderName; }
	virtual std::wstring GetShaderFilePath() { return FilePath; }
	
protected:
	std::string ShaderName;
	std::wstring FilePath;
	std::string VSEntry;
	std::string PSEntry;
};

