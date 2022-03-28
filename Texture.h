#pragma once
class Texture
{
public:
	Texture() {};
	Texture(std::string TN, std::wstring FN) { TextureName = TN; FileName = FN; }
	virtual ~Texture() {}

	std::string TextureName;
	std::wstring FileName;
};

