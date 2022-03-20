#pragma once
class Texture
{
public:
	Texture() {};
	Texture(std::string TN, std::wstring FN) { TextureName = TN; FileName = FN; }
	virtual ~Texture() {}
	bool operator==(const Texture* t) { return this->GetTextureName() == t->TextureName; }

	virtual std::string GetTextureName() { return TextureName; }
	virtual std::wstring GetFileName() { return FileName; }
private:
	std::string TextureName;
	std::wstring FileName;
};

