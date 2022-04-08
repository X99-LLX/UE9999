#pragma once
class Texture
{
public:
	Texture() {};
	Texture(Texture* t)
	{
		TextureName = t->GetName();
		FileName = t->GetPath();
	}
	virtual ~Texture() {}
	void SetName(std::string n) { TextureName = n; }
	void SetPath(std::wstring n) { FileName = n; }

	std::string GetName() { return TextureName; }
	std::wstring& GetPath() { return FileName; }

protected:
	std::string TextureName;
	std::wstring FileName;
};

