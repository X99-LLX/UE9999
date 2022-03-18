#pragma once
class GPUTexture
{
public:
	GPUTexture() {}
	GPUTexture(std::string n, std::wstring fn) {
		Name = n;
		Filename = fn;
	}
public:
	std::string Name;
	std::wstring Filename;
};

