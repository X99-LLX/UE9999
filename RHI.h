#pragma once
#include "RenderItem.h"
#include "Scene.h"

class RHI 
{
public:
	virtual ~RHI();

	virtual bool Init() = 0;
	virtual void Update(Scene* mScene, Primitive* actor) = 0;
	virtual void DrawCall(Primitive* actor) = 0;
	virtual void BuildGeo(Primitive* actor) = 0;
	virtual void CreateRenderItem(Primitive* actor) = 0;
	
	virtual void ResetViewportsAndScissorRects() = 0;
	virtual void ClearRTVAndDSV() = 0;
	virtual void SetRTVAndDSV() = 0;
	virtual void SetRootSignature() = 0;
	virtual void OpenRtv() = 0;
	virtual void CloseRtv() = 0;
	virtual void Swapchain() = 0;
};

