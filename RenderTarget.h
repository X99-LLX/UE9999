#pragma once

enum class RtType
{
	BaseRt,
	ShadowRt,
};

class RenderTarget
{
public:
	RenderTarget() {}
	RenderTarget(std::string name, UINT32 width, UINT32 height, int num, RtType type) {
		RendertargetName = name;
		Width = width;
		Height = height;
		RTNum = num;
		mType = type;
	}
	virtual ~RenderTarget() {}
	RtType GetRTType() { return mType; }
	UINT32 GetWidth() { return Width; }
	UINT32 GetHeight() { return Height; }
	std::string GetRenderTargetName() { return RendertargetName; }

	glm::vec2 mSize;
protected:
	std::string RendertargetName;
	UINT32 Width;
	UINT32 Height;
	int RTNum;
	RtType mType;
};

